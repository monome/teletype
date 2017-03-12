#include <ctype.h>   // isdigit
#include <stdint.h>  // types
#include <stdio.h>   // printf
#include <stdlib.h>  // rand, strtol
#include <string.h>

#include "helpers.h"
#include "ops/op.h"
#include "table.h"
#include "teletype.h"
#include "teletype_io.h"
#include "util.h"

#ifdef SIM
#define DBG printf("%s", dbg);
#else
#define DBG print_dbg(dbg);
#endif

// static char dbg[32];

static const char *errordesc[] = { "OK",
                                   WELCOME,
                                   "UNKNOWN WORD",
                                   "COMMAND TOO LONG",
                                   "NOT ENOUGH PARAMS",
                                   "TOO MANY PARAMS",
                                   "MOD NOT ALLOWED HERE",
                                   "EXTRA SEPARATOR",
                                   "NEED SEPARATOR",
                                   "BAD SEPARATOR",
                                   "MOVE LEFT" };

const char *tele_error(error_t e) {
    return errordesc[e];
}


/////////////////////////////////////////////////////////////////
// STATE ////////////////////////////////////////////////////////

// eventually these will not be global variables
static scene_state_t scene_state = {
    // variables that haven't been explicitly initialised, will be set to 0
    .variables = {.a = 1,
                  .b = 2,
                  .c = 3,
                  .cv_slew = { 1, 1, 1, 1 },
                  .d = 4,
                  .drunk_min = 0,
                  .drunk_max = 255,
                  .m = 1000,
                  .m_act = 1,
                  .o_inc = 1,
                  .o_min = 0,
                  .o_max = 63,
                  .o_wrap = 1,
                  .q_n = 1,
                  .time_act = 1,
                  .tr_pol = { 1, 1, 1, 1 },
                  .tr_time = { 100, 100, 100, 100 } }
};

/////////////////////////////////////////////////////////////////
// DELAY ////////////////////////////////////////////////////////

void clear_delays(void) {
    for (int16_t i = 0; i < TR_COUNT; i++) {
        scene_state.tr_pulse_timer[i] = 0;
    }

    for (int16_t i = 0; i < DELAY_SIZE; i++) { scene_state.delay.time[i] = 0; }

    scene_state.delay.count = 0;

    scene_state.stack_op.top = 0;

    tele_delay(0);
    tele_s(0);
}


/////////////////////////////////////////////////////////////////
// PARSE ////////////////////////////////////////////////////////

error_t parse(const char *cmd, tele_command_t *out,
              char error_msg[ERROR_MSG_LENGTH]) {
    error_msg[0] = 0;
    char cmd_copy[32];
    strcpy(cmd_copy, cmd);
    const char *delim = " \n";
    const char *s = strtok(cmd_copy, delim);

    uint8_t n = 0;
    out->l = n;
    out->separator = -1;

    while (s) {
        // CHECK IF NUMBER
        if (isdigit(s[0]) || s[0] == '-') {
            out->data[n].t = NUMBER;
            out->data[n].v = strtol(s, NULL, 0);
        }
        else if (s[0] == ':') {
            out->data[n].t = SEP;
            out->separator = n;
        }
        else {
            int16_t i = -1;

            if (i == -1) {
                // CHECK AGAINST OPS
                i = OPS;

                while (i--) {
                    if (!strcmp(s, tele_ops[i]->name)) {
                        out->data[n].t = OP;
                        out->data[n].v = i;
                        break;
                    }
                }
            }

            if (i == -1) {
                // CHECK AGAINST MOD
                i = MODS;

                while (i--) {
                    if (!strcmp(s, tele_mods[i]->name)) {
                        out->data[n].t = MOD;
                        out->data[n].v = i;
                        break;
                    }
                }
            }

            if (i == -1) {
                strcpy(error_msg, s);
                return E_PARSE;
            }
        }

        s = strtok(NULL, delim);

        n++;
        out->l = n;

        if (n == COMMAND_MAX_LENGTH) return E_LENGTH;
    }

    return E_OK;
}

/////////////////////////////////////////////////////////////////
// VALIDATE /////////////////////////////////////////////////////

