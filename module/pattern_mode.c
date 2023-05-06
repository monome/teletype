#include "pattern_mode.h"

// this
#include "globals.h"
#include "keyboard_helper.h"

// tables
#include "table.h"

// teletype
#include "teletype.h"
#include "teletype_io.h"

// libavr32
#include "font.h"
#include "region.h"
#include "util.h"

// asf
#include "conf_usb_host.h"  // needed in order to include "usb_protocol_hid.h"
#include "usb_protocol_hid.h"

static int16_t value_copy_buffer;
static uint8_t pattern;  // which pattern are we editting
static uint8_t base;     // base + offset determine what we are editting
static uint8_t offset;

static bool dirty;
static bool editing_number;
static int32_t edit_buffer;
static bool edit_negative;

// teletype_io.h
void tele_pattern_updated() {
    dirty = true;
}

void set_pattern_mode() {
    dirty = true;
    editing_number = false;
    edit_negative = false;
    edit_buffer = 0;
}

uint8_t get_pattern_offset() {
    return offset;
}

void set_pattern_offset(uint8_t o) {
    base = 0;
    offset = o;
    dirty = true;
}

void set_pattern_selected_value(uint8_t p, uint8_t offset) {
    pattern = p;
    base = offset;
    dirty = true;
}

void pattern_up() {
    editing_number = false;
    if (base)
        base--;
    else if (offset)
        offset--;
    dirty = true;
}

void pattern_down() {
    editing_number = false;
    base++;
    if (base == 8) {
        base = 7;
        if (offset < 56) { offset++; }
    }
    dirty = true;
}

static int16_t transpose_n_value(int16_t value, int8_t interval) {
    uint8_t last_note = 127;
    if (interval > last_note) { interval = last_note; }
    else if (interval < -last_note) { interval = -last_note; }
    if (value > table_n[last_note]) {
        uint8_t idx = last_note;
        if (interval < 0) idx++;
        return table_n[(idx + interval) % (last_note + 1)];
    }
    int16_t new_value = 0;
    for (int i = 0; i <= last_note; i++) {
        if (table_n[i] >= value) {
            int8_t j = i + interval;
            if (table_n[i] > value && interval > 0)
                j--;  // quantize to lower note
            if (j > last_note) { j = j % last_note + 1; }
            else if (j < 0) { j = j + last_note + 1; }
            new_value = table_n[j];
            break;
        }
    }
    return new_value;
}

void note_nudge(int8_t semitones) {
    if (editing_number) {
        edit_buffer = transpose_n_value(edit_buffer, semitones);
    }
    else {
        int16_t pattern_val =
            ss_get_pattern_val(&scene_state, pattern, base + offset);
        int16_t new_val = transpose_n_value(pattern_val, semitones);
        ss_set_pattern_val(&scene_state, pattern, base + offset, new_val);
    }
    dirty = true;
}

