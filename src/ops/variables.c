#include "ops/variables.h"

#include <stdlib.h>  // rand

#include "helpers.h"
#include "ops/op.h"
#include "teletype.h"


static void op_LAST_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_DRUNK_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_DRUNK_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_FLIP_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_FLIP_set(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_O_get(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_O_set(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_I_get(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_I_set(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);

// clang-format off
const tele_op_t op_A          = MAKE_SIMPLE_VARIABLE_OP(A         , variables.a         );
const tele_op_t op_B          = MAKE_SIMPLE_VARIABLE_OP(B         , variables.b         );
const tele_op_t op_C          = MAKE_SIMPLE_VARIABLE_OP(C         , variables.c         );
const tele_op_t op_D          = MAKE_SIMPLE_VARIABLE_OP(D         , variables.d         );
const tele_op_t op_DRUNK_MAX  = MAKE_SIMPLE_VARIABLE_OP(DRUNK.MAX , variables.drunk_max );
const tele_op_t op_DRUNK_MIN  = MAKE_SIMPLE_VARIABLE_OP(DRUNK.MIN , variables.drunk_min );
const tele_op_t op_DRUNK_WRAP = MAKE_SIMPLE_VARIABLE_OP(DRUNK.WRAP, variables.drunk_wrap);
const tele_op_t op_O_INC      = MAKE_SIMPLE_VARIABLE_OP(O.INC     , variables.o_inc     );
const tele_op_t op_O_MAX      = MAKE_SIMPLE_VARIABLE_OP(O.MAX     , variables.o_max     );
const tele_op_t op_O_MIN      = MAKE_SIMPLE_VARIABLE_OP(O.MIN     , variables.o_min     );
const tele_op_t op_O_WRAP     = MAKE_SIMPLE_VARIABLE_OP(O.WRAP    , variables.o_wrap    );
const tele_op_t op_T          = MAKE_SIMPLE_VARIABLE_OP(T         , variables.t         );
const tele_op_t op_TIME       = MAKE_SIMPLE_VARIABLE_OP(TIME      , variables.time      );
const tele_op_t op_TIME_ACT   = MAKE_SIMPLE_VARIABLE_OP(TIME.ACT  , variables.time_act  );
const tele_op_t op_LAST       =             MAKE_GET_OP(LAST  , op_LAST_get, 1, true);
const tele_op_t op_X          = MAKE_SIMPLE_VARIABLE_OP(X         , variables.x         );
const tele_op_t op_Y          = MAKE_SIMPLE_VARIABLE_OP(Y         , variables.y         );
const tele_op_t op_Z          = MAKE_SIMPLE_VARIABLE_OP(Z         , variables.z         );

const tele_op_t op_DRUNK = MAKE_GET_SET_OP(DRUNK, op_DRUNK_get, op_DRUNK_set, 0, true);
const tele_op_t op_FLIP  = MAKE_GET_SET_OP(FLIP , op_FLIP_get , op_FLIP_set , 0, true);
const tele_op_t op_O     = MAKE_GET_SET_OP(O    , op_O_get    , op_O_set    , 0, true);
const tele_op_t op_I     = MAKE_GET_SET_OP(I    , op_I_get, op_I_set, 0, true);
// clang-format on

static void op_LAST_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t script_number = cs_pop(cs) - 1;
    int16_t last = ss_get_script_last(ss, script_number);
    cs_push(cs, last);
}

static void op_DRUNK_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t min = ss->variables.drunk_min;
    int16_t max = ss->variables.drunk_max;
    int16_t wrap = ss->variables.drunk_wrap;

    // restrict current_value to (wrapped) bounds
    int16_t current_value =
        normalise_value(min, max, wrap, ss->variables.drunk);
    cs_push(cs, current_value);

    // calculate new value
    int16_t new_value = current_value + (rand() % 3) - 1;
    ss->variables.drunk = normalise_value(min, max, wrap, new_value);
}

static void op_DRUNK_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    ss->variables.drunk = cs_pop(cs);
}

static void op_FLIP_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t flip = ss->variables.flip;
    cs_push(cs, flip);
    ss->variables.flip = flip == 0;
}

static void op_FLIP_set(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    ss->variables.flip = cs_pop(cs) != 0;
}

static void op_O_get(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t min = ss->variables.o_min;
    int16_t max = ss->variables.o_max;
    int16_t wrap = ss->variables.o_wrap;

    // restrict current_value to (wrapped) bounds
    int16_t current_value = normalise_value(min, max, wrap, ss->variables.o);
    cs_push(cs, current_value);

    // calculate new value
    ss->variables.o =
        normalise_value(min, max, wrap, current_value + ss->variables.o_inc);
}

static void op_O_set(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    ss->variables.o = cs_pop(cs);
}

static void op_I_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *es, command_state_t *cs) {
    cs_push(cs, es_variables(es)->i);
}

static void op_I_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *es, command_state_t *cs) {
    es_variables(es)->i = cs_pop(cs);
}
