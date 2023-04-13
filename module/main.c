#include <ctype.h>
#include <stdio.h>
#include <string.h>

// asf
#include "compiler.h"
#include "delay.h"
#include "gpio.h"
#include "intc.h"
#include "pm.h"
#include "preprocessor.h"
#include "print_funcs.h"
#include "spi.h"
#include "sysclk.h"
#include "usb_protocol_hid.h"

// system
#include "adc.h"
#include "cdc.h"
#include "events.h"
#include "font.h"
#include "hid.h"
#include "i2c.h"
#include "init_common.h"
#include "init_teletype.h"
#include "interrupts.h"
#include "kbd.h"
#include "midi.h"
#include "midi_common.h"
#include "monome.h"
#include "region.h"
#include "screen.h"
#include "timers.h"
#include "util.h"

// this
#include "chaos.h"
#include "conf_board.h"
#include "edit_mode.h"
#include "flash.h"
#include "globals.h"
#include "grid.h"
#include "help_mode.h"
#include "keyboard_helper.h"
#include "live_mode.h"
#include "pattern_mode.h"
#include "preset_r_mode.h"
#include "preset_w_mode.h"
#include "teletype.h"
#include "teletype_io.h"
#include "usb_disk_mode.h"

#ifdef TELETYPE_PROFILE
#include "profile.h"

profile_t prof_Script[TOTAL_SCRIPT_COUNT], prof_Delay[DELAY_SIZE], prof_CV,
    prof_ADC, prof_ScreenRefresh;

void tele_profile_script(size_t s) {
    profile_update(&prof_Script[s]);
}

void tele_profile_delay(uint8_t d) {
    profile_update(&prof_Delay[d]);
}

#endif

////////////////////////////////////////////////////////////////////////////////
// constants

#define RATE_CLOCK 10
#define RATE_CV 6
#define SS_TIMEOUT 90 /* minutes */ * 60 * 100


////////////////////////////////////////////////////////////////////////////////
// globals (defined in globals.h)

scene_state_t scene_state;
char scene_text[SCENE_TEXT_LINES][SCENE_TEXT_CHARS];
uint8_t preset_select;
region line[8] = {
    {.w = 128, .h = 8, .x = 0, .y = 0 },  {.w = 128, .h = 8, .x = 0, .y = 8 },
    {.w = 128, .h = 8, .x = 0, .y = 16 }, {.w = 128, .h = 8, .x = 0, .y = 24 },
    {.w = 128, .h = 8, .x = 0, .y = 32 }, {.w = 128, .h = 8, .x = 0, .y = 40 },
    {.w = 128, .h = 8, .x = 0, .y = 48 }, {.w = 128, .h = 8, .x = 0, .y = 56 }
};
char copy_buffer[SCENE_TEXT_LINES][SCENE_TEXT_CHARS];
uint8_t copy_buffer_len = 0;


////////////////////////////////////////////////////////////////////////////////
// locals

static device_config_t device_config;
static tele_mode_t mode = M_LIVE;
static tele_mode_t last_mode = M_LIVE;
static uint32_t ss_counter = 0;
static u8 grid_connected = 0;
static u8 grid_control_mode = 0;
static u8 midi_clock_counter = 0;

static uint16_t adc[4];

typedef struct {
    uint16_t now;
    uint16_t off;
    uint16_t target;
    uint16_t slew;
    uint16_t step;
    int32_t delta;
    uint32_t a;
} aout_t;

static u8 ignore_front_press = 0;
static aout_t aout[4];
static bool metro_timer_enabled;
static uint8_t front_timer;
static uint8_t mod_key = 0, hold_key, hold_key_count = 0;
static uint64_t last_adc_tick = 0;
static midi_behavior_t midi_behavior;

// timers
static softTimer_t clockTimer = {.next = NULL, .prev = NULL };
static softTimer_t refreshTimer = {.next = NULL, .prev = NULL };
static softTimer_t keyTimer = {.next = NULL, .prev = NULL };
static softTimer_t cvTimer = {.next = NULL, .prev = NULL };
static softTimer_t adcTimer = {.next = NULL, .prev = NULL };
static softTimer_t hidTimer = {.next = NULL, .prev = NULL };
static softTimer_t metroTimer = {.next = NULL, .prev = NULL };
static softTimer_t monomePollTimer = {.next = NULL, .prev = NULL };
static softTimer_t monomeRefreshTimer = {.next = NULL, .prev = NULL };
static softTimer_t gridFaderTimer = {.next = NULL, .prev = NULL };
static softTimer_t midiScriptTimer = {.next = NULL, .prev = NULL };


////////////////////////////////////////////////////////////////////////////////
// prototypes

// timer callback prototypes
static void cvTimer_callback(void* o);
static void clockTimer_callback(void* o);
static void refreshTimer_callback(void* o);
static void keyTimer_callback(void* o);
static void adcTimer_callback(void* o);
static void hidTimer_callback(void* o);
static void metroTimer_callback(void* o);
static void monome_poll_timer_callback(void* obj);
static void monome_refresh_timer_callback(void* obj);
static void grid_fader_timer_callback(void* obj);
static void midiScriptTimer_callback(void* obj);

