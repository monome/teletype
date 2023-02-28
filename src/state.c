#include "state.h"

#include <stdlib.h>
#include <string.h>

#include "helpers.h"
#include "teletype_io.h"

////////////////////////////////////////////////////////////////////////////////
// SCENE STATE /////////////////////////////////////////////////////////////////

void ss_init(scene_state_t *ss) {
    ss->initializing = true;
    ss_cal_init(ss);
    ss_variables_init(ss);
    ss_patterns_init(ss);
    ss_grid_init(ss);
    ss_rand_init(ss);
    ss_midi_init(ss);
    ss->delay.count = 0;
    for (size_t i = 0; i < TR_COUNT; i++) { ss->tr_pulse_timer[i] = 0; }
    for (size_t i = 0; i < NB_NBX_SCALES; i++) {
        ss->variables.n_scale_bits[i] = bit_reverse(0b101011010101, 12);
        ss->variables.n_scale_root[i] = 0;
    }
    ss->stack_op.top = 0;
    memset(&ss->scripts, 0, ss_scripts_size());
    turtle_init(&ss->turtle);
    uint32_t ticks = tele_get_ticks();
    for (size_t i = 0; i < TEMP_SCRIPT; i++) ss->scripts[i].last_time = ticks;
    ss->variables.time = 0;
    ss->variables.time_act = 1;
    ss->i2c_op_address = -1;
}

void ss_variables_init(scene_state_t *ss) {
    const scene_variables_t default_variables = {
        // variables that haven't been explicitly initialised, will be set to 0
        // TODO: verify no missing
        .a = 1,
        .b = 2,
        .c = 3,
        .cv_slew = { 1, 1, 1, 1 },
        .d = 4,
        .drunk_min = 0,
        .drunk_max = 255,
        .m = 1000,
        .m_act = 1,
        .o_inc = 1,
        .o_min = 0,
        .o_max = 63,
        .o_wrap = 1,
        .q_n = 1,
        .q_grow = 0,
        .r_min = 0,
        .r_max = 16383,
        .script_pol = { 1, 1, 1, 1, 1, 1, 1, 1 },
        .time_act = 1,
        .tr_pol = { 1, 1, 1, 1 },
        .tr_time = { 100, 100, 100, 100 },
        .in_range = { 0, 16383 },
        .param_range = { 0, 16383 },
        .fader_ranges = {
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
            { 0, 16383 }, { 0, 16383 }, { 0, 16383 }, { 0, 16383 },
        },
    };

    memcpy(&ss->variables, &default_variables, sizeof(default_variables));
    tele_update_adc(1);
    ss_update_param_scale(ss);
    ss_update_in_scale(ss);
    ss_update_fader_scale_all(ss);
}

void ss_patterns_init(scene_state_t *ss) {
    for (size_t i = 0; i < PATTERN_COUNT; i++) { ss_pattern_init(ss, i); }
}

void ss_pattern_init(scene_state_t *ss, size_t pattern_no) {
    if (pattern_no >= PATTERN_COUNT) return;

    scene_pattern_t *p = &ss->patterns[pattern_no];
    p->idx = 0;
    p->len = 0;
    p->wrap = 1;
    p->start = 0;
    p->end = 63;
    for (size_t i = 0; i < PATTERN_LENGTH; i++) { p->val[i] = 0; }
}

// grid

