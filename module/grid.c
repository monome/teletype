#include "grid.h"
#include "edit_mode.h"
#include "flash.h"
#include "font.h"
#include "globals.h"
#include "live_mode.h"
#include "pattern_mode.h"
#include "preset_r_mode.h"
#include "state.h"
#include "teletype.h"
#include "teletype_io.h"
#include "timers.h"
#include "util.h"

#define GRID_MAX_KEY_PRESSED 10
#define GRID_KEY_HOLD_DELAY 700
#define GRID_KEY_REPEAT_RATE 40
#define GRID_KEY_REPEAT_RATE_CTL 120
#define GRID_ON_BRIGHTNESS 13
#define GRID_SCRIPT_TRIGGER 60

typedef enum {
    G_LIVE_V,
    G_LIVE_D,
    G_LIVE_G,
    G_LIVE_GF,
    G_EDIT,
    G_TRACKER,
    G_PRESET
} grid_control_mode_t;

// clang-format off

static const u8 glyph[16][6] = {
    {
    0b000000,
    0b000000,
    0b000000,
    0b000000,
    0b000000,
    0b000000
    },
    {
    0b000000,
    0b011110,
    0b010010,
    0b010010,
    0b011110,
    0b000000
    },
    {
    0b000000,
    0b011110,
    0b010010,
    0b010010,
    0b010010,
    0b010010
    },
    {
    0b010010,
    0b010010,
    0b010010,
    0b010010,
    0b010010,
    0b010010
    },
    {
    0b010010,
    0b010010,
    0b010010,
    0b010010,
    0b011110,
    0b000000
    },
    {
    0b000000,
    0b011111,
    0b010000,
    0b010000,
    0b011111,
    0b000000
    },
    {
    0b000000,
    0b111111,
    0b000000,
    0b000000,
    0b111111,
    0b000000
    },
    {
    0b000000,
    0b111110,
    0b000010,
    0b000010,
    0b111110,
    0b000000
    },
    {
    0b000000,
    0b011111,
    0b010000,
    0b010000,
    0b010000,
    0b010000
    },
    {
    0b000000,
    0b111111,
    0b000000,
    0b000000,
    0b000000,
    0b000000
    },
    {
    0b000000,
    0b111110,
    0b000010,
    0b000010,
    0b000010,
    0b000010
    },
    {
    0b010000,
    0b010000,
    0b010000,
    0b010000,
    0b010000,
    0b010000
    },
    {
    0b000010,
    0b000010,
    0b000010,
    0b000010,
    0b000010,
    0b000010
    },
    {
    0b010000,
    0b010000,
    0b010000,
    0b010000,
    0b011111,
    0b000000
    },
    {
    0b000000,
    0b000000,
    0b000000,
    0b000000,
    0b111111,
    0b000000
    },
    {
    0b000010,
    0b000010,
    0b000010,
    0b000010,
    0b111110,
    0b000000
    }
};

// clang-format on

typedef struct {
    u8 used;
    u8 key;
    u8 x;
    u8 y;
    scene_state_t *ss;
    softTimer_t timer;
} hold_repeat_info;

typedef struct {
    u8 on;
    scene_state_t *ss;
    softTimer_t timer;
} script_trigger_info;

static grid_control_mode_t tt_mode = G_LIVE_V, tt_last_mode = G_LIVE_V;
static u8 control_mode_on, tt_script, variable_edit, variable_changed;
static u8 preset_write, tracker_pressed, tracker_x, tracker_y;
static u8 tracker_changed, tracker_select, tracker_selected;
static u8 tracker_set_start, tracker_set_end;
static s16 tracker_last, variable_last;
static u16 size_x = 16, size_y = 8;
static u8 screen[GRID_MAX_DIMENSION][GRID_MAX_DIMENSION / 2];
static hold_repeat_info held_keys[GRID_MAX_KEY_PRESSED];
static u8 timers_uninitialized = 1;
static script_trigger_info script_triggers[11];

static void grid_control_refresh(scene_state_t *ss);
static u8 grid_control_process_key(scene_state_t *ss, u8 x, u8 y, u8 z,
                                   u8 from_held);
static void hold_repeat_timer_callback(void *o);
static void grid_process_key_hold_repeat(scene_state_t *ss, u8 x, u8 y);
static void grid_screen_refresh_ctrl(scene_state_t *ss, u8 page, u8 x1, u8 y1,
                                     u8 x2, u8 y2);
static void grid_screen_refresh_led(scene_state_t *ss, u8 full_grid, u8 page,
                                    u8 x1, u8 y1, u8 x2, u8 y2);
static void grid_screen_refresh_info(scene_state_t *ss, u8 page, u8 x1, u8 y1,
                                     u8 x2, u8 y2);
static bool grid_within_area(u8 x, u8 y, grid_common_t *gc);
static void grid_fill_area(s8 x, s8 y, s8 w, s8 h, s8 level);
static void grid_fill_area_scr(s8 x, s8 y, s8 w, s8 h, s8 level, u8 page);
static u16 calc_fader_level(scene_state_t *ss, u8 i, u8 vert);

void grid_set_control_mode(u8 control, u8 mode, scene_state_t *ss) {
    if (mode == M_LIVE) {
        u8 sub_mode = get_live_sub_mode();
        if (sub_mode == SUB_MODE_GRID)
            tt_mode = G_LIVE_G;
        else if (sub_mode == SUB_MODE_FULLGRID)
            tt_mode = G_LIVE_GF;
        else if (sub_mode == SUB_MODE_DASH)
            tt_mode = G_LIVE_D;
        else
            tt_mode = G_LIVE_V;
    }
    else if (mode == M_EDIT) {
        tt_mode = G_EDIT;
        tt_script = get_edit_script();
    }
    else if (mode == M_PATTERN) {
        tt_mode = G_TRACKER;
    }
    else if (mode == M_PRESET_W || mode == M_PRESET_R) {
        tt_mode = G_PRESET;
    }
    control_mode_on = control;
    grid_clear_held_keys();
    ss->grid.grid_dirty = 1;
}