// event handler prototypes
static void handler_None(int32_t data);
static void handler_Front(int32_t data);
static void handler_PollADC(int32_t data);
static void handler_KeyTimer(int32_t data);
static void handler_HidConnect(int32_t data);
static void handler_HidDisconnect(int32_t data);
static void handler_HidTimer(int32_t data);
static void handler_MscConnect(int32_t data);
static void handler_Trigger(int32_t data);
static void handler_ScreenRefresh(int32_t data);
static void handler_EventTimer(int32_t data);
static void handler_AppCustom(int32_t data);

// event queue
static void empty_event_handlers(void);
static void assign_main_event_handlers(void);
static void assign_msc_event_handlers(void);
static void check_events(void);

// key handling
static void process_keypress(uint8_t key, uint8_t mod_key, bool is_held_key,
                             bool is_release);
static bool process_global_keys(uint8_t key, uint8_t mod_key, bool is_held_key);

// start/stop monome polling/refresh timers
void timers_set_monome(void);
void timers_unset_monome(void);

// other
static void render_init(void);
static void exit_screensaver(void);
static void update_device_config(u8 refresh);


////////////////////////////////////////////////////////////////////////////////
// timer callbacks

void cvTimer_callback(void* o) {
#ifdef TELETYPE_PROFILE
    profile_update(&prof_CV);
#endif
    bool updated = false;
    bool slewing = false;

    for (size_t i = 0; i < 4; i++) {
        if (aout[i].step) {
            aout[i].step--;

            if (aout[i].step == 0) { aout[i].now = aout[i].target; }
            else {
                aout[i].a += aout[i].delta;
                aout[i].now = aout[i].a >> 16;
                slewing = true;
            }

            updated = true;
        }
    }

    set_slew_icon(slewing);

    if (updated) {
        uint16_t a0, a1, a2, a3;

        if (device_config.flip) {
            a0 = aout[3].now >> 2;
            a1 = aout[2].now >> 2;
            a2 = aout[1].now >> 2;
            a3 = aout[0].now >> 2;
        }
        else {
            a0 = aout[0].now >> 2;
            a1 = aout[1].now >> 2;
            a2 = aout[2].now >> 2;
            a3 = aout[3].now >> 2;
        }

        spi_selectChip(DAC_SPI, DAC_SPI_NPCS);
        spi_write(DAC_SPI, 0x31);
        spi_write(DAC_SPI, a2 >> 4);
        spi_write(DAC_SPI, a2 << 4);
        spi_write(DAC_SPI, 0x31);
        spi_write(DAC_SPI, a0 >> 4);
        spi_write(DAC_SPI, a0 << 4);
        spi_unselectChip(DAC_SPI, DAC_SPI_NPCS);

        spi_selectChip(DAC_SPI, DAC_SPI_NPCS);
        spi_write(DAC_SPI, 0x38);
        spi_write(DAC_SPI, a3 >> 4);
        spi_write(DAC_SPI, a3 << 4);
        spi_write(DAC_SPI, 0x38);
        spi_write(DAC_SPI, a1 >> 4);
        spi_write(DAC_SPI, a1 << 4);
        spi_unselectChip(DAC_SPI, DAC_SPI_NPCS);
    }
#ifdef TELETYPE_PROFILE
    profile_update(&prof_CV);
#endif
}

void clockTimer_callback(void* o) {
    event_t e = {.type = kEventTimer, .data = 0 };
    event_post(&e);
}

void refreshTimer_callback(void* o) {
    event_t e = {.type = kEventScreenRefresh, .data = 0 };
    event_post(&e);
}

void keyTimer_callback(void* o) {
    event_t e = {.type = kEventKeyTimer, .data = 0 };
    event_post(&e);
}

void adcTimer_callback(void* o) {
    event_t e = {.type = kEventPollADC, .data = 0 };
    event_post(&e);
}

void hidTimer_callback(void* o) {
    event_t e = {.type = kEventHidTimer, .data = 0 };
    event_post(&e);
}

void metroTimer_callback(void* o) {
    event_t e = {.type = kEventAppCustom, .data = 0 };
    event_post(&e);
}

// monome polling callback
static void monome_poll_timer_callback(void* obj) {
    // asynchronous, non-blocking read
    // UHC callback spawns appropriate events
    serial_read();
}

// monome refresh callback
static void monome_refresh_timer_callback(void* obj) {
    if (grid_connected && scene_state.grid.grid_dirty) {
        static event_t e;
        e.type = kEventMonomeRefresh;
        event_post(&e);
    }
}

// monome: start polling
void timers_set_monome(void) {
    timer_add(&monomePollTimer, 20, &monome_poll_timer_callback, NULL);
    timer_add(&monomeRefreshTimer, 30, &monome_refresh_timer_callback, NULL);
}

// monome stop polling
void timers_unset_monome(void) {
    timer_remove(&monomePollTimer);
    timer_remove(&monomeRefreshTimer);
}

void grid_fader_timer_callback(void* o) {
    grid_process_fader_slew(&scene_state);
}

