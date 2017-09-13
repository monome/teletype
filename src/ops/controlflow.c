#include "ops/controlflow.h"

#include <stdlib.h>

#include "helpers.h"
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

static void op_SCENE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_SCENE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_SCRIPT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KILL_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_BREAK_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);


const tele_mod_t mod_PROB = MAKE_MOD(PROB, mod_PROB_func, 1);
const tele_mod_t mod_IF = MAKE_MOD(IF, mod_IF_func, 1);
const tele_mod_t mod_ELIF = MAKE_MOD(ELIF, mod_ELIF_func, 1);
const tele_mod_t mod_ELSE = MAKE_MOD(ELSE, mod_ELSE_func, 0);
const tele_mod_t mod_L = MAKE_MOD(L, mod_L_func, 2);
const tele_mod_t mod_W = MAKE_MOD(W, mod_W_func, 1);

const tele_op_t op_SCRIPT = MAKE_GET_OP(SCRIPT, op_SCRIPT_get, 1, false);
const tele_op_t op_KILL = MAKE_GET_OP(KILL, op_KILL_get, 0, false);
const tele_op_t op_SCENE =
    MAKE_GET_SET_OP(SCENE, op_SCENE_get, op_SCENE_set, 0, true);
const tele_op_t op_BREAK = MAKE_GET_OP(BREAK, op_BREAK_get, 0, false);
const tele_op_t op_BRK = MAKE_ALIAS_OP(BRK, op_BREAK_get, NULL, 0, false);


static void mod_PROB_func(scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs,
                          const tele_command_t *post_command) {
    int16_t a = cs_pop(cs);

    if (rand() % 101 < a) { process_command(ss, es, post_command); }
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

    if (a < b) {
        for(*i = a; *i <= b; (*i)++)
            process_command(ss, es, post_command);
        (*i)--;  // leave es_variables.i in the correct state
    }
    else {
        for(*i = a; *i >= b; (*i)--) 
            process_command(ss, es, post_command);
        (*i)++;
    }
}

static void mod_W_func(scene_state_t *ss, exec_state_t *es, command_state_t *cs,
                       const tele_command_t *post_command) {
    int16_t a = cs_pop(cs);
    if (a) {
        process_command(ss, es, post_command);
        es_variables(es)->while_depth++;
        if(es_variables(es)->while_depth < WHILE_DEPTH)
            es_variables(es)->while_continue = true;
        else
            es_variables(es)->while_continue = false;

    }
    else
        es_variables(es)->while_continue = false;
}

static void op_SCENE_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->variables.scene);
}

static void op_SCENE_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t scene = cs_pop(cs);
    if (!ss->initializing) {
        ss->variables.scene = scene;
        tele_scene(scene);
    }
}

static void op_SCRIPT_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *es, command_state_t *cs) {
    uint16_t a = cs_pop(cs) - 1;
    if (a >= SCRIPT_COUNT || a == INIT_SCRIPT || a == METRO_SCRIPT) return;

    es_push(es);
    // an overflow causes all future SCRIPT calls to fail
    // indicates a bad user script
    if (!es->overflow)
        run_script_with_exec_state(ss, es, a);
    es_pop(es);
}

static void op_KILL_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es),
                        command_state_t *NOTUSED(cs)) {
    clear_delays(ss);
    tele_kill();
}

static void op_BREAK_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                        exec_state_t *es, command_state_t *NOTUSED(cs)) {
    es_variables(es)->breaking = true;
}