void grid_control_refresh(scene_state_t *ss) {
    size_x = monome_size_x();
    size_y = monome_size_y();

    u16 d = size_y == 16 ? 128 : 0;
    if (size_x == 16) d += 8;

    u8 mode_on = 15;
    u8 mode_off = 7;
    u8 exec = 15;
    u8 trig = 7;
    u8 kill = 11;
    u8 tracker_in = 6;
    u8 tracker_out = 2;
    u8 tracker_on = 13;
    u8 tracker_pos = 2;
    u8 tracker_page_on = 11;
    u8 tracker_page_off = 5;
    u8 tracker_loop = 5;
    u8 tracker_control = 11;
    u8 preset_selected = 15;
    u8 preset_unselected = 4;
    u8 preset_scroll = 8;
    u8 preset_load = 11;
    u8 preset_save = 15;
    u8 var_edit_on = 11;
    u8 var_edit_off = 6;
    u8 var_value_on = 11;
    u8 var_value_off = 4;
    u8 live_hist = 7;
    u8 live_exec = 11;
    u8 mute_on = 8;
    u8 mute_off = 4;
    u8 grid_page_on = 8;
    u8 grid_page_off = 4;
    u8 line_on = 8;
    u8 line_off = 4;
    u8 script_control = 8;

    if (!monome_is_vari()) {
        mode_on = 15;
        mode_off = 15;
        exec = 15;
        trig = 15;
        kill = 15;
        tracker_in = 0;
        tracker_out = 0;
        tracker_on = 15;
        tracker_page_on = 15;
        tracker_page_off = 0;
        tracker_loop = 15;
        tracker_control = 15;
        preset_selected = 15;
        preset_unselected = 15;
        preset_scroll = 15;
        preset_load = 15;
        preset_save = 15;
        var_edit_on = 15;
        var_edit_off = 15;
        var_value_on = 15;
        var_value_off = 0;
        live_hist = 15;
        live_exec = 15;
        mute_on = 15;
        mute_off = 15;
        grid_page_on = 15;
        grid_page_off = 15;
        line_on = 15;
        line_off = 15;
        script_control = 15;
    }

    for (u16 i = 0; i < 8; i++)
        for (u16 j = 0; j < 8; j++) monomeLedBuffer[d + i + (j << 4)] = 0;

    if (tt_mode == G_TRACKER) {
        monomeLedBuffer[d + 7] = mode_on;
        u8 offset = get_pattern_offset(), in, off, rem;
        for (u16 j = 0; j < 8; j++) {
            for (u16 i = 0; i < 4; i++) {
                in = offset + j >= ss_get_pattern_start(ss, i) &&
                     offset + j <= ss_get_pattern_end(ss, i);
                monomeLedBuffer[d + i + 2 + (j << 4)] =
                    ss_get_pattern_val(ss, i, j + offset)
                        ? tracker_on
                        : (in ? tracker_in : tracker_out);
            }
            off = offset >> 3;
            rem = (offset & 7) >> 1;
            monomeLedBuffer[d + (j << 4)] =
                j == off ? tracker_page_on - rem
                         : (j == off + 1 ? tracker_page_off + rem
                                         : tracker_page_off);
        }
        for (u16 i = 0; i < 4; i++) {
            u8 index = ss_get_pattern_idx(ss, i);
            if (index >= offset && index <= offset + 7) {
                monomeLedBuffer[d + i + 2 + (index << 4)] += tracker_pos;
            }
        }

        d += 32;
        monomeLedBuffer[d + 7] = tracker_control;
        d += 16;
        monomeLedBuffer[d + 7] =
            turtle_get_shown(&ss->turtle) ? tracker_control : tracker_loop;
        d += 16;
        monomeLedBuffer[d + 7] = tracker_loop;
        d += 16;
        monomeLedBuffer[d + 7] = tracker_loop;
        d += 16;
        monomeLedBuffer[d + 7] = tracker_control;
        d += 16;
        monomeLedBuffer[d + 7] = tracker_control;
        return;
    }

    // mode selection
    monomeLedBuffer[d] =
        tt_mode == G_EDIT && tt_script == METRO_SCRIPT ? mode_on : mode_off;
    monomeLedBuffer[d + 1] =
        tt_mode == G_EDIT && tt_script == INIT_SCRIPT ? mode_on : mode_off;
    monomeLedBuffer[d + 3] =
        tt_mode == G_LIVE_V || tt_mode == G_LIVE_D ? mode_on : mode_off;
    monomeLedBuffer[d + 4] =
        tt_mode == G_LIVE_G || tt_mode == G_LIVE_GF ? mode_on : mode_off;
    monomeLedBuffer[d + 6] = tt_mode == G_PRESET ? mode_on : mode_off;
    monomeLedBuffer[d + 7] = mode_off;
    d += 16;
    for (u16 i = 0; i < 8; i++)
        monomeLedBuffer[d + i] =
            tt_mode == G_EDIT && tt_script == i ? mode_on : mode_off;
    d += 16;

    if (tt_mode == G_PRESET) {
        for (u8 j = 0; j < 25; j += 8) {
            for (u16 i = 0; i < 8; i++)
                monomeLedBuffer[d + i] = i + j == preset_select
                                             ? preset_selected
                                             : preset_unselected;
            d += 16;
        }

        monomeLedBuffer[d + 7] = preset_scroll;
        d += 16;
        monomeLedBuffer[d + 2] = preset_load;
        monomeLedBuffer[d + 4] = preset_save;
        monomeLedBuffer[d + 7] = preset_scroll;
        return;
    }

    monomeLedBuffer[d + 16] = ss->variables.m_act ? mute_off : mute_on;
    monomeLedBuffer[d + 17] = script_triggers[10].on ? exec : kill;
    monomeLedBuffer[d + 32] = script_triggers[8].on ? exec : trig;
    monomeLedBuffer[d + 33] = script_triggers[9].on ? exec : trig;

    if (tt_mode == G_LIVE_V || tt_mode == G_LIVE_D) {
        monomeLedBuffer[d + 3] =
            variable_edit == 1 ? var_edit_on : var_edit_off;
        monomeLedBuffer[d + 4] =
            variable_edit == 2 ? var_edit_on : var_edit_off;
        d += 16;
        monomeLedBuffer[d + 3] =
            variable_edit == 3 ? var_edit_on : var_edit_off;
        monomeLedBuffer[d + 4] =
            variable_edit == 4 ? var_edit_on : var_edit_off;
        monomeLedBuffer[d + 6] = live_hist;
        d += 16;
        monomeLedBuffer[d + 3] =
            variable_edit == 5 ? var_edit_on : var_edit_off;
        monomeLedBuffer[d + 4] =
            variable_edit == 6 ? var_edit_on : var_edit_off;
        monomeLedBuffer[d + 6] = live_hist;
        monomeLedBuffer[d + 7] = live_exec;
        d += 16;
        monomeLedBuffer[d + 3] =
            variable_edit == 7 ? var_edit_on : var_edit_off;
        monomeLedBuffer[d + 4] =
            variable_edit == 8 ? var_edit_on : var_edit_off;
    }
    else if (tt_mode == G_LIVE_G || tt_mode == G_LIVE_GF) {
        d += 16;
        monomeLedBuffer[d + 7] = grid_page == 0 ? grid_page_on : grid_page_off;
        d += 16;
        monomeLedBuffer[d + 3] = ss->grid.rotate ? grid_page_on : grid_page_off;
        monomeLedBuffer[d + 5] =
            grid_show_controls ? grid_page_on : grid_page_off;
        monomeLedBuffer[d + 7] = grid_page == 1 ? grid_page_on : grid_page_off;
        d += 16;
    }
    else if (tt_mode == G_EDIT) {
        d += 16;
        monomeLedBuffer[d + 3] =
            ss_get_script_comment(ss, tt_script, 0) ? line_off : line_on;
        monomeLedBuffer[d + 4] =
            ss_get_script_comment(ss, tt_script, 3) ? line_off : line_on;
        monomeLedBuffer[d + 7] = script_control;
        d += 16;
        monomeLedBuffer[d + 3] =
            ss_get_script_comment(ss, tt_script, 1) ? line_off : line_on;
        monomeLedBuffer[d + 4] =
            ss_get_script_comment(ss, tt_script, 4) ? line_off : line_on;
        monomeLedBuffer[d + 7] = script_control;
        d += 16;
        monomeLedBuffer[d + 3] =
            ss_get_script_comment(ss, tt_script, 2) ? line_off : line_on;
        monomeLedBuffer[d + 4] =
            ss_get_script_comment(ss, tt_script, 5) ? line_off : line_on;
    }
    d += 16;

    // script mutes
    for (u16 i = 0; i < 8; i++)
        monomeLedBuffer[d + i] = ss_get_mute(ss, i) ? mute_on : mute_off;
    d += 16;

    // triggered scripts
    for (u16 i = 0; i < 8; i++)
        monomeLedBuffer[d + i] = script_triggers[i].on ? exec : trig;

    if (variable_edit) {
        u8 ve = variable_edit - 1;
        int16_t *v = &(ss->variables.a);
        if (size_x == 8) {
            if (v[ve] < 0)
                for (u16 i = 0; i < 8; i++)
                    monomeLedBuffer[d + i] = var_value_off;
            else if (v[ve] > 8)
                for (u16 i = 0; i < 8; i++)
                    monomeLedBuffer[d + i] = var_value_on;
            else
                for (u16 i = 0; i < 8; i++)
                    monomeLedBuffer[d + i] =
                        i < v[ve] ? var_value_on : var_value_off;
        }
        else {
            d -= 8;
            if (v[ve] < 0)
                for (u16 i = 0; i < 16; i++)
                    monomeLedBuffer[d + i] = var_value_off;
            else if (v[ve] > 16)
                for (u16 i = 0; i < 16; i++)
                    monomeLedBuffer[d + i] = var_value_on;
            else
                for (u16 i = 0; i < 16; i++)
                    monomeLedBuffer[d + i] =
                        i < v[ve] ? var_value_on : var_value_off;
        }
    }
}

static void script_triggers_callback(void *o) {
    script_trigger_info *st = o;
    timer_remove(&st->timer);
    st->on = 0;
    st->ss->grid.grid_dirty = 1;
}

void grid_metro_triggered(scene_state_t *ss) {
    script_triggers[8].on = 1;
    script_triggers[8].ss = ss;
    timer_remove(&script_triggers[8].timer);
    timer_add(&script_triggers[8].timer,
              min(GRID_SCRIPT_TRIGGER, ss->variables.m >> 1),
              &script_triggers_callback, (void *)&script_triggers[8]);
    ss->grid.grid_dirty = 1;
}

static void restore_last_mode(scene_state_t *ss) {
    tt_mode = tt_last_mode;
    if (tt_mode == G_EDIT) {
        set_edit_mode_script(tt_script);
        set_mode(M_EDIT);
    }
    else if (tt_mode == G_PRESET) {
        set_mode(M_PRESET_R);
    }
    else if (tt_mode == G_LIVE_V) {
        set_mode(M_LIVE);
        set_live_submode(SUB_MODE_VARS);
    }
    else if (tt_mode == G_LIVE_D) {
        set_mode(M_LIVE);
        set_live_submode(SUB_MODE_DASH);
    }
    else if (tt_mode == G_LIVE_G) {
        set_mode(M_LIVE);
        set_live_submode(SUB_MODE_GRID);
    }
    else if (tt_mode == G_LIVE_GF) {
        set_mode(M_LIVE);
        set_live_submode(SUB_MODE_FULLGRID);
    }
    else if (tt_mode == G_TRACKER) {
        tt_mode = G_TRACKER;
        set_mode(M_PATTERN);
    }
    grid_clear_held_keys();
    ss->grid.grid_dirty = 1;
}