error_t validate(const tele_command_t *c, char error_msg[ERROR_MSG_LENGTH]) {
    error_msg[0] = 0;
    int16_t stack_depth = 0;
    uint8_t idx = c->l;
    int8_t sep_count = 0;

    while (idx--) {  // process words right to left
        tele_word_t word_type = c->data[idx].t;
        int16_t word_value = c->data[idx].v;
        // A first_cmd is either at the beginning of the command or immediately
        // after the SEP
        bool first_cmd = idx == 0 || c->data[idx - 1].t == SEP;

        if (word_type == NUMBER) { stack_depth++; }
        else if (word_type == OP) {
            const tele_op_t *op = tele_ops[word_value];

            // if we're not a first_cmd we need to return something
            if (!first_cmd && !op->returns) {
                strcpy(error_msg, op->name);
                return E_NOT_LEFT;
            }

            stack_depth -= op->params;

            if (stack_depth < 0) {
                strcpy(error_msg, op->name);
                return E_NEED_PARAMS;
            }

            stack_depth += op->returns ? 1 : 0;

            // if we are in the first_cmd position and there is a set fn
            // decrease the stack depth
            // TODO this is technically wrong. the only reason we get away with
            // it is that it's idx == 0, and the while loop is about to end.
            if (first_cmd && op->set != NULL) stack_depth--;
        }
        else if (word_type == MOD) {
            error_t mod_error = E_OK;

            if (idx != 0)
                mod_error = E_NO_MOD_HERE;
            else if (c->separator == -1)
                mod_error = E_NEED_SEP;
            else if (stack_depth < tele_mods[word_value]->params)
                mod_error = E_NEED_PARAMS;
            else if (stack_depth > tele_mods[word_value]->params)
                mod_error = E_EXTRA_PARAMS;

            if (mod_error != E_OK) {
                strcpy(error_msg, tele_mods[word_value]->name);
                return mod_error;
            }

            stack_depth = 0;
        }
        else if (word_type == SEP) {
            sep_count++;
            if (sep_count > 1)
                return E_MANY_SEP;
            else if (idx == 0)
                return E_PLACE_SEP;

            if (stack_depth > 1)
                return E_EXTRA_PARAMS;
            else
                stack_depth = 0;
        }
    }

    if (stack_depth > 1)
        return E_EXTRA_PARAMS;
    else
        return E_OK;
}

/////////////////////////////////////////////////////////////////
// RUN //////////////////////////////////////////////////////////

process_result_t run_script(size_t script_no) {
    process_result_t result = {.has_value = false, .value = 0 };
    exec_state_t es;
    es_init(&es);
    for (size_t i = 0; i < tele_get_script_l(script_no); i++) {
        result = process(&es, tele_get_script_c(script_no, i));
    }
    return result;
}

process_result_t run_command(const tele_command_t *cmd) {
    exec_state_t es;
    es_init(&es);
    return process(&es, cmd);
}


/////////////////////////////////////////////////////////////////
// PROCESS //////////////////////////////////////////////////////

process_result_t process(exec_state_t *es, const tele_command_t *c) {
    command_state_t cs;
    cs_init(&cs);

    // if the command has a MOD, only process it
    // allow the MOD to deal with processing the remainder
    int16_t idx = c->separator == -1 ? c->l : c->separator;

    while (idx--) {  // process from right to left
        tele_word_t word_type = c->data[idx].t;
        int16_t word_value = c->data[idx].v;

        if (word_type == NUMBER) { cs_push(&cs, word_value); }
        else if (word_type == OP) {
            const tele_op_t *op = tele_ops[word_value];

            // if we're in the first command position, and there is a set fn
            // pointer and we have enough params, then run set, else run get
            if (idx == 0 && op->set != NULL &&
                cs_stack_size(&cs) >= op->params + 1)
                op->set(op->data, &scene_state, es, &cs);
            else
                op->get(op->data, &scene_state, es, &cs);
        }
        else if (word_type == MOD) {
            tele_command_t sub_command;
            copy_sub_command(&sub_command, c);
            tele_mods[word_value]->func(&scene_state, es, &cs, &sub_command);
        }
    }

    if (cs_stack_size(&cs)) {
        process_result_t o = {.has_value = true, .value = cs_pop(&cs) };
        return o;
    }
    else {
        process_result_t o = {.has_value = false, .value = 0 };
        return o;
    }
}

/////////////////////////////////////////////////////////////////
// GETTERS & SETTERS ////////////////////////////////////////////

