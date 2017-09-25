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
#include "events.h"
#include "font.h"
#include "hid.h"
#include "i2c.h"
#include "init_common.h"
#include "init_teletype.h"
#include "interrupts.h"
#include "kbd.h"
#include "region.h"
#include "screen.h"
#include "timers.h"
#include "util.h"

// this
#include "conf_board.h"
#include "edit_mode.h"
#include "flash.h"
#include "globals.h"
#include "help_mode.h"
#include "keyboard_helper.h"
#include "live_mode.h"
#include "pattern_mode.h"
#include "preset_r_mode.h"
#include "preset_w_mode.h"
#include "teletype.h"
#include "teletype_io.h"
#include "usb_disk_mode.h"


////////////////////////////////////////////////////////////////////////////////
// constants

#define RATE_CLOCK 10
#define RATE_CV 6


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


////////////////////////////////////////////////////////////////////////////////
// locals

static tele_mode_t mode = M_LIVE;
static tele_mode_t last_mode = M_LIVE;

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

static aout_t aout[4];
static bool metro_timer_enabled;
static uint8_t front_timer;
static uint8_t mod_key = 0, hold_key, hold_key_count = 0;

// timers
static softTimer_t clockTimer = {.next = NULL, .prev = NULL };
static softTimer_t refreshTimer = {.next = NULL, .prev = NULL };
static softTimer_t keyTimer = {.next = NULL, .prev = NULL };
static softTimer_t cvTimer = {.next = NULL, .prev = NULL };
static softTimer_t adcTimer = {.next = NULL, .prev = NULL };
static softTimer_t hidTimer = {.next = NULL, .prev = NULL };
static softTimer_t metroTimer = {.next = NULL, .prev = NULL };


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
static void process_keypress(uint8_t key, uint8_t mod_key, bool is_held_key);
static bool process_global_keys(uint8_t key, uint8_t mod_key, bool is_held_key);

// other
static void render_init(void);


////////////////////////////////////////////////////////////////////////////////
// timer callbacks