static u8 grid_control_process_key(scene_state_t *ss, u8 x, u8 y, u8 z,
                                   u8 from_held) {
    if (size_y == 16) {
        if (y < 8) return 0;
        y -= 8;
    }

    if (variable_edit && y == 7) {
        if (!z || from_held) return 1;

        int16_t *v = &(ss->variables.a);
        v[variable_edit - 1] = v[variable_edit - 1] == x + 1 ? 0 : x + 1;
        variable_changed = 1;
        set_vars_updated();
        ss->grid.grid_dirty = 1;
        return 1;
    }

    if (size_x == 16) {
        if (x < 8) return 0;
        x -= 8;
    }

    // tracker
    if (tt_mode == G_TRACKER) {
        u8 offset = get_pattern_offset();

        if (x == 7 && y == 4) tracker_set_start = z;
        if (x == 7 && y == 5) tracker_set_end = z;

        if (tracker_pressed) {
            s16 value =
                ss_get_pattern_val(ss, tracker_x - 2, tracker_y + offset);

            if (x == tracker_x && y == tracker_y && !z) {
                if (!tracker_changed) {
                    s16 value = ss_get_pattern_val(
                        ss, tracker_x - 2, tracker_y + get_pattern_offset());
                    if (value) {
                        tracker_last = value;
                        value = 0;
                    }
                    else {
                        value = tracker_last ? tracker_last : 1;
                    }
                    ss_set_pattern_val(ss, tracker_x - 2, tracker_y + offset,
                                       value);
                }
                tracker_pressed = 0;
                tele_pattern_updated();
                ss->grid.grid_dirty = 1;
                return 1;
            }

            if (!z) return 1;

            u8 updated = 0;
            if (y == tracker_y) {
                if (x == tracker_x + 1) {
                    if (value < 32767) {
                        ss_set_pattern_val(ss, tracker_x - 2,
                                           tracker_y + offset, value + 1);
                        updated = 1;
                    }
                }
                else if (x == tracker_x + 2) {
                    if (value < 32758) {
                        ss_set_pattern_val(ss, tracker_x - 2,
                                           tracker_y + offset, value + 10);
                        updated = 1;
                    }
                    else if (value < 32767) {
                        ss_set_pattern_val(ss, tracker_x - 2,
                                           tracker_y + offset, 32767);
                        updated = 1;
                    }
                }
                else if (x == tracker_x - 1) {
                    if (value > -32768) {
                        ss_set_pattern_val(ss, tracker_x - 2,
                                           tracker_y + offset, value - 1);
                        updated = 1;
                    }
                }
                else if (x == tracker_x - 2) {
                    if (value > -32759) {
                        ss_set_pattern_val(ss, tracker_x - 2,
                                           tracker_y + offset, value - 10);
                        updated = 1;
                    }
                    else if (value > -32768) {
                        ss_set_pattern_val(ss, tracker_x - 2,
                                           tracker_y + offset, -32768);
                        updated = 1;
                    }
                }
            }
            else if (x > 1 && x < 6) {
                // set loop
                if (from_held) return 1;
                for (u8 i = min(tracker_x, x); i <= max(tracker_x, x); i++) {
                    ss_set_pattern_start(ss, i - 2, min(y, tracker_y) + offset);
                    ss_set_pattern_end(ss, i - 2, max(y, tracker_y) + offset);
                    ss_set_pattern_len(ss, i - 2,
                                       max(y, tracker_y) + offset + 1);
                }
                updated = 1;
            }

            if (updated) {
                tracker_changed = 1;
                tele_pattern_updated();
                ss->grid.grid_dirty = 1;
            }
            return 1;
        }

        if (tracker_select || tracker_set_start || tracker_set_end) {
            if (x == 7 && y == tracker_select && !z) {
                if (y == 3 && !tracker_selected) {
                    turtle_set_shown(&ss->turtle,
                                     !turtle_get_shown(&ss->turtle));
                    tele_pattern_updated();
                    ss->grid.grid_dirty = 1;
                }
                tracker_select = 0;
            }
            else if (x > 1 && x < 6 && z && !from_held) {
                if (tracker_select == 2) {
                    // set current position
                    tracker_selected = 1;
                    ss_set_pattern_idx(ss, x - 2, offset + y);
                    tele_pattern_updated();
                    ss->grid.grid_dirty = 1;
                }
                else if (tracker_select == 3) {
                    // set turtle position
                    tracker_selected = 1;
                    turtle_set_x(&ss->turtle, x - 2);
                    turtle_set_y(&ss->turtle, offset + y);
                    turtle_set_shown(&ss->turtle, 1);
                    tele_pattern_updated();
                    ss->grid.grid_dirty = 1;
                }
                if (tracker_set_start) {
                    // set start
                    ss_set_pattern_start(ss, x - 2, y + offset);
                    tracker_selected = 1;
                    tele_pattern_updated();
                    ss->grid.grid_dirty = 1;
                }
                if (tracker_set_end) {
                    // set end
                    ss_set_pattern_end(ss, x - 2, y + offset);
                    ss_set_pattern_len(ss, x - 2, y + offset + 1);
                    tracker_selected = 1;
                    tele_pattern_updated();
                    ss->grid.grid_dirty = 1;
                }
            }
            return 1;
        }

        if (x > 1 && x < 6 && z) {
            // pattern value selected
            if (x != tracker_x || y != tracker_y) tracker_last = 0;
            tracker_pressed = 1;
            tracker_changed = 0;
            tracker_x = x;
            tracker_y = y;
            set_pattern_selected_value(x - 2, y);
            tele_pattern_updated();
            ss->grid.grid_dirty = 1;
        }
        else if (x == 7 && y == 0 && !from_held && z) {
            // exit tracker
            restore_last_mode(ss);
            ss->grid.grid_dirty = 1;
        }
        else if (x == 0 && !from_held && z) {
            // select page
            set_pattern_offset(y << 3);
            ss->grid.grid_dirty = 1;
        }
        else if (x == 7 && y == 6 && z) {
            u8 offset = get_pattern_offset();
            if (offset) set_pattern_offset(offset - 1);
            ss->grid.grid_dirty = 1;
        }
        else if (x == 7 && y == 7 && z) {
            u8 offset = get_pattern_offset();
            if (offset < 56) set_pattern_offset(offset + 1);
            ss->grid.grid_dirty = 1;
        }
        else if (x == 7 && y > 1 && y != 4 && y != 5 && z) {
            tracker_select = y;
            tracker_selected = 0;
        }
        return 1;
    }

    // select page
    if (y == 0) {
        if (!z || from_held) return 1;

        switch (x) {
            case 0:
                tt_mode = G_EDIT;
                tt_script = METRO_SCRIPT;
                set_edit_mode_script(tt_script);
                set_mode(M_EDIT);
                ss->grid.grid_dirty = 1;
                break;
            case 1:
                tt_mode = G_EDIT;
                tt_script = INIT_SCRIPT;
                set_edit_mode_script(tt_script);
                set_mode(M_EDIT);
                ss->grid.grid_dirty = 1;
                break;
            case 3:
                tt_mode = tt_mode == G_LIVE_D ? G_LIVE_V : G_LIVE_D;
                set_mode(M_LIVE);
                set_live_submode(tt_mode == G_LIVE_V ? SUB_MODE_VARS
                                                     : SUB_MODE_DASH);
                ss->grid.grid_dirty = 1;
                break;
            case 4:
                tt_mode = tt_mode == G_LIVE_G ? G_LIVE_GF : G_LIVE_G;
                set_mode(M_LIVE);
                set_live_submode(tt_mode == G_LIVE_G ? SUB_MODE_GRID
                                                     : SUB_MODE_FULLGRID);
                ss->grid.grid_dirty = 1;
                break;
            case 6:
                tt_last_mode = tt_mode;
                tt_mode = G_PRESET;
                set_mode(M_PRESET_R);
                ss->grid.grid_dirty = 1;
                break;
            case 7:
                tt_last_mode = tt_mode;
                tt_mode = G_TRACKER;
                set_mode(M_PATTERN);
                ss->grid.grid_dirty = 1;
                break;
            default: break;
        }
        grid_clear_held_keys();
        return 1;
    }

    // select script for editing
    if (y == 1) {
        if (!z || from_held) return 1;
        tt_mode = G_EDIT;
        tt_script = x;
        set_edit_mode_script(tt_script);
        set_mode(M_EDIT);
        ss->grid.grid_dirty = 1;
        return 1;
    }

    // presets
    if (tt_mode == G_PRESET) {
        if (!z) return 1;
        if (y > 1 && y < 6 && !from_held) {
            if (preset_write) {
                preset_write = 0;
                set_mode(M_PRESET_R);
            }
            process_preset_r_preset(x + ((y - 2) << 3));
            ss->grid.grid_dirty = 1;
        }
        else if (y == 6 && x == 7) {
            if (preset_write) {
                preset_write = 0;
                set_mode(M_PRESET_R);
            }
            preset_line_up();
            ss->grid.grid_dirty = 1;
        }
        else if (y == 7 && x == 7) {
            if (preset_write) {
                preset_write = 0;
                set_mode(M_PRESET_R);
            }
            preset_line_down();
            ss->grid.grid_dirty = 1;
        }
        else if (y == 7 && x == 2 && !from_held) {
            if (preset_write) {
                preset_write = 0;
                set_mode(M_PRESET_R);
            }
            else {
                process_preset_r_load();
                set_mode(M_PRESET_R);
                restore_last_mode(ss);
            }
            ss->grid.grid_dirty = 1;
        }
        else if (y == 7 && x == 4 && !from_held) {
            if (preset_write) {
                flash_write(preset_select, ss, &scene_text);
                flash_update_last_saved_scene(preset_select);
                preset_write = 0;
                restore_last_mode(ss);
            }
            else {
                set_mode(M_PRESET_W);
                preset_write = 1;
            }
            ss->grid.grid_dirty = 1;
        }
        else {
            preset_write = 0;
            set_mode(M_PRESET_R);
        }
        return 1;
    }

    // mutes
    if (y == 6) {
        if (!z || from_held) return 1;
        bool muted = ss_get_mute(ss, x);
        ss_set_mute(ss, x, !muted);
        screen_mutes_updated();
        set_mutes_updated();
        ss->grid.grid_dirty = 1;
        return 1;
    }

    // trigger scripts
    if (y == 7) {
        if (!z) return 1;
        script_triggers[x].on = 1;
        script_triggers[x].ss = ss;
        timer_remove(&script_triggers[x].timer);
        timer_add(&script_triggers[x].timer, GRID_SCRIPT_TRIGGER,
                  &script_triggers_callback, (void *)&script_triggers[x]);
        ss->grid.grid_dirty = 1;
        run_script(ss, x);
        return 1;
    }

    if (variable_edit && y > 1 && y < 6) {
        int16_t *v = &(ss->variables.a);
        u8 ve = variable_edit - 1;

        if (!z && x > 2 && x < 5 && variable_edit == x - 2 + ((y - 2) << 1)) {
            if (!variable_changed) {
                if (v[ve]) {
                    variable_last = v[ve];
                    v[ve] = 0;
                }
                else {
                    v[ve] = variable_last ? variable_last : 1;
                }
            }
            variable_edit = 0;
            set_vars_updated();
            ss->grid.grid_dirty = 1;
            return 1;
        }
        if (!z) return 1;

        u8 v_x = 3 + (ve & 1);
        if (x == v_x + 1) {
            if (v[ve] < 32767) v[ve]++;
        }
        else if (x == v_x + 2) {
            if (v[ve] < 32758)
                v[ve] += 10;
            else
                v[ve] = 32767;
        }
        else if (x == v_x - 1) {
            if (v[ve] > -32768) v[ve]--;
        }
        else if (x == v_x - 2) {
            if (v[ve] > -32759)
                v[ve] -= 10;
            else
                v[ve] = -32768;
        }

        variable_changed = 1;
        set_vars_updated();
        ss->grid.grid_dirty = 1;
        return 1;
    }

    // metro on/off
    if (y == 3 && x == 0 && !from_held && !z) {
        ss->variables.m_act = !ss->variables.m_act;
        screen_mutes_updated();
        set_mutes_updated();
        tele_metro_updated();
        ss->grid.grid_dirty = 1;
        return 1;
    }

    // kill slews/delays
    if (y == 3 && x == 1 && !from_held && z) {
        script_triggers[10].on = 1;
        script_triggers[10].ss = ss;
        timer_remove(&script_triggers[x].timer);
        timer_add(&script_triggers[10].timer, GRID_SCRIPT_TRIGGER,
                  &script_triggers_callback, (void *)&script_triggers[10]);
        clear_delays_and_slews(ss);
        ss->grid.grid_dirty = 1;
        return 1;
    }

    // trigger metro
    if (y == 4 && x == 0 && z) {
        script_triggers[8].on = 1;
        script_triggers[8].ss = ss;
        timer_remove(&script_triggers[8].timer);
        timer_add(&script_triggers[8].timer, GRID_SCRIPT_TRIGGER,
                  &script_triggers_callback, (void *)&script_triggers[8]);
        ss->grid.grid_dirty = 1;
        run_script(ss, METRO_SCRIPT);
        return 1;
    }

    // trigger init
    if (y == 4 && x == 1 && z) {
        script_triggers[9].on = 1;
        script_triggers[9].ss = ss;
        timer_remove(&script_triggers[9].timer);
        timer_add(&script_triggers[9].timer, GRID_SCRIPT_TRIGGER,
                  &script_triggers_callback, (void *)&script_triggers[9]);
        ss->grid.grid_dirty = 1;
        run_script(ss, INIT_SCRIPT);
        return 1;
    }

    // live variables
    if (tt_mode == G_LIVE_V || tt_mode == G_LIVE_D) {
        if (y > 1 && y < 6 && x > 2 && x < 5 && !from_held) {
            variable_edit = z ? x - 2 + ((y - 2) << 1) : 0;
            if (variable_edit) variable_changed = 0;
            ss->grid.grid_dirty = 1;
            return 1;
        }

        if (!z) return 1;

        if (y == 3 && x == 6) { history_prev(); }
        else if (y == 4 && x == 6) {
            history_next();
        }
        else if (y == 4 && x == 7 && !from_held) {
            execute_line();
        }

        return 1;
    }

    // live grid preview
    if (tt_mode == G_LIVE_G || tt_mode == G_LIVE_GF) {
        if (!z || from_held) return 1;

        if (y == 3 && x == 7) {
            grid_page = 0;
            set_grid_updated();
            ss->grid.grid_dirty = 1;
        }
        else if (y == 4 && x == 3) {
            ss->grid.rotate = ss->grid.rotate == 0;
            set_grid_updated();
            ss->grid.grid_dirty = 1;
        }
        else if (y == 4 && x == 5) {
            grid_show_controls = !grid_show_controls;
            set_grid_updated();
            ss->grid.grid_dirty = 1;
        }
        else if (y == 4 && x == 7) {
            grid_page = 1;
            set_grid_updated();
            ss->grid.grid_dirty = 1;
        }

        return 1;
    }

    // edit scripts
    if (tt_mode == G_EDIT) {
        if (!z || from_held) return 1;

        if (y > 2 && y < 6 && x > 2 && x < 5) {
            u8 i = (x - 3) * 3 + y - 3;
            if (i >= ss_get_script_len(ss, tt_script)) return 1;
            ss_toggle_script_comment(ss, tt_script, i);
            edit_mode_refresh();
            ss->grid.grid_dirty = 1;
        }
        else if (y == 3 && x == 7) {
            for (u8 i = 0; i < ss_get_script_len(ss, tt_script); i++)
                ss_set_script_comment(ss, tt_script, i, 1);
            edit_mode_refresh();
            ss->grid.grid_dirty = 1;
        }
        else if (y == 4 && x == 7) {
            for (u8 i = 0; i < ss_get_script_len(ss, tt_script); i++)
                ss_set_script_comment(ss, tt_script, i, 0);
            edit_mode_refresh();
            ss->grid.grid_dirty = 1;
        }
        return 1;
    }

    return 1;
}

