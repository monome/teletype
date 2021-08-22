#include "live_mode.h"

#include <string.h>

// this
#include "flash.h"
#include "gitversion.h"
#include "globals.h"
#include "keyboard_helper.h"
#include "line_editor.h"

// teletype
#include "helpers.h"
#include "teletype_io.h"

// libavr32
#include "font.h"
#include "region.h"
#include "util.h"

// asf
#include "conf_usb_host.h"  // needed in order to include "usb_protocol_hid.h"
#include "usb_protocol_hid.h"

#define MAX_HISTORY_SIZE 16
#define MAX_DASH_VARS 16

static uint8_t sub_mode;

static tele_command_t history[MAX_HISTORY_SIZE];  // newest entry in index 0
static int8_t history_line;                       // -1 for not selected
static int8_t history_top;                        // -1 when empty

static line_editor_t le;
static process_result_t output;
static error_t status;
static char error_msg[TELE_ERROR_MSG_LENGTH];
static bool show_welcome_message;

static uint8_t grid_x1 = 0, grid_y1 = 0, grid_x2 = 0, grid_y2 = 0;
static uint8_t grid_pressed = 0;
uint8_t grid_page = 0, grid_show_controls = 0;

static uint8_t dash_text_start, dash_text_end;
static int8_t dash_line_updated;
static int16_t dash_values[MAX_DASH_VARS];
static int8_t dash_values_start[MAX_DASH_VARS];
static uint8_t dash_values_line_format[MAX_DASH_VARS];
static uint8_t dash_screen;

static const uint8_t D_INPUT = 1 << 0;
static const uint8_t D_MESSAGE = 1 << 1;
static const uint8_t D_ACTIVITY = 1 << 2;
static const uint8_t D_VARS = 1 << 3;
static const uint8_t D_GRID = 1 << 4;
static const uint8_t D_DASH = 1 << 5;
static const uint8_t D_ALL = 0xFF;
static uint8_t dirty;

static const uint8_t A_METRO = 1 << 0;
static const uint8_t A_SLEW = 1 << 1;
static const uint8_t A_DELAY = 1 << 2;
static const uint8_t A_STACK = 1 << 3;
static const uint8_t A_MUTES = 1 << 4;
static uint8_t activity;
static int16_t vars_prev[8];
char var_names[] = { 'A', 0, 'X', 0, 'B', 0, 'Y', 0,
                     'C', 0, 'Z', 0, 'D', 0, 'T', 0 };

static void parse_dash_coordinates(void);
static void refresh_dashboard(uint8_t force_refresh);
static void refresh_activities(void);

// teletype_io.h
void tele_has_delays(bool has_delays) {
    if (has_delays)
        activity |= A_DELAY;
    else
        activity &= ~A_DELAY;
    dirty |= D_ACTIVITY;
}

void tele_has_stack(bool has_stack) {
    if (has_stack)
        activity |= A_STACK;
    else
        activity &= ~A_STACK;
    dirty |= D_ACTIVITY;
}

void tele_mute() {
    activity |= A_MUTES;
    dirty |= D_ACTIVITY;
}

void set_live_submode(u8 submode) {
    line_editor_set(&le, "");
    history_line = -1;
    sub_mode = submode;
    dirty = D_ALL;
}

void select_dash_screen(uint8_t screen) {
    dash_screen = screen;
    set_dash_updated();
    if (sub_mode != SUB_MODE_DASH) {
        sub_mode = SUB_MODE_DASH;
        line_editor_set(&le, "");
        history_line = -1;
        dirty = D_ALL;
    }
}

void print_dashboard_value(uint8_t index, int16_t value) {
    if (index >= MAX_DASH_VARS) return;
    if (dash_values[index] != value) {
        dash_values[index] = value;
        dash_line_updated |= 1 << (dash_values_line_format[index] & 0b111);
        dirty |= D_DASH;
    }
}

int16_t get_dashboard_value(uint8_t index) {
    if (index >= MAX_DASH_VARS) return 0;
    return dash_values[index];
}