void midiScriptTimer_callback(void* obj) {
    u8 executed[EDITABLE_SCRIPT_COUNT];
    for (uint8_t i = 0; i < EDITABLE_SCRIPT_COUNT; i++) executed[i] = 0;

    if (scene_state.midi.on_count && scene_state.midi.on_script >= 0 &&
        scene_state.midi.on_script < EDITABLE_SCRIPT_COUNT) {
        run_script(&scene_state, scene_state.midi.on_script);
        executed[scene_state.midi.on_script] = 1;
    }

    if (scene_state.midi.off_count && scene_state.midi.off_script >= 0 &&
        scene_state.midi.off_script < EDITABLE_SCRIPT_COUNT) {
        if (!executed[scene_state.midi.off_script])
            run_script(&scene_state, scene_state.midi.off_script);
        executed[scene_state.midi.off_script] = 1;
    }

    if (scene_state.midi.cc_count && scene_state.midi.cc_script >= 0 &&
        scene_state.midi.cc_script < EDITABLE_SCRIPT_COUNT) {
        if (!executed[scene_state.midi.cc_script])
            run_script(&scene_state, scene_state.midi.cc_script);
    }

    scene_state.midi.on_count = 0;
    scene_state.midi.off_count = 0;
    scene_state.midi.cc_count = 0;
}

////////////////////////////////////////////////////////////////////////////////
// event handlers

void handler_None(int32_t data) {}

void handler_Front(int32_t data) {
    if (ss_counter >= SS_TIMEOUT) {
        exit_screensaver();
        return;
    }
    ss_counter = 0;

    if (data == 0) {
        if (ignore_front_press) {
            ignore_front_press = 0;
            return;
        }

        if (grid_connected) {
            grid_control_mode = !grid_control_mode;
            if (grid_control_mode && mode == M_HELP) set_mode(M_LIVE);
            grid_set_control_mode(grid_control_mode, mode, &scene_state);
            return;
        }

        if (mode != M_PRESET_R) {
            front_timer = 0;
            set_preset_r_mode(adc[1] >> 7);
            set_mode(M_PRESET_R);
        }
        else
            front_timer = 15;
    }
    else {
        if (front_timer) { set_last_mode(); }
        front_timer = 0;
    }
}


void handler_PollADC(int32_t data) {
#ifdef TELETYPE_PROFILE
    profile_update(&prof_ADC);
#endif
    static int16_t last_knob = 0;

    adc_convert(&adc);

    ss_set_in(&scene_state, adc[0] << 2);

    if (ss_counter >= SS_TIMEOUT && (adc[1] >> 8 != last_knob >> 8)) {
        exit_screensaver();
        return;
    }
    last_knob = adc[1];

    if (mode == M_PATTERN) {
        process_pattern_knob(adc[1], mod_key);
        ss_set_param(&scene_state, adc[1] << 2);
    }
    else if (mode == M_PRESET_R && !(grid_connected && grid_control_mode)) {
        uint8_t preset = adc[1] >> 6;
        uint8_t deadzone = preset & 1;
        preset >>= 1;
        if (!deadzone || abs(preset - get_preset()) > 1)
            process_preset_r_preset(preset);
    }
    else {
        ss_set_param(&scene_state, adc[1] << 2);
    }
#ifdef TELETYPE_PROFILE
    profile_update(&prof_ADC);
#endif
}

void handler_KeyTimer(int32_t data) {
    if (front_timer) {
        if (front_timer == 1 && !grid_connected) {
            if (mode == M_PRESET_R) { process_preset_r_load(); }
            front_timer = 0;
        }
        else
            front_timer--;
    }

    if (hold_key) {
        if (hold_key_count > 4)
            process_keypress(hold_key, mod_key, true, false);
        else
            hold_key_count++;
    }
}

void handler_HidConnect(int32_t data) {
    timer_add(&hidTimer, 47, &hidTimer_callback, NULL);
}

void handler_HidDisconnect(int32_t data) {
    timer_remove(&hidTimer);
}

void handler_HidTimer(int32_t data) {
    if (hid_get_frame_dirty()) {
        const int8_t* frame = (const int8_t*)hid_get_frame_data();

        mod_key = frame[0];
        for (size_t i = 2; i < 8; i++) {
            if (frame[i] == 0) {
                if (i == 2) {
                    hold_key_count = 0;
                    process_keypress(hold_key, mod_key, false, true);
                    hold_key = 0;
                }
            }
            else if (frame_compare(frame[i]) == false) {
                hold_key = frame[i];
                hold_key_count = 0;
                process_keypress(hold_key, mod_key, false, false);
            }
        }

        set_old_frame(frame);
    }

    hid_clear_frame_dirty();
}

void handler_MscConnect(int32_t data) {
    // disable event handlers while doing USB write
    assign_msc_event_handlers();

    // disable timers
    u8 flags = irqs_pause();

    // clear screen
    for (size_t i = 0; i < 8; i++) {
        region_fill(&line[i], 0);
        region_draw(&line[i]);
    }

    // do USB
    tele_usb_disk();

    // renable teletype
    set_mode(M_LIVE);
    assign_main_event_handlers();
    irqs_resume(flags);
}

void handler_Trigger(int32_t data) {
    u8 input = device_config.flip ? 7 - data : data;
    if (!ss_get_mute(&scene_state, input)) {
        bool tr_state = gpio_get_pin_value(A00 + data);
        if (tr_state) {
            if (scene_state.variables.script_pol[input] & 1) {
                run_script(&scene_state, input);
            }
        }
        else {
            if (scene_state.variables.script_pol[input] & 2) {
                run_script(&scene_state, input);
            }
        }
    }
}

