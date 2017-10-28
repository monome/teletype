#include "preset_w_mode.h"

#include <string.h>

// this
#include "flash.h"
#include "globals.h"
#include "keyboard_helper.h"
#include "line_editor.h"

// libavr32
#include "font.h"
#include "kbd.h"
#include "region.h"
#include "util.h"

// asf
#include "conf_usb_host.h"  // needed in order to include "usb_protocol_hid.h"
#include "usb_protocol_hid.h"

static uint8_t edit_line;
static uint8_t edit_offset;
static line_editor_t le;

static const uint8_t D_INPUT = 1 << 0;
static const uint8_t D_LIST = 1 << 1;
static const uint8_t D_ALL = 0xFF;
static uint8_t dirty;

void set_preset_w_mode() {
    edit_line = 0;
    edit_offset = 0;
    line_editor_set(&le, scene_text[0]);
    dirty = D_ALL;
}

void process_preset_w_keys(uint8_t k, uint8_t m, bool is_held_key) {
    // <down> or C-n: line down
    if (match_no_mod(m, k, HID_DOWN) || match_ctrl(m, k, HID_N)) {
        if ((edit_offset + edit_line) < 31) {
            if (edit_line == 5)
                edit_offset++;
            else
                edit_line++;
            line_editor_set(&le, scene_text[edit_line + edit_offset]);
            dirty |= D_LIST;
            dirty |= D_INPUT;
        }
    }
    // <up> or C-p: line up
    else if (match_no_mod(m, k, HID_UP) || match_ctrl(m, k, HID_P)) {
        if (edit_line + edit_offset) {
            if (edit_line)
                edit_line--;
            else
                edit_offset--;
            line_editor_set(&le, scene_text[edit_line + edit_offset]);
            dirty |= D_LIST;
            dirty |= D_INPUT;
        }
    }
    // [: preset down
    else if (match_no_mod(m, k, HID_OPEN_BRACKET)) {
        if (preset_select) preset_select--;
        dirty |= D_LIST;
    }
    // ]: preset up
    else if (match_no_mod(m, k, HID_CLOSE_BRACKET)) {
        if (preset_select < SCENE_SLOTS - 1) preset_select++;
        dirty |= D_LIST;
    }
    // <enter>: enter text
    else if (match_no_mod(m, k, HID_ENTER)) {
        strcpy(scene_text[edit_line + edit_offset], line_editor_get(&le));
        if (edit_line + edit_offset < 31) {
            if (edit_line == 5)
                edit_offset++;
            else
                edit_line++;
        }
        line_editor_set(&le, scene_text[edit_line + edit_offset]);
        dirty |= D_LIST;
        dirty |= D_INPUT;
    }
    // shift-<enter>: insert text
    else if (match_shift(m, k, HID_ENTER)) {
        for (uint8_t i = SCENE_TEXT_LINES - 1; i > edit_line + edit_offset; i--)
            strcpy(scene_text[i], scene_text[i - 1]);  // overwrites final line!
        strcpy(scene_text[edit_line + edit_offset], line_editor_get(&le));
        dirty |= D_LIST;
    }
    // alt-<enter>: save preset
    else if (match_alt(m, k, HID_ENTER)) {
        if (!is_held_key) {
            strcpy(scene_text[edit_line + edit_offset], line_editor_get(&le));
            flash_write(preset_select, &scene_state, &scene_text);
            flash_update_last_saved_scene(preset_select);
            set_last_mode();
        }
    }
    else {  // pass to line editor
        bool processed = line_editor_process_keys(&le, k, m, is_held_key);
        if (processed) dirty |= D_INPUT;
    }
}


uint8_t screen_refresh_preset_w() {
    if (!(dirty & D_ALL)) { return 0; }

    uint8_t screen_dirty = 0;
    if (dirty & D_LIST) {
        char header[6] = ">>> ";
        itoa(preset_select, header + 4, 10);
        region_fill(&line[0], 1);
        font_string_region_clip_right(&line[0], header, 126, 0, 0xf, 1);
        font_string_region_clip(&line[0], "WRITE", 2, 0, 0xf, 1);

        for (uint8_t y = 1; y < 7; y++) {
            uint8_t a = edit_line == (y - 1);
            region_fill(&line[y], a);
            font_string_region_clip(&line[y], scene_text[edit_offset + y - 1],
                                    2, 0, 0xa + a * 5, a);
        }
        dirty &= ~D_LIST;
        screen_dirty |= 0x7F;
    }

    if (dirty & D_INPUT) {
        line_editor_draw(&le, '+', &line[7]);
        dirty &= ~D_INPUT;
        screen_dirty |= (1 << 7);
    }

    return screen_dirty;
}