void ss_grid_init(scene_state_t *ss) {
    ss->grid.rotate = 0;
    ss->grid.dim = 0;

    ss->grid.current_group = 0;
    ss->grid.latest_group = 0;
    ss->grid.latest_button = 0;
    ss->grid.latest_fader = 0;

    for (u8 i = 0; i < GRID_MAX_DIMENSION; i++)
        for (u8 j = 0; j < GRID_MAX_DIMENSION; j++)
            ss->grid.leds[i][j] = LED_OFF;

    for (u8 i = 0; i < GRID_GROUP_COUNT; i++) {
        ss->grid.group[i].enabled = true;
        ss->grid.group[i].script = -1;
        ss->grid.group[i].fader_min = 0;
        ss->grid.group[i].fader_max = 16383;
    }

    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++) {
        ss_grid_common_init(&(ss->grid.button[i].common));
        ss->grid.button[i].latch = 0;
        ss->grid.button[i].state = 0;
    }

    for (u8 i = 0; i < GRID_FADER_COUNT; i++) {
        ss_grid_common_init(&(ss->grid.fader[i].common));
        ss->grid.fader[i].type = FADER_CH_BAR;
        ss->grid.fader[i].value = 0;
        ss->grid.fader[i].slide = 0;
    }

    for (u8 i = 0; i < GRID_XYPAD_COUNT; i++) {
        ss_grid_common_init(&(ss->grid.xypad[i].common));
        ss->grid.xypad[i].value_x = 0;
        ss->grid.xypad[i].value_y = 0;
    }

    ss->grid.grid_dirty = ss->grid.scr_dirty = ss->grid.clear_held = true;
}

void ss_grid_common_init(grid_common_t *gc) {
    gc->enabled = false;
    gc->group = 0;
    gc->x = gc->y = 0;
    gc->w = gc->h = 1;
    gc->level = 5;
    gc->script = -1;
}

// rand

void ss_rand_init(scene_state_t *ss) {
    for (u8 i = 0; i < RAND_STATES_COUNT; i++) {
        tele_rand_t *r = &ss->rand_states.a[i];
        r->seed = rand();
        random_seed(&r->rand, r->seed);
    }
}

// MIDI

void ss_midi_init(scene_state_t *ss) {
    ss->midi.on_script = -1;
    ss->midi.off_script = -1;
    ss->midi.cc_script = -1;
    ss->midi.clk_script = -1;
    ss->midi.start_script = -1;
    ss->midi.stop_script = -1;
    ss->midi.continue_script = -1;

    ss->midi.last_event_type = 0;
    ss->midi.last_channel = 0;
    ss->midi.last_note = 0;
    ss->midi.last_velocity = 0;
    ss->midi.last_controller = 0;
    ss->midi.last_cc = 0;

    ss->midi.on_count = 0;
    ss->midi.off_count = 0;
    ss->midi.cc_count = 0;
    for (u8 i = 0; i < MAX_MIDI_EVENTS; i++) {
        ss->midi.note_on[i] = 0;
        ss->midi.note_vel[i] = 0;
        ss->midi.note_off[i] = 0;
        ss->midi.cn[i] = 0;
        ss->midi.cc[i] = 0;
        ss->midi.on_channel[i] = 0;
        ss->midi.off_channel[i] = 0;
        ss->midi.cc_channel[i] = 0;
    }
    ss->midi.clock_div = 24;
}

void ss_cal_init(scene_state_t *ss) {
    ss->cal = blank_cal_data;
}

// Hardware

void ss_set_in(scene_state_t *ss, int16_t value) {
    ss->variables.in = value;
}

void ss_set_param(scene_state_t *ss, int16_t value) {
    ss->variables.param = value;
}

void ss_set_scene(scene_state_t *ss, int16_t value) {
    ss->variables.scene = value;
}

uint8_t ss_get_script_pol(scene_state_t *ss, size_t idx) {
    return ss->variables.script_pol[idx];
}

void ss_set_script_pol(scene_state_t *ss, size_t idx, uint8_t value) {
    ss->variables.script_pol[idx] = value;
    tele_mute();  // to redraw indicators
}

// mutes
// TODO: size_t SHOULD be a script_number_t
bool ss_get_mute(scene_state_t *ss, size_t idx) {
    return ss->variables.mutes[idx];
}

void ss_set_mute(scene_state_t *ss, size_t idx, bool value) {
    ss->variables.mutes[idx] = value;
    tele_mute();
}

// pattern getters and setters

int16_t ss_get_pattern_idx(scene_state_t *ss, size_t pattern) {
    return ss->patterns[pattern].idx;
}