// dashboard

static void parse_dash_coordinates(void) {
    uint8_t screen = 1;
    dash_text_start = 0;
    for (uint8_t line = 0; line < SCENE_TEXT_LINES; line++) {
        if (dash_screen < screen) break;
        if (scene_text[line][0] == '=' && scene_text[line][1] == '=' &&
            scene_text[line][2] == '=') {
            if (screen == dash_screen) {
                dash_text_start = line + 1;
                break;
            }
            screen++;
        }
    }

    dash_text_end = SCENE_TEXT_LINES - 1;
    for (uint8_t line = dash_text_start + 1; line < SCENE_TEXT_LINES; line++) {
        if (scene_text[line][0] == '=' && scene_text[line][1] == '=' &&
            scene_text[line][2] == '=') {
            dash_text_end = line - 1;
            break;
        }
    }

    for (uint8_t i = 0; i < MAX_DASH_VARS; i++) dash_values_start[i] = -1;

    u8 index, var_state, var_start = 0, var_format = 0;
    int8_t var_index = 0;
    for (uint8_t y = 0; y < 6; y++) {
        index = dash_text_start + y;
        if (index > dash_text_end) break;

        var_state = 0;
        for (uint8_t c = 0; c < SCENE_TEXT_CHARS - 1; c++) {
            if (scene_text[index][c] == 0) {
                if (var_state == 2) {
                    var_index--;
                    if (var_index >= 0 && var_index < MAX_DASH_VARS) {
                        dash_values_line_format[var_index] =
                            (var_format << 4) | y;
                        dash_values_start[var_index] =
                            font_string_position(scene_text[index], var_start) +
                            2;
                    }
                }
                break;
            }

            if (scene_text[index][c] == '%') {
                var_start = c;
                var_format = 0;  // decimal by default
                var_index = 0;
                var_state = 1;
                continue;
            }

            if (var_state == 1) {
                if (scene_text[index][c] == 'B') {
                    var_format = 1;  // binary
                }
                else if (scene_text[index][c] == 'R') {
                    var_format = 2;  // reversed binary
                }
                else if (scene_text[index][c] == 'X') {
                    var_format = 3;  // hex
                }
                else if (scene_text[index][c] >= '0' &&
                         scene_text[index][c] <= '1') {
                    var_index = scene_text[index][c] - '0';
                    var_state = 2;
                }
                else if (scene_text[index][c] >= '2' &&
                         scene_text[index][c] <= '9') {
                    var_index = scene_text[index][c] - '0' - 1;
                    if (var_index >= 0 && var_index < MAX_DASH_VARS) {
                        dash_values_line_format[var_index] =
                            (var_format << 4) | y;
                        dash_values_start[var_index] =
                            font_string_position(scene_text[index], var_start) +
                            2;
                    }
                    var_state = 0;
                }
                else
                    var_state = 0;
                continue;
            }

            if (var_state == 2) {
                if (scene_text[index][c] >= '0' &&
                    scene_text[index][c] <= '9') {
                    var_index = var_index * 10 + (scene_text[index][c] - '0');
                }
                var_index--;
                if (var_index >= 0 && var_index < MAX_DASH_VARS) {
                    dash_values_line_format[var_index] = (var_format << 4) | y;
                    dash_values_start[var_index] =
                        font_string_position(scene_text[index], var_start) + 2;
                }
                var_state = 0;
            }
        }
    }
}

// set icons
void set_slew_icon(bool display) {
    if (display)
        activity |= A_SLEW;
    else
        activity &= ~A_SLEW;
    dirty |= D_ACTIVITY;
}

void set_metro_icon(bool display) {
    if (display)
        activity |= A_METRO;
    else
        activity &= ~A_METRO;
    dirty |= D_ACTIVITY;
}

void set_vars_updated() {
    dirty |= D_VARS;
}

void set_dash_updated() {
    parse_dash_coordinates();
    dash_line_updated = -1;
    dirty |= D_DASH;
}

