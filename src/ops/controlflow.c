#include "ops/controlflow.h"
#include "../module/flash.h"

#include "helpers.h"
#include "random.h"
#include "teletype.h"
#include "teletype_io.h"

static void mod_PROB_func(scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs,
                          const tele_command_t *post_command);
static void mod_IF_func(scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs,
                        const tele_command_t *post_command);
static void mod_ELIF_func(scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs,
                          const tele_command_t *post_command);
static void mod_ELSE_func(scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs,
                          const tele_command_t *post_command);
static void mod_L_func(scene_state_t *ss, exec_state_t *es, command_state_t *cs,
                       const tele_command_t *post_command);
static void mod_W_func(scene_state_t *ss, exec_state_t *es, command_state_t *cs,
                       const tele_command_t *post_command);
static void mod_EVERY_func(scene_state_t *ss, exec_state_t *es,
                           command_state_t *cs,
                           const tele_command_t *post_command);
static void mod_SKIP_func(scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs,
                          const tele_command_t *post_command);
static void mod_OTHER_func(scene_state_t *ss, exec_state_t *es,
                           command_state_t *cs,
                           const tele_command_t *post_command);

static void op_SCENE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_SCENE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_SCENE_G_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_SCENE_P_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_SCRIPT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_SCRIPT_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_SCRIPT_POL_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_SCRIPT_POL_set(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_KILL_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_BREAK_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_SYNC_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);


// clang-format off
const tele_mod_t mod_PROB = MAKE_MOD(PROB, mod_PROB_func, 1);
const tele_mod_t mod_IF = MAKE_MOD(IF, mod_IF_func, 1);
const tele_mod_t mod_ELIF = MAKE_MOD(ELIF, mod_ELIF_func, 1);
const tele_mod_t mod_ELSE = MAKE_MOD(ELSE, mod_ELSE_func, 0);
const tele_mod_t mod_L = MAKE_MOD(L, mod_L_func, 2);
const tele_mod_t mod_W = MAKE_MOD(W, mod_W_func, 1);
const tele_mod_t mod_EVERY = MAKE_MOD(EVERY, mod_EVERY_func, 1);
const tele_mod_t mod_EV = MAKE_MOD(EV, mod_EVERY_func, 1);
const tele_mod_t mod_SKIP = MAKE_MOD(SKIP, mod_SKIP_func, 1);
const tele_mod_t mod_OTHER = MAKE_MOD(OTHER, mod_OTHER_func, 0);

const tele_op_t op_SCRIPT = MAKE_GET_SET_OP(SCRIPT, op_SCRIPT_get, op_SCRIPT_set, 0, true);
const tele_op_t op_SYM_DOLLAR = MAKE_ALIAS_OP($, op_SCRIPT_get, op_SCRIPT_set, 0, true);
const tele_op_t op_SCRIPT_POL = MAKE_GET_SET_OP(SCRIPT.POL, op_SCRIPT_POL_get, op_SCRIPT_POL_set, 1, true);
const tele_op_t op_SYM_DOLLAR_POL = MAKE_ALIAS_OP($.POL, op_SCRIPT_POL_get, op_SCRIPT_POL_set, 1, true);
const tele_op_t op_KILL = MAKE_GET_OP(KILL, op_KILL_get, 0, false);
const tele_op_t op_SCENE_G = MAKE_GET_OP(SCENE.G, op_SCENE_G_get, 1, false);
const tele_op_t op_SCENE_P = MAKE_GET_OP(SCENE.P, op_SCENE_P_get, 1, false);
const tele_op_t op_SCENE = MAKE_GET_SET_OP(SCENE, op_SCENE_get, op_SCENE_set, 0, true);
const tele_op_t op_BREAK = MAKE_GET_OP(BREAK, op_BREAK_get, 0, false);
const tele_op_t op_BRK = MAKE_ALIAS_OP(BRK, op_BREAK_get, NULL, 0, false);
const tele_op_t op_SYNC = MAKE_GET_OP(SYNC, op_SYNC_get, 1, false);
// clang-format on

static void mod_PROB_func(scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs,
                          const tele_command_t *post_command) {
    int16_t a = cs_pop(cs);
    random_state_t *r = &ss->rand_states.s.prob.rand;

    if (random_next(r) % 101 < a) { process_command(ss, es, post_command); }
}

static void mod_IF_func(scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs,
                        const tele_command_t *post_command) {
    int16_t a = cs_pop(cs);

    es_variables(es)->if_else_condition = false;
    if (a) {
        es_variables(es)->if_else_condition = true;
        process_command(ss, es, post_command);
    }
}

static void mod_ELIF_func(scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs,
                          const tele_command_t *post_command) {
    int16_t a = cs_pop(cs);

    if (!es_variables(es)->if_else_condition) {
        if (a) {
            es_variables(es)->if_else_condition = true;
            process_command(ss, es, post_command);
        }
    }
}

static void mod_ELSE_func(scene_state_t *ss, exec_state_t *es,
                          command_state_t *NOTUSED(cs),
                          const tele_command_t *post_command) {
    if (!es_variables(es)->if_else_condition) {
        es_variables(es)->if_else_condition = true;
        process_command(ss, es, post_command);
    }
}

