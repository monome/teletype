#include "state.h"

#include <string.h>

#include "teletype_io.h"

////////////////////////////////////////////////////////////////////////////////
// SCENE STATE /////////////////////////////////////////////////////////////////

void ss_init(scene_state_t *ss) {
    ss->initializing = true;
    ss_variables_init(ss);
    ss_patterns_init(ss);
    ss->delay.count = 0;
    for (size_t i = 0; i < TR_COUNT; i++) { ss->tr_pulse_timer[i] = 0; }
    ss->stack_op.top = 0;
    memset(&ss->scripts, 0, ss_scripts_size());
    turtle_init(&ss->turtle);
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
        .time_act = 1,
        .tr_pol = { 1, 1, 1, 1 },
        .tr_time = { 100, 100, 100, 100 }
    };

    memcpy(&ss->variables, &default_variables, sizeof(default_variables));
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

// external variable setting

void ss_set_in(scene_state_t *ss, int16_t value) {
    ss->variables.in = value;
}

void ss_set_param(scene_state_t *ss, int16_t value) {
    ss->variables.param = value;
}

void ss_set_scene(scene_state_t *ss, int16_t value) {
    ss->variables.scene = value;
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

// private
static void ss_set_script_command(scene_state_t *ss, script_number_t script_idx,
                                  size_t c_idx, const tele_command_t *cmd) {
    memcpy(&ss->scripts[script_idx].c[c_idx], cmd, sizeof(tele_command_t));
}

bool ss_get_script_comment(scene_state_t *ss, script_number_t script_idx,
                           size_t c_idx) {
    return ss->scripts[script_idx].comment[c_idx];
}

void ss_toggle_script_comment(scene_state_t *ss, script_number_t script_idx,
                              size_t c_idx) {
    ss->scripts[script_idx].comment[c_idx] =
        !ss->scripts[script_idx].comment[c_idx];
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
    if (command_idx >= SCRIPT_MAX_COMMANDS) return;

    uint8_t script_len = ss_get_script_len(ss, script_idx);
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
        ss_set_script_command(ss, script_idx, script_len, &blank_command);
    }
}

void ss_clear_script(scene_state_t *ss, size_t script_idx) {
    ss_set_script_len(ss, script_idx, 0);
}

scene_script_t *ss_scripts_ptr(scene_state_t *ss) {
    return ss->scripts;
}

size_t ss_scripts_size() {
    return sizeof(scene_script_t) * SCRIPT_COUNT;
}

int16_t ss_get_script_last(scene_state_t *ss, script_number_t idx) {
    int16_t now = ss->variables.time;
    if (idx < TT_SCRIPT_1) return 0;
    if (idx > INIT_SCRIPT) return 0;
    int16_t last = ss->scripts[idx].last_time;
    if (now < last)
        return (INT16_MAX - last) + (now - INT16_MIN);  // I must be dense?
    return now - last;
}

void ss_update_script_last(scene_state_t *ss, script_number_t idx) {
    ss->scripts[idx].last_time = ss->variables.time;
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
        es->variables[es->exec_depth].if_else_condition = true;
        es->variables[es->exec_depth].i = 0;
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