void ss_set_pattern_idx(scene_state_t *ss, size_t pattern, int16_t i) {
    ss->patterns[pattern].idx = i;
}

int16_t ss_get_pattern_len(scene_state_t *ss, size_t pattern) {
    return ss->patterns[pattern].len;
}

void ss_set_pattern_len(scene_state_t *ss, size_t pattern, int16_t l) {
    ss->patterns[pattern].len = l;
}

uint16_t ss_get_pattern_wrap(scene_state_t *ss, size_t pattern) {
    return ss->patterns[pattern].wrap;
}

void ss_set_pattern_wrap(scene_state_t *ss, size_t pattern, uint16_t wrap) {
    ss->patterns[pattern].wrap = wrap;
}

int16_t ss_get_pattern_start(scene_state_t *ss, size_t pattern) {
    return ss->patterns[pattern].start;
}

void ss_set_pattern_start(scene_state_t *ss, size_t pattern, int16_t start) {
    ss->patterns[pattern].start = start;
}

int16_t ss_get_pattern_end(scene_state_t *ss, size_t pattern) {
    return ss->patterns[pattern].end;
}

void ss_set_pattern_end(scene_state_t *ss, size_t pattern, int16_t end) {
    ss->patterns[pattern].end = end;
}

int16_t ss_get_pattern_val(scene_state_t *ss, size_t pattern, size_t idx) {
    return ss->patterns[pattern].val[idx];
}

void ss_set_pattern_val(scene_state_t *ss, size_t pattern, size_t idx,
                        int16_t val) {
    ss->patterns[pattern].val[idx] = val;
}

scene_pattern_t *ss_patterns_ptr(scene_state_t *ss) {
    return ss->patterns;
}

size_t ss_patterns_size() {
    return sizeof(scene_pattern_t) * PATTERN_COUNT;
}

// script manipulation

uint8_t ss_get_script_len(scene_state_t *ss, script_number_t idx) {
    return ss->scripts[idx].l;
}

// private
static void ss_set_script_len(scene_state_t *ss, script_number_t idx,
                              uint8_t l) {
    ss->scripts[idx].l = l;
}

const tele_command_t *ss_get_script_command(scene_state_t *ss,
                                            script_number_t script_idx,
                                            size_t c_idx) {
    return &ss->scripts[script_idx].c[c_idx];
}

void ss_copy_script_command(tele_command_t *dest, scene_state_t *ss,
                            script_number_t script_idx, size_t c_idx) {
    memcpy(dest, &ss->scripts[script_idx].c[c_idx], sizeof(tele_command_t));
}

// private
static void ss_set_script_command(scene_state_t *ss, script_number_t script_idx,
                                  size_t c_idx, const tele_command_t *cmd) {
    memcpy(&ss->scripts[script_idx].c[c_idx], cmd, sizeof(tele_command_t));
}

bool ss_get_script_comment(scene_state_t *ss, script_number_t script_idx,
                           size_t c_idx) {
    return ss->scripts[script_idx].c[c_idx].comment;
}

void ss_set_script_comment(scene_state_t *ss, script_number_t script_idx,
                           size_t c_idx, uint8_t on) {
    ss->scripts[script_idx].c[c_idx].comment = on;
}

void ss_toggle_script_comment(scene_state_t *ss, script_number_t script_idx,
                              size_t c_idx) {
    ss->scripts[script_idx].c[c_idx].comment =
        !ss->scripts[script_idx].c[c_idx].comment;
}

void ss_overwrite_script_command(scene_state_t *ss, script_number_t script_idx,
                                 size_t command_idx,
                                 const tele_command_t *cmd) {
    // Few of the commands in this file bounds-check.
    // Are we trusting calling code in this file or not?
    // If so, why here?  If not, we need much more bounds-checking
    // If we start running up against processor limits, we should not
    // Well-validated upstream code doesn't _need_ bounds-checking here IMO
    // -- burnsauce (sliderule)

    // TODO: why check upper bound here but not lower?
    if (command_idx >= SCRIPT_MAX_COMMANDS) return;

    ss_set_script_command(ss, script_idx, command_idx, cmd);

    const uint8_t script_len = ss_get_script_len(ss, script_idx);

    if (script_len < SCRIPT_MAX_COMMANDS && command_idx >= script_len) {
        ss_set_script_len(ss, script_idx, script_len + 1);
    }
}