void set_grid_updated() {
    dirty |= D_GRID;
}

void set_mutes_updated() {
    dirty |= D_ACTIVITY;
}

// main mode functions
void init_live_mode() {
    sub_mode = SUB_MODE_OFF;
    status = E_OK;
    show_welcome_message = true;
    history_top = -1;
    history_line = -1;
    dash_screen = 0;
    dash_text_start = 0;
    dash_line_updated = 0;
    for (uint8_t i = 0; i < MAX_DASH_VARS; i++) {
        dash_values_start[i] = -1;
        dash_values[i] = 0;
    }
    parse_dash_coordinates();
    dirty = D_ALL;
}

void set_live_mode() {
    line_editor_set(&le, "");
    history_line = -1;
    if (sub_mode == SUB_MODE_FULLGRID) sub_mode = SUB_MODE_GRID;
    dirty = D_ALL;
}

uint8_t get_live_sub_mode() {
    return sub_mode;
}

void history_next() {
    if (history_line > 0) {
        history_line--;
        line_editor_set_command(&le, &history[history_line]);
    }
    else {
        history_line = -1;
        line_editor_set(&le, "");
    }
    dirty |= D_INPUT;
}

void history_prev() {
    if (history_line < history_top) {
        history_line++;
        line_editor_set_command(&le, &history[history_line]);
        dirty |= D_INPUT;
    }
}

void execute_line() {
    dirty |= D_MESSAGE;  // something will definitely happen
    dirty |= D_INPUT;

    tele_command_t command;
    command.comment = false;

    status = parse(line_editor_get(&le), &command, error_msg);
    if (status != E_OK)
        return;  // quit, screen_refresh_live will display the error message

    status = validate(&scene_state, &command, error_msg);
    if (status != E_OK)
        return;  // quit, screen_refresh_live will display the error message

    if (command.length) {
        s16 found = -1;
        for (s16 i = history_top; i >= 0; i--)
            if (command.length == history[i].length &&
                memcmp(&(command.data), &(history[i].data),
                       command.length * sizeof(tele_data_t)) == 0) {
                found = i;
                break;
            }

        if (found == -1) {
            // increase history_size up to a maximum
            if (history_top < MAX_HISTORY_SIZE - 1) history_top++;
            found = history_top;
        }

        // shuffle the history up
        // should really use some sort of ring buffer
        for (size_t i = found; i > 0; i--) {
            memcpy(&history[i], &history[i - 1], sizeof(command));
        }
        memcpy(&history[0], &command, sizeof(command));

        ss_clear_script(&scene_state, TEMP_SCRIPT);
        ss_overwrite_script_command(&scene_state, TEMP_SCRIPT, 0, &command);
        exec_state_t es;
        es_init(&es);
        es_push(&es);
        es_variables(&es)->script_number = TEMP_SCRIPT;

        output = run_script_with_exec_state(&scene_state, &es, TEMP_SCRIPT);
    }

    history_line = -1;
    line_editor_set(&le, "");
}

static void emulate_grid_release(scene_state_t *ss) {
    grid_process_key(ss, grid_x1, grid_y1, 0, 1);
    if (grid_x1 != grid_x2 || grid_y1 != grid_y2)
        grid_process_key(ss, grid_x2, grid_y2, 0, 1);
    grid_pressed = 0;
}

