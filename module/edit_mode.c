#include "edit_mode.h"

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

static line_editor_t le;
static uint8_t line_no;
static uint8_t script;
static error_t status;
static char error_msg[TELE_ERROR_MSG_LENGTH];

static const uint8_t D_INPUT = 1 << 0;
static const uint8_t D_LIST = 1 << 1;
static const uint8_t D_MESSAGE = 1 << 2;
static const uint8_t D_ALL = 0xFF;
static uint8_t dirty;

void set_edit_mode() {
    status = E_OK;
    error_msg[0] = 0;
    line_no = 0;
    line_editor_set_command(
        &le, ss_get_script_command(&scene_state, script, line_no));
    dirty = D_ALL;
}

void set_edit_mode_script(uint8_t new_script) {
    script = new_script;
    if (script >= SCRIPT_COUNT) script = SCRIPT_COUNT - 1;
    dirty = D_ALL;
}

void process_edit_keys(uint8_t k, uint8_t m, bool is_held_key) {
    // <down> or C-n: line down
    if (match_no_mod(m, k, HID_DOWN) || match_ctrl(m, k, HID_N)) {
        if (line_no < (SCRIPT_MAX_COMMANDS - 1) &&
            line_no < ss_get_script_len(&scene_state, script)) {
            line_no++;
            line_editor_set_command(
                &le, ss_get_script_command(&scene_state, script, line_no));
            dirty |= D_LIST;
            dirty |= D_INPUT;
        }
    }
    // <up> or C-p: line up
    else if (match_no_mod(m, k, HID_UP) || match_ctrl(m, k, HID_P)) {
        if (line_no) {
            line_no--;
            line_editor_set_command(
                &le, ss_get_script_command(&scene_state, script, line_no));
            dirty |= D_LIST;
            dirty |= D_INPUT;
        }
    }
    // [: previous script
    else if (match_no_mod(m, k, HID_OPEN_BRACKET)) {
        status = E_OK;
        error_msg[0] = 0;
        if (script)
            script--;
        else
            script = SCRIPT_COUNT - 2;  // due to TEMP_SCRIPT
        if (line_no > ss_get_script_len(&scene_state, script))
            line_no = ss_get_script_len(&scene_state, script);
        line_editor_set_command(
            &le, ss_get_script_command(&scene_state, script, line_no));
        dirty |= D_LIST;
        dirty |= D_INPUT;
    }
    // ]: next script
    else if (match_no_mod(m, k, HID_CLOSE_BRACKET)) {
        status = E_OK;
        error_msg[0] = 0;
        script++;
        if (script >= SCRIPT_COUNT - 1) script = 0;  // due to TEMP_SCRIPT
        if (line_no > ss_get_script_len(&scene_state, script))
            line_no = ss_get_script_len(&scene_state, script);
        line_editor_set_command(
            &le, ss_get_script_command(&scene_state, script, line_no));
        dirty |= D_LIST;
        dirty |= D_INPUT;
    }
    // ctrl-x or alt-x: override line editors cut
    else if (match_ctrl(m, k, HID_X) || match_alt(m, k, HID_X)) {
        line_editor_set_copy_buffer(line_editor_get(&le));
        ss_delete_script_command(&scene_state, script, line_no);
        if (line_no > ss_get_script_len(&scene_state, script)) {
            line_no = ss_get_script_len(&scene_state, script);
        }
        line_editor_set_command(
            &le, ss_get_script_command(&scene_state, script, line_no));

        dirty |= D_LIST;
        dirty |= D_INPUT;
    }
    // <enter>: enter command
    else if (match_no_mod(m, k, HID_ENTER)) {
        dirty |= D_MESSAGE;  // something will happen

        tele_command_t command;
        status = parse(line_editor_get(&le), &command, error_msg);

        if (status != E_OK)
            return;  // quit, screen_refresh_edit will display the error message

        status = validate(&command, error_msg);
        if (status != E_OK)
            return;  // quit, screen_refresh_edit will display the error message

        if (command.length == 0) {  // blank line, delete the command
            ss_delete_script_command(&scene_state, script, line_no);
            if (line_no > ss_get_script_len(&scene_state, script)) {
                line_no = ss_get_script_len(&scene_state, script);
            }
        }
        else {
            ss_overwrite_script_command(&scene_state, script, line_no,
                                        &command);
            if (line_no < SCRIPT_MAX_COMMANDS - 1) { line_no++; }
        }
        line_editor_set_command(
            &le, ss_get_script_command(&scene_state, script, line_no));
        dirty |= D_LIST;
        dirty |= D_INPUT;
    }
    // shift-<enter>: insert command
    else if (match_shift(m, k, HID_ENTER)) {
        dirty |= D_MESSAGE;  // something will happen

        tele_command_t command;
        status = parse(line_editor_get(&le), &command, error_msg);

        if (status != E_OK)
            return;  // quit, screen_refresh_edit will display the error message

        status = validate(&command, error_msg);
        if (status != E_OK)
            return;  // quit, screen_refresh_edit will display the error message

        if (command.length > 0) {
            ss_insert_script_command(&scene_state, script, line_no, &command);
            if (line_no < (SCRIPT_MAX_COMMANDS - 1)) { line_no++; }
        }

        line_editor_set_command(
            &le, ss_get_script_command(&scene_state, script, line_no));
        dirty |= D_LIST;
        dirty |= D_INPUT;
    }
    // alt-slash comment toggle current line
    else if (match_alt(m, k, HID_SLASH)) {
        ss_toggle_script_comment(&scene_state, script, line_no);
        dirty |= D_LIST;
    }
    else {  // pass the key though to the line editor
        bool processed = line_editor_process_keys(&le, k, m, is_held_key);
        if (processed) dirty |= D_INPUT;
    }
}

