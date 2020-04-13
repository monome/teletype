#include "ops/fader.h"

#include "helpers.h"
#include "ii.h"
#include "teletype.h"
#include "teletype_io.h"
#include "telex.h"


static void op_FADER_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);

static void op_FADER_SCALE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                               command_state_t *cs);

static void op_FADER_CAL_MIN_set(const void *data, scene_state_t *ss, exec_state_t *es,
                                 command_state_t *cs);

static void op_FADER_CAL_MAX_set(const void *data, scene_state_t *ss, exec_state_t *es,
                                 command_state_t *cs);

static void op_FADER_CAL_RESET_set(const void *data, scene_state_t *ss, exec_state_t *es,
                                   command_state_t *cs);

const tele_op_t op_FADER = MAKE_GET_OP(FADER, op_FADER_get, 1, true);
const tele_op_t op_FADER_SCALE = MAKE_GET_OP(FADER.SCALE, op_FADER_SCALE_set, 3, false);
const tele_op_t op_FADER_CAL_MIN = MAKE_GET_OP (FADER.CAL.MIN, op_FADER_CAL_MIN_set, 1, true);
const tele_op_t op_FADER_CAL_MAX = MAKE_GET_OP (FADER.CAL.MAX, op_FADER_CAL_MAX_set, 1, true);
const tele_op_t op_FADER_CAL_RESET  = MAKE_GET_OP (FADER.CAL.RESET, op_FADER_CAL_RESET_set, 1, false);

const tele_op_t op_FB = MAKE_ALIAS_OP(FB, op_FADER_get, NULL, 1, true);
const tele_op_t op_FB_S = MAKE_ALIAS_OP(FB.S, op_FADER_SCALE_set, NULL, 3, false);
const tele_op_t op_FB_C_MIN = MAKE_ALIAS_OP(FB.C.MIN, op_FADER_CAL_MIN_set, NULL, 1, true);
const tele_op_t op_FB_C_MAX = MAKE_ALIAS_OP(FB.C.MAX, op_FADER_CAL_MAX_set, NULL, 1, true);
const tele_op_t op_FB_C_R = MAKE_ALIAS_OP(FB.C.R, op_FADER_CAL_RESET_set, NULL, 1, false);

static int16_t receive_fader(int16_t input) {
    // convert the input to the device and the port
    uint8_t port = input % 16;
    uint8_t device = input / 16;
    uint8_t address = FADER + device;
    int16_t value = ReceiveValue(address, port);
    return value;
}

static void op_FADER_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint16_t input = cs_pop(cs);
    // zero-index the input
    input -= 1;
    // return if out of range
    if (input < 0 || input > 15) {
        cs_push(cs, 0);
        return;
    }
    int16_t value = receive_fader(input);
    cs_push(cs, scale_get(ss->variables.fader_scales[input], value));
}

static void op_FADER_SCALE_set(const void *NOTUSED(data), scene_state_t *ss,
                               exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t fader = cs_pop(cs);
    int16_t min = cs_pop(cs);
    int16_t max = cs_pop(cs);
    fader -= 1;
    if (fader < 0 || fader > 15) {
        return;
    }
    ss_set_fader_scale(ss, fader, min, max);
}

static void op_FADER_CAL_MIN_set(const void *NOTUSED(data), scene_state_t *ss,
                                 exec_state_t *NOTUSED(es),
                                 command_state_t *cs) {
    uint16_t input = cs_pop(cs);
    // zero-index the input
    input -= 1;
    // return if out of range
    if (input < 0 || input > 15) {
        cs_push(cs, 0);
        return;
    }
    int16_t value = receive_fader(input);
    ss_set_fader_min(ss, input, value);
    cs_push(cs, value);
}

static void op_FADER_CAL_MAX_set(const void *NOTUSED(data), scene_state_t *ss,
                                 exec_state_t *NOTUSED(es),
                                 command_state_t *cs) {
    uint16_t input = cs_pop(cs);
    // zero-index the input
    input -= 1;
    // return if out of range
    if (input < 0 || input > 15) {
        cs_push(cs, 0);
        return;
    }
    int16_t value = receive_fader(input);
    ss_set_fader_max(ss, input, value);
    cs_push(cs, value);
}

static void op_FADER_CAL_RESET_set(const void *NOTUSED(data), scene_state_t *ss,
                                   exec_state_t *NOTUSED(es),
                                   command_state_t *cs) {
    uint16_t fader = cs_pop(cs);
    // zero-index the input
    fader -= 1;
    // return if out of range
    if (fader < 0 || fader > 15) {
        return;
    }
    ss_reset_fader_cal(ss, fader);
}