void process_pattern_keys(uint8_t k, uint8_t m, bool is_held_key) {
    // <down>: move down
    if (match_no_mod(m, k, HID_DOWN)) { pattern_down(); }
    // alt-<down>: move a page down
    else if (match_alt(m, k, HID_DOWN)) {
        editing_number = false;
        if (offset < 48)
            offset += 8;
        else {
            offset = 56;
            base = 7;
        }
        dirty = true;
    }
    // <up>: move up
    else if (match_no_mod(m, k, HID_UP)) { pattern_up(); }
    // alt-<up>: move a page up
    else if (match_alt(m, k, HID_UP)) {
        editing_number = false;
        if (offset > 8) { offset -= 8; }
        else {
            offset = 0;
            base = 0;
        }
        dirty = true;
    }
    // <left>: move left
    else if (match_no_mod(m, k, HID_LEFT)) {
        editing_number = false;
        if (pattern > 0) pattern--;
        dirty = true;
    }
    // alt-<left>: move to the very left
    else if (match_alt(m, k, HID_LEFT)) {
        editing_number = false;
        base = 0;
        offset = 0;
        dirty = true;
    }
    // <right>: move right
    else if (match_no_mod(m, k, HID_RIGHT)) {
        editing_number = false;
        if (pattern < 3) pattern++;
        dirty = true;
    }
    // alt-<right>: move to the very right
    else if (match_alt(m, k, HID_RIGHT)) {
        editing_number = false;
        base = 7;
        offset = 56;
        dirty = true;
    }
    // [: decrement by 1
    else if (match_no_mod(m, k, HID_OPEN_BRACKET)) {
        if (editing_number) {
            if (edit_buffer == INT16_MIN)
                edit_buffer = INT16_MAX;
            else
                edit_buffer -= 1;
            dirty = true;
        }
        else {
            int16_t v =
                ss_get_pattern_val(&scene_state, pattern, base + offset);
            if (v == INT16_MIN)
                ss_set_pattern_val(&scene_state, pattern, base + offset,
                                   INT16_MAX);
            else
                ss_set_pattern_val(&scene_state, pattern, base + offset, v - 1);
            dirty = true;
        }
    }
    // ]: increment by 1
    else if (match_no_mod(m, k, HID_CLOSE_BRACKET)) {
        if (editing_number) {
            if (edit_buffer == INT16_MAX)
                edit_buffer = INT16_MIN;
            else
                edit_buffer += 1;
            dirty = true;
        }
        else {
            int16_t v =
                ss_get_pattern_val(&scene_state, pattern, base + offset);
            if (v == INT16_MAX)
                ss_set_pattern_val(&scene_state, pattern, base + offset,
                                   INT16_MIN);
            else
                ss_set_pattern_val(&scene_state, pattern, base + offset, v + 1);
            dirty = true;
        }
    }
    // alt-[: decrement by 1 semitone
    else if (match_alt(m, k, HID_OPEN_BRACKET)) { note_nudge(-1); }
    // alt-]: increment by 1 semitone
    else if (match_alt(m, k, HID_CLOSE_BRACKET)) { note_nudge(1); }
    // ctrl-[: decrement by a fifth (7 semitones)
    else if (match_ctrl(m, k, HID_OPEN_BRACKET)) { note_nudge(-7); }
    // ctrl-]: increment by a fifth (7 semitones)
    else if (match_ctrl(m, k, HID_CLOSE_BRACKET)) { note_nudge(7); }
    // sh-[: decrement by 1 octave
    else if (match_shift(m, k, HID_OPEN_BRACKET)) { note_nudge(-12); }
    // sh-]: increment by 1 octave
    else if (match_shift(m, k, HID_CLOSE_BRACKET)) { note_nudge(12); }
    // alt-<0-9>: transpose up by numeric semitones
    else if (mod_only_alt(m) && k >= HID_1 && k <= HID_0) {
        uint8_t n = (k - HID_1 + 1);  // convert HID numbers to decimal,
                                      // leave 0 = 10 semitones
        if (n == 1) n = 11;  // 1 = 11 semitones since we already have alt-[ ]
        note_nudge(n);
    }
    // sh-alt-<0-9>: transpose down by numeric semitones
    else if (mod_only_shift_alt(m) && k >= HID_1 && k <= HID_0) {
        uint8_t n = (k - HID_1 + 1);  // convert HID numbers to decimal,
                                      // leave 0 = 10 semitones
        if (n == 1) n = 11;  // 1 = 11 semitones since we already have alt-[ ]
        note_nudge(-n);
    }
    // <backspace>: delete a digit
    else if (match_no_mod(m, k, HID_BACKSPACE)) {
        if (editing_number)
            edit_buffer /= 10;
        else {
            editing_number = true;
            edit_buffer =
                ss_get_pattern_val(&scene_state, pattern, base + offset) / 10;
        }
        dirty = true;
    }
    // shift-<backspace>: delete an entry, shift numbers up
    else if (match_shift(m, k, HID_BACKSPACE)) {
        editing_number = false;
        for (size_t i = base + offset; i < 63; i++) {
            int16_t v = ss_get_pattern_val(&scene_state, pattern, i + 1);
            ss_set_pattern_val(&scene_state, pattern, i, v);
        }

        uint16_t l = ss_get_pattern_len(&scene_state, pattern);
        if (l > base + offset) ss_set_pattern_len(&scene_state, pattern, l - 1);
        dirty = true;
    }
    // <enter>: commit edit, extend pattern length
    else if (match_no_mod(m, k, HID_ENTER)) {
        // commit an edit if active
        if (editing_number) {
            ss_set_pattern_val(&scene_state, pattern, base + offset,
                               edit_buffer);
            editing_number = false;
            edit_negative = false;
        }
        uint16_t l = ss_get_pattern_len(&scene_state, pattern);
        if (base + offset == l && l < 64)
            ss_set_pattern_len(&scene_state, pattern, l + 1);
        dirty = true;
    }
    // shift-<enter>: duplicate entry and shift downwards (increase length only
    // if on the entry immediately after the current length)
    else if (match_shift(m, k, HID_ENTER)) {
        // commit an edit before duplication
        if (editing_number) {
            ss_set_pattern_val(&scene_state, pattern, base + offset,
                               edit_buffer);
            editing_number = false;
            edit_negative = false;
        }
        for (int i = 63; i > base + offset; i--) {
            int16_t v = ss_get_pattern_val(&scene_state, pattern, i - 1);
            ss_set_pattern_val(&scene_state, pattern, i, v);
        }
        uint16_t l = ss_get_pattern_len(&scene_state, pattern);
        if (base + offset == l && l < 64) {
            ss_set_pattern_len(&scene_state, pattern, l + 1);
        }
        dirty = true;
    }
    // alt-x: cut value (n.b. ctrl-x not supported)
    else if (match_alt(m, k, HID_X)) {
        editing_number = false;
        value_copy_buffer =
            ss_get_pattern_val(&scene_state, pattern, base + offset);
        for (int i = base + offset; i < 63; i++) {
            int16_t v = ss_get_pattern_val(&scene_state, pattern, i + 1);
            ss_set_pattern_val(&scene_state, pattern, i, v);
        }

        uint16_t l = ss_get_pattern_len(&scene_state, pattern);
        if (l > base + offset) {
            ss_set_pattern_len(&scene_state, pattern, l - 1);
        }
        dirty = true;
    }
    // alt-c: copy value (n.b. ctrl-c not supported)
    else if (match_alt(m, k, HID_C)) {
        if (editing_number)
            value_copy_buffer = edit_buffer;
        else
            value_copy_buffer =
                ss_get_pattern_val(&scene_state, pattern, base + offset);
    }
    // alt-v: paste value (n.b. ctrl-v not supported)
    else if (match_alt(m, k, HID_V)) {
        editing_number = false;
        ss_set_pattern_val(&scene_state, pattern, base + offset,
                           value_copy_buffer);
        dirty = true;
    }
    // shift-alt-v: insert value
    else if (match_shift_alt(m, k, HID_V)) {
        editing_number = false;
        for (int i = 63; i > base + offset; i--) {
            int16_t v = ss_get_pattern_val(&scene_state, pattern, i - 1);
            ss_set_pattern_val(&scene_state, pattern, i, v);
        }
        uint16_t l = ss_get_pattern_len(&scene_state, pattern);
        if (l >= base + offset && l < 63) {
            ss_set_pattern_len(&scene_state, pattern, l + 1);
        }
        ss_set_pattern_val(&scene_state, pattern, base + offset,
                           value_copy_buffer);
        dirty = true;
    }
    // shift-l: set length to current position
    else if (match_shift(m, k, HID_L)) {
        editing_number = false;
        ss_set_pattern_len(&scene_state, pattern, base + offset + 1);
        dirty = true;
    }
    // alt-l: go to current length entry
    else if (match_alt(m, k, HID_L)) {
        editing_number = false;
        uint16_t l = ss_get_pattern_len(&scene_state, pattern);
        if (l) {
            offset = ((l - 1) >> 3) << 3;
            base = (l - 1) & 0x7;
            int8_t delta = base - 3;
            if ((offset + delta > 0) && (offset + delta < 56)) {
                offset += delta;
                base = 3;
            }
        }
        else {
            offset = 0;
            base = 0;
        }
        dirty = true;
    }
    // shift-s: set start to current position
    else if (match_shift(m, k, HID_S)) {
        editing_number = false;
        ss_set_pattern_start(&scene_state, pattern, offset + base);
        dirty = true;
    }
    // alt-s: go to start entry
    else if (match_alt(m, k, HID_S)) {
        editing_number = false;
        int16_t start = ss_get_pattern_start(&scene_state, pattern);
        if (start) {
            offset = (start >> 3) << 3;
            base = start & 0x7;
            int8_t delta = base - 3;
            if ((offset + delta > 0) && (offset + delta < 56)) {
                offset += delta;
                base = 3;
            }
        }
        else {
            offset = 0;
            base = 0;
        }
        dirty = true;
    }
    // shift-e: set end to current position
    else if (match_shift(m, k, HID_E)) {
        editing_number = false;
        ss_set_pattern_end(&scene_state, pattern, offset + base);
        dirty = true;
    }
    // alt-e: go to end entry
    else if (match_alt(m, k, HID_E)) {
        editing_number = false;
        int16_t end = ss_get_pattern_end(&scene_state, pattern);
        if (end) {
            offset = (end >> 3) << 3;
            base = end & 0x7;
            int8_t delta = base - 3;
            if ((offset + delta > 0) && (offset + delta < 56)) {
                offset += delta;
                base = 3;
            }
        }
        else {
            offset = 0;
            base = 0;
        }
        dirty = true;
    }
    // -: negate value
    else if (match_no_mod(m, k, HID_UNDERSCORE)) {
        int16_t v = ss_get_pattern_val(&scene_state, pattern, base + offset);
        if (v == 0 && !editing_number) {
            editing_number = true;
            edit_buffer = 0;
        }
        if (editing_number) {
            if (edit_buffer == 0)
                edit_negative = !edit_negative;
            else
                edit_buffer *= -1;
        }
        else { ss_set_pattern_val(&scene_state, pattern, base + offset, -v); }
        dirty = true;
    }
    // <space>: toggle non-zero to zero, and zero to 1
    else if (match_no_mod(m, k, HID_SPACEBAR)) {
        editing_number = false;
        if (ss_get_pattern_val(&scene_state, pattern, base + offset))
            ss_set_pattern_val(&scene_state, pattern, base + offset, 0);
        else
            ss_set_pattern_val(&scene_state, pattern, base + offset, 1);
        dirty = true;
    }
    else if (match_shift(m, k, HID_2)) {
        turtle_set_shown(&scene_state.turtle,
                         !turtle_get_shown(&scene_state.turtle));
        dirty = true;
    }
    // 0-9: numeric entry
    else if (no_mod(m) && k >= HID_1 && k <= HID_0) {
        if (!editing_number) {
            editing_number = true;
            edit_buffer = 0;
        }
        uint8_t n = (k - HID_1 + 1) % 10;  // convert HID numbers to decimal,
                                           // taking care of HID_0
        uint32_t old_buffer = edit_buffer;

        edit_buffer *= 10;
        if (edit_buffer == 0) { edit_buffer = n; }
        else if (edit_buffer < 0) {
            edit_buffer -= n;
            if (edit_buffer < INT16_MIN) edit_buffer = old_buffer;
        }
        else {
            edit_buffer += n;
            if (edit_buffer > INT16_MAX) edit_buffer = old_buffer;
        }
        if (edit_negative && edit_buffer != 0) {
            edit_negative = false;
            edit_buffer *= -1;
        }
        dirty = true;
    }
    if (!editing_number) edit_negative = false;
}

