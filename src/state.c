#include "state.h"

#include <string.h>

#include "teletype_io.h"

#define min(X, Y) ((X) < (Y) ? (X) : (Y))
#define max(X, Y) ((X) > (Y) ? (X) : (Y))


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
    memcpy(&ss->turtle, &t, sizeof(t));
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
    if (idx < TT_SCRIPT_1)
        return 0;
    if (idx > INIT_SCRIPT)
        return 0;
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
//
void scene_set_turtle(scene_state_t *ss, scene_turtle_t *ts) {
    //TODO validate the turtle?
    ss->turtle = *ts; // structs copy with value assignment
}


scene_turtle_t* scene_get_turtle(scene_state_t *ss) {
    return &ss->turtle;
}


// In this code, we use signed fixed-point maths.  This means that the int16_t
// is used to store a signed number with both an integer and a fraction part.
//
// Q_BITS represents how many are used for the fractional component.  Because
// we work with signed integers, 1 bit facilitates the sign component.
// Therefore, INT_BITS = 16 - Q_BITS - 1
//
// The main thing we do in fixed-point is keep a fractional index into the
// pattern data, so it will be our main reference point.
//
// 63 is 6 bits, so Q_BITS = 16 - 6 - 1 = 9
// The notation for this system is Q6.9

#define Q_BITS 9
#define Q_1 (1 << Q_BITS)                           // 1.0
#define Q_05 (1 << (Q_BITS - 1))                    // 0.5
#define Q_ROUND(X) ((((X >> (Q_BITS - 1)) + 1) >> 1) << Q_BITS)   // (int)(X + 0.5)
#define QT int16_t
#define TO_Q(X) (X << Q_BITS) 
#define TO_I(X) (Q_ROUND(X) >> Q_BITS)

typedef struct {
    QT x1, y1, x2, y2;
} Q_fence_t;

static inline Q_fence_t normalize_fence(turtle_fence_t in) {
    Q_fence_t out;

    out.x1 = in.x1 << Q_BITS;
    out.x2 = in.x2 << Q_BITS;
    out.y1 = in.y1 << Q_BITS;
    out.y2 = in.y2 << Q_BITS;
    return out;
}


void turtle_normalize_position(scene_turtle_t *t, turtle_position_t *tp, 
        turtle_mode_t mode) {
    // a normal_fence operates in fixed-point at Q = Q_BITS
    Q_fence_t f = normalize_fence(t->fence);
    
    // fence values are inclusive so 0 to 0 is L:1
    // literally avoid a fencepost error ;)
    QT fxl = f.x2 - f.x1 + Q_1;
    QT fyl = f.y2 - f.y1 + Q_1;
  
    if (mode == TURTLE_BUMP) {
        tp->x = min(f.x2, max(f.x1, tp->x));
        tp->y = min(f.y2, max(f.y1, tp->y));
    }
    else if (mode == TURTLE_WRAP) {
        if (tp->x < f.x1)
            tp->x = f.x2 + tp->x - f.x1 + Q_1;
        else if (tp->x > f.x2)
            tp->x = f.x1 + tp->x - f.x2 - Q_1;
        
        if (tp->y < f.y1)
            tp->y = f.y2 + tp->y + f.y1 + Q_1;
        else if (tp->y > f.y2)
            tp->y = f.y1 + tp->y - f.y2 - Q_1;
    }
    else if (mode == TURTLE_BOUNCE) {
    // it's not correct with offsets larger than the pattern, needs to wavefold
        if (tp->x > f.x2)
            tp->x = f.x2 - ((tp->x - f.x1) % fxl) - Q_1;
        else if (tp->x < f.x1)
            tp->x = f.x1 - ((f.x1 + tp->x) % fxl); 
        if (tp->y > f.y2)
            tp->y = f.y2 - ((tp->y - f.y1) % fyl) - Q_1;
        else if (tp->y < f.y1)
            tp->y = f.y1 - ((f.y1 + tp->y) % fyl);
    }
}

// Produce rounded Q0 positions in dst, for external use only
void turtle_resolve_position(scene_turtle_t *t, turtle_position_t *src,
                             turtle_position_t *dst) {
    *dst = *src;
    dst->x = TO_I(Q_ROUND(dst->x));
    dst->y = TO_I(Q_ROUND(dst->y));
}

// Round the position to the nearest cell
// This effectively sets the turtle in the middle of each cell
static inline void turtle_Q_position(scene_turtle_t *t, turtle_position_t *src,
                             turtle_position_t *dst) {
    *dst = *src;
    dst->x = Q_ROUND(dst->x);
    dst->y = Q_ROUND(dst->y);
}

uint8_t turtle_get_x(scene_turtle_t *st) {
    turtle_position_t t;
    turtle_resolve_position(st, &st->position, &t);
    return t.x;
}

void turtle_set_x(scene_turtle_t *st, int16_t x) {
    st->position.x = TO_Q(x);
    turtle_normalize_position(st, &st->position, TURTLE_BUMP);
}

uint8_t turtle_get_y(scene_turtle_t *st) {
    turtle_position_t t;
    turtle_resolve_position(st, &st->position, &t);
    return t.y;
}

void turtle_set_y(scene_turtle_t *st, int16_t y) {
    st->position.y = TO_Q(y);
    turtle_normalize_position(st, &st->position, TURTLE_BUMP);
}

void turtle_goto(scene_turtle_t *st, turtle_position_t *tp) {
    st->position = *tp;
    turtle_normalize_position(st, &st->position, st->mode);
}

