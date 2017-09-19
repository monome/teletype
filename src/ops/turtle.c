#include "ops/turtle.h"
#include "helpers.h"

#include "ops/op.h"
#include "teletype.h"
#include "turtle.h"
#include "state.h"

static void op_TURTLE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs); 
static void op_TURTLE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_X_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_X_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_Y_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_Y_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_MOVE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_F_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_FX1_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_FX1_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_FY1_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_FY1_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_FX2_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_FX2_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_FY2_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_FY2_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_SPEED_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_SPEED_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_DIR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_DIR_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_FRICTION_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_FRICTION_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_ACCEL_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_ACCEL_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_STEP_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_BUMP_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_BUMP_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_WRAP_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_WRAP_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_BOUNCE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_BOUNCE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_SCRIPT_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_SCRIPT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_SHOW_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_SHOW_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);

const tele_op_t op_TURTLE =
        MAKE_GET_SET_OP(@, op_TURTLE_get, op_TURTLE_set, 0, true);
const tele_op_t op_TURTLE_X =
        MAKE_GET_SET_OP(@X, op_TURTLE_X_get, op_TURTLE_X_set, 0, true);
const tele_op_t op_TURTLE_Y =
        MAKE_GET_SET_OP(@Y, op_TURTLE_Y_get, op_TURTLE_Y_set, 0, true);
const tele_op_t op_TURTLE_MOVE =
        MAKE_GET_OP(@MOVE, op_TURTLE_MOVE_get, 2, false);
const tele_op_t op_TURTLE_F =
        MAKE_GET_OP(@F, op_TURTLE_F_get, 4, false);
const tele_op_t op_TURTLE_FX1 =
        MAKE_GET_SET_OP(@FX1, op_TURTLE_FX1_get, op_TURTLE_FX1_set, 0, true);
const tele_op_t op_TURTLE_FY1 =
        MAKE_GET_SET_OP(@FY1, op_TURTLE_FY1_get, op_TURTLE_FY1_set, 0, true);
const tele_op_t op_TURTLE_FX2 =
        MAKE_GET_SET_OP(@FX2, op_TURTLE_FX2_get, op_TURTLE_FX2_set, 0, true);
const tele_op_t op_TURTLE_FY2  =
        MAKE_GET_SET_OP(@FY2, op_TURTLE_FY2_get, op_TURTLE_FY2_set, 0, true);
const tele_op_t op_TURTLE_SPEED =
        MAKE_GET_SET_OP(@SPEED, op_TURTLE_SPEED_get, op_TURTLE_SPEED_set, 0,
                true);
const tele_op_t op_TURTLE_DIR =
        MAKE_GET_SET_OP(@DIR, op_TURTLE_DIR_get, op_TURTLE_DIR_set, 0, true);
const tele_op_t op_TURTLE_FRICTION =
        MAKE_GET_SET_OP(@FRICTION, op_TURTLE_FRICTION_get,
                op_TURTLE_FRICTION_set, 0, true);
const tele_op_t op_TURTLE_ACCEL =
        MAKE_GET_SET_OP(@ACCEL, op_TURTLE_ACCEL_get,
                op_TURTLE_ACCEL_set, 0, true);
const tele_op_t op_TURTLE_STEP =
        MAKE_GET_OP(@STEP, op_TURTLE_STEP_get, 0, false);
const tele_op_t op_TURTLE_BUMP =
        MAKE_GET_SET_OP(@BUMP, op_TURTLE_BUMP_get, op_TURTLE_BUMP_set, 0, true);
const tele_op_t op_TURTLE_WRAP =
        MAKE_GET_SET_OP(@WRAP, op_TURTLE_WRAP_get, op_TURTLE_WRAP_set, 0, true);
const tele_op_t op_TURTLE_BOUNCE =
        MAKE_GET_SET_OP(@BOUNCE, op_TURTLE_BOUNCE_get, op_TURTLE_BOUNCE_set, 0,
                true);
const tele_op_t op_TURTLE_SCRIPT =
        MAKE_GET_SET_OP(@SCRIPT, op_TURTLE_SCRIPT_get, op_TURTLE_SCRIPT_set, 0,
                true);
const tele_op_t op_TURTLE_SHOW =
        MAKE_GET_SET_OP(@SHOW, op_TURTLE_SHOW_get, op_TURTLE_SHOW_set, 0, true);

static void op_TURTLE_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss_turtle_get_val(ss, &ss->turtle));
}

static void op_TURTLE_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    ss_turtle_set_val(ss, &ss->turtle, cs_pop(cs));
}