static void mod_L_func(scene_state_t *ss, exec_state_t *es, command_state_t *cs,
                       const tele_command_t *post_command) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);

    // using a pointer means that the loop contents can a interact with the
    // iterator, allowing users to roll back a loop or advance it faster
    int16_t *i = &es_variables(es)->i;
    *i = a;

    // Forward loop
    if (a < b) {
        // continue the loop whenever the _pointed-to_ I meets the condition
        // this means that I can be interacted with inside the loop command

        // iterate with higher precision to account for b == 32767
        for (int32_t l = a; l <= b; l++) {
            process_command(ss, es, post_command);
            if (es_variables(es)->breaking) break;
            // the increment statement has careful syntax, because the
            // ++ operator has precedence over the dereference * operator
            (*i)++;
        }

        if (!es_variables(es)->breaking)
            (*i)--;  // past end of loop, leave I in the correct state
    }
    // Reverse loop (also works for equal values (either loop would))
    else {
        for (int32_t l = a; l >= b && !es_variables(es)->breaking; l--) {
            process_command(ss, es, post_command);
            (*i)--;
        }
        if (!es_variables(es)->breaking) (*i)++;
    }
}

static void mod_W_func(scene_state_t *ss, exec_state_t *es, command_state_t *cs,
                       const tele_command_t *post_command) {
    int16_t a = cs_pop(cs);
    if (a) {
        process_command(ss, es, post_command);
        es_variables(es)->while_depth++;
        if (es_variables(es)->while_depth < WHILE_DEPTH)
            es_variables(es)->while_continue = true;
        else
            es_variables(es)->while_continue = false;
    }
    else
        es_variables(es)->while_continue = false;
}

static void mod_EVERY_func(scene_state_t *ss, exec_state_t *es,
                           command_state_t *cs,
                           const tele_command_t *post_command) {
    int16_t mod = cs_pop(cs);
    every_count_t *every = ss_get_every(ss, es_variables(es)->script_number,
                                        es_variables(es)->line_number);
    every_set_skip(every, false);
    every_set_mod(every, mod);
    every_tick(every);
    if (every_is_now(ss, every)) process_command(ss, es, post_command);
}

static void mod_SKIP_func(scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs,
                          const tele_command_t *post_command) {
    int16_t mod = cs_pop(cs);
    every_count_t *every = ss_get_every(ss, es_variables(es)->script_number,
                                        es_variables(es)->line_number);
    every_set_skip(every, true);
    every_set_mod(every, mod);
    every_tick(every);
    if (skip_is_now(ss, every)) process_command(ss, es, post_command);
}

static void mod_OTHER_func(scene_state_t *ss, exec_state_t *es,
                           command_state_t *NOTUSED(cs),
                           const tele_command_t *post_command) {
    if (!ss->every_last) process_command(ss, es, post_command);
}


static void op_SYNC_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t count = cs_pop(cs);
    ss->every_last = false;
    ss_sync_every(ss, count);
}

static void op_SCENE_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->variables.scene);
}

static void op_SCENE_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t scene = cs_pop(cs);
    if (scene < 0) { scene = 0; }
    if (scene >= SCENE_SLOTS) { scene = SCENE_SLOTS - 1; }

    if (!ss->initializing) {
        ss->variables.scene = scene;
        tele_scene(scene, 1, 1);
    }
}

static void op_SCENE_G_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t scene = cs_pop(cs);
    if (scene < 0) { scene = 0; }
    if (scene >= SCENE_SLOTS) { scene = SCENE_SLOTS - 1; }

    if (!ss->initializing) {
        ss->variables.scene = scene;
        tele_scene(scene, 0, 1);
    }
}

static void op_SCENE_P_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t scene = cs_pop(cs);
    if (scene < 0) { scene = 0; }
    if (scene >= SCENE_SLOTS) { scene = SCENE_SLOTS - 1; }

    if (!ss->initializing) {
        ss->variables.scene = scene;
        tele_scene(scene, 1, 0);
    }
}

static void op_SCRIPT_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *es, command_state_t *cs) {
    int16_t sn = es_variables(es)->script_number + 1;
    if (sn == 11) sn = 0;
    cs_push(cs, sn);
}

static void op_SCRIPT_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *es, command_state_t *cs) {
    uint16_t a = cs_pop(cs) - 1;
    if (a > INIT_SCRIPT || a < TT_SCRIPT_1) return;

    es_push(es);
    // an overflow causes all future SCRIPT calls to fail
    // indicates a bad user script
    if (!es->overflow) run_script_with_exec_state(ss, es, a);
    es_pop(es);
}

static void op_SCRIPT_POL_get(const void *NOTUSED(data), scene_state_t *ss,
                              exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint16_t a = cs_pop(cs) - 1;
    if (a > TT_SCRIPT_8 || a < TT_SCRIPT_1) {
        cs_push(cs, 0);
        return;
    }
    cs_push(cs, ss_get_script_pol(ss, a));
}

static void op_SCRIPT_POL_set(const void *NOTUSED(data), scene_state_t *ss,
                              exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t a = cs_pop(cs);
    uint8_t pol = cs_pop(cs);
    if (pol > 3) return;
    if (a == 0) {
        for (uint8_t i = 0; i < 8; i++) { ss_set_script_pol(ss, i, pol); }
    }
    else {
        uint8_t s = a - 1;
        if (s >= TT_SCRIPT_1 && s <= TT_SCRIPT_8) {
            ss_set_script_pol(ss, s, pol);
        }
    }
}

static void op_KILL_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es),
                        command_state_t *NOTUSED(cs)) {
    // clear stack
    ss->stack_op.top = 0;
    tele_has_stack(false);
    // disable metronome
    ss->variables.m_act = 0;
    tele_metro_updated();
    clear_delays(ss);
    tele_kill();
}

static void op_BREAK_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *es, command_state_t *NOTUSED(cs)) {
    es_variables(es)->breaking = true;
}