void handler_ScreenRefresh(int32_t data) {
#ifdef TELETYPE_PROFILE
    profile_update(&prof_ScreenRefresh);
#endif
    uint8_t screen_dirty = 0;

    switch (mode) {
        case M_PATTERN: screen_dirty = screen_refresh_pattern(); break;
        case M_PRESET_W: screen_dirty = screen_refresh_preset_w(); break;
        case M_PRESET_R: screen_dirty = screen_refresh_preset_r(); break;
        case M_HELP: screen_dirty = screen_refresh_help(); break;
        case M_LIVE: screen_dirty = screen_refresh_live(); break;
        case M_EDIT: screen_dirty = screen_refresh_edit(); break;
    }

    u8 grid = 0;
    for (size_t i = 0; i < 8; i++)
        if (screen_dirty & (1 << i)) {
            grid = 1;
            if (ss_counter < SS_TIMEOUT) region_draw(&line[i]);
        }
    if (grid_control_mode && grid) scene_state.grid.grid_dirty = 1;

#ifdef TELETYPE_PROFILE
    profile_update(&prof_ScreenRefresh);
#endif
}

void handler_EventTimer(int32_t data) {
    tele_tick(&scene_state, RATE_CLOCK);

    if (ss_counter < SS_TIMEOUT) {
        ss_counter++;
        if (ss_counter == SS_TIMEOUT) {
            u8 empty = 0;
            for (int i = 0; i < 64; i++)
                for (int j = 0; j < 64; j++)
                    screen_draw_region(i << 1, j, 2, 1, &empty);
        }
    }
}

void handler_AppCustom(int32_t data) {
    // If we need multiple custom event handlers then we can use an enum in the
    // data argument. For now, we're just using it for the metro
    if (ss_get_script_len(&scene_state, METRO_SCRIPT)) {
        set_metro_icon(true);
        run_script(&scene_state, METRO_SCRIPT);
        if (grid_connected && grid_control_mode)
            grid_metro_triggered(&scene_state);
    }
    else
        set_metro_icon(false);
}

static void handler_FtdiConnect(s32 data) {
    ftdi_setup();
}

static void handler_SerialConnect(s32 data) {
    monome_setup_mext();
}

static void handler_FtdiDisconnect(s32 data) {
    grid_connected = 0;
    timers_unset_monome();
}

static void handler_MonomeConnect(s32 data) {
    hold_key = 0;
    timers_set_monome();
    grid_connected = 1;

    if (grid_control_mode && mode == M_HELP) set_mode(M_LIVE);
    grid_set_control_mode(grid_control_mode, mode, &scene_state);

    scene_state.grid.grid_dirty = 1;
    grid_clear_held_keys();
}

static void handler_MonomePoll(s32 data) {
    monome_read_serial();
}

static void handler_MonomeRefresh(s32 data) {
    grid_refresh(&scene_state);
    monomeFrameDirty = 0b1111;
    (*monome_refresh)();
}

static void handler_MonomeGridKey(s32 data) {
    if (grid_control_mode && ss_counter >= SS_TIMEOUT) {
        exit_screensaver();
        return;
    }
    if (grid_control_mode) ss_counter = 0;

    u8 x, y, z;
    monome_grid_key_parse_event_data(data, &x, &y, &z);
    grid_process_key(&scene_state, x, y, z, 0);
}

static void handler_midi_connect(s32 data) {}

static void handler_midi_disconnect(s32 data) {}

static void handler_standard_midi_packet(s32 data) {
    midi_packet_parse(&midi_behavior, (u32)data);
}

static void midi_note_on(u8 ch, u8 num, u8 vel) {
    scene_state.midi.last_event_type = 1;
    scene_state.midi.last_channel = ch;
    scene_state.midi.last_note = num;
    scene_state.midi.last_velocity = vel;

    if (scene_state.midi.on_script != -1 &&
        scene_state.midi.on_count < MAX_MIDI_EVENTS) {
        scene_state.midi.on_channel[scene_state.midi.on_count] = ch;
        scene_state.midi.note_on[scene_state.midi.on_count] = num;
        scene_state.midi.note_vel[scene_state.midi.on_count] = vel;
        scene_state.midi.on_count++;
    }
}

static void midi_note_off(u8 ch, u8 num, u8 vel) {
    scene_state.midi.last_event_type = 2;
    scene_state.midi.last_channel = ch;
    scene_state.midi.last_note = num;
    scene_state.midi.last_velocity = vel;

    if (scene_state.midi.off_script != -1 &&
        scene_state.midi.off_count < MAX_MIDI_EVENTS) {
        scene_state.midi.off_channel[scene_state.midi.off_count] = ch;
        scene_state.midi.note_off[scene_state.midi.off_count] = num;
        scene_state.midi.off_count++;
    }
}