void process_live_keys(uint8_t k, uint8_t m, bool is_held_key, bool is_release,
                       scene_state_t *ss) {
    if (is_release) {
        if (match_alt(m, k, HID_SPACEBAR) ||
            (sub_mode == SUB_MODE_FULLGRID && match_no_mod(m, k, HID_SPACEBAR)))
            emulate_grid_release(ss);
        return;
    }

    // <down> or C-n: history next
    if ((match_no_mod(m, k, HID_DOWN) || match_ctrl(m, k, HID_N)) &&
        sub_mode != SUB_MODE_FULLGRID) {
        history_next();
    }
    // <up> or C-p: history previous
    else if ((match_no_mod(m, k, HID_UP) || match_ctrl(m, k, HID_P)) &&
             sub_mode != SUB_MODE_FULLGRID) {
        history_prev();
    }
    // A-D: toggle dash view
    else if (match_shift(m, k, HID_D)) {
        if (sub_mode == SUB_MODE_DASH)
            sub_mode = SUB_MODE_OFF;
        else
            sub_mode = SUB_MODE_DASH;
        dirty = D_ALL;
    }
    // A-G: toggle grid view
    else if (match_alt(m, k, HID_G) ||
             (sub_mode == SUB_MODE_FULLGRID && match_no_mod(m, k, HID_G))) {
        if (sub_mode == SUB_MODE_GRID)
            sub_mode = SUB_MODE_FULLGRID;
        else if (sub_mode == SUB_MODE_FULLGRID)
            sub_mode = SUB_MODE_OFF;
        else
            sub_mode = SUB_MODE_GRID;
        dirty = D_ALL;
    }
    // A-<up>: move grid cursor
    else if (match_alt(m, k, HID_UP) ||
             (sub_mode == SUB_MODE_FULLGRID && match_no_mod(m, k, HID_UP))) {
        if (grid_pressed) emulate_grid_release(ss);
        grid_y1 = (grid_y1 + GRID_MAX_DIMENSION - 1) % GRID_MAX_DIMENSION;
        grid_x2 = grid_x1;
        grid_y2 = grid_y1;
        dirty |= D_GRID;
    }
    // A-<down>: move grid cursor
    else if (match_alt(m, k, HID_DOWN) ||
             (sub_mode == SUB_MODE_FULLGRID && match_no_mod(m, k, HID_DOWN))) {
        if (grid_pressed) emulate_grid_release(ss);
        grid_y1 = (grid_y1 + 1) % GRID_MAX_DIMENSION;
        grid_x2 = grid_x1;
        grid_y2 = grid_y1;
        dirty |= D_GRID;
    }
    // A-<left>: move grid cursor
    else if (match_alt(m, k, HID_LEFT) ||
             (sub_mode == SUB_MODE_FULLGRID && match_no_mod(m, k, HID_LEFT))) {
        if (grid_pressed) emulate_grid_release(ss);
        grid_x1 = (grid_x1 + GRID_MAX_DIMENSION - 1) % GRID_MAX_DIMENSION;
        grid_x2 = grid_x1;
        grid_y2 = grid_y1;
        dirty |= D_GRID;
    }
    // A-<right>: move grid cursor
    else if (match_alt(m, k, HID_RIGHT) ||
             (sub_mode == SUB_MODE_FULLGRID && match_no_mod(m, k, HID_RIGHT))) {
        if (grid_pressed) emulate_grid_release(ss);
        grid_x1 = (grid_x1 + 1) % GRID_MAX_DIMENSION;
        grid_x2 = grid_x1;
        grid_y2 = grid_y1;
        dirty |= D_GRID;
    }
    // A-S-<up>: expand grid area up
    else if (match_shift_alt(m, k, HID_UP) ||
             (sub_mode == SUB_MODE_FULLGRID && match_shift(m, k, HID_UP))) {
        if (grid_y2 > 0) {
            if (grid_pressed) emulate_grid_release(ss);
            grid_y2--;
            dirty |= D_GRID;
        }
    }
    // A-S-<down>: expand grid area down
    else if (match_shift_alt(m, k, HID_DOWN) ||
             (sub_mode == SUB_MODE_FULLGRID && match_shift(m, k, HID_DOWN))) {
        if (grid_y2 < GRID_MAX_DIMENSION - 1) {
            if (grid_pressed) emulate_grid_release(ss);
            grid_y2++;
            dirty |= D_GRID;
        }
    }
    // A-S-<left>: expand grid area left
    else if (match_shift_alt(m, k, HID_LEFT) ||
             (sub_mode == SUB_MODE_FULLGRID && match_shift(m, k, HID_LEFT))) {
        if (grid_x2 > 0) {
            if (grid_pressed) emulate_grid_release(ss);
            grid_x2--;
            dirty |= D_GRID;
        }
    }
    // A-S-<right>: expand grid area right
    else if (match_shift_alt(m, k, HID_RIGHT) ||
             (sub_mode == SUB_MODE_FULLGRID && match_shift(m, k, HID_RIGHT))) {
        if (grid_x2 < GRID_MAX_DIMENSION - 1) {
            if (grid_pressed) emulate_grid_release(ss);
            grid_x2++;
            dirty |= D_GRID;
        }
    }
    // A-<space>: emulate grid press
    else if (!is_held_key && (match_alt(m, k, HID_SPACEBAR) ||
                              (sub_mode == SUB_MODE_FULLGRID &&
                               match_no_mod(m, k, HID_SPACEBAR)))) {
        grid_process_key(ss, grid_x1, grid_y1, 1, 1);
        if (grid_x1 != grid_x2 || grid_y1 != grid_y2)
            grid_process_key(ss, grid_x2, grid_y2, 1, 1);
        grid_pressed = 1;
        dirty |= D_GRID;
    }
    // A-<PrtSc>: insert coordinates / size
    else if (!is_held_key && match_alt(m, k, HID_PRINTSCREEN) &&
             sub_mode == SUB_MODE_GRID) {
        u8 area_x, area_y, area_w, area_h;
        if (grid_x1 < grid_x2) {
            area_x = grid_x1;
            area_w = grid_x2 + 1 - grid_x1;
        }
        else {
            area_x = grid_x2;
            area_w = grid_x1 + 1 - grid_x2;
        }
        if (grid_y1 < grid_y2) {
            area_y = grid_y1;
            area_h = grid_y2 + 1 - grid_y1;
        }
        else {
            area_y = grid_y2;
            area_h = grid_y1 + 1 - grid_y2;
        }
        if (area_x > 9) {
            line_editor_process_keys(&le, HID_1, HID_MODIFIER_NONE, false);
            area_x -= 10;
        }
        line_editor_process_keys(&le, area_x ? HID_1 + area_x - 1 : HID_0,
                                 HID_MODIFIER_NONE, false);
        line_editor_process_keys(&le, HID_SPACEBAR, HID_MODIFIER_NONE, false);
        if (area_y > 9) {
            line_editor_process_keys(&le, HID_1, HID_MODIFIER_NONE, false);
            area_y -= 10;
        }
        line_editor_process_keys(&le, area_y ? HID_1 + area_y - 1 : HID_0,
                                 HID_MODIFIER_NONE, false);
        line_editor_process_keys(&le, HID_SPACEBAR, HID_MODIFIER_NONE, false);
        if (area_w > 9) {
            line_editor_process_keys(&le, HID_1, HID_MODIFIER_NONE, false);
            area_w -= 10;
        }
        line_editor_process_keys(&le, area_w ? HID_1 + area_w - 1 : HID_0,
                                 HID_MODIFIER_NONE, false);
        line_editor_process_keys(&le, HID_SPACEBAR, HID_MODIFIER_NONE, false);
        if (area_h > 9) {
            line_editor_process_keys(&le, HID_1, HID_MODIFIER_NONE, false);
            area_h -= 10;
        }
        line_editor_process_keys(&le, area_h ? HID_1 + area_h - 1 : HID_0,
                                 HID_MODIFIER_NONE, false);
        line_editor_process_keys(&le, HID_SPACEBAR, HID_MODIFIER_NONE, false);
        dirty |= D_INPUT;
    }
    // A-</>: toggle grid page
    else if (match_alt(m, k, HID_SLASH) ||
             (sub_mode == SUB_MODE_FULLGRID && match_no_mod(m, k, HID_SLASH))) {
        if (++grid_page > 1) grid_page = 0;
        dirty |= D_GRID;
    }
    // A-<\>: toggle control view
    else if (match_alt(m, k, HID_BACKSLASH) ||
             (sub_mode == SUB_MODE_FULLGRID &&
              match_no_mod(m, k, HID_BACKSLASH))) {
        grid_show_controls = !grid_show_controls;
        dirty |= D_GRID;
    }
    // <enter>: execute command
    else if (match_no_mod(m, k, HID_ENTER) && sub_mode != SUB_MODE_FULLGRID) {
        execute_line();
    }
    // [ or ]: switch to edit mode
    else if (match_no_mod(m, k, HID_OPEN_BRACKET) ||
             match_no_mod(m, k, HID_CLOSE_BRACKET)) {
        set_mode(M_EDIT);
    }
    // tilde: show the variables
    else if (match_no_mod(m, k, HID_TILDE)) {
        if (sub_mode == SUB_MODE_VARS) { sub_mode = SUB_MODE_OFF; }
        else {
            sub_mode = SUB_MODE_VARS;
        }
        dirty = D_ALL;
    }
    // pass the key though to the line editor
    else if (sub_mode != SUB_MODE_FULLGRID) {
        bool processed = line_editor_process_keys(&le, k, m, is_held_key);
        if (processed) dirty |= D_INPUT;
    }
    show_welcome_message = false;
}