static void op_TURTLE_X_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, turtle_get_x(&ss->turtle));
}

static void op_TURTLE_X_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_set_x(&ss->turtle, cs_pop(cs));
}

static void op_TURTLE_Y_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, turtle_get_y(&ss->turtle));
}

static void op_TURTLE_Y_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_set_y(&ss->turtle, cs_pop(cs));
}

static void op_TURTLE_MOVE_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t x = cs_pop(cs);
    int16_t y = cs_pop(cs);
    turtle_move(&ss->turtle, x, y);
}

static void op_TURTLE_F_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t x1 = cs_pop(cs);
    int16_t y1 = cs_pop(cs);
    int16_t x2 = cs_pop(cs);
    int16_t y2 = cs_pop(cs);

    turtle_set_fence(&ss->turtle, x1, y1, x2, y2);
}

static void op_TURTLE_FX1_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->turtle.fence.x1);
}

static void op_TURTLE_FX1_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v = cs_pop(cs);
    ss->turtle.fence.x1 = v > 0 ? v : 0;
    turtle_correct_fence(&ss->turtle);
}

static void op_TURTLE_FY1_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->turtle.fence.y1);
}

static void op_TURTLE_FY1_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v = cs_pop(cs);
    ss->turtle.fence.y1 = v > 0 ? v : 0;
    turtle_correct_fence(&ss->turtle);
}

static void op_TURTLE_FX2_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->turtle.fence.x2);
}

static void op_TURTLE_FX2_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v = cs_pop(cs);
    ss->turtle.fence.x2 = v > 0 ? v : 0;
    turtle_correct_fence(&ss->turtle);
}

static void op_TURTLE_FY2_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->turtle.fence.y2);
}

static void op_TURTLE_FY2_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v = cs_pop(cs);
    ss->turtle.fence.y2 = v > 0 ? v : 0;
    turtle_correct_fence(&ss->turtle);
}

static void op_TURTLE_SPEED_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, turtle_get_speed(&ss->turtle));
}

static void op_TURTLE_SPEED_set(const void *NOTUSED(data), scene_state_t *ss,
    exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_set_speed(&ss->turtle, cs_pop(cs));
}

static void op_TURTLE_DIR_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, turtle_get_heading(&ss->turtle));
}

static void op_TURTLE_DIR_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_set_heading(&ss->turtle, cs_pop(cs));
}

static void op_TURTLE_FRICTION_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    // TODO implement
    cs_push(cs, 0);
}

static void op_TURTLE_FRICTION_set(const void *NOTUSED(data), scene_state_t *ss,
    exec_state_t *NOTUSED(es), command_state_t *cs) {
    // TODO implement
    cs_pop(cs);
}

static void op_TURTLE_ACCEL_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    // TODO implement
    cs_push(cs, 0);
}

static void op_TURTLE_ACCEL_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    // TODO implement
    cs_pop(cs);
}

static void op_TURTLE_STEP_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    turtle_step(&ss->turtle);
}

static void op_TURTLE_BUMP_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->turtle.mode == TURTLE_BUMP);
}

static void op_TURTLE_BUMP_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    if (cs_pop(cs))
        turtle_set_mode(&ss->turtle, TURTLE_BUMP);
}

static void op_TURTLE_WRAP_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->turtle.mode == TURTLE_WRAP);
}

static void op_TURTLE_WRAP_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    if (cs_pop(cs))
        turtle_set_mode(&ss->turtle, TURTLE_WRAP);
}

static void op_TURTLE_BOUNCE_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->turtle.mode == TURTLE_BOUNCE);
}

static void op_TURTLE_BOUNCE_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    if (cs_pop(cs))
        turtle_set_mode(&ss->turtle, TURTLE_BOUNCE);
}

static void op_TURTLE_SCRIPT_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    script_number_t s = turtle_get_script(&ss->turtle);
    if (s == TEMP_SCRIPT)
        cs_push(cs, 0);
    else
        cs_push(cs, turtle_get_script(&ss->turtle) + 1);
}

static void op_TURTLE_SCRIPT_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t sn = cs_pop(cs);
    if (sn == 0)
        turtle_set_script(&ss->turtle, TEMP_SCRIPT); // magic number
    else
        turtle_set_script(&ss->turtle, sn - 1);
}

static void op_TURTLE_SHOW_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    // TODO implement
    cs_push(cs, 0);
}

static void op_TURTLE_SHOW_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    // TODO implement
    cs_pop(cs);
}