static void midi_control_change(u8 ch, u8 num, u8 val) {
    scene_state.midi.last_event_type = 3;
    scene_state.midi.last_channel = ch;
    scene_state.midi.last_controller = num;
    scene_state.midi.last_cc = val;

    if (scene_state.midi.cc_script != -1) {
        u8 found = 0;
        for (u8 i = 0; i < scene_state.midi.cc_count; i++) {
            if (scene_state.midi.cn[i] == num &&
                scene_state.midi.cc_channel[i] == ch) {
                scene_state.midi.cc[i] = val;
                found = 1;
                break;
            }
        }

        if (!found && scene_state.midi.cc_count < MAX_MIDI_EVENTS) {
            scene_state.midi.cc_channel[scene_state.midi.cc_count] = ch;
            scene_state.midi.cn[scene_state.midi.cc_count] = num;
            scene_state.midi.cc[scene_state.midi.cc_count] = val;
            scene_state.midi.cc_count++;
        }
    }
}

static void midi_clock_tick(void) {
    if (++midi_clock_counter >= scene_state.midi.clock_div) {
        midi_clock_counter = 0;
        scene_state.midi.last_event_type = 4;
        if (scene_state.midi.clk_script >= 0 &&
            scene_state.midi.clk_script < EDITABLE_SCRIPT_COUNT)
            run_script(&scene_state, scene_state.midi.clk_script);
    }
}

static void midi_seq_start(void) {
    scene_state.midi.last_event_type = 5;
    if (scene_state.midi.start_script >= 0 &&
        scene_state.midi.start_script < EDITABLE_SCRIPT_COUNT)
        run_script(&scene_state, scene_state.midi.start_script);
}

static void midi_seq_stop(void) {
    scene_state.midi.last_event_type = 6;
    if (scene_state.midi.stop_script >= 0 &&
        scene_state.midi.stop_script < EDITABLE_SCRIPT_COUNT)
        run_script(&scene_state, scene_state.midi.stop_script);
}

static void midi_seq_continue(void) {
    scene_state.midi.last_event_type = 7;
    if (scene_state.midi.continue_script >= 0 &&
        scene_state.midi.continue_script < EDITABLE_SCRIPT_COUNT)
        run_script(&scene_state, scene_state.midi.continue_script);
}

////////////////////////////////////////////////////////////////////////////////
// event queue

void empty_event_handlers() {
    for (size_t i = 0; i < kNumEventTypes; i++) {
        app_event_handlers[i] = &handler_None;
    }
}

void assign_main_event_handlers() {
    empty_event_handlers();

    app_event_handlers[kEventFront] = &handler_Front;
    app_event_handlers[kEventPollADC] = &handler_PollADC;
    app_event_handlers[kEventKeyTimer] = &handler_KeyTimer;
    app_event_handlers[kEventHidConnect] = &handler_HidConnect;
    app_event_handlers[kEventHidDisconnect] = &handler_HidDisconnect;
    app_event_handlers[kEventHidTimer] = &handler_HidTimer;
    app_event_handlers[kEventMscConnect] = &handler_MscConnect;
    app_event_handlers[kEventTrigger] = &handler_Trigger;
    app_event_handlers[kEventScreenRefresh] = &handler_ScreenRefresh;
    app_event_handlers[kEventTimer] = &handler_EventTimer;
    app_event_handlers[kEventAppCustom] = &handler_AppCustom;
    app_event_handlers[kEventFtdiConnect] = &handler_FtdiConnect;
    app_event_handlers[kEventFtdiDisconnect] = &handler_FtdiDisconnect;
    app_event_handlers[kEventMonomeConnect] = &handler_MonomeConnect;
    app_event_handlers[kEventMonomeDisconnect] = &handler_None;
    app_event_handlers[kEventMonomePoll] = &handler_MonomePoll;
    app_event_handlers[kEventMonomeRefresh] = &handler_MonomeRefresh;
    app_event_handlers[kEventMonomeGridKey] = &handler_MonomeGridKey;
    app_event_handlers[kEventMidiConnect] = &handler_midi_connect;
    app_event_handlers[kEventMidiDisconnect] = &handler_midi_disconnect;
    app_event_handlers[kEventMidiPacket] = &handler_standard_midi_packet;
    app_event_handlers[kEventSerialConnect] = &handler_SerialConnect;
    app_event_handlers[kEventSerialDisconnect] = &handler_FtdiDisconnect;
}

static void assign_msc_event_handlers(void) {
    empty_event_handlers();

    // one day this could be used to map the front button and pot to be used as
    // a UI with a memory stick
}

// app event loop
void check_events(void) {
    event_t e;
    if (event_next(&e)) { (app_event_handlers)[e.type](e.data); }
}


////////////////////////////////////////////////////////////////////////////////
// mode handling

// defined in globals.h
void set_mode(tele_mode_t m) {
    last_mode = mode;
    switch (m) {
        case M_LIVE:
            set_live_mode();
            mode = M_LIVE;
            break;
        case M_EDIT:
            set_edit_mode();
            mode = M_EDIT;
            break;
        case M_PATTERN:
            set_pattern_mode();
            mode = M_PATTERN;
            break;
        case M_PRESET_W:
            set_preset_w_mode();
            mode = M_PRESET_W;
            break;
        case M_PRESET_R:
            set_preset_r_mode(adc[1] >> 7);
            mode = M_PRESET_R;
            break;
        case M_HELP:
            set_help_mode();
            mode = M_HELP;
            break;
    }
    if (mode != M_HELP) flash_update_last_mode(mode);
}