// screen functions

void refresh_dashboard(uint8_t force_refresh) {
    u8 index;
    for (u8 y = 0; y < 6; y++) {
        if (force_refresh || (dash_line_updated & (1 < y))) {
            region_fill(&line[y], 0);
            index = dash_text_start + y;
            if (index <= dash_text_end) {
                font_string_region_clip(&line[y], scene_text[index], 2, 0,
                                        y ? 0x8 : 0xC, 0);
            }
        }
    }

    u8 line_no, s_start;
    int16_t pixels_to_clear;
    char s[18];
    for (u8 var = 0; var < MAX_DASH_VARS; var++) {
        if (dash_values_start[var] == -1) continue;

        line_no = dash_values_line_format[var] & 0b111;
        if (force_refresh || (dash_line_updated & (1 < line_no))) {
            switch (dash_values_line_format[var] >> 4) {
                case 0: {
                    itoa(dash_values[var], s, 10);
                    pixels_to_clear = 22;  // up to 5 digits x 3 + sign
                    s_start = 0;
                    break;
                }
                case 1: {
                    itoa_bin(dash_values[var], s);
                    pixels_to_clear = 63;
                    s_start = 1;
                    break;
                }
                case 2: {
                    itoa_rbin(dash_values[var], s);
                    pixels_to_clear = 63;
                    s_start = 1;
                    break;
                }
                case 3: {
                    itoa_hex(dash_values[var], s);
                    pixels_to_clear = 15;
                    s_start = 1;
                    break;
                }
            }

            if (pixels_to_clear > 128 - dash_values_start[var])
                pixels_to_clear = 128 - dash_values_start[var];

            for (u8 y = 0; y < 7; y++) {
                u8 *p = (line[line_no].data) + dash_values_start[var] + y * 128;
                for (u32 i = 0; i < pixels_to_clear; i++) *p++ = 0;
            }

            font_string_region_clip(&line[line_no], &s[s_start],
                                    dash_values_start[var], 0, 0xf, 0);
        }
    }

    if (force_refresh || (dash_line_updated & 1)) {
        // clear space for activity monitor
        for (u8 y = 0; y < 7; y++) {
            u8 *p = (line[0].data) + 85 + y * 128;
            for (u32 i = 0; i < 43; i++) *p++ = 0;
        }
    }

    dash_line_updated = 0;
}