void process_pattern_knob(uint16_t knob, uint8_t m) {
    if (mod_only_ctrl_alt(m)) {
        ss_set_pattern_val(&scene_state, pattern, base + offset, knob >> 7);
        dirty = true;
    }
    else if (mod_only_shift_ctrl(m)) {
        ss_set_pattern_val(&scene_state, pattern, base + offset, knob >> 2);
        dirty = true;
    }
}

uint8_t screen_refresh_pattern() {
    if (!dirty) { return 0; }

    char s[32];
    for (uint8_t y = 0; y < 8; y++) {
        region_fill(&line[y], 0);
        itoa(y + offset, s, 10);
        font_string_region_clip_right(&line[y], s, 4, 0, 0x1, 0);

        for (uint8_t x = 0; x < 4; x++) {
            uint8_t a = 1;
            if (ss_get_pattern_len(&scene_state, x) > y + offset) a = 6;

            itoa(ss_get_pattern_val(&scene_state, x, y + offset), s, 10);
            font_string_region_clip_right(&line[y], s, (x + 1) * 30 + 4, 0, a,
                                          0);

            if (y + offset >= ss_get_pattern_start(&scene_state, x)) {
                if (y + offset <= ss_get_pattern_end(&scene_state, x)) {
                    for (uint8_t i = 0; i < 8; i += 2) {
                        line[y].data[i * 128 + (x + 1) * 30 + 6] = 1;
                    }
                }
            }

            if (y + offset == ss_get_pattern_idx(&scene_state, x)) {
                line[y].data[2 * 128 + (x + 1) * 30 + 6] = 11;
                line[y].data[3 * 128 + (x + 1) * 30 + 6] = 11;
                line[y].data[4 * 128 + (x + 1) * 30 + 6] = 11;
            }
        }
    }

    if (editing_number) {
        font_string_region_clip_right(&line[base], "      ",
                                      (pattern + 1) * 30 + 4, 0, 0xf, 0);
        if (edit_negative && edit_buffer == 0)
            font_string_region_clip_right(&line[base], "    -0",
                                          (pattern + 1) * 30 + 4, 0, 0xf, 0);
        else {
            itoa(edit_buffer, s, 10);
            font_string_region_clip_right(&line[base], s,
                                          (pattern + 1) * 30 + 4, 0, 0xf, 0);
        }
    }
    else {
        itoa(ss_get_pattern_val(&scene_state, pattern, base + offset), s, 10);
        font_string_region_clip_right(&line[base], s, (pattern + 1) * 30 + 4, 0,
                                      0xf, 0);
    }

    if (scene_state.turtle.shown) {
        int16_t y = turtle_get_y(&scene_state.turtle);
        int16_t x = turtle_get_x(&scene_state.turtle);
        if (y >= offset && y < offset + 8) {
            font_string_region_clip_right(&line[y - offset], "<",
                                          (x + 1) * 30 + 9, 0, 0xf, 0);
        }
    }


    for (uint8_t y = 0; y < 64; y += 2) {
        line[y >> 3].data[(y & 0x7) * 128 + 8] = 1;
    }

    for (uint8_t y = 0; y < 8; y++) {
        line[(offset + y) >> 3].data[((offset + y) & 0x7) * 128 + 8] = 6;
    }

    dirty = false;

    return 0xFF;
}