void grid_process_key(scene_state_t *ss, u8 _x, u8 _y, u8 z, u8 emulated) {
    if (timers_uninitialized) {
        timers_uninitialized = 0;
        for (u8 i = 0; i < GRID_MAX_KEY_PRESSED; i++) held_keys[i].used = 0;
    }

    size_x = monome_size_x();
    size_y = monome_size_y();
    u8 x = SG.rotate && !emulated ? size_x - _x - 1 : _x;
    u8 y = SG.rotate && !emulated ? size_y - _y - 1 : _y;

    if (SG.clear_held) {
        grid_clear_held_keys();
        SG.clear_held = 0;
    }

    if (control_mode_on ? !emulated : true) {
        u8 key = (y << 4) | x;
        if (z) {
            for (u8 i = 0; i < GRID_MAX_KEY_PRESSED; i++)
                if (!held_keys[i].used || held_keys[i].key == key) {
                    held_keys[i].used = 1;
                    held_keys[i].key = key;
                    held_keys[i].x = x;
                    held_keys[i].y = y;
                    held_keys[i].ss = ss;
                    timer_add(&held_keys[i].timer, GRID_KEY_HOLD_DELAY,
                              &hold_repeat_timer_callback,
                              (void *)&held_keys[i]);
                    break;
                }
        }
        else {
            for (u8 i = 0; i < GRID_MAX_KEY_PRESSED; i++)
                if (held_keys[i].key == key) {
                    timer_remove(&held_keys[i].timer);
                    held_keys[i].used = 0;
                }
        }
    }

    if (control_mode_on && !emulated)
        if (grid_control_process_key(ss, x, y, z, 0)) return;

    u8 refresh = 0;
    u8 scripts[EDITABLE_SCRIPT_COUNT];
    for (u8 i = 0; i < EDITABLE_SCRIPT_COUNT; i++) scripts[i] = 0;

    for (u8 i = 0; i < GRID_XYPAD_COUNT; i++) {
        if (z && GXYC.enabled && SG.group[GXYC.group].enabled &&
            grid_within_area(x, y, &GXYC)) {
            GXY.value_x = x - GXYC.x;
            GXY.value_y = y - GXYC.y;
            if (GXYC.script != -1) scripts[GXYC.script] = 1;
            SG.latest_group = GXYC.group;
            if (SG.group[GXYC.group].script != -1)
                scripts[SG.group[GXYC.group].script] = 1;
            refresh = 1;
        }
    }

    u16 value;
    s8 held;
    if (z) {
        for (u8 i = 0; i < GRID_FADER_COUNT; i++) {
            if (GFC.enabled && SG.group[GFC.group].enabled &&
                grid_within_area(x, y, &GFC)) {
                held = -1;
                if (GF.type & 1) {
                    for (u8 j = 0; j < GRID_MAX_KEY_PRESSED; j++)
                        if (held_keys[j].used && (held_keys[j].y != y) &&
                            grid_within_area(held_keys[j].x, held_keys[j].y,
                                             &GFC)) {
                            held = j;
                            break;
                        }
                }
                else {
                    for (u8 j = 0; j < GRID_MAX_KEY_PRESSED; j++)
                        if (held_keys[j].used && (held_keys[j].x != x) &&
                            grid_within_area(held_keys[j].x, held_keys[j].y,
                                             &GFC)) {
                            held = j;
                            break;
                        }
                }

                switch (GF.type) {
                    case FADER_CH_BAR:
                    case FADER_CH_DOT:
                        if (held == -1) {
                            GF.slide = 0;
                            GF.value = x - GFC.x;
                        }
                        else {
                            GF.slide = 1;
                            GF.slide_acc = 0;
                            GF.slide_end = x - GFC.x;
                            GF.slide_delta = 16;
                            GF.slide_dir = GF.slide_end > GF.value;
                        }
                        break;
                    case FADER_CV_BAR:
                    case FADER_CV_DOT:
                        if (held == -1) {
                            GF.slide = 0;
                            GF.value = GFC.h + GFC.y - y - 1;
                        }
                        else {
                            GF.slide = 1;
                            GF.slide_acc = 0;
                            GF.slide_end = GFC.h + GFC.y - y - 1;
                            GF.slide_delta = 16;
                            GF.slide_dir = GF.slide_end > GF.value;
                        }
                        break;
                    case FADER_FH_BAR:
                    case FADER_FH_DOT:
                        if (held != -1 &&
                            (held_keys[held].x == GFC.x ||
                             held_keys[held].x == (GFC.x + GFC.w - 1)))
                            held = -1;
                        if (held == -1) {
                            GF.slide = 0;
                            if (x == GFC.x) {
                                if (GF.value) GF.value--;
                            }
                            else if (x == GFC.x + GFC.w - 1) {
                                if (GF.value < GFC.level) GF.value++;
                            }
                            else {
                                value =
                                    ((((x - GFC.x - 1) << 1) + 1) * GFC.level) /
                                    (GFC.w - 2);
                                GF.value = (value >> 1) + (value & 1);
                            }
                        }
                        else {
                            GF.slide = 1;
                            GF.slide_acc = 0;
                            if (x == GFC.x)
                                value = 0;
                            else if (x == (GFC.x + GFC.w - 1))
                                value = GFC.level;
                            else {
                                value =
                                    ((((x - GFC.x - 1) << 1) + 1) * GFC.level) /
                                    (GFC.w - 2);
                                value = (value >> 1) + (value & 1);
                            }
                            GF.slide_end = value;
                            value = ((GFC.w - 2) << 4) / GFC.level;
                            if (value == 0) value = 1;
                            GF.slide_delta = value;
                            GF.slide_dir = GF.slide_end > GF.value;
                        }
                        break;
                    case FADER_FV_BAR:
                    case FADER_FV_DOT:
                        if (held != -1 &&
                            (held_keys[held].y == GFC.y ||
                             held_keys[held].y == (GFC.y + GFC.h - 1)))
                            held = -1;
                        if (held == -1) {
                            GF.slide = 0;
                            if (y == GFC.y) {
                                if (GF.value < GFC.level) GF.value++;
                            }
                            else if (y == GFC.y + GFC.h - 1) {
                                if (GF.value) GF.value--;
                            }
                            else {
                                value = ((((GFC.h + GFC.y - y - 2) << 1) + 1) *
                                         GFC.level) /
                                        (GFC.h - 2);
                                GF.value = (value >> 1) + (value & 1);
                            }
                        }
                        else {
                            GF.slide = 1;
                            GF.slide_acc = 0;
                            if (y == GFC.y)
                                value = GFC.level;
                            else if (y == (GFC.y + GFC.h - 1))
                                value = 0;
                            else {
                                value = ((((GFC.h + GFC.y - y - 2) << 1) + 1) *
                                         GFC.level) /
                                        (GFC.h - 2);
                                value = (value >> 1) + (value & 1);
                            }
                            GF.slide_end = value;
                            value = ((GFC.h - 2) << 4) / GFC.level;
                            if (value == 0) value = 1;
                            GF.slide_delta = value;
                            GF.slide_dir = GF.slide_end > GF.value;
                        }
                        break;
                }

                if (GFC.script != -1) scripts[GFC.script] = 1;
                SG.latest_fader = i;
                SG.latest_group = GFC.group;
                if (SG.group[GFC.group].script != -1)
                    scripts[SG.group[GFC.group].script] = 1;
                refresh = 1;
            }
        }
    }

    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++) {
        if (GBC.enabled && SG.group[GBC.group].enabled &&
            grid_within_area(x, y, &GBC)) {
            if (GB.latch) {
                if (z) {
                    GB.state = !GB.state;
                    if (GBC.script != -1) scripts[GBC.script] = 1;
                }
            }
            else {
                GB.state = z;
                if (GBC.script != -1) scripts[GBC.script] = 1;
            }
            SG.latest_button = i;
            SG.latest_group = GBC.group;
            if (SG.group[GBC.group].script != -1)
                scripts[SG.group[GBC.group].script] = 1;
            refresh = 1;
        }
    }

    for (u8 i = 0; i < EDITABLE_SCRIPT_COUNT; i++)
        if (scripts[i]) run_script(ss, i);

    if (refresh) SG.grid_dirty = SG.scr_dirty = 1;
}