void refresh_activities() {
    // slew icon
    uint8_t slew_fg = activity & A_SLEW ? 15 : 1;
    line[0].data[98 + 0 + 512] = slew_fg;
    line[0].data[98 + 1 + 384] = slew_fg;
    line[0].data[98 + 2 + 256] = slew_fg;
    line[0].data[98 + 3 + 128] = slew_fg;
    line[0].data[98 + 4 + 0] = slew_fg;

    // delay icon
    uint8_t delay_fg = activity & A_DELAY ? 15 : 1;
    line[0].data[106 + 0 + 0] = delay_fg;
    line[0].data[106 + 1 + 0] = delay_fg;
    line[0].data[106 + 2 + 0] = delay_fg;
    line[0].data[106 + 3 + 0] = delay_fg;
    line[0].data[106 + 4 + 0] = delay_fg;
    line[0].data[106 + 0 + 128] = delay_fg;
    line[0].data[106 + 0 + 256] = delay_fg;
    line[0].data[106 + 0 + 384] = delay_fg;
    line[0].data[106 + 0 + 512] = delay_fg;
    line[0].data[106 + 4 + 128] = delay_fg;
    line[0].data[106 + 4 + 256] = delay_fg;
    line[0].data[106 + 4 + 384] = delay_fg;
    line[0].data[106 + 4 + 512] = delay_fg;

    // queue icon
    uint8_t stack_fg = activity & A_STACK ? 15 : 1;
    line[0].data[114 + 0 + 0] = stack_fg;
    line[0].data[114 + 1 + 0] = stack_fg;
    line[0].data[114 + 2 + 0] = stack_fg;
    line[0].data[114 + 3 + 0] = stack_fg;
    line[0].data[114 + 4 + 0] = stack_fg;
    line[0].data[114 + 0 + 256] = stack_fg;
    line[0].data[114 + 1 + 256] = stack_fg;
    line[0].data[114 + 2 + 256] = stack_fg;
    line[0].data[114 + 3 + 256] = stack_fg;
    line[0].data[114 + 4 + 256] = stack_fg;
    line[0].data[114 + 0 + 512] = stack_fg;
    line[0].data[114 + 1 + 512] = stack_fg;
    line[0].data[114 + 2 + 512] = stack_fg;
    line[0].data[114 + 3 + 512] = stack_fg;
    line[0].data[114 + 4 + 512] = stack_fg;

    // metro icon
    uint8_t metro_fg = activity & A_METRO ? 15 : 1;
    line[0].data[122 + 0 + 0] = metro_fg;
    line[0].data[122 + 0 + 128] = metro_fg;
    line[0].data[122 + 0 + 256] = metro_fg;
    line[0].data[122 + 0 + 384] = metro_fg;
    line[0].data[122 + 0 + 512] = metro_fg;
    line[0].data[122 + 1 + 128] = metro_fg;
    line[0].data[122 + 2 + 256] = metro_fg;
    line[0].data[122 + 3 + 128] = metro_fg;
    line[0].data[122 + 4 + 0] = metro_fg;
    line[0].data[122 + 4 + 128] = metro_fg;
    line[0].data[122 + 4 + 256] = metro_fg;
    line[0].data[122 + 4 + 384] = metro_fg;
    line[0].data[122 + 4 + 512] = metro_fg;

    // mutes
    for (size_t i = 0; i < 8; i++) {
        // make it staggered to match how the device looks
        size_t stagger = i % 2 ? 384 : 128;
        uint8_t mute_fg = ss_get_mute(&scene_state, i) ? 15 : 1;
        uint8_t script_pol = ss_get_script_pol(&scene_state, i);
        if (script_pol & 1) { line[0].data[87 + i + stagger] = mute_fg; }
        if (script_pol & 2) { line[0].data[87 + i + stagger + 1] = mute_fg; }
    }

    line[0].dirty = 1;
}

