#include "ops/wslash.h"

#include "helpers.h"
#include "ii.h"
#include "teletype_io.h"

static void op_WS_REC_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_WS_REC_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_WS_PLAY_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_WS_PLAY_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_WS_LOOP_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_WS_LOOP_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_WS_CUE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_WS_CUE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);

// clang-format off
const tele_op_t op_WS_REC  = MAKE_GET_SET_OP(WS.REC , op_WS_REC_get , op_WS_REC_set , 0, true);
const tele_op_t op_WS_PLAY = MAKE_GET_SET_OP(WS.PLAY, op_WS_PLAY_get, op_WS_PLAY_set, 0, true);
const tele_op_t op_WS_LOOP = MAKE_GET_SET_OP(WS.LOOP, op_WS_LOOP_get, op_WS_LOOP_set, 0, true);
const tele_op_t op_WS_CUE  = MAKE_GET_SET_OP(WS.CUE , op_WS_CUE_get , op_WS_CUE_set , 0, true);
// clang-format on


static void op_WS_REC_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { WS_REC, a };
    tele_ii_tx(WS_T_ADDR, d, 2);
}
static void op_WS_REC_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { WS_REC | II_GET };
    uint8_t addr = WS_T_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_WS_PLAY_set(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { WS_PLAY, a };
    tele_ii_tx(WS_T_ADDR, d, 2);
}
static void op_WS_PLAY_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { WS_PLAY | II_GET };
    uint8_t addr = WS_T_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_WS_LOOP_set(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { WS_LOOP, a };
    tele_ii_tx(WS_T_ADDR, d, 2);
}
static void op_WS_LOOP_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { WS_LOOP | II_GET };
    uint8_t addr = WS_T_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_WS_CUE_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { WS_CUE, a };
    tele_ii_tx(WS_T_ADDR, d, 2);
}
static void op_WS_CUE_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { WS_CUE | II_GET };
    uint8_t addr = WS_T_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}
