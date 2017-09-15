#include "state.h"

#include <string.h>
#include <math.h>

#include "teletype_io.h"


////////////////////////////////////////////////////////////////////////////////
// SCENE STATE /////////////////////////////////////////////////////////////////

// scene init

void ss_init(scene_state_t *ss) {
    ss->initializing = true;
    ss_variables_init(ss);
    ss_patterns_init(ss);
    ss->delay.count = 0;
    for (size_t i = 0; i < TR_COUNT; i++) { ss->tr_pulse_timer[i] = 0; }
    ss->stack_op.top = 0;
    scene_turtle_t t = { 
            .home = { .x = 0, .y = 0 },
            .position = { .x = 0, .y = 0},
            .fence = { .x1 = 0, .y1 = 0, .x2 = 3, .y2 = 63},
            .mode = TURTLE_BUMP,
            .heading = 180,
            .velocity = 1
            };
    ss->turtle = t;
    memset(&ss->scripts, 0, ss_scripts_size());
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
static void ss_set_script_len(scene_state_t *ss, script_number_t idx, uint8_t l) {
    ss->scripts[idx].l = l;
}

const tele_command_t *ss_get_script_command(scene_state_t *ss,
                                            script_number_t script_idx, size_t c_idx) {
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
    ss->scripts[script_idx].comment[c_idx] = !
        ss->scripts[script_idx].comment[c_idx];
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
    for (uint8_t i = 0; i < SCRIPT_MAX_COMMANDS; i++)
        if (ss_get_script_len(ss, script_idx))
            ss_delete_script_command(ss, script_idx, i);
}

scene_script_t *ss_scripts_ptr(scene_state_t *ss) {
    return ss->scripts;
}

size_t ss_scripts_size() {
    return sizeof(scene_script_t) * SCRIPT_COUNT;
}

int16_t ss_get_script_last(scene_state_t *ss, script_number_t idx) {
    int16_t now = ss->variables.time;
    int16_t last = ss->scripts[idx].last_time;
    if (now < last) 
        return (INT16_MAX - last) + (now - INT16_MIN); // I must be dense? 
    return now - last;
}

void ss_update_script_last(scene_state_t *ss, script_number_t idx) {
    ss->scripts[idx].last_time = ss->variables.time;
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
        es->exec_depth += 1;                   // exec_depth = 1 at the root
    }
    else
        es->overflow = true;
    return es->exec_depth;
}

size_t es_pop(exec_state_t *es) {
    if (es->exec_depth > 0)
        es->exec_depth -= 1; 
    return es->exec_depth;
}

void es_set_script_number(exec_state_t *es, uint8_t script_number) {
    if (!es_variables(es)->delayed)
        es_variables(es)->script_number = script_number;
}

exec_vars_t *es_variables(exec_state_t *es) {
    return &es->variables[es->exec_depth - 1]; // but array is 0-indexed
}

////////////////////////////////////////////////////////////////////////////////
// COMMAND STATE ///////////////////////////////////////////////////////////////

void cs_init(command_state_t *cs) {
    cs->stack.top = 0;
}

int16_t cs_stack_size(command_state_t *cs) {
    return cs->stack.top;
}


////////////////////////////////////////////////////////////////////////////////
// TURTLE STATE ////////////////////////////////////////////////////////////////


void scene_set_turtle(scene_state_t *ss, scene_turtle_t *ts) {
    //TODO validate the turtle?
    ss->turtle = *ts; // structs copy with value assignment
}


scene_turtle_t* scene_get_turtle(scene_state_t *ss) {
    return &ss->turtle;
}

typedef struct {
    uint16_t x1, y1, x2, y2;
} normal_fence_t;

// 0-63 is 6 bits, so on a 16-bit signed integer, we have 9 to use.
// :. the internal turtle position system has 9 bits right of the point.
#define TURTLE_QBITS 9

static inline normal_fence_t normalize_fence(turtle_fence_t in) {
    normal_fence_t out;

    out.x1 = in.x1 << TURTLE_QBITS;
    out.x2 = in.x2 << TURTLE_QBITS;
    out.y1 = in.y1 << TURTLE_QBITS;
    out.y2 = in.y2 << TURTLE_QBITS;
    return out;
}

#define min(X, Y) ((X) < (Y) ? (X) : (Y))
#define max(X, Y) ((X) > (Y) ? (X) : (Y))

void turtle_normalize_position(scene_turtle_t *t, turtle_position_t *tp, 
        turtle_mode_t mode) {
    normal_fence_t f = normalize_fence(t->fence);
    uint16_t fxl = f.x2 - f.x1 + 1; // fences are inclusive so 0 to 0 is L:1
    uint16_t fyl = f.y2 - f.y1 + 1;
  
    switch (mode) {
        case TURTLE_BUMP:
            tp->x = max(min(tp->x, f.x2), f.x1);
            tp->y = max(min(tp->y, f.y2), f.y1);
            break;
        case TURTLE_WRAP:
            tp->x = (tp->x % (f.x2 + 1)) + f.x1;
            tp->y = (tp->y % (f.x2 + 1)) + f.y1;
            break;
        // TODO: check these maths and finish
        // HINT: it's not correct at the edges for offsets larger than the
        //       pattern, needs to wavefold.
        case TURTLE_BOUNCE:
            if (tp->x > f.x2)
                tp->x = f.x2 - ((tp->x - f.x1) % fxl);
            else if (tp->x < f.x1)
                tp->x = f.x1 + (-(tp->x - f.x1) % fxl); 
            if (tp->y > f.y2)
                tp->y = f.y2 - ((tp->y - f.y1) % fyl);
            else if (tp->x < f.x1)
                tp->y = f.y1 + (-(tp->y - f.y1) % fyl); 
            break;
    }
}
#undef min
#undef max