void grid_process_key_hold_repeat(scene_state_t *ss, u8 x, u8 y) {
    if (control_mode_on)
        if (grid_control_process_key(ss, x, y, 1, 1)) return;

    u8 refresh = 0;
    u8 scripts[EDITABLE_SCRIPT_COUNT];
    for (u8 i = 0; i < EDITABLE_SCRIPT_COUNT; i++) scripts[i] = 0;

    u8 update = 0;
    for (u8 i = 0; i < GRID_FADER_COUNT; i++) {
        if (GFC.enabled && SG.group[GFC.group].enabled &&
            grid_within_area(x, y, &GFC)) {
            update = 0;
            if (GF.type == FADER_FH_BAR || GF.type == FADER_FH_DOT) {
                if (x == GFC.x) {
                    if (GF.value) GF.value--;
                    update = 1;
                }
                else if (x == GFC.x + GFC.w - 1) {
                    if (GF.value < GFC.level) GF.value++;
                    update = 1;
                }
            }
            else if (GF.type == FADER_FV_BAR || GF.type == FADER_FV_DOT) {
                if (y == GFC.y) {
                    if (GF.value < GFC.level) GF.value++;
                    update = 1;
                }
                else if (y == GFC.y + GFC.h - 1) {
                    if (GF.value) GF.value--;
                    update = 1;
                }
            }

            if (update) {
                if (GFC.script != -1) scripts[GFC.script] = 1;
                SG.latest_fader = i;
                SG.latest_group = GFC.group;
                if (SG.group[GFC.group].script != -1)
                    scripts[SG.group[GFC.group].script] = 1;
                refresh = 1;
            }
        }
    }

    for (u8 i = 0; i < EDITABLE_SCRIPT_COUNT; i++)
        if (scripts[i]) run_script(ss, i);

    if (refresh) SG.grid_dirty = SG.scr_dirty = 1;
}

void hold_repeat_timer_callback(void *o) {
    hold_repeat_info *hr = o;
    u8 is_hold = hr->used == 1;
    if (is_hold) {
        timer_reset_set(&hr->timer, control_mode_on && hr->x > 7
                                        ? GRID_KEY_REPEAT_RATE_CTL
                                        : GRID_KEY_REPEAT_RATE);
        hr->used = 2;
    }
    grid_process_key_hold_repeat(hr->ss, hr->x, hr->y);
}

void grid_process_fader_slew(scene_state_t *ss) {
    u8 refresh = 0;
    u8 scripts[EDITABLE_SCRIPT_COUNT];
    for (u8 i = 0; i < EDITABLE_SCRIPT_COUNT; i++) scripts[i] = 0;

    for (u8 i = 0; i < GRID_FADER_COUNT; i++) {
        if (!GF.slide) continue;
        GF.slide_acc++;
        if (GF.slide_acc >= GF.slide_delta) {
            GF.slide_acc = 0;
            if (GF.slide_dir)
                GF.value++;
            else
                GF.value--;
            if ((GF.slide_dir && GF.value >= GF.slide_end) ||
                (!GF.slide_dir && GF.value <= GF.slide_end)) {
                GF.value = GF.slide_end;
                GF.slide = 0;
            }
            SG.latest_fader = i;
            SG.latest_group = GFC.group;
            if (GFC.script != -1) run_script(ss, GFC.script);
            if (SG.group[GFC.group].script != -1)
                scripts[SG.group[GFC.group].script] = 1;
            refresh = 1;
        }
    }

    for (u8 i = 0; i < EDITABLE_SCRIPT_COUNT; i++)
        if (scripts[i]) run_script(ss, i);

    if (refresh) SG.grid_dirty = SG.scr_dirty = 1;
}

void grid_clear_held_keys() {
    for (u8 i = 0; i < GRID_MAX_KEY_PRESSED; i++) {
        held_keys[i].used = 0;
        timer_remove(&held_keys[i].timer);
    }
}

bool grid_within_area(u8 x, u8 y, grid_common_t *gc) {
    return x >= gc->x && x < (gc->x + gc->w) && y >= gc->y &&
           y < (gc->y + gc->h);
}

u16 calc_fader_level(scene_state_t *ss, u8 i, u8 vert) {
    u32 fl = ((GF.value * ((vert ? GFC.h : GFC.w) - 2)) << 5) / GFC.level;
    fl = (fl >> 1) + (fl & 1);
    return fl;
}

