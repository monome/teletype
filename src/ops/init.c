#include "ops/init.h"

#include <string.h>  // memset()

#include "helpers.h"
#include "ops/op.h"
#include "teletype.h"
#include "teletype_io.h"

static void op_INIT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_INIT_SCENE_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_INIT_SCRIPT_get(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs);
static void op_INIT_SCRIPT_ALL_get(const void *data, scene_state_t *ss,
                                   exec_state_t *es, command_state_t *cs);
static void op_INIT_P_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_INIT_P_ALL_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_INIT_CV_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_INIT_CV_ALL_get(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs);
static void op_INIT_TR_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_INIT_TR_ALL_get(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs);
static void op_INIT_DATA_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_INIT_TIME_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);

const tele_op_t op_INIT = MAKE_GET_OP(INIT, op_INIT_get, 0, false);
const tele_op_t op_INIT_SCENE =
    MAKE_GET_OP(INIT.SCENE, op_INIT_SCENE_get, 0, false);
const tele_op_t op_INIT_SCRIPT =
    MAKE_GET_OP(INIT.SCRIPT, op_INIT_SCRIPT_get, 1, false);
const tele_op_t op_INIT_SCRIPT_ALL =
    MAKE_GET_OP(INIT.SCRIPT.ALL, op_INIT_SCRIPT_ALL_get, 0, false);
const tele_op_t op_INIT_P = MAKE_GET_OP(INIT.P, op_INIT_P_get, 1, false);
const tele_op_t op_INIT_P_ALL =
    MAKE_GET_OP(INIT.P.ALL, op_INIT_P_ALL_get, 0, false);
const tele_op_t op_INIT_CV = MAKE_GET_OP(INIT.CV, op_INIT_CV_get, 1, false);
const tele_op_t op_INIT_CV_ALL =
    MAKE_GET_OP(INIT.CV.ALL, op_INIT_CV_ALL_get, 0, false);
const tele_op_t op_INIT_TR = MAKE_GET_OP(INIT.TR, op_INIT_TR_get, 1, false);
const tele_op_t op_INIT_TR_ALL =
    MAKE_GET_OP(INIT.TR.ALL, op_INIT_TR_ALL_get, 0, false);
const tele_op_t op_INIT_DATA =
    MAKE_GET_OP(INIT.DATA, op_INIT_DATA_get, 0, false);
const tele_op_t op_INIT_TIME =
    MAKE_GET_OP(INIT.TIME, op_INIT_TIME_get, 0, false);


// identical with below, for now.
static void op_INIT_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es),
                        command_state_t *NOTUSED(cs)) {
    // Because we can't see the flash from this context, we cache calibration
    cal_data_t caldata = ss->cal;
    // At boot, all data is zeroed
    memset(ss, 0, sizeof(scene_state_t));
    ss_init(ss);

    ss->cal = caldata;
    // Once calibration data is loaded, the scales need to be reset
    ss_update_param_scale(ss);
    ss_update_in_scale(ss);
    ss_update_fader_scale_all(ss);

    tele_vars_updated();
    tele_metro_updated();
}

static void op_INIT_SCENE_get(const void *NOTUSED(data), scene_state_t *ss,
                              exec_state_t *NOTUSED(es),
                              command_state_t *NOTUSED(cs)) {
    cal_data_t caldata = ss->cal;
    memset(ss, 0, sizeof(scene_state_t));
    ss_init(ss);
    ss->cal = caldata;
    ss_update_param_scale(ss);
    ss_update_in_scale(ss);
    ss_update_fader_scale_all(ss);

    tele_vars_updated();
    tele_metro_updated();
}

static void op_INIT_SCRIPT_get(const void *NOTUSED(data), scene_state_t *ss,
                               exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v = cs_pop(cs) - 1;
    if (v >= 0 && v < EDITABLE_SCRIPT_COUNT) ss_clear_script(ss, (size_t)v);
}

static void op_INIT_SCRIPT_ALL_get(const void *NOTUSED(data), scene_state_t *ss,
                                   exec_state_t *NOTUSED(es),
                                   command_state_t *NOTUSED(cs)) {
    for (size_t i = 0; i < EDITABLE_SCRIPT_COUNT; i++) ss_clear_script(ss, i);
}

static void op_INIT_P_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v = cs_pop(cs);
    if (v >= 0 && v < 4) {
        ss_pattern_init(ss, v);
        tele_pattern_updated();
    }
}

static void op_INIT_P_ALL_get(const void *NOTUSED(data), scene_state_t *ss,
                              exec_state_t *NOTUSED(es),
                              command_state_t *NOTUSED(cs)) {
    ss_patterns_init(ss);
    tele_pattern_updated();
}

static void op_INIT_CV_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v = cs_pop(cs) - 1;
    if (v >= 0 && v < TR_COUNT) {
        ss->variables.cv[v] = 0;
        ss->variables.cv_off[v] = 0;
        ss->variables.cv_slew[v] = 1;
        tele_cv(v, 0, 1);
    }
}

static void op_INIT_CV_ALL_get(const void *NOTUSED(data), scene_state_t *ss,
                               exec_state_t *NOTUSED(es),
                               command_state_t *NOTUSED(cs)) {
    for (size_t i = 0; i < TR_COUNT; i++) {
        ss->variables.cv[i] = 0;
        ss->variables.cv_off[i] = 0;
        ss->variables.cv_slew[i] = 1;
        tele_cv(i, 0, 1);
    }
}

static void op_INIT_TR_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v = cs_pop(cs) - 1;
    if (v >= 0 && v < TR_COUNT) {
        ss->variables.tr[v] = 0;
        ss->variables.tr_pol[v] = 1;
        ss->variables.tr_time[v] = 100;
        tele_tr_pulse_clear(v);
        tele_tr(v, 0);
    }
}

static void op_INIT_TR_ALL_get(const void *NOTUSED(data), scene_state_t *ss,
                               exec_state_t *NOTUSED(es),
                               command_state_t *NOTUSED(cs)) {
    for (size_t i = 0; i < TR_COUNT; i++) {
        ss->variables.tr[i] = 0;
        ss->variables.tr_pol[i] = 1;
        ss->variables.tr_time[i] = 100;
        tele_tr_pulse_clear(i);
        tele_tr(i, 0);
    }
}

static void op_INIT_DATA_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es),
                             command_state_t *NOTUSED(cs)) {
    ss_variables_init(ss);
    tele_vars_updated();
    tele_metro_updated();
}

static void op_INIT_TIME_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es),
                             command_state_t *NOTUSED(cs)) {
    clear_delays(ss);
    ss->variables.time = 0;
    uint32_t ticks = tele_get_ticks();
    for (uint8_t i = 0; i < EDITABLE_SCRIPT_COUNT; i++)
        ss->scripts[i].last_time = ticks;
    ss->variables.time = 0;
    ss_sync_every(ss, 0);
}