/// http://www.coranac.com/2009/07/sines/
/// A sine approximation via a third-order approx.
/// @param x    Angle (with 2^15 units/circle) (Q13)
/// @return     Sine value (Q12)
static inline int32_t sin(uint32_t x)
{
    // S(x) = x * ( (3<<p) - (x*x>>r) ) >> s
    // n : Q-pos for quarter circle             13
    // A : Q-pos for output                     12
    // p : Q-pos for parentheses intermediate   15
    // r = 2n-p                                 11
    // s = A-1-p-n                              17

    int qN = 13, qA= 12, qP= 15, qR= 2*qN-qP, qS= qN+qP+1-qA;

    x= x<<(30-qN);          // shift to full s32 range (Q13->Q30)

    if( (x^(x<<1)) < 0)     // test for quadrant 1 or 2
        x= (1<<31) - x;

    x= x>>(30-qN);

    return x * ( (3<<qP) - (x*x>>qR) ) >> qS;
}

void turtle_step(scene_turtle_t *st, int16_t h, int16_t v) {
    // TODO watch out, it's a doozie ;)
    int32_t dx = 0, dy = 0;
    uint32_t h1, h2;

    // Kludgey hack due to sin() errors
    switch(h) {
        case 0:
            dy = TO_Q(-v);
            break;
        case 90:
            dx = TO_Q(v);
            break;
        case 180:
            dy = TO_Q(v);
            break;
        case 270:
            dx = TO_Q(-v);
            break;
        default:
            h1 = ((h % 360) << 12) / 180;
            h2 = (((h + 180) % 360) << 12) / 180; // sin() is phased cos()

            int32_t  dy_d_Q12 = v * sin(h1);
            int32_t  dx_d_Q12 = v * sin(h2); 
            
            if (dx_d_Q12 < 0)
                // delta x = round(v *sin(heading))
                dx = ((dx_d_Q12 >> (11 - Q_BITS)) - 1) >> 1;
            else
                dx = ((dx_d_Q12 >> (11 - Q_BITS)) + 1) >> 1;
            if (dy_d_Q12 < 0)
                dy = ((dy_d_Q12 >> (11 - Q_BITS)) - 1) >> 1;
            else
                dy = ((dy_d_Q12 >> (11 - Q_BITS)) + 1) >> 1;
    }

    st->position.x += dx;
    st->position.y += dy;
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

void turtle_set_home(scene_turtle_t *st, int16_t x, int16_t y) {
    if (x > 3)
        x = 3;
    else if (x < 0)
        x = 0;
    if (y > 63)
        y = 63;
    else if (y < 0)
        y = 0;
    st->home.x = TO_Q(x);
    st->home.y = TO_Q(y);
    turtle_normalize_position(st, &st->home, TURTLE_BUMP);
}

uint8_t turtle_get_home_x(scene_turtle_t *st) {
    turtle_position_t t;
    turtle_resolve_position(st, &st->home, &t);
    return t.x;
}

void turtle_set_home_x(scene_turtle_t *st, int16_t x) {
    if (x > 3)
        x = 3;
    else if (x < 0)
        x = 0;
    st->home.x = TO_Q(x);
    turtle_normalize_position(st, &st->home, TURTLE_BUMP);
}

uint8_t  turtle_get_home_y(scene_turtle_t *st) {
    turtle_position_t t;
    turtle_resolve_position(st, &st->home, &t);
    return t.y;
}

void turtle_set_home_y(scene_turtle_t *st, int16_t y) {
    if (y > 63)
        y = 63;
    else if (y < 0)
        y = 0;
    turtle_position_t h = { .x = st->home.x, .y = y << Q_BITS };
    st->home = h;
    turtle_normalize_position(st, &st->home, TURTLE_BUMP);
}

inline void turtle_correct_fence(scene_turtle_t *st) {
    int16_t t;
    st->fence.x1 = min(3, max(0, st->fence.x1));
    st->fence.x2 = min(3, max(0, st->fence.x2));
    st->fence.y1 = min(63, max(0, st->fence.y1));
    st->fence.y2 = min(63, max(0, st->fence.y2));

    if (st->fence.x1 > st->fence.x2) {
        t = st->fence.x2;
        st->fence.x2 = st->fence.x1;
        st->fence.x1 = t;
    }
    if (st->fence.y1 > st->fence.y2) {
        t = st->fence.y2;
        st->fence.y2 = st->fence.y1;
        st->fence.y1 = t;
    }
    turtle_normalize_position(st, &st->position, TURTLE_BUMP);
}

void turtle_set_fence(scene_turtle_t *st, int16_t x1, int16_t y1, 
                                          int16_t x2, int16_t y2) {

    st->fence.x1 = min(3, max(0, x1));
    st->fence.x2 = min(3, max(0, x2));
    st->fence.y1 = min(63, max(0, y1));
    st->fence.y2 = min(63, max(0, y2));
    turtle_correct_fence(st);
}

turtle_mode_t turtle_get_mode(scene_turtle_t *st) {
    return st->mode;
}

void turtle_set_mode(scene_turtle_t *st, turtle_mode_t m) {
    if (m != TURTLE_WRAP && m != TURTLE_BUMP && m != TURTLE_BOUNCE)
        m = TURTLE_BUMP;
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

void turtle_set_velocity(scene_turtle_t *st, int16_t v) {
    if (v > INT8_MAX)
        v = INT8_MAX;
    else if (v < INT8_MIN)
        v = INT8_MIN;
    st->velocity = v;
}

void turtle_forward(scene_turtle_t *st, int16_t v) {
    turtle_step(st, st->heading, v);
}