void ss_insert_script_command(scene_state_t *ss, script_number_t script_idx,
                              size_t command_idx, const tele_command_t *cmd) {
    if (command_idx >= SCRIPT_MAX_COMMANDS) return;

    uint8_t script_len = ss_get_script_len(ss, script_idx);
    if (script_len == SCRIPT_MAX_COMMANDS) {  // no room to insert
        ss_delete_script_command(ss, script_idx, script_len - 1);  // make room
        script_len = ss_get_script_len(ss, script_idx);
    }

    // shuffle down
    for (size_t i = script_len; i > command_idx; i--) {
        const tele_command_t *cmd =
            ss_get_script_command(ss, script_idx, i - 1);
        ss_set_script_command(ss, script_idx, i, cmd);
    }

    // increase length
    ss_set_script_len(ss, script_idx, script_len + 1);

    // overwrite at command_idx
    ss_overwrite_script_command(ss, script_idx, command_idx, cmd);
}

void ss_delete_script_command(scene_state_t *ss, script_number_t script_idx,
                              size_t command_idx) {
    uint8_t script_len = ss_get_script_len(ss, script_idx);
    if (command_idx >= SCRIPT_MAX_COMMANDS || command_idx >= script_len) return;

    if (script_len &&
        ss_get_script_command(ss, script_idx, command_idx)->length) {
        script_len--;
        ss_set_script_len(ss, script_idx, script_len);

        for (size_t n = command_idx; n < script_len; n++) {
            const tele_command_t *cmd =
                ss_get_script_command(ss, script_idx, n + 1);
            ss_set_script_command(ss, script_idx, n, cmd);
        }

        tele_command_t blank_command;
        blank_command.length = 0;
        blank_command.comment = false;
        ss_set_script_command(ss, script_idx, script_len, &blank_command);
    }
}

void ss_clear_script(scene_state_t *ss, size_t script_idx) {
    memset(&ss->scripts[script_idx], 0, sizeof(scene_script_t));
    ss->variables.j[script_idx] = 0;
    ss->variables.k[script_idx] = 0;
}

scene_script_t *ss_scripts_ptr(scene_state_t *ss) {
    return ss->scripts;
}

size_t ss_scripts_size() {
    return sizeof(scene_script_t) * SCRIPT_COUNT;
}

int16_t ss_get_script_last(scene_state_t *ss, script_number_t idx) {
    if (idx < TT_SCRIPT_1) return 0;
    if (idx > INIT_SCRIPT) return 0;
    uint32_t delta = (tele_get_ticks() - ss->scripts[idx].last_time) & 0x7fff;
    return delta;
}

void ss_update_script_last(scene_state_t *ss, script_number_t idx) {
    ss->scripts[idx].last_time = tele_get_ticks();
}

every_count_t *ss_get_every(scene_state_t *ss, script_number_t idx,
                            uint8_t line_number) {
    return &ss->scripts[idx].every[line_number];
}

void ss_sync_every(scene_state_t *ss, int16_t count) {
    for (int script = 0; script < SCRIPT_COUNT; script++)
        for (int line = 0; line < SCRIPT_MAX_COMMANDS; line++) {
            int16_t count_e = count;
            if (ss->scripts[script].every[line].mod == 0)
                ss->scripts[script].every[line].mod = 1;  // lazy init
            while (count_e < 0) count_e += ss->scripts[script].every[line].mod;
            ss->scripts[script].every[line].count = count_e;
        }
}

bool every_is_now(scene_state_t *ss, every_count_t *e) {
    ss->every_last = e->count == 0;
    return e->count == 0;
}

