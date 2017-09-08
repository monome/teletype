#include "preset_r_mode.h"

// this
#include "flash.h"
#include "globals.h"
#include "keyboard_helper.h"

// libavr32
#include "font.h"
#include "region.h"
#include "util.h"

// asf
#include "conf_usb_host.h"  // needed in order to include "usb_protocol_hid.h"
#include "usb_protocol_hid.h"

static uint8_t offset;
static uint8_t knob_last;
static bool dirty;

static void do_preset_read(void);

void set_preset_r_mode(uint16_t knob) {
    knob_last = knob >> 7;
    offset = 0;
    dirty = true;
}

void process_preset_r_knob(uint16_t knob, uint8_t mod_key) {
    uint8_t knob_now = knob >> 7;
    if (knob_now != knob_last) {
        preset_select = knob_now;
        knob_last = knob_now;
        dirty = true;
    }
}

void process_preset_r_long_front() {
    do_preset_read();
}

void process_preset_r_keys(uint8_t k, uint8_t m, bool is_held_key) {
    // <down> or C-n: line down
    if (match_no_mod(m, k, HID_DOWN) || match_ctrl(m, k, HID_N)) {
        if (offset < SCENE_TEXT_LINES - 8) {
            offset++;
            dirty = true;
        }
    }
    // <up> or C-p: line up
    else if (match_no_mod(m, k, HID_UP) || match_ctrl(m, k, HID_P)) {
        if (offset) {
            offset--;
            dirty = true;
        }
    }
    // <left> or [: preset down
    else if (match_no_mod(m, k, HID_LEFT) ||
             match_no_mod(m, k, HID_OPEN_BRACKET)) {
        if (preset_select) preset_select--;
        dirty = true;
    }
    // <right> or ]: preset up
    else if (match_no_mod(m, k, HID_RIGHT) ||
             match_no_mod(m, k, HID_CLOSE_BRACKET)) {
        if (preset_select < SCENE_SLOTS - 1) preset_select++;
        dirty = true;
    }
    // <enter>: load preset
    else if (match_no_mod(m, k, HID_ENTER) && !is_held_key) {
        do_preset_read();
    }
}

bool screen_refresh_preset_r() {
    if (!dirty) { return false; }

    char s[32];
    itoa(preset_select, s, 10);
    region_fill(&line[0], 1);
    font_string_region_clip_right(&line[0], s, 126, 0, 0xf, 1);
    font_string_region_clip(&line[0], flash_scene_text(preset_select, 0), 2, 0,
                            0xf, 1);


    for (uint8_t y = 1; y < 8; y++) {
        region_fill(&line[y], 0);
        font_string_region_clip(&line[y],
                                flash_scene_text(preset_select, offset + y), 2,
                                0, 0xa, 0);
    }

    dirty = false;
    return true;
};

void do_preset_read() {
    flash_read(preset_select, &scene_state, &scene_text);
    flash_update_last_saved_scene(preset_select);
    ss_set_scene(&scene_state, preset_select);

    scene_state.initializing = true;
    run_script(&scene_state, INIT_SCRIPT);
    scene_state.initializing = false;

    set_last_mode();
}