void grid_refresh(scene_state_t *ss) {
    size_x = monome_size_x();
    size_y = monome_size_y();

    if (size_x == 0) size_x = 16;
    if (size_y == 0) size_y = 8;

    grid_fill_area(0, 0, size_x, size_y, 0);

    u16 x, y;
    for (u8 i = 0; i < GRID_XYPAD_COUNT; i++) {
        if (GXYC.enabled && SG.group[GXYC.group].enabled) {
            if (GXY.value_x || GXY.value_y) {
                x = GXYC.x + GXY.value_x;
                y = GXYC.y + GXY.value_y;
                grid_fill_area(GXYC.x, y, GXYC.w, 1, GXYC.level);
                grid_fill_area(x, GXYC.y, 1, GXYC.h, GXYC.level);
                grid_fill_area(x, y, 1, 1, GRID_ON_BRIGHTNESS);
            }
        }
    }

    u16 fv, ff, fp;
    for (u8 i = 0; i < GRID_FADER_COUNT; i++) {
        if (GFC.enabled && SG.group[GFC.group].enabled) {
            switch (GF.type) {
                case FADER_CH_BAR:
                    grid_fill_area(GFC.x, GFC.y, GF.value + 1, GFC.h,
                                   GRID_ON_BRIGHTNESS);
                    grid_fill_area(GFC.x + GF.value + 1, GFC.y,
                                   GFC.w - GF.value - 1, GFC.h, GFC.level);
                    break;
                case FADER_CH_DOT:
                    grid_fill_area(GFC.x, GFC.y, GFC.w, GFC.h, GFC.level);
                    grid_fill_area(GFC.x + GF.value, GFC.y, 1, GFC.h,
                                   GRID_ON_BRIGHTNESS);
                    break;
                case FADER_CV_BAR:
                    grid_fill_area(GFC.x, GFC.y, GFC.w, GFC.h - GF.value - 1,
                                   GFC.level);
                    grid_fill_area(GFC.x, GFC.y + GFC.h - GF.value - 1, GFC.w,
                                   GF.value + 1, GRID_ON_BRIGHTNESS);
                    break;
                case FADER_CV_DOT:
                    grid_fill_area(GFC.x, GFC.y, GFC.w, GFC.h, GFC.level);
                    grid_fill_area(GFC.x, GFC.y + GFC.h - GF.value - 1, GFC.w,
                                   1, GRID_ON_BRIGHTNESS);
                    break;
                case FADER_FH_BAR:
                    fv = calc_fader_level(ss, i, 0);
                    ff = fv >> 4;
                    fp = fv & 15;
                    grid_fill_area(GFC.x + 1, GFC.y, ff, GFC.h, 15);
                    if (fp) grid_fill_area(GFC.x + ff + 1, GFC.y, 1, GFC.h, fp);

                    grid_fill_area(GFC.x, GFC.y, 1, GFC.h, GRID_ON_BRIGHTNESS);
                    grid_fill_area(GFC.x + GFC.w - 1, GFC.y, 1, GFC.h,
                                   GRID_ON_BRIGHTNESS);
                    break;
                case FADER_FH_DOT:
                    fv = calc_fader_level(ss, i, 0);
                    ff = fv >> 4;
                    fp = fv & 15;
                    if (fp) {
                        if (GFC.w - 1 >= GFC.level)
                            fp = GRID_ON_BRIGHTNESS;
                        else if (fp < 3)
                            fp = 3;
                        grid_fill_area(GFC.x + ff + 1, GFC.y, 1, GFC.h,
                                       fp < 3 ? 3 : fp);
                    }
                    else if (ff)
                        grid_fill_area(GFC.x + ff, GFC.y, 1, GFC.h, 15);

                    grid_fill_area(GFC.x, GFC.y, 1, GFC.h, GRID_ON_BRIGHTNESS);
                    grid_fill_area(GFC.x + GFC.w - 1, GFC.y, 1, GFC.h,
                                   GRID_ON_BRIGHTNESS);
                    break;
                case FADER_FV_BAR:
                    fv = calc_fader_level(ss, i, 1);
                    ff = fv >> 4;
                    fp = fv & 15;
                    grid_fill_area(GFC.x, GFC.y + GFC.h - 1 - ff, GFC.w, ff,
                                   15);
                    if (fp)
                        grid_fill_area(GFC.x, GFC.y + GFC.h - 2 - ff, GFC.w, 1,
                                       fp);

                    grid_fill_area(GFC.x, GFC.y + GFC.h - 1, GFC.w, 1,
                                   GRID_ON_BRIGHTNESS);
                    grid_fill_area(GFC.x, GFC.y, GFC.w, 1, GRID_ON_BRIGHTNESS);
                    break;
                case FADER_FV_DOT:
                    fv = calc_fader_level(ss, i, 1);
                    ff = fv >> 4;
                    fp = fv & 15;
                    if (fp) {
                        if (GFC.h - 1 >= GFC.level)
                            fp = GRID_ON_BRIGHTNESS;
                        else if (fp < 3)
                            fp = 3;
                        grid_fill_area(GFC.x, GFC.y + GFC.h - ff - 2, GFC.w, 1,
                                       fp);
                    }
                    else if (ff)
                        grid_fill_area(GFC.x, GFC.y + GFC.h - ff - 1, GFC.w, 1,
                                       15);

                    grid_fill_area(GFC.x, GFC.y + GFC.h - 1, GFC.w, 1,
                                   GRID_ON_BRIGHTNESS);
                    grid_fill_area(GFC.x, GFC.y, GFC.w, 1, GRID_ON_BRIGHTNESS);
                    break;
            }
        }
    }

    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++)
        if (GBC.enabled && SG.group[GBC.group].enabled)
            grid_fill_area(GBC.x, GBC.y, GBC.w, GBC.h,
                           GB.state ? GRID_ON_BRIGHTNESS : GBC.level);

    u16 led;
    for (u16 i = 0; i < size_x; i++)
        for (u16 j = 0; j < size_y; j++) {
            led = (j << 4) + i;
            if (led >= MONOME_MAX_LED_BYTES) continue;

            if (SG.leds[i][j] >= 0)
                monomeLedBuffer[led] = SG.leds[i][j];
            else if (SG.leds[i][j] == LED_DIM) {
                if (monomeLedBuffer[led] > 3)
                    monomeLedBuffer[led] -= 3;
                else
                    monomeLedBuffer[led] = 0;
            }
            else if (SG.leds[i][j] == LED_BRI) {
                if (monomeLedBuffer[led] > 12)
                    monomeLedBuffer[led] = 15;
                else
                    monomeLedBuffer[led] += 3;
            }

            if (monomeLedBuffer[led] < SG.dim)
                monomeLedBuffer[led] = 0;
            else
                monomeLedBuffer[led] -= SG.dim;
        }

    if (control_mode_on) grid_control_refresh(ss);

    u8 temp;
    if (SG.rotate) {
        if (size_x == 8) {
            u16 a, b;
            for (u16 row = 0; row < 4; row++)
                for (u16 col = 0; col < 8; col++) {
                    a = (row << 4) + col;
                    b = ((7 - row) << 4) + 7 - col;
                    temp = monomeLedBuffer[a];
                    monomeLedBuffer[a] = monomeLedBuffer[b];
                    monomeLedBuffer[b] = temp;
                }
        }
        else {
            u16 total = size_x * size_y;
            if (total > MONOME_MAX_LED_BYTES) total = MONOME_MAX_LED_BYTES;
            for (u16 i = 0; i < (total >> 1); i++) {
                temp = monomeLedBuffer[i];
                monomeLedBuffer[i] = monomeLedBuffer[total - i - 1];
                monomeLedBuffer[total - i - 1] = temp;
            }
        }
    }

    SG.grid_dirty = 0;
}

void grid_fill_area(s8 x, s8 y, s8 w, s8 h, s8 level) {
    if (level == LED_OFF) return;

    u16 index;
    s16 x_end = min(size_x, x + w);
    s16 y_end = min(size_y, y + h);

    if (level == LED_DIM) {
        for (s16 _x = x; _x < x_end; _x++)
            for (s16 _y = y; _y < y_end; _y++) {
                index = _x + (_y << 4);
                if (index < MONOME_MAX_LED_BYTES) {
                    if (monomeLedBuffer[index] > 3)
                        monomeLedBuffer[index] -= 3;
                    else
                        monomeLedBuffer[index] = 0;
                }
            }
    }
    else if (level == LED_BRI) {
        for (s16 _x = x; _x < x_end; _x++)
            for (s16 _y = y; _y < y_end; _y++) {
                index = _x + (_y << 4);
                if (index < MONOME_MAX_LED_BYTES) {
                    if (monomeLedBuffer[index] > 12)
                        monomeLedBuffer[index] = 15;
                    else
                        monomeLedBuffer[index] += 3;
                }
            }
    }
    else {
        for (s16 _x = x; _x < x_end; _x++)
            for (s16 _y = y; _y < y_end; _y++) {
                index = _x + (_y << 4);
                if (index < MONOME_MAX_LED_BYTES)
                    monomeLedBuffer[index] = level;
            }
    }
}

///////////////////////////////////////// screen functions

void grid_screen_refresh(scene_state_t *ss, u8 is_full, u8 page, u8 ctrl, u8 x1,
                         u8 y1, u8 x2, u8 y2) {
    if (is_full) { grid_screen_refresh_led(ss, 1, page, x1, y1, x2, y2); }
    else {
        grid_screen_refresh_led(ss, 0, page, x1, y1, x2, y2);
        if (ctrl) grid_screen_refresh_ctrl(ss, page, x1, y1, x2, y2);
        grid_screen_refresh_info(ss, page, x1, y1, x2, y2);
    }
    SG.scr_dirty = 0;
}

