#include "ops/turtle.h"
#include "helpers.h"

#include "ops/op.h"
#include "teletype.h"
#include "turtle.h"

static void op_TURTLE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs); 
static void op_TURTLE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_UP_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_DOWN_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_LEFT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_RIGHT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_BUMP_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_WRAP_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_BOUNCE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_DIR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_DIR_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_SPEED_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_SPEED_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_X_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_X_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_Y_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_Y_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_POS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_STEP_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_FWD_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_REV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_TURN_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_HOME_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_HOME_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_SHOW_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_SHOW_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_FENCE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);


const tele_op_t op_TURTLE     = MAKE_GET_SET_OP(TURTLE, op_TURTLE_get, 
                                                op_TURTLE_set, 0, true);
const tele_op_t op_TURTLE_DIR = MAKE_GET_SET_OP(TURTLE_DIR, op_TURTLE_DIR_get,
                                                op_TURTLE_DIR_set, 1, true);
const tele_op_t op_TURTLE_SPEED = MAKE_GET_SET_OP(TURTLE_SPEED, op_TURTLE_SPEED_get,
                                                op_TURTLE_SPEED_set, 1, true);
const tele_op_t op_TURTLE_X = MAKE_GET_SET_OP(TURTLE_X, op_TURTLE_X_get,
                                                op_TURTLE_X_set, 1, true);
const tele_op_t op_TURTLE_Y = MAKE_GET_SET_OP(TURTLE_Y, op_TURTLE_Y_get,
                                                op_TURTLE_Y_set, 1, true);
const tele_op_t op_TURTLE_POS = MAKE_GET_SET_OP(TURTLE_POS, op_TURTLE_POS_get,
                                                op_TURTLE_POS_set, 2, true);
// TODO: Is this okay?  HOME_get takes no args, HOME_set takes 2
const tele_op_t op_TURTLE_HOME = MAKE_GET_SET_OP(TURTLE_HOME, op_TURTLE_HOME_get,
                                                op_TURTLE_HOME_set, 2, true);
const tele_op_t op_TURTLE_SHOW = MAKE_GET_SET_OP(TURTLE_SHOW, op_TURTLE_SHOW_get,
                                                op_TURTLE_SHOW_set, 1, true);
const tele_op_t op_TURTLE_UP = MAKE_GET_OP(TURTLE_UP, op_TURTLE_UP_get, 1, false);
const tele_op_t op_TURTLE_DOWN = MAKE_GET_OP(TURTLE_DOWN, op_TURTLE_DOWN_get, 1, false);
const tele_op_t op_TURTLE_LEFT = MAKE_GET_OP(TURTLE_LEFT, op_TURTLE_LEFT_get, 1, false);
const tele_op_t op_TURTLE_RIGHT = MAKE_GET_OP(TURTLE_RIGHT, op_TURTLE_RIGHT_get, 1, false);
const tele_op_t op_TURTLE_BUMP = MAKE_GET_OP(TURTLE_BUMP, op_TURTLE_BUMP_get, 0, false);
const tele_op_t op_TURTLE_WRAP = MAKE_GET_OP(TURTLE_WRAP, op_TURTLE_WRAP_get, 0, false);
const tele_op_t op_TURTLE_BOUNCE = MAKE_GET_OP(TURTLE_BOUNCE, op_TURTLE_BOUNCE_get, 0, false);
const tele_op_t op_TURTLE_TURN = MAKE_GET_OP(TURTLE_TURN, op_TURTLE_TURN_get, 1, false);
const tele_op_t op_TURTLE_STEP = MAKE_GET_OP(TURTLE_STEP, op_TURTLE_STEP_get, 0, false);
const tele_op_t op_TURTLE_FWD = MAKE_GET_OP(TURTLE_FWD, op_TURTLE_FWD_get, 0, false);
const tele_op_t op_TURTLE_REV = MAKE_GET_OP(TURTLE_REV, op_TURTLE_REV_get, 0, false);
const tele_op_t op_TURTLE_FENCE = MAKE_GET_OP(TURTLE_FENCE, op_TURTLE_FENCE_get, 4, false);