uint8_t screen_refresh_live() {
    if (sub_mode == SUB_MODE_FULLGRID) {
        if (dirty & D_GRID || scene_state.grid.scr_dirty) {
            grid_screen_refresh(&scene_state, 1, grid_page, grid_show_controls,
                                grid_x1, grid_y1, grid_x2, grid_y2);
            dirty &= ~D_GRID;
            return 0b11111111;
        }

        return 0;
    }

    uint8_t screen_dirty = 0;

    // D_ALL has a meaning different than just combining all other flags:
    // with individual flags, submodes can decide what needs to be refreshed
    // D_ALL means render everything

    if (dirty & D_INPUT) {
        line_editor_draw(&le, '>', &line[7]);
        dirty &= ~D_INPUT;
        screen_dirty |= (1 << 7);
    }

    if (dirty & D_MESSAGE) {
        char s[36];
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
        else if (output.has_value) {
            itoa(output.value, s, 10);
            output.has_value = false;
        }
        else if (show_welcome_message) {
            strcpy(s, "TELETYPE ");
            strncat(s, git_version, 35 - strlen(s));
            show_welcome_message = false;
        }
        else {
            s[0] = 0;
        }

        region_fill(&line[6], 0);
        font_string_region_clip(&line[6], s, 0, 0, 0x4, 0);

        dirty &= ~D_MESSAGE;
        screen_dirty |= (1 << 6);
    }

    if (sub_mode == SUB_MODE_GRID) {
        if (dirty & D_GRID || scene_state.grid.scr_dirty) {
            grid_screen_refresh(&scene_state, 0, grid_page, grid_show_controls,
                                grid_x1, grid_y1, grid_x2, grid_y2);
            dirty &= ~D_GRID;
            screen_dirty |= 0b111111;
        }

        return screen_dirty;
    }

    if (sub_mode == SUB_MODE_VARS) {
        if (dirty & D_VARS) {
            int16_t *vp =
                &scene_state.variables
                     .a;  // 8 int16_t all in a row, point at the first one
                          // relies on variable ordering. see: src/state.h
            char s[8];

            dirty &= ~D_VARS;
            u8 force_refresh = dirty & D_ALL;

            if (force_refresh) {
                region_fill(&line[0], 0);
                region_fill(&line[1], 0);
                screen_dirty |= 0b11;
            }

            for (size_t i = 0; i < 8; i += 2)
                if (force_refresh || (vp[i] != vars_prev[i]) ||
                    (vp[i + 1] != vars_prev[i + 1])) {
                    region_fill(&line[i / 2 + 2], 0);
                    vars_prev[i] = vp[i];
                    vars_prev[i + 1] = vp[i + 1];
                    itoa(vp[i], s, 10);
                    font_string_region_clip_right(&line[i / 2 + 2], s, 11 * 4,
                                                  0, 0xf, 0);
                    font_string_region_clip_right(&line[i / 2 + 2],
                                                  var_names + (i * 2), 14 * 4,
                                                  0, 0x1, 0);
                    itoa(vp[i + 1], s, 10);
                    font_string_region_clip_right(&line[i / 2 + 2], s, 25 * 4,
                                                  0, 0xf, 0);
                    font_string_region_clip_right(&line[i / 2 + 2],
                                                  var_names + ((i + 1) * 2),
                                                  28 * 4, 0, 0x1, 0);
                    screen_dirty |= (1 << (i / 2 + 2));
                    for (int row = 1; row < 9; row += 2) {
                        line[i / 2 + 2].data[row * 128 + 12 * 4 - 1] = 0x1;
                        line[i / 2 + 2].data[row * 128 + 26 * 4 - 1] = 0x1;
                    }
                }
        }
    }

    else if (sub_mode == SUB_MODE_DASH) {
        if (dirty & D_DASH) {
            if (dash_line_updated & 1) {
                // dashboard overwrites the top line, refresh activity monitor
                dirty |= D_ACTIVITY;
            }
            refresh_dashboard(dirty & D_ALL);
            dirty &= ~D_DASH;
            screen_dirty |= 0x3F;
        }
    }

    else {
        if (dirty & D_ALL) {
            for (int i = 0; i < 6; i++) region_fill(&line[i], 0);
            dirty = D_ACTIVITY;
            screen_dirty |= 0x3F;
        }
    }

    if (dirty & D_ACTIVITY) {
        refresh_activities();
        screen_dirty |= 1;
    }

    dirty = 0;
    return screen_dirty;
}