bool skip_is_now(scene_state_t *ss, every_count_t *e) {
    ss->every_last = e->count != 0;
    return e->count != 0;
}


int16_t ss_turtle_get_val(scene_state_t *ss, scene_turtle_t *st) {
    turtle_position_t p;
    turtle_resolve_position(st, &st->position, &p);
    if (p.x > 3 || p.x < 0 || p.y > 63 || p.y < 0) return 0;
    return ss_get_pattern_val(ss, p.x, p.y);
}

void ss_turtle_set_val(scene_state_t *ss, scene_turtle_t *st, int16_t val) {
    turtle_position_t p;
    turtle_resolve_position(st, &st->position, &p);
    if (p.x > 3 || p.x < 0 || p.y > 63 || p.y < 0) return;
    ss_set_pattern_val(ss, p.x, p.y, val);
}

void ss_turtle_set(scene_state_t *ss, scene_turtle_t *ts) {
    // TODO validate the turtle?
    ss->turtle = *ts;  // structs shallow copy with value assignment
}

scene_turtle_t *ss_turtle_get(scene_state_t *ss) {
    return &ss->turtle;
}

void ss_update_param_scale(scene_state_t *ss) {
    ss->variables.param_scale = scale_init(ss->cal.p_min, ss->cal.p_max,
                                           ss->variables.param_range.out_min,
                                           ss->variables.param_range.out_max);
}

void ss_update_fader_scale(scene_state_t *ss, int16_t fader) {
    ss->variables.fader_scales[fader] =
        scale_init(ss->cal.f_min[fader], ss->cal.f_max[fader],
                   ss->variables.fader_ranges[fader].out_min,
                   ss->variables.fader_ranges[fader].out_max);
}

void ss_update_fader_scale_all(scene_state_t *ss) {
    for (size_t fader = 0; fader < 64; fader++) {
        ss_update_fader_scale(ss, fader);
    }
}

void ss_update_in_scale(scene_state_t *ss) {
    ss->variables.in_scale =
        scale_init(ss->cal.i_min, ss->cal.i_max, ss->variables.in_range.out_min,
                   ss->variables.in_range.out_max);
}

void ss_set_param_scale(scene_state_t *ss, int16_t min, int16_t max) {
    ss->variables.param_range.out_min = min;
    ss->variables.param_range.out_max = max;
    ss_update_param_scale(ss);
}

void ss_set_in_scale(scene_state_t *ss, int16_t min, int16_t max) {
    ss->variables.in_range.out_min = min;
    ss->variables.in_range.out_max = max;
    ss_update_in_scale(ss);
}

void ss_set_fader_scale(scene_state_t *ss, int16_t fader, int16_t min,
                        int16_t max) {
    ss->variables.fader_ranges[fader].out_min = min;
    ss->variables.fader_ranges[fader].out_max = max;
    ss_update_fader_scale(ss, fader);
}

int16_t ss_get_param(scene_state_t *ss) {
    return scale_get(ss->variables.param_scale, ss->variables.param);
}

int16_t ss_get_in(scene_state_t *ss) {
    return scale_get(ss->variables.in_scale, ss->variables.in);
}

int16_t ss_get_param_min(scene_state_t *ss) {
    return ss->cal.p_min;
}

int16_t ss_get_param_max(scene_state_t *ss) {
    return ss->cal.p_max;
}

int16_t ss_get_fader_min(scene_state_t *ss, int16_t fader) {
    return ss->cal.f_min[fader];
}

int16_t ss_get_fader_max(scene_state_t *ss, int16_t fader) {
    return ss->cal.f_max[fader];
}

void ss_set_param_min(scene_state_t *ss, int16_t min) {
    ss->cal.p_min = min;
    ss_update_param_scale(ss);
    tele_save_calibration();
}

void ss_set_param_max(scene_state_t *ss, int16_t max) {
    ss->cal.p_max = max;
    ss_update_param_scale(ss);
    tele_save_calibration();
}