void grid_screen_refresh_ctrl(scene_state_t *ss, u8 page, u8 x1, u8 y1, u8 x2,
                              u8 y2) {
    grid_fill_area_scr(0, 0, GRID_MAX_DIMENSION, GRID_MAX_DIMENSION, 0, 0);

    u8 last_x, last_y;

    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++) {
        if (!SG.group[GBC.group].enabled || !GBC.enabled) continue;
        last_x = GBC.x + GBC.w - 1;
        last_y = GBC.y + GBC.h - 1;
        if (GBC.w == 1 && GBC.h == 1) {
            grid_fill_area_scr(GBC.x, GBC.y, 1, 1, 1, page);
        }
        else if (GBC.w == 1 && GBC.h > 1) {
            grid_fill_area_scr(GBC.x, GBC.y, 1, 1, 2, page);
            grid_fill_area_scr(GBC.x, GBC.y + 1, 1, GBC.h - 2, 3, page);
            grid_fill_area_scr(GBC.x, last_y, 1, 1, 4, page);
        }
        else if (GBC.w > 1 && GBC.h == 1) {
            grid_fill_area_scr(GBC.x, GBC.y, 1, 1, 5, page);
            grid_fill_area_scr(GBC.x + 1, GBC.y, GBC.w - 2, 1, 6, page);
            grid_fill_area_scr(last_x, GBC.y, 1, 1, 7, page);
        }
        else {
            grid_fill_area_scr(GBC.x, GBC.y, 1, 1, 8, page);
            grid_fill_area_scr(GBC.x + 1, GBC.y, GBC.w - 2, 1, 9, page);
            grid_fill_area_scr(last_x, GBC.y, 1, 1, 10, page);
            grid_fill_area_scr(GBC.x, GBC.y + 1, 1, GBC.h - 2, 11, page);
            grid_fill_area_scr(last_x, GBC.y + 1, 1, GBC.h - 2, 12, page);
            grid_fill_area_scr(GBC.x, last_y, 1, 1, 13, page);
            grid_fill_area_scr(GBC.x + 1, last_y, GBC.w - 2, 1, 14, page);
            grid_fill_area_scr(last_x, last_y, 1, 1, 15, page);
        }
    }

    for (u16 i = 0; i < GRID_FADER_COUNT; i++) {
        if (!SG.group[GFC.group].enabled || !GFC.enabled) continue;
        last_x = GFC.x + GFC.w - 1;
        last_y = GFC.y + GFC.h - 1;
        if (GFC.w == 1 && GFC.h == 1) {
            grid_fill_area_scr(GFC.x, GFC.y, 1, 1, 1, page);
        }
        else if (GFC.w == 1 && GFC.h > 1) {
            grid_fill_area_scr(GFC.x, GFC.y, 1, 1, 2, page);
            grid_fill_area_scr(GFC.x, GFC.y + 1, 1, GFC.h - 2, 3, page);
            grid_fill_area_scr(GFC.x, last_y, 1, 1, 4, page);
        }
        else if (GFC.w > 1 && GFC.h == 1) {
            grid_fill_area_scr(GFC.x, GFC.y, 1, 1, 5, page);
            grid_fill_area_scr(GFC.x + 1, GFC.y, GFC.w - 2, 1, 6, page);
            grid_fill_area_scr(last_x, GFC.y, 1, 1, 7, page);
        }
        else {
            grid_fill_area_scr(GFC.x, GFC.y, 1, 1, 8, page);
            grid_fill_area_scr(GFC.x + 1, GFC.y, GFC.w - 2, 1, 9, page);
            grid_fill_area_scr(last_x, GFC.y, 1, 1, 10, page);
            grid_fill_area_scr(GFC.x, GFC.y + 1, 1, GFC.h - 2, 11, page);
            grid_fill_area_scr(last_x, GFC.y + 1, 1, GFC.h - 2, 12, page);
            grid_fill_area_scr(GFC.x, last_y, 1, 1, 13, page);
            grid_fill_area_scr(GFC.x + 1, last_y, GFC.w - 2, 1, 14, page);
            grid_fill_area_scr(last_x, last_y, 1, 1, 15, page);
        }
    }

    u8 l, _y, __y;
    u16 d;
    for (u16 y = 0; y < 48; y++) {
        l = y >> 3;
        d = 10 + ((y & 7) << 7);
        _y = y / 6;
        __y = y % 6;
        for (u16 x = 0; x < 96; x++)
            if ((1 << (5 - (x % 6))) & glyph[screen[x / 6][_y]][__y])
                line[l].data[x + d] = 10;
    }

    return;
}

void grid_screen_refresh_led(scene_state_t *ss, u8 full_grid, u8 page, u8 x1,
                             u8 y1, u8 x2, u8 y2) {
    grid_fill_area_scr(0, 0, GRID_MAX_DIMENSION, GRID_MAX_DIMENSION, 0, 0);

    u16 x, y;
    for (u8 i = 0; i < GRID_XYPAD_COUNT; i++) {
        if (GXYC.enabled && SG.group[GXYC.group].enabled) {
            if (GXY.value_x || GXY.value_y) {
                x = GXYC.x + GXY.value_x;
                y = GXYC.y + GXY.value_y;
                grid_fill_area_scr(GXYC.x, y, GXYC.w, 1, GXYC.level, page);
                grid_fill_area_scr(x, GXYC.y, 1, GXYC.h, GXYC.level, page);
                grid_fill_area_scr(x, y, 1, 1, GRID_ON_BRIGHTNESS, page);
            }
        }
    }

    u32 fv, ff, fp;
    for (u8 i = 0; i < GRID_FADER_COUNT; i++) {
        if (GFC.enabled && SG.group[GFC.group].enabled) {
            switch (GF.type) {
                case FADER_CH_BAR:
                    grid_fill_area_scr(GFC.x, GFC.y, GF.value + 1, GFC.h,
                                       GRID_ON_BRIGHTNESS, page);
                    grid_fill_area_scr(GFC.x + GF.value + 1, GFC.y,
                                       GFC.w - GF.value - 1, GFC.h, GFC.level,
                                       page);
                    break;
                case FADER_CV_BAR:
                    grid_fill_area_scr(GFC.x, GFC.y, GFC.w,
                                       GFC.h - GF.value - 1, GFC.level, page);
                    grid_fill_area_scr(GFC.x, GFC.y + GFC.h - GF.value - 1,
                                       GFC.w, GF.value + 1, GRID_ON_BRIGHTNESS,
                                       page);
                    break;
                case FADER_CH_DOT:
                    grid_fill_area_scr(GFC.x, GFC.y, GFC.w, GFC.h, GFC.level,
                                       page);
                    grid_fill_area_scr(GFC.x + GF.value, GFC.y, 1, GFC.h,
                                       GRID_ON_BRIGHTNESS, page);
                    break;
                case FADER_CV_DOT:
                    grid_fill_area_scr(GFC.x, GFC.y, GFC.w, GFC.h, GFC.level,
                                       page);
                    grid_fill_area_scr(GFC.x, GFC.y + GFC.h - GF.value - 1,
                                       GFC.w, 1, GRID_ON_BRIGHTNESS, page);
                    break;
                case FADER_FH_BAR:
                    fv = calc_fader_level(ss, i, 0);
                    ff = fv >> 4;
                    fp = fv & 15;
                    grid_fill_area_scr(GFC.x + 1, GFC.y, ff, GFC.h, 15, page);
                    if (fp)
                        grid_fill_area_scr(GFC.x + ff + 1, GFC.y, 1, GFC.h, fp,
                                           page);

                    grid_fill_area_scr(GFC.x, GFC.y, 1, GFC.h,
                                       GRID_ON_BRIGHTNESS, page);
                    grid_fill_area_scr(GFC.x + GFC.w - 1, GFC.y, 1, GFC.h,
                                       GRID_ON_BRIGHTNESS, page);
                    break;
                case FADER_FH_DOT:
                    fv = calc_fader_level(ss, i, 0);
                    ff = fv >> 4;
                    fp = fv & 15;
                    if (fp) {
                        if (GFC.w - 1 >= GFC.level)
                            fp = GRID_ON_BRIGHTNESS;
                        else if (fp < 3)
                            fp = 3;
                        grid_fill_area_scr(GFC.x + ff + 1, GFC.y, 1, GFC.h,
                                           fp < 3 ? 3 : fp, page);
                    }
                    else if (ff)
                        grid_fill_area_scr(GFC.x + ff, GFC.y, 1, GFC.h, 15,
                                           page);

                    grid_fill_area_scr(GFC.x, GFC.y, 1, GFC.h,
                                       GRID_ON_BRIGHTNESS, page);
                    grid_fill_area_scr(GFC.x + GFC.w - 1, GFC.y, 1, GFC.h,
                                       GRID_ON_BRIGHTNESS, page);
                    break;
                case FADER_FV_BAR:
                    fv = calc_fader_level(ss, i, 1);
                    ff = fv >> 4;
                    fp = fv & 15;
                    grid_fill_area_scr(GFC.x, GFC.y + GFC.h - 1 - ff, GFC.w, ff,
                                       15, page);
                    if (fp)
                        grid_fill_area_scr(GFC.x, GFC.y + GFC.h - 2 - ff, GFC.w,
                                           1, fp, page);

                    grid_fill_area_scr(GFC.x, GFC.y + GFC.h - 1, GFC.w, 1,
                                       GRID_ON_BRIGHTNESS, page);
                    grid_fill_area_scr(GFC.x, GFC.y, GFC.w, 1,
                                       GRID_ON_BRIGHTNESS, page);
                    break;
                case FADER_FV_DOT:
                    fv = calc_fader_level(ss, i, 1);
                    ff = fv >> 4;
                    fp = fv & 15;
                    if (fp) {
                        if (GFC.h - 1 >= GFC.level)
                            fp = GRID_ON_BRIGHTNESS;
                        else if (fp < 3)
                            fp = 3;
                        grid_fill_area_scr(GFC.x, GFC.y + GFC.h - ff - 2, GFC.w,
                                           1, fp, page);
                    }
                    else if (ff)
                        grid_fill_area_scr(GFC.x, GFC.y + GFC.h - ff - 1, GFC.w,
                                           1, 15, page);

                    grid_fill_area_scr(GFC.x, GFC.y + GFC.h - 1, GFC.w, 1,
                                       GRID_ON_BRIGHTNESS, page);
                    grid_fill_area_scr(GFC.x, GFC.y, GFC.w, 1,
                                       GRID_ON_BRIGHTNESS, page);
                    break;
            }
        }
    }

    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++)
        if (GBC.enabled && SG.group[GBC.group].enabled)
            grid_fill_area_scr(GBC.x, GBC.y, GBC.w, GBC.h,
                               GB.state ? GRID_ON_BRIGHTNESS : GBC.level, page);

    u16 pd = page ? 8 : 0;
    s8 l;
    for (u16 i = 0; i < GRID_MAX_DIMENSION; i++)
        for (u16 j = 0; j < GRID_MAX_DIMENSION / 2; j++) {
            l = SG.leds[i][j + pd];
            if (l >= 0)
                screen[i][j] = l;
            else if (l == LED_DIM) {
                if (screen[i][j] > 3)
                    screen[i][j] -= 3;
                else
                    screen[i][j] = 0;
            }
            else if (l == LED_BRI) {
                if (screen[i][j] > 12)
                    screen[i][j] = 15;
                else
                    screen[i][j] += 3;
            }
        }

    u16 _y, cell, size, left;
    if (full_grid) {
        cell = 8;
        size = 5;
        left = 0;
        for (int i = 0; i < 8; i++) region_fill(&line[i], 0);
    }
    else {
        cell = 6;
        size = 4;
        left = 10;
        for (int i = 0; i < 6; i++) region_fill(&line[i], 0);
    }

    u8 _line;
    u16 _data;
    for (u16 x = 0; x < GRID_MAX_DIMENSION; x++)
        for (u16 y = 0; y < GRID_MAX_DIMENSION / 2; y++)
            for (u16 j = 0; j < size; j++) {
                _y = y * cell + j + 1;
                _line = _y >> 3;
                for (u16 i = 0; i < size; i++) {
                    _data = left + x * cell + i + ((_y & 7) << 7) + 1;
                    if (_line > 7 || _data > 1023) continue;
                    if (screen[x][y] == 0) {
                        if (i == 0 || i == size - 1 || j == 0 || j == size - 1)
                            line[_line].data[_data] = 1;
                    }
                    else
                        line[_line].data[_data] = screen[x][y];
                }
            }

    // draw selected area

    u8 _x1, _x2;
    u16 _y1, _y2;
    _x1 = min(x1, x2) * cell;
    _y1 = min(y1, y2) * cell;
    if (full_grid) {
        _x2 = (max(x1, x2) + 1) * cell - 2;
        _y2 = (max(y1, y2) + 1) * cell - 2;
    }
    else {
        _x2 = (max(x1, x2) + 1) * cell - 1;
        _y2 = (max(y1, y2) + 1) * cell - 1;
    }

    u8 show_y1, show_y2;
    show_y1 = show_y2 = true;

    u16 p = cell << 3;
    if (page) {
        if (_y2 < p) return;
        if (_y1 < p) {
            show_y1 = false;
            _y1 = p;
        };
        _y1 -= p;
        _y2 -= p;
    }
    else {
        if (_y1 >= p) return;
        if (_y2 >= p) {
            show_y2 = false;
            _y2 = p - 1;
        }
    }

    if (show_y1) {
        _line = _y1 >> 3;
        _data = left + ((_y1 & 7) << 7);
        for (u16 x = _x1; x <= _x2; x++)
            line[_line].data[_data + x] = x & 1 ? 4 : 15;
    }
    if (show_y2) {
        _line = _y2 >> 3;
        _data = left + ((_y2 & 7) << 7);
        if (full_grid)
            for (u16 x = _x1 + 1; x <= _x2; x++)
                line[_line].data[_data + x] = x & 1 ? 4 : 15;
        else
            for (u16 x = _x1 + 1; x <= _x2; x++)
                line[_line].data[_data + x] = x & 1 ? 15 : 4;
    }

    if (full_grid)
        for (u16 y = _y1; y <= _y2; y++) {
            line[y >> 3].data[left + ((y & 7) << 7) + _x1] =
                line[y >> 3].data[left + ((y & 7) << 7) + _x2] = y & 1 ? 4 : 15;
        }
    else
        for (u16 y = _y1; y <= _y2; y++) {
            line[y >> 3].data[left + ((y & 7) << 7) + _x1] = y & 1 ? 4 : 15;
            line[y >> 3].data[left + ((y & 7) << 7) + _x2] = y & 1 ? 15 : 4;
        }
}