#define Q_1 (1 << TURTLE_QBITS)
#define Q_POINT_5 (1 << (TURTLE_QBITS - 1))
void turtle_resolve_position(scene_turtle_t *t, turtle_position_t *src,
                             turtle_position_t *dst) {
    *dst = *src;
    if (dst->x % Q_1 >= Q_POINT_5)
        dst->x = dst->x / Q_1 + 1;
    else
        dst->x = dst->x / Q_1;
    if (dst->y % Q_1 >= Q_POINT_5)
        dst->y = dst->y / Q_1 + 1;
    else
        dst->y = dst->y / Q_1;

}
#undef Q_1
#undef Q_POINT_5

uint8_t turtle_get_x(scene_turtle_t *st) {
    turtle_position_t t;
    turtle_resolve_position(st, &st->position, &t);
    return t.x;
}

void turtle_set_x(scene_turtle_t *st, uint8_t x) {
    st->position.x = x << TURTLE_QBITS;
    turtle_normalize_position(st, &st->position, TURTLE_BUMP);
}

uint8_t turtle_get_y(scene_turtle_t *st) {
    turtle_position_t t;
    turtle_resolve_position(st, &st->position, &t);
    return t.y;
}

void turtle_set_y(scene_turtle_t *st, uint8_t y) {
    st->position.y = y << TURTLE_QBITS;
    turtle_normalize_position(st, &st->position, TURTLE_BUMP);
}

void turtle_goto(scene_turtle_t *st, turtle_position_t *tp) {
    st->position = *tp;
    turtle_normalize_position(st, &st->position, TURTLE_BUMP);
}

void turtle_step(scene_turtle_t *st) {
    // TODO watch out, it's a doozie ;)
    int16_t dx, dy, sign;

    // Sorry, little processor!
    // TODO: fixed point sin()
    double h_rad = st->heading * M_PI / 180;
    double dx_d = 5 * cos(h_rad);
    double dy_d = 5 * sin(h_rad);

    dx = dx_d;
    dy = dy_d;
    dx_d -= dx;
    dy_d -= dy;
    sign = dx < 0 ? -1 : 1;
    dx_d *= sign;
    if (dx_d * sign >= 0.5)
        dx += sign;
    sign = dy < 0 ? -1 : 1;
    if (dy_d * sign >= 0.5)
        dy += sign;

    st->position.x += dx << TURTLE_QBITS;
    st->position.y += dy << TURTLE_QBITS;
    turtle_normalize_position(st, &st->position, st->mode);
}

int16_t  turtle_get(scene_state_t *ss, scene_turtle_t *st) {
    turtle_position_t p;
    turtle_resolve_position(st, &st->position, &p);
    return ss_get_pattern_val(ss, p.x, p.y);
}

void turtle_set(scene_state_t *ss, scene_turtle_t *st, int16_t val) {
    turtle_position_t p;
    turtle_resolve_position(st, &st->position, &p);
    return ss_set_pattern_val(ss, p.x, p.y, val);
}

void turtle_set_home(scene_turtle_t *st, uint8_t x, uint8_t y) {
    turtle_position_t h = { .x = x << TURTLE_QBITS, .y = y << TURTLE_QBITS };
    st->home = h;
    turtle_normalize_position(st, &st->home, TURTLE_BUMP);
}

uint8_t  turtle_get_home_x(scene_turtle_t *st) {
    turtle_position_t t;
    turtle_resolve_position(st, &st->home, &t);
    return t.x;
}

uint8_t  turtle_get_home_y(scene_turtle_t *st) {
    turtle_position_t t;
    turtle_resolve_position(st, &st->home, &t);
    return t.y;
}

turtle_fence_t turtle_get_fence(scene_turtle_t *st) {
    return st->fence;
}

void turtle_set_fence(scene_turtle_t *st, uint8_t x1, uint8_t y1, 
                                          uint8_t x2, uint8_t y2) {
    uint8_t t;
    x1 = x1 > 3 ? 3 : x1;
    x2 = x2 > 3 ? 3 : x2; 
    y1 = y1 > 63 ? 63 : y1;
    y2 = y2 > 63 ? 63 : y2; 
    
    if (x1 > x2) {
        t = x2;
        x2 = x1;
        x1 = t;
    }
    if (y1 > y2) {
        t = y2;
        y2 = y1;
        y1 = t;
    }

    turtle_fence_t f = { .x1 = x1, .y1 = y1, .x2 = x2, .y2 = y2 };
    st->fence = f;
    turtle_normalize_position(st, &st->position, TURTLE_BUMP);
}

turtle_mode_t turtle_get_mode(scene_turtle_t *st) {
    return st->mode;
}

void turtle_set_mode(scene_turtle_t *st, turtle_mode_t m) {
    if (m < TURTLE_WRAP)
        m = TURTLE_WRAP;
    if (m > TURTLE_BOUNCE)
        m = TURTLE_BOUNCE;
    st->mode = m;
}

uint16_t turtle_get_heading(scene_turtle_t *st) {
    return st->heading;   
}

void turtle_set_heading(scene_turtle_t *st, uint16_t h) {
    st->heading = h % 360;
}

void turtle_turn(scene_turtle_t *st, uint16_t h) {
    st->heading = (st->heading + h) % 360;
}

uint8_t turtle_get_velocity(scene_turtle_t *st) {
    return st->velocity;
}

void turtle_set_velocity(scene_turtle_t *st, int8_t v) {
    st->velocity = v;
}

#undef TURTLE_QBITS
