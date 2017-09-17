#include "ops/turtle.h"
#include "helpers.h"

#include "ops/op.h"
#include "teletype.h"
#include "turtle.h"

static void op_TURTLE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs); 
static void op_TURTLE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_N_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_S_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_W_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_E_get(const void *data, scene_state_t *ss, exec_state_t *es,
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
static void op_TURTLE_HX_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_HX_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_HY_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_TURTLE_HY_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);


const tele_op_t op_TURTLE     = MAKE_GET_SET_OP(@, op_TURTLE_get, 
                                                op_TURTLE_set, 0, true);
const tele_op_t op_TURTLE_DIR = MAKE_GET_SET_OP(@DIR, op_TURTLE_DIR_get,
                                                op_TURTLE_DIR_set, 0, true);
const tele_op_t op_TURTLE_SPEED = MAKE_GET_SET_OP(@SPEED, op_TURTLE_SPEED_get,
                                                op_TURTLE_SPEED_set, 0, true);
const tele_op_t op_TURTLE_X = MAKE_GET_SET_OP(@X, op_TURTLE_X_get,
                                                op_TURTLE_X_set, 0, true);
const tele_op_t op_TURTLE_Y = MAKE_GET_SET_OP(@Y, op_TURTLE_Y_get,
                                                op_TURTLE_Y_set, 0, true);
const tele_op_t op_TURTLE_FX1 = MAKE_GET_SET_OP(@FX1, op_TURTLE_FX1_get,
                                                op_TURTLE_FX1_set, 0, true);
const tele_op_t op_TURTLE_FY1 = MAKE_GET_SET_OP(@FY1, op_TURTLE_FY1_get,
                                                op_TURTLE_FY1_set, 0, true);
const tele_op_t op_TURTLE_FX2 = MAKE_GET_SET_OP(@FX2, op_TURTLE_FX2_get,
                                                op_TURTLE_FX2_set, 0, true);
const tele_op_t op_TURTLE_FY2 = MAKE_GET_SET_OP(@FY2, op_TURTLE_FY2_get,
                                                op_TURTLE_FY2_set, 0, true);
const tele_op_t op_TURTLE_HX = MAKE_GET_SET_OP(@HX, op_TURTLE_HX_get,
                                                op_TURTLE_HX_set, 0, true);
const tele_op_t op_TURTLE_HY = MAKE_GET_SET_OP(@HY, op_TURTLE_HY_get,
                                                op_TURTLE_HY_set, 0, true);
// TODO: Is this okay?  HOME_get takes no args, HOME_set takes 2
//                     NOPE!  Fails stack size test.
const tele_op_t op_TURTLE_HOME = MAKE_GET_SET_OP(@HOME, op_TURTLE_HOME_get,
                                                op_TURTLE_HOME_set, 0, true);
const tele_op_t op_TURTLE_SHOW = MAKE_GET_SET_OP(@SHOW, op_TURTLE_SHOW_get,
                                                op_TURTLE_SHOW_set, 0, true);
const tele_op_t op_TURTLE_N = MAKE_GET_OP(@N, op_TURTLE_N_get, 1, false);
const tele_op_t op_TURTLE_S = MAKE_GET_OP(@S, op_TURTLE_S_get, 1, false);
const tele_op_t op_TURTLE_W = MAKE_GET_OP(@W, op_TURTLE_W_get, 1, false);
const tele_op_t op_TURTLE_E = MAKE_GET_OP(@E, op_TURTLE_E_get, 1, false);
const tele_op_t op_TURTLE_BUMP = MAKE_GET_OP(@BUMP, op_TURTLE_BUMP_get, 0, false);
const tele_op_t op_TURTLE_WRAP = MAKE_GET_OP(@WRAP, op_TURTLE_WRAP_get, 0, false);
const tele_op_t op_TURTLE_BOUNCE = MAKE_GET_OP(@BOUNCE, op_TURTLE_BOUNCE_get, 0, false);
const tele_op_t op_TURTLE_TURN = MAKE_GET_OP(@TURN, op_TURTLE_TURN_get, 1, false);
const tele_op_t op_TURTLE_STEP = MAKE_GET_OP(@STEP, op_TURTLE_STEP_get, 0, false);
const tele_op_t op_TURTLE_FWD = MAKE_GET_OP(@FWD, op_TURTLE_FWD_get, 1, false);
const tele_op_t op_TURTLE_REV = MAKE_GET_OP(@REV, op_TURTLE_REV_get, 1, false);
const tele_op_t op_TURTLE_FENCE = MAKE_GET_OP(@FENCE, op_TURTLE_FENCE_get, 4, false);
const tele_op_t op_TURTLE_POS = MAKE_GET_OP(@POS, op_TURTLE_POS_get, 2, false);



static void op_TURTLE_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, turtle_get(ss, &ss->turtle));
}

static void op_TURTLE_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_set(ss, &ss->turtle, cs_pop(cs));
}

static void op_TURTLE_N_get(const void *NOTUSED(data), scene_state_t *ss, 
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_step(&ss->turtle, 0, cs_pop(cs));
}

static void op_TURTLE_S_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_step(&ss->turtle, 180, cs_pop(cs));
}

static void op_TURTLE_W_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_step(&ss->turtle, 270, cs_pop(cs));
}

static void op_TURTLE_E_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_step(&ss->turtle, 90, cs_pop(cs));
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
    turtle_set_mode(&ss->turtle, TURTLE_BOUNCE);
}

static void op_TURTLE_DIR_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, turtle_get_heading(&ss->turtle));
}

static void op_TURTLE_DIR_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_set_heading(&ss->turtle, cs_pop(cs));
}

static void op_TURTLE_SPEED_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, turtle_get_velocity(&ss->turtle));
}

static void op_TURTLE_SPEED_set(const void *NOTUSED(data), scene_state_t *ss,
    exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_set_velocity(&ss->turtle, cs_pop(cs));
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

static void op_TURTLE_POS_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    turtle_set_y(&ss->turtle, cs_pop(cs));
    turtle_set_x(&ss->turtle, cs_pop(cs));
}

static void op_TURTLE_STEP_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    turtle_step(&ss->turtle, ss->turtle.heading, ss->turtle.velocity);
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

static void op_TURTLE_HX_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->turtle.home.x);
}

static void op_TURTLE_HX_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v = cs_pop(cs);
    ss->turtle.home.x = v > 0 ? v : 0;
    // maybe we shouldn't have to call this here?
    turtle_normalize_position(&ss->turtle, &ss->turtle.home, TURTLE_BUMP);
}

static void op_TURTLE_HY_get(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->turtle.home.y);
}

static void op_TURTLE_HY_set(const void *NOTUSED(data), scene_state_t *ss,
        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v = cs_pop(cs);
    ss->turtle.home.y = v > 0 ? v : 0;
    // maybe we shouldn't have to call this here?
    turtle_normalize_position(&ss->turtle, &ss->turtle.home, TURTLE_BUMP);
}