// defined in globals.h
void set_last_mode() {
    if (mode == last_mode) return;

    if (last_mode == M_LIVE || last_mode == M_EDIT || last_mode == M_PATTERN)
        set_mode(last_mode);
    else
        set_mode(M_LIVE);
}

// defined in globals.h
void clear_delays_and_slews(scene_state_t* ss) {
    clear_delays(ss);
    for (int i = 0; i < 4; i++) { aout[i].step = 1; }
}

////////////////////////////////////////////////////////////////////////////////
// key handling

void process_keypress(uint8_t key, uint8_t mod_key, bool is_held_key,
                      bool is_release) {
    // reset inactivity counter
    if (ss_counter >= SS_TIMEOUT) {
        exit_screensaver();
        return;
    }
    ss_counter = 0;

    // release is a special case for live mode
    if (is_release) {
        if (mode == M_LIVE)
            process_live_keys(key, mod_key, is_held_key, true, &scene_state);
        return;
    }

    // first try global keys
    if (process_global_keys(key, mod_key, is_held_key)) return;


    switch (mode) {
        case M_EDIT: process_edit_keys(key, mod_key, is_held_key); break;
        case M_LIVE:
            process_live_keys(key, mod_key, is_held_key, false, &scene_state);
            break;
        case M_PATTERN: process_pattern_keys(key, mod_key, is_held_key); break;
        case M_PRESET_W:
            process_preset_w_keys(key, mod_key, is_held_key);
            break;
        case M_PRESET_R:
            process_preset_r_keys(key, mod_key, is_held_key);
            break;
        case M_HELP: process_help_keys(key, mod_key, is_held_key); break;
    }
}

bool process_global_keys(uint8_t k, uint8_t m, bool is_held_key) {
    if (is_held_key)  // none of these want to work with held keys
        return false;

    // <tab>: change modes, live to edit to pattern and back
    if (match_no_mod(m, k, HID_TAB)) {
        if (mode == M_LIVE)
            set_mode(M_EDIT);
        else if (mode == M_EDIT)
            set_mode(M_PATTERN);
        else
            set_mode(M_LIVE);
        return true;
    }
    // <esc>: preset read mode, or return to last mode
    else if (match_no_mod(m, k, HID_ESCAPE)) {
        if (mode == M_PRESET_R)
            set_last_mode();
        else {
            set_mode(M_PRESET_R);
        }
        return true;
    }
    // alt-<esc>: preset write mode
    else if (match_alt(m, k, HID_ESCAPE)) {
        set_mode(M_PRESET_W);
        return true;
    }
    // win-<esc>: clear delays, stack and slews
    else if (match_win(m, k, HID_ESCAPE)) {
        if (!is_held_key) clear_delays_and_slews(&scene_state);
        return true;
    }
    // <alt>-?: help text, or return to last mode
    else if (match_shift_alt(m, k, HID_SLASH) || match_alt(m, k, HID_H)) {
        if (mode == M_HELP)
            set_last_mode();
        else {
            set_mode(M_HELP);
        }
        return true;
    }
    // <F1> through <F8>: run corresponding script
    else if (no_mod(m) && k >= HID_F1 && k <= HID_F8) {
        run_script(&scene_state, k - HID_F1);
        return true;
    }
    // <F9>: run metro script
    else if (no_mod(m) && k == HID_F9) {
        run_script(&scene_state, METRO_SCRIPT);
        return true;
    }
    // <F10>: run init script
    else if (no_mod(m) && k == HID_F10) {
        run_script(&scene_state, INIT_SCRIPT);
        return true;
    }
    // alt-<F1> through alt-<F8>: edit corresponding script
    else if (mod_only_alt(m) && k >= HID_F1 && k <= HID_F8) {
        set_edit_mode_script(k - HID_F1);
        set_mode(M_EDIT);
        return true;
    }
    // alt-<F9>: edit metro script
    else if (mod_only_alt(m) && k == HID_F9) {
        set_edit_mode_script(METRO_SCRIPT);
        set_mode(M_EDIT);
        return true;
    }
    // alt-<F10>: edit init script
    else if (mod_only_alt(m) && k == HID_F10) {
        set_edit_mode_script(INIT_SCRIPT);
        set_mode(M_EDIT);
        return true;
    }
    // ctrl-<F1> through ctrl-<F8> mute triggers
    else if (mod_only_ctrl(m) && k >= HID_F1 && k <= HID_F8) {
        bool muted = ss_get_mute(&scene_state, (k - HID_F1));
        ss_set_mute(&scene_state, (k - HID_F1), !muted);
        screen_mutes_updated();
        set_mutes_updated();
        return true;
    }
    // ctrl-<F9> toggle metro
    else if (mod_only_ctrl(m) && k == HID_F9) {
        scene_state.variables.m_act = !scene_state.variables.m_act;
        tele_metro_updated();
        return true;
    }
    // <numpad-1> through <numpad-8>: run corresponding script
    else if (no_mod(m) && k >= HID_KEYPAD_1 && k <= HID_KEYPAD_8) {
        run_script(&scene_state, k - HID_KEYPAD_1);
        return true;
    }
    // <num lock>: jump to pattern mode
    else if (match_no_mod(m, k, HID_KEYPAD_NUM_LOCK) ||
             match_no_mod(m, k, HID_F11)) {
        if (mode != M_PATTERN) { set_mode(M_PATTERN); }
        return true;
    }
    // <print screen>: jump to live mode
    else if (match_no_mod(m, k, HID_PRINTSCREEN) ||
             match_no_mod(m, k, HID_F12)) {
        if (mode != M_LIVE) { set_mode(M_LIVE); }
        return true;
    }
    else {
        return false;
    }
}


