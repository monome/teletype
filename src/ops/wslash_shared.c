#include "ops/wslash_shared.h"

#include "helpers.h"
#include "teletype.h"

static void mod_WS1_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command);
static void mod_WS2_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command);
static void op_WS_SEL_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);

// clang-format off

const tele_mod_t mod_WS1  = MAKE_MOD(W/1, mod_WS1_func, 0);
const tele_mod_t mod_WS2  = MAKE_MOD(W/2, mod_WS2_func, 0);
const tele_op_t op_WS_SEL = MAKE_GET_OP(W/.SEL, op_WS_SEL_get , 1, false);

// clang-format on

uint8_t wslash_unit = 1;

static void mod_WS1_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command) {
    u8 u = wslash_unit;
    wslash_unit = 1;
    process_command(ss, es, post_command);
    wslash_unit = u;
}

static void mod_WS2_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command) {
    u8 u = wslash_unit;
    wslash_unit = 2;
    process_command(ss, es, post_command);
    wslash_unit = u;
}

static void op_WS_SEL_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    u8 unit = cs_pop(cs);
    if (unit > 0 && unit < 3) wslash_unit = unit;
}