static void op_TURTLE_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, turtle_get(&ss->turtle));
}

static void op_TURTLE_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_set(&ss->turtle, cs_pop(cs));
}

static void op_TURTLE_UP_get(const void *NOTUSED(data), scene_state_t *ss, 
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t y = turtle_get_y(&ss->turtle) - cs_pop(cs);
    if (y < 0) 
        turtle_set_y(&ss->turtle, 0);
    else if (y > 63)
        turtle_set_y(&ss->turtle, 63);
    else
        turtle_set_y(&ss->turtle, y);

}

static void op_TURTLE_DOWN_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t y = turtle_get_y(&ss->turtle) + cs_pop(cs);
    if (y < 0) 
        turtle_set_y(&ss->turtle, 0);
    else if (y > 63)
        turtle_set_y(&ss->turtle, 63);
    else
        turtle_set_y(&ss->turtle, y);
}

static void op_TURTLE_LEFT_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t x = turtle_get_x(&ss->turtle) - cs_pop(cs);
    if (x < 0) 
        turtle_set_x(&ss->turtle, 0);
    else if (x > 3)
        turtle_set_x(&ss->turtle, 3);
    else
        turtle_set_x(&ss->turtle, x);
}

static void op_TURTLE_RIGHT_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t x = turtle_get_x(&ss->turtle) + cs_pop(cs);
    if (x < 0) 
        turtle_set_x(&ss->turtle, 0);
    else if (x > 3)
        turtle_set_x(&ss->turtle, 3);
    else
        turtle_set_x(&ss->turtle, x);
}

static void op_TURTLE_BUMP_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    turtle_set_mode(&ss->turtle, TURTLE_BUMP);
}

static void op_TURTLE_WRAP_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    turtle_set_mode(&ss->turtle, TURTLE_WRAP);
}

static void op_TURTLE_BOUNCE_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    turtle_set_mode(&ss->turtle, TURTLE_WRAP);
}

static void op_TURTLE_DIR_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, turtle_get_dir(&ss->turtle));
}

static void op_TURTLE_DIR_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_set_dir(&ss->turtle, cs_pop(cs));
}

static void op_TURTLE_SPEED_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, turtle_get_speed(&ss->turtle));
}

static void op_TURTLE_SPEED_set(const void *NOTUSED(data), scene_state_t *ss,
    exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_speed_set(&ss->turtle, cs_pop(cs));
}

static void op_TURTLE_X_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, turtle_get_x(&ss->turtle))
}

static void op_TURTLE_X_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_set_x(&ss->turtle, cs_pop(cs));
}

static void op_TURTLE_Y_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, turtle_get_y(&ss->turtle))
}

static void op_TURTLE_Y_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_set_y(&ss->turtle, cs_pop(cs));
}

static void op_TURTLE_POS_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_set_y(&ss->turtle, cs_pop(cs));
    turtle_set_x(&ss->turtle, cs_pop(cs));
}

static void op_TURTLE_STEP_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    turtle_step(&ss->turtle);
}

static void op_TURTLE_FWD_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t s = cs_pop(cs);
    turtle_forward(&ss->turtle, s);
}

static void op_TURTLE_REV_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t s = cs_pop(cs);
    turtle_forward(&ss->turtle, -s);
}

static void op_TURTLE_TURN_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_turn(&ss->turtle, cs_pop(cs));
}

static void op_TURTLE_HOME_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    turtle_goto(&ss->turtle, &ss->turtle.home);
}

static void op_TURTLE_HOME_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_set_home_x(&ss->turtle, cs_pop(cs));
    turtle_set_home_y(&ss->turtle, cs_pop(cs));
}

static void op_TURTLE_SHOW_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, 0);
}

static void op_TURTLE_SHOW_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_pop(cs);
}

static void op_TURTLE_FENCE_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_fence_set(&ss->turtle, cs_pop(cs), cs_pop(cs), cs_pop(cs),
            cs_pop(cs));
}