////////////////////////////////////////////////////////////////////////////////
// other

void render_init(void) {
    region_alloc(&line[0]);
    region_alloc(&line[1]);
    region_alloc(&line[2]);
    region_alloc(&line[3]);
    region_alloc(&line[4]);
    region_alloc(&line[5]);
    region_alloc(&line[6]);
    region_alloc(&line[7]);
}

void exit_screensaver(void) {
    ss_counter = 0;
    set_mode(mode);
}

void update_device_config(u8 refresh) {
    screen_set_direction(device_config.flip);
    if (refresh) set_mode(mode);
    flash_update_device_config(&device_config);
}

static void setup_midi(void) {
    midi_behavior.note_on = &midi_note_on;
    midi_behavior.note_off = &midi_note_off;
    midi_behavior.channel_pressure = NULL;
    midi_behavior.pitch_bend = NULL;
    midi_behavior.control_change = &midi_control_change;
    midi_behavior.clock_tick = &midi_clock_tick;
    midi_behavior.seq_start = &midi_seq_start;
    midi_behavior.seq_stop = &midi_seq_stop;
    midi_behavior.seq_continue = &midi_seq_continue;
    midi_behavior.panic = NULL;
}


////////////////////////////////////////////////////////////////////////////////
// teletype_io.h

uint32_t tele_get_ticks() {
    return get_ticks();
}

void tele_metro_updated() {
    uint32_t metro_time = scene_state.variables.m;

    bool m_act = scene_state.variables.m_act;
    if (metro_time < METRO_MIN_UNSUPPORTED_MS) {
        metro_time = METRO_MIN_UNSUPPORTED_MS;
    }

    if (m_act && !metro_timer_enabled) {  // enable the timer
        timer_add(&metroTimer, metro_time, &metroTimer_callback, NULL);
        metro_timer_enabled = true;
    }
    else if (!m_act && metro_timer_enabled) {  // disable the timer
        timer_remove(&metroTimer);
        metro_timer_enabled = false;
    }
    else if (metro_timer_enabled) {  // just update the time
        metroTimer.ticks = metro_time;
    }

    if (metro_timer_enabled && ss_get_script_len(&scene_state, METRO_SCRIPT))
        set_metro_icon(true);
    else
        set_metro_icon(false);

    if (grid_connected && grid_control_mode) scene_state.grid.grid_dirty = 1;

    edit_mode_refresh();
}

void tele_metro_reset() {
    if (metro_timer_enabled) timer_reset(&metroTimer);
}

void tele_tr(uint8_t i, int16_t v) {
    uint32_t pin = B08 + (device_config.flip ? 3 - i : i);

    if (v)
        gpio_set_pin_high(pin);
    else
        gpio_set_pin_low(pin);
}

void tele_cv(uint8_t i, int16_t v, uint8_t s) {
    int16_t t = v + aout[i].off;
    if (t < 0)
        t = 0;
    else if (t > 16383)
        t = 16383;
    aout[i].target = t;
    if (s) {
        aout[i].step = aout[i].slew;
        aout[i].delta = ((aout[i].target - aout[i].now) << 16) / aout[i].step;
    }
    else {
        aout[i].step = 1;
        aout[i].now = aout[i].target;
    }

    aout[i].a = aout[i].now << 16;

    timer_manual(&cvTimer);
}

void tele_cv_slew(uint8_t i, int16_t v) {
    aout[i].slew = v / RATE_CV;
    if (aout[i].slew == 0) aout[i].slew = 1;
}

void tele_cv_off(uint8_t i, int16_t v) {
    aout[i].off = v;
}

void tele_update_adc(u8 force) {
    if (!force && get_ticks() == last_adc_tick) return;
    last_adc_tick = get_ticks();
    adc_convert(&adc);
    ss_set_in(&scene_state, adc[0] << 2);
    ss_set_param(&scene_state, adc[1] << 2);
}

void tele_ii_tx(uint8_t addr, uint8_t* data, uint8_t l) {
    i2c_leader_tx(addr, data, l);
}

void tele_ii_rx(uint8_t addr, uint8_t* data, uint8_t l) {
    i2c_leader_rx(addr, data, l);
}

void tele_scene(uint8_t i, uint8_t init_grid, uint8_t init_pattern) {
    if (i >= SCENE_SLOTS) return;
    preset_select = i;
    flash_read(i, &scene_state, &scene_text, init_pattern, init_grid, 0);
    set_dash_updated();
    if (init_grid) scene_state.grid.scr_dirty = scene_state.grid.grid_dirty = 1;
}

void tele_kill() {
    for (int i = 0; i < 4; i++) {
        aout[i].step = 1;
        tele_tr(i, 0);
    }
}

bool tele_get_input_state(uint8_t n) {
    u8 input = device_config.flip ? 7 - n : n;
    return gpio_get_pin_value(A00 + input) > 0;
}