void tele_set_in(int16_t value) {
    scene_state.variables.in = value;
}

void tele_set_param(int16_t value) {
    scene_state.variables.param = value;
}

void tele_set_scene(int16_t value) {
    scene_state.variables.scene = value;
}

int16_t tele_get_pattern_i(size_t pattern) {
    return scene_state.patterns[pattern].i;
}

void tele_set_pattern_i(size_t pattern, int16_t i) {
    scene_state.patterns[pattern].i = i;
}

int16_t tele_get_pattern_l(size_t pattern) {
    return scene_state.patterns[pattern].l;
}

void tele_set_pattern_l(size_t pattern, int16_t l) {
    scene_state.patterns[pattern].l = l;
}

uint16_t tele_get_pattern_wrap(size_t pattern) {
    return scene_state.patterns[pattern].wrap;
}

void tele_set_pattern_wrap(size_t pattern, uint16_t wrap) {
    scene_state.patterns[pattern].wrap = wrap;
}

int16_t tele_get_pattern_start(size_t pattern) {
    return scene_state.patterns[pattern].start;
}

void tele_set_pattern_start(size_t pattern, int16_t start) {
    scene_state.patterns[pattern].start = start;
}

int16_t tele_get_pattern_end(size_t pattern) {
    return scene_state.patterns[pattern].end;
}

void tele_set_pattern_end(size_t pattern, int16_t end) {
    scene_state.patterns[pattern].end = end;
}

int16_t tele_get_pattern_val(size_t pattern, size_t idx) {
    return scene_state.patterns[pattern].v[idx];
}

void tele_set_pattern_val(size_t pattern, size_t idx, int16_t val) {
    scene_state.patterns[pattern].v[idx] = val;
}

scene_pattern_t *tele_patterns_ptr() {
    return scene_state.patterns;
}

size_t tele_patterns_size() {
    return sizeof(scene_state.patterns);
}

uint8_t tele_get_script_l(size_t idx) {
    return scene_state.scripts[idx].l;
}

void tele_set_script_l(size_t idx, uint8_t l) {
    scene_state.scripts[idx].l = l;
}

const tele_command_t *tele_get_script_c(size_t script_idx, size_t c_idx) {
    return &scene_state.scripts[script_idx].c[c_idx];
}

void tele_set_script_c(size_t script_idx, size_t c_idx,
                       const tele_command_t *cmd) {
    memcpy(&scene_state.scripts[script_idx].c[c_idx], cmd,
           sizeof(tele_command_t));
}

scene_script_t *tele_script_ptr() {
    return scene_state.scripts;
}

size_t tele_script_size() {
    return sizeof(scene_state.scripts);
}

/////////////////////////////////////////////////////////////////
// TICK /////////////////////////////////////////////////////////

void tele_tick(uint8_t time) {
    // inc time
    if (scene_state.variables.time_act) scene_state.variables.time += time;

    // process delays
    for (int16_t i = 0; i < DELAY_SIZE; i++) {
        if (scene_state.delay.time[i]) {
            scene_state.delay.time[i] -= time;
            if (scene_state.delay.time[i] <= 0) {
                // sprintf(dbg,"\r\ndelay %d", i);
                // DBG
                run_command(&scene_state.delay.commands[i]);
                scene_state.delay.time[i] = 0;
                scene_state.delay.count--;
                if (scene_state.delay.count == 0) tele_delay(0);
            }
        }
    }

    // process tr pulses
    for (int16_t i = 0; i < TR_COUNT; i++) {
        if (scene_state.tr_pulse_timer[i]) {
            scene_state.tr_pulse_timer[i] -= time;
            if (scene_state.tr_pulse_timer[i] <= 0) {
                scene_state.tr_pulse_timer[i] = 0;
                scene_state.variables.tr[i] =
                    scene_state.variables.tr_pol[i] == 0;
                tele_tr(i, scene_state.variables.tr[i]);
            }
        }
    }
}

/////////////////////////////////////////////////////////////////
// INIT /////////////////////////////////////////////////////////

void tele_init() {
    for (size_t i = 0; i < 4; i++) {
        tele_set_pattern_i(i, 0);
        tele_set_pattern_l(i, 0);
        tele_set_pattern_wrap(i, 1);
        tele_set_pattern_start(i, 0);
        tele_set_pattern_end(i, 63);
    }
}