static void grid_screen_refresh_info(scene_state_t *ss, u8 page, u8 x1, u8 y1,
                                     u8 x2, u8 y2) {
    char s[32];
    u8 area_x, area_y, area_w, area_h;

    if (x1 < x2) {
        area_x = x1;
        area_w = x2 + 1 - x1;
    }
    else {
        area_x = x2;
        area_w = x1 + 1 - x2;
    }
    if (y1 < y2) {
        area_y = y1;
        area_h = y2 + 1 - y1;
    }
    else {
        area_y = y2;
        area_h = y1 + 1 - y2;
    }

    s[1] = 0;
    s[0] = 'G';
    font_string_region_clip_right(&line[0], s, 127, 0, 1, 0);
    s[0] = 'X';
    font_string_region_clip_right(&line[2], s, 127, 0, 1, 0);
    s[0] = 'Y';
    font_string_region_clip_right(&line[3], s, 127, 0, 1, 0);
    s[0] = 'W';
    font_string_region_clip_right(&line[4], s, 128, 0, 1, 0);
    s[0] = 'H';
    font_string_region_clip_right(&line[5], s, 127, 0, 1, 0);

    itoa(ss->grid.current_group, s, 10);
    font_string_region_clip_right(&line[0], s, 117, 0, 8, 0);
    itoa(area_x, s, 10);
    font_string_region_clip_right(&line[2], s, 117, 0, 8, 0);
    itoa(area_y, s, 10);
    font_string_region_clip_right(&line[3], s, 117, 0, 8, 0);
    itoa(area_w, s, 10);
    font_string_region_clip_right(&line[4], s, 117, 0, 8, 0);
    itoa(area_h, s, 10);
    font_string_region_clip_right(&line[5], s, 117, 0, 8, 0);

    for (u16 j = 0; j < 9; j += 2) line[j >> 3].data[119 + ((j & 7) << 7)] = 1;
    for (u16 j = 17; j < 48; j += 2)
        line[j >> 3].data[119 + ((j & 7) << 7)] = 1;

    // icons

    if (page == 0) {
        for (u16 i = 0; i < 5; i++)
            line[0].data[i] = line[0].data[i + 128] = 10;
        line[0].data[0 + 256] = 1;
        line[0].data[4 + 256] = 1;
        line[0].data[0 + 384] = 1;
        line[0].data[4 + 384] = 1;
        for (u16 i = 512; i < 517; i++) line[0].data[i] = 1;
    }
    else {
        for (u16 i = 0; i < 5; i++) line[0].data[i] = 1;
        line[0].data[0 + 128] = 1;
        line[0].data[4 + 128] = 1;
        line[0].data[0 + 256] = 1;
        line[0].data[4 + 256] = 1;
        for (u16 i = 384; i < 389; i++)
            line[0].data[i] = line[0].data[i + 128] = 10;
    }

    u8 l = ss->grid.rotate ? 10 : 1;
    line[1].data[0 + 0] = l;
    line[1].data[1 + 0] = l;
    line[1].data[2 + 0] = l;
    line[1].data[2 + 128] = l;
    line[1].data[0 + 256] = l;
    line[1].data[2 + 256] = l;
    line[1].data[4 + 256] = l;
    line[1].data[1 + 384] = l;
    line[1].data[2 + 384] = l;
    line[1].data[3 + 384] = l;
    line[1].data[2 + 512] = l;
}

void grid_fill_area_scr(s8 x, s8 y, s8 w, s8 h, s8 level, u8 page) {
    if (level == LED_OFF) return;

    s16 x_end = min(GRID_MAX_DIMENSION, x + w);
    s16 y_start = y;
    s16 y_end = min(GRID_MAX_DIMENSION, y + h);

    if (page) {
        if (y_end <= 8) return;
        if (y_start < 8) y_start = 8;
        y_start -= 8;
        y_end -= 8;
    }
    else {
        if (y_start >= 8) return;
        if (y_end > 8) y_end = 8;
    }

    if (level == LED_DIM) {
        for (s16 _x = x; _x < x_end; _x++)
            for (s16 _y = y_start; _y < y_end; _y++)
                if (screen[_x][_y] > 3)
                    screen[_x][_y] -= 3;
                else
                    screen[_x][_y] = 0;
    }
    else if (level == LED_BRI) {
        for (s16 _x = x; _x < x_end; _x++)
            for (s16 _y = y_start; _y < y_end; _y++)
                if (screen[_x][_y] > 12)
                    screen[_x][_y] = 15;
                else
                    screen[_x][_y] += 3;
    }
    else {
        for (s16 _x = x; _x < x_end; _x++)
            for (s16 _y = y_start; _y < y_end; _y++) screen[_x][_y] = level;
    }
}
