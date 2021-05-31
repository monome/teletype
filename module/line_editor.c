#include "line_editor.h"

#include <string.h>

// this
#include "globals.h"
#include "keyboard_helper.h"

// teletype
#include "teletype.h"

// libavr32
#include "font.h"
#include "kbd.h"
#include "region.h"

// asf
#include "conf_usb_host.h"  // needed in order to include "usb_protocol_hid.h"
#include "usb_protocol_hid.h"

void line_editor_set(line_editor_t *le, const char value[LINE_EDITOR_SIZE]) {
    size_t length = strlen(value);
    if (length < LINE_EDITOR_SIZE) {
        strcpy(le->buffer, value);
        le->cursor = length;
        le->length = length;
    }
    else {
        le->buffer[0] = 0;
        le->cursor = 0;
        le->length = 0;
    }
}

void line_editor_set_command(line_editor_t *le, const tele_command_t *command) {
    print_command(command, le->buffer);
    le->length = strlen(le->buffer);
    le->cursor = le->length;
}

char *line_editor_get(line_editor_t *le) {
    return le->buffer;
}

bool line_editor_process_keys(line_editor_t *le, uint8_t k, uint8_t m,
                              bool is_key_held) {
    // <left> or ctrl-b: move cursor left
    if (match_no_mod(m, k, HID_LEFT) || match_ctrl(m, k, HID_B)) {
        if (le->cursor) { le->cursor--; }
        return true;
    }
    // <right> or ctrl-f: move cursor right
    else if (match_no_mod(m, k, HID_RIGHT) || match_ctrl(m, k, HID_F)) {
        if (le->cursor < le->length) { le->cursor++; }
        return true;
    }
    // <home> or ctrl-a: move to beginning of line
    else if (match_no_mod(m, k, HID_HOME) || match_ctrl(m, k, HID_A)) {
        le->cursor = 0;
        return true;
    }
    // <end> or ctrl-e: move to end of line
    else if (match_no_mod(m, k, HID_END) || match_ctrl(m, k, HID_E)) {
        le->cursor = le->length;
        return true;
    }
    // ctrl-<left> or alt-b: move cursor to previous word
    else if (match_ctrl(m, k, HID_LEFT) || match_alt(m, k, HID_B)) {
        bool encountered_word = false;
        while (le->cursor) {
            if (le->buffer[le->cursor - 1] == ' ') {
                if (encountered_word) break;
            }
            else {
                encountered_word = true;
            }
            le->cursor--;
        }
        return true;
    }
    // ctrl-<right> or alt-f: move cursor to next word
    else if (match_ctrl(m, k, HID_RIGHT) || match_alt(m, k, HID_F)) {
        bool encountered_word = false;
        while (le->cursor < le->length) {
            if (le->buffer[le->cursor] == ' ') {
                if (encountered_word) break;
            }
            else {
                encountered_word = true;
            }
            le->cursor++;
        }
        return true;
    }
    // <backspace> or ctrl-h: backwards delete one character
    else if (match_no_mod(m, k, HID_BACKSPACE) || match_ctrl(m, k, HID_H)) {
        if (le->cursor) {
            le->cursor--;
            for (size_t x = le->cursor; x < LINE_EDITOR_SIZE - 1; x++) {
                le->buffer[x] = le->buffer[x + 1];
            }
            le->length--;
        }
        return true;
    }
    // <delete> or ctrl-d: forwards delete one character
    else if (match_no_mod(m, k, HID_DELETE) || match_ctrl(m, k, HID_D)) {
        if (le->cursor < le->length) {
            for (size_t x = le->cursor; x < LINE_EDITOR_SIZE - 1; x++) {
                le->buffer[x] = le->buffer[x + 1];
            }
            le->length--;
        }
        return true;
    }
    // shift-<backspace> or ctrl-u: delete from cursor to beginning
    else if (match_shift(m, k, HID_BACKSPACE) || match_ctrl(m, k, HID_U)) {
        // strings will overlap, so we need to use an intermediate buffer
        char temp[LINE_EDITOR_SIZE];
        strcpy(temp, &le->buffer[le->cursor]);
        line_editor_set(le, temp);
        le->cursor = 0;
        return true;
    }
    // shift-<delete> or ctrl-k: delete from cursor to end
    else if (match_shift(m, k, HID_DELETE) || match_ctrl(m, k, HID_K)) {
        le->buffer[le->cursor] = 0;
        le->length = le->cursor;
        return true;
    }
    // alt-<backspace> or ctrl-w: delete from cursor to beginning of word
    else if (match_alt(m, k, HID_BACKSPACE) || match_ctrl(m, k, HID_W)) {
        bool encountered_word = false;
        while (le->cursor) {
            // Stop after reaching space on the other side of the word.
            if (le->buffer[le->cursor - 1] == ' ') {
                if (encountered_word) break;
            }
            else {
                encountered_word = true;
            }
            // Delete preceeding character by shifting buffer down.
            le->cursor--;
            for (size_t x = le->cursor; x < LINE_EDITOR_SIZE - 1; x++) {
                le->buffer[x] = le->buffer[x + 1];
            }
            le->length--;
        }
        return true;
    }
    // alt-d: delete from cursor to end of word.
    else if (match_alt(m, k, HID_D)) {
        bool encountered_word = false;
        while (le->cursor < le->length) {
            // Stop after reaching space on the other side of the word.
            if (le->buffer[le->cursor] == ' ') {
                if (encountered_word) break;
            }
            else {
                encountered_word = true;
            }
            // Delete this character by shifting the buffer down.
            for (size_t x = le->cursor; x < LINE_EDITOR_SIZE - 1; ++x) {
                le->buffer[x] = le->buffer[x + 1];
            }
            le->length--;
        }
        return true;
    }
    // ctrl-x or alt-x: cut
    else if (match_ctrl(m, k, HID_X) || match_alt(m, k, HID_X)) {
        strcpy(copy_buffer[0], le->buffer);
        copy_buffer_len = 1;
        line_editor_set(le, "");
        return true;
    }
    // ctrl-c or alt-c: copy
    else if (match_ctrl(m, k, HID_C) || match_alt(m, k, HID_C)) {
        strcpy(copy_buffer[0], le->buffer);
        copy_buffer_len = 1;
        return true;
    }
    // ctrl-v or alt-v: paste
    else if (match_ctrl(m, k, HID_V) || match_alt(m, k, HID_V)) {
        line_editor_set(le, copy_buffer[0]);
        return true;
    }
    else if (no_mod(m) || mod_only_shift(m)) {
        if (le->length < LINE_EDITOR_SIZE - 2) {  // room for another char & 0
            uint8_t n = hid_to_ascii(k, m);
            if (n) {
                for (size_t x = LINE_EDITOR_SIZE - 1; x > le->cursor; x--) {
                    le->buffer[x] = le->buffer[x - 1];  // shuffle forwards
                }

                le->buffer[le->cursor] = n;
                le->cursor++;
                le->length++;
                return true;
            }
        }
    }

    // did't process a key
    return false;
}

void line_editor_draw(line_editor_t *le, char prefix, region *reg) {
    // LINE_EDITOR_SIZE includes space for null, need to also include space for
    // the prefix, the space after the prefix and a space at the very end
    char s[LINE_EDITOR_SIZE + 3] = { prefix, ' ', 0 };
    strcat(s, le->buffer);
    strcat(s, " ");

    region_fill(reg, 0);
    font_string_region_clip_hid(reg, s, 0, 0, 0xf, 0, le->cursor + 2, 3);
}