void tele_vars_updated() {
    set_vars_updated();
}

void tele_save_calibration() {
    flash_update_cal(&scene_state.cal);
}

void grid_key_press(uint8_t x, uint8_t y, uint8_t z) {
    grid_process_key(&scene_state, x, y, z, 1);
}

void device_flip() {
    device_config.flip = !device_config.flip;
    update_device_config(1);
}

void reset_midi_counter() {
    midi_clock_counter = 0;
}

////////////////////////////////////////////////////////////////////////////////
// main

int main(void) {
    sysclk_init();

    init_dbg_rs232(FMCK_HZ);

    init_gpio();
    assign_main_event_handlers();
    init_events();
    init_tc();
    init_spi();
    init_adc();

    irq_initialize_vectors();
    register_interrupts();
    cpu_irq_enable();

    init_usb_host();
    init_monome();
    init_oled();
    setup_midi();

    // wait to allow for any i2c devices to fully initalise
    delay_ms(1500);

    init_i2c_leader();

    print_dbg("\r\n\r\n// teletype! //////////////////////////////// ");

    ss_init(&scene_state);

    // screen init
    render_init();

    if (is_flash_fresh()) {
        char s[36];
        strcpy(s, "SCENES WILL BE OVERWRITTEN!");
        region_fill(&line[5], 0);
        font_string_region_clip(&line[5], s, 0, 0, 0x4, 0);
        region_draw(&line[5]);

        strcpy(s, "PRESS TO CONFIRM");
        region_fill(&line[6], 0);
        font_string_region_clip(&line[6], s, 0, 0, 0x4, 0);
        region_draw(&line[6]);

        strcpy(s, "DO NOT PRESS OTHERWISE!");
        region_fill(&line[7], 0);
        font_string_region_clip(&line[7], s, 0, 0, 0x4, 0);
        region_draw(&line[7]);
        ignore_front_press = 1;
    }

    // prepare flash (if needed)
    flash_prepare();

    // load device config
    flash_get_device_config(&device_config);
    update_device_config(0);

    // load calibration data from flash
    flash_get_cal(&scene_state.cal);
    ss_update_param_scale(&scene_state);
    ss_update_in_scale(&scene_state);
    ss_update_fader_scale_all(&scene_state);

    // load preset from flash
    preset_select = flash_last_saved_scene();
    ss_set_scene(&scene_state, preset_select);
    flash_read(preset_select, &scene_state, &scene_text, 1, 1, 1);

    // setup daisy chain for two dacs
    spi_selectChip(DAC_SPI, DAC_SPI_NPCS);
    spi_write(DAC_SPI, 0x80);
    spi_write(DAC_SPI, 0xff);
    spi_write(DAC_SPI, 0xff);
    spi_unselectChip(DAC_SPI, DAC_SPI_NPCS);

    timer_add(&clockTimer, RATE_CLOCK, &clockTimer_callback, NULL);
    timer_add(&cvTimer, RATE_CV, &cvTimer_callback, NULL);
    timer_add(&keyTimer, 71, &keyTimer_callback, NULL);
    timer_add(&adcTimer, 61, &adcTimer_callback, NULL);
    timer_add(&refreshTimer, 63, &refreshTimer_callback, NULL);
    timer_add(&gridFaderTimer, 25, &grid_fader_timer_callback, NULL);
    timer_add(&midiScriptTimer, 25, &midiScriptTimer_callback, NULL);

    // update IN and PARAM in case Init uses them
    tele_update_adc(1);

    // manually call tele_metro_updated to sync metro to scene_state
    metro_timer_enabled = false;
    tele_metro_updated();

    // init chaos generator
    chaos_init();
    clear_delays(&scene_state);

    aout[0].slew = 1;
    aout[1].slew = 1;
    aout[2].slew = 1;
    aout[3].slew = 1;

    init_live_mode();
    set_mode(M_LIVE);

    run_script(&scene_state, INIT_SCRIPT);
    scene_state.initializing = false;

#ifdef TELETYPE_PROFILE
    uint32_t count = 0;
#endif
    while (true) {
        midi_read();
        check_events();
#ifdef TELETYPE_PROFILE
        count = (count + 1) % (FCPU_HZ / 10);
        if (count == 0) {
            print_dbg("\r\n\r\nProfile Data (us)");
            for (uint8_t i = 0; i < TOTAL_SCRIPT_COUNT - 1; i++) {
                print_dbg("\r\nScript ");
                print_dbg_ulong(i);
                print_dbg(":\t");
                print_dbg_ulong(profile_delta_us(&prof_Script[i]));
            }
            uint32_t total = 0;
            for (uint8_t i = 0; i < DELAY_SIZE; i++)
                total += profile_delta_us(&prof_Delay[i]);
            print_dbg("\r\nDelays (total):\t");
            print_dbg_ulong(total);
            print_dbg("\r\nCV Write:\t");
            print_dbg_ulong(profile_delta_us(&prof_CV));
            print_dbg("\r\nADC Read:\t");
            print_dbg_ulong(profile_delta_us(&prof_ADC));
            print_dbg("\r\nScreen Refresh:\t");
            print_dbg_ulong(profile_delta_us(&prof_ScreenRefresh));
        }
#endif
    }
}