void screen_mutes_updated() {
    dirty |= D_INPUT;
}

uint8_t screen_refresh_edit() {
    uint8_t screen_dirty = 0;

    if (dirty & D_INPUT) {
        char prefix = script + '1';
        if (script == METRO_SCRIPT)
            prefix = 'M';
        else if (script == INIT_SCRIPT)
            prefix = 'I';

        line_editor_draw(&le, prefix, &line[7]);
        // maybe find a better way than stomping it?
        if (ss_get_mute(&scene_state, script)) {
            char shaded[2] = { prefix, '\0' };
            font_string_region_clip(&line[7], shaded, 0, 0, 0x4, 0);
        }
        screen_dirty |= (1 << 7);
        dirty &= ~D_INPUT;
    }

    if (dirty & D_MESSAGE) {
        char s[32];
        if (status != E_OK) {
            strcpy(s, tele_error(status));
            if (error_msg[0]) {
                size_t len = strlen(s);
                strcat(s, ": ");
                strncat(s, error_msg, 32 - len - 3);
                error_msg[0] = 0;
            }
            status = E_OK;
        }
        else {
            s[0] = 0;
        }

        region_fill(&line[6], 0);
        font_string_region_clip(&line[6], s, 0, 0, 0x4, 0);

        screen_dirty |= (1 << 6);
        dirty &= ~D_MESSAGE;
    }

    if (dirty & D_LIST) {
        for (int i = 0; i < 6; i++) {
            uint8_t a = line_no == i;
            uint8_t fg =
                ss_get_script_comment(&scene_state, script, i) ? 0x7 : 0xf;
            region_fill(&line[i], a);
            if (ss_get_script_len(&scene_state, script) > i) {
                char s[32];
                print_command(ss_get_script_command(&scene_state, script, i),
                              s);
                region_string(&line[i], s, 2, 0, fg, a, 0);
            }
        }

        screen_dirty |= 0x3F;
        dirty &= ~D_LIST;
    }

    return screen_dirty;
}