void cvTimer_callback(void* o) {
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
        uint16_t a0 = aout[0].now >> 2;
        uint16_t a1 = aout[1].now >> 2;
        uint16_t a2 = aout[2].now >> 2;
        uint16_t a3 = aout[3].now >> 2;

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


////////////////////////////////////////////////////////////////////////////////
// event handlers

void handler_None(int32_t data) {}

void handler_Front(int32_t data) {
    if (data == 0) {
        if (mode != M_PRESET_R) {
            front_timer = 0;
            set_preset_r_mode(adc[1]);
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
    adc_convert(&adc);

    ss_set_in(&scene_state, adc[0] << 2);

    if (mode == M_PATTERN) {
        process_pattern_knob(adc[1], mod_key);
        ss_set_param(&scene_state, adc[1] << 2);
    }
    else if (mode == M_PRESET_R) {
        process_preset_r_knob(adc[1], mod_key);
    }
    else {
        ss_set_param(&scene_state, adc[1] << 2);
    }
}

void handler_KeyTimer(int32_t data) {
    if (front_timer) {
        if (front_timer == 1) {
            if (mode == M_PRESET_R) { process_preset_r_long_front(); }
            front_timer = 0;
        }
        else
            front_timer--;
    }

    if (hold_key) {
        if (hold_key_count > 4)
            process_keypress(hold_key, mod_key, true);
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

        for (size_t i = 2; i < 8; i++) {
            if (frame[i] == 0) {
                mod_key = frame[0];
                if (i == 2) {
                    hold_key = 0;
                    hold_key_count = 0;
                }

                break;
            }

            if (frame_compare(frame[i]) == false) {
                hold_key = frame[i];
                hold_key_count = 0;
                process_keypress(hold_key, mod_key, false);
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
    if (!ss_get_mute(&scene_state, data)) { run_script(&scene_state, data); }
}

void handler_ScreenRefresh(int32_t data) {
    bool screen_dirty = false;

    switch (mode) {
        case M_PATTERN: screen_dirty = screen_refresh_pattern(); break;
        case M_PRESET_W: screen_dirty = screen_refresh_preset_w(); break;
        case M_PRESET_R: screen_dirty = screen_refresh_preset_r(); break;
        case M_HELP: screen_dirty = screen_refresh_help(); break;
        case M_LIVE: screen_dirty = screen_refresh_live(); break;
        case M_EDIT: screen_dirty = screen_refresh_edit(); break;
    }

    if (screen_dirty) {
        for (size_t i = 0; i < 8; i++) { region_draw(&line[i]); }
    }
}

void handler_EventTimer(int32_t data) {
    tele_tick(&scene_state, RATE_CLOCK);
}

void handler_AppCustom(int32_t data) {
    // If we need multiple custom event handlers then we can use an enum in the
    // data argument. For now, we're just using it for the metro
    if (ss_get_script_len(&scene_state, METRO_SCRIPT)) {
        set_metro_icon(true);
        run_script(&scene_state, METRO_SCRIPT);
    }
    else
        set_metro_icon(false);
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
            set_preset_r_mode(adc[1]);
            mode = M_PRESET_R;
            break;
        case M_HELP:
            set_help_mode();
            mode = M_HELP;
            break;
    }
}

// defined in globals.h
void set_last_mode() {
    if (mode == last_mode) return;

    if (last_mode == M_LIVE || last_mode == M_EDIT || last_mode == M_PATTERN)
        set_mode(last_mode);
    else
        set_mode(M_LIVE);
}


////////////////////////////////////////////////////////////////////////////////
// key handling

void process_keypress(uint8_t key, uint8_t mod_key, bool is_held_key) {
    // first try global keys
    if (process_global_keys(key, mod_key, is_held_key)) return;

    switch (mode) {
        case M_EDIT: process_edit_keys(key, mod_key, is_held_key); break;
        case M_LIVE: process_live_keys(key, mod_key, is_held_key); break;
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
        if (!is_held_key) {
            clear_delays(&scene_state);
            for (int i = 0; i < 4; i++) { aout[i].step = 1; }
        }
        return true;
    }
    // <alt>-?: help text, or return to last mode
    else if (match_shift_alt(m, k, HID_SLASH)) {
        if (mode == M_HELP)
            set_last_mode();
        else {
            set_mode(M_HELP);
        }
        return true;
    }
    // <F1> through <F8>: run corresponding script
    // <F9>: run metro script
    // <F10>: run init script
    else if (no_mod(m) && k >= HID_F1 && k <= HID_F10) {
        run_script(&scene_state, k - HID_F1);
        return true;
    }
    // alt-<F1> through alt-<F8>: edit corresponding script
    // alt-<F9>: edit metro script
    // alt-<F10>: edit init script
    else if (mod_only_alt(m) && k >= HID_F1 && k <= HID_F10) {
        set_edit_mode_script(k - HID_F1);
        set_mode(M_EDIT);
        return true;
    }
    // ctrl-<F1> through ctrl-<F8> mute triggers
    // ctrl-<F9> toggle metro
    else if (mod_only_ctrl(m) && k >= HID_F1 && k <= HID_F8) {
        bool muted = ss_get_mute(&scene_state, (k - HID_F1));
        ss_set_mute(&scene_state, (k - HID_F1), !muted);
        screen_mutes_updated();
        return true;
    }
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


////////////////////////////////////////////////////////////////////////////////
// teletype_io.h

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
        timer_set(&metroTimer, metro_time);
    }

    if (metro_timer_enabled && ss_get_script_len(&scene_state, METRO_SCRIPT))
        set_metro_icon(true);
    else
        set_metro_icon(false);
}

void tele_metro_reset() {
    if (metro_timer_enabled) { timer_reset(&metroTimer); }
}

void tele_tr(uint8_t i, int16_t v) {
    if (v)
        gpio_set_pin_high(B08 + i);
    else
        gpio_set_pin_low(B08 + i);
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

    timer_manual(&adcTimer);
}

void tele_cv_slew(uint8_t i, int16_t v) {
    aout[i].slew = v / RATE_CV;
    if (aout[i].slew == 0) aout[i].slew = 1;
}

void tele_cv_off(uint8_t i, int16_t v) {
    aout[i].off = v;
}

void tele_ii_tx(uint8_t addr, uint8_t* data, uint8_t l) {
    i2c_master_tx(addr, data, l);
}

void tele_ii_rx(uint8_t addr, uint8_t* data, uint8_t l) {
    i2c_master_rx(addr, data, l);
}

void tele_scene(uint8_t i) {
    preset_select = i;
    flash_read(i, &scene_state, &scene_text);
}

void tele_kill() {
    for (int i = 0; i < 4; i++) {
        aout[i].step = 1;
        tele_tr(i, 0);
    }
}


bool tele_get_input_state(uint8_t n) {
    return gpio_get_pin_value(A00 + n) > 0;
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
    init_oled();
    init_i2c_master();

    print_dbg("\r\n\n// teletype! //////////////////////////////// ");

    ss_init(&scene_state);

    // prepare flash (if needed)
    flash_prepare();

    // load preset from flash
    preset_select = flash_last_saved_scene();
    ss_set_scene(&scene_state, preset_select);
    flash_read(preset_select, &scene_state, &scene_text);

    // screen init
    render_init();

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

    // manually call tele_metro_updated to sync metro to scene_state
    metro_timer_enabled = false;
    tele_metro_updated();

    clear_delays(&scene_state);

    aout[0].slew = 1;
    aout[1].slew = 1;
    aout[2].slew = 1;
    aout[3].slew = 1;

    init_live_mode();
    set_mode(M_LIVE);

    // wait 50ms before running the init script to allow for any i2c devices to
    // fully initalise
    delay_ms(50);

    run_script(&scene_state, INIT_SCRIPT);
    scene_state.initializing = false;

    while (true) { check_events(); }
}
