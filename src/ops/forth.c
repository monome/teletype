#include "ops/forth.h"

static void op_DUP_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_DROP_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_SWAP_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_OVER_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_ROT_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);

// clang-format off
// NOTE: all of the params vales are artificial to satisfy the validator
// Instead of saying how many values we accept, we instead just note stack count change
// params==0 adds 1 stack element
// params==1 no change to stack size
// params==2 drops 1 stack element
const tele_op_t op_DUP   = MAKE_GET_OP(DUP     , op_DUP_get     , 0, true);
const tele_op_t op_DROP  = MAKE_GET_OP(DROP    , op_DROP_get    , 2, true);
const tele_op_t op_SWAP  = MAKE_GET_OP(SWAP    , op_SWAP_get    , 1, true);
const tele_op_t op_OVER  = MAKE_GET_OP(OVER    , op_OVER_get    , 0, true);
const tele_op_t op_ROT   = MAKE_GET_OP(ROT     , op_ROT_get     , 1, true);
// clang-format on


static void op_DUP_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs)
{
    int16_t a = cs_pop(cs);
    cs_push(cs, a);
    cs_push(cs, a);
}
static void op_DROP_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs)
{
    cs_pop(cs);
}
static void op_SWAP_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs)
{
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    cs_push(cs, a);
    cs_push(cs, b);
}
static void op_OVER_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs)
{
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    cs_push(cs, b);
    cs_push(cs, a);
    cs_push(cs, b);
}
static void op_ROT_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs)
{
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    cs_push(cs, b);
    cs_push(cs, a);
    cs_push(cs, c);
}