void ss_set_fader_min(scene_state_t *ss, int16_t fader, int16_t min) {
    ss->cal.f_min[fader] = min;
    ss_update_fader_scale(ss, fader);
    tele_save_calibration();
}

void ss_set_fader_max(scene_state_t *ss, int16_t fader, int16_t max) {
    ss->cal.f_max[fader] = max;
    ss_update_fader_scale(ss, fader);
    tele_save_calibration();
}

void ss_reset_param_cal(scene_state_t *ss) {
    ss->cal.p_max = 16383;
    ss->cal.p_min = 0;
    ss_update_param_scale(ss);
    tele_save_calibration();
}

void ss_reset_fader_cal(scene_state_t *ss, int16_t fader) {
    ss->cal.f_max[fader] = 16383;
    ss->cal.f_min[fader] = 0;
    ss_update_fader_scale(ss, fader);
    tele_save_calibration();
}

int16_t ss_get_in_min(scene_state_t *ss) {
    return ss->cal.i_min;
}

int16_t ss_get_in_max(scene_state_t *ss) {
    return ss->cal.i_max;
}

void ss_set_in_min(scene_state_t *ss, int16_t min) {
    ss->cal.i_min = min;
    ss_update_in_scale(ss);
    tele_save_calibration();
}

void ss_set_in_max(scene_state_t *ss, int16_t max) {
    ss->cal.i_max = max;
    ss_update_in_scale(ss);
    tele_save_calibration();
}

void ss_reset_in_cal(scene_state_t *ss) {
    ss->cal.i_max = 16383;
    ss->cal.i_min = 0;
    ss_update_in_scale(ss);
    tele_save_calibration();
}

////////////////////////////////////////////////////////////////////////////////
// EXEC STATE //////////////////////////////////////////////////////////////////

void es_init(exec_state_t *es) {
    es->exec_depth = 0;
    es->overflow = false;
}

size_t es_depth(exec_state_t *es) {
    return es->exec_depth;
}

size_t es_push(exec_state_t *es) {
    // I'd cache es->variables[es->exec_depth] as an optimization,
    // but the compiler will probably do it for me?
    if (es->exec_depth < EXEC_DEPTH) {
        es->variables[es->exec_depth].delayed = false;
        es->variables[es->exec_depth].while_depth = 0;
        es->variables[es->exec_depth].while_continue = false;
        if (es->exec_depth > 0) {
            es->variables[es->exec_depth].if_else_condition =
                es->variables[es->exec_depth - 1].if_else_condition;
            es->variables[es->exec_depth].i =
                es->variables[es->exec_depth - 1].i;
        }
        else {
            es->variables[es->exec_depth].if_else_condition = true;
            es->variables[es->exec_depth].i = 0;
        }
        es->variables[es->exec_depth].breaking = false;
        es->exec_depth += 1;  // exec_depth = 1 at the root
    }
    else
        es->overflow = true;
    return es->exec_depth;
}

size_t es_pop(exec_state_t *es) {
    if (es->exec_depth > 0) es->exec_depth -= 1;
    return es->exec_depth;
}

void es_set_script_number(exec_state_t *es, uint8_t script_number) {
    if (!es_variables(es)->delayed)
        es_variables(es)->script_number = script_number;
}

void es_set_line_number(exec_state_t *es, uint8_t line_number) {
    es_variables(es)->line_number = line_number;
}

uint8_t es_get_line_number(exec_state_t *es) {
    return es_variables(es)->line_number;
}

exec_vars_t *es_variables(exec_state_t *es) {
    return &es->variables[es->exec_depth - 1];  // but array is 0-indexed
}

////////////////////////////////////////////////////////////////////////////////
// COMMAND STATE ///////////////////////////////////////////////////////////////

void cs_init(command_state_t *cs) {
    cs->stack.top = 0;
}

int16_t cs_stack_size(command_state_t *cs) {
    return cs->stack.top;
}
