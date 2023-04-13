#include "ops/midi.h"

#include "helpers.h"
#include "table.h"
#include "teletype_io.h"

static void op_MI_SYM_DOLLAR_get(const void *data, scene_state_t *ss,
                                 exec_state_t *es, command_state_t *cs);
static void op_MI_SYM_DOLLAR_set(const void *data, scene_state_t *ss,
                                 exec_state_t *es, command_state_t *cs);
static void op_MI_LE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_MI_LN_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_MI_LNV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_MI_LV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_MI_LVV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_MI_LO_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_MI_LC_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_MI_LCC_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_MI_LCCV_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_MI_NL_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_MI_N_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_MI_NV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_MI_V_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_MI_VV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_MI_OL_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_MI_O_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_MI_CL_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_MI_C_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_MI_CC_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_MI_CCV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_MI_LCH_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_MI_NCH_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_MI_OCH_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_MI_CCH_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_MI_CLKD_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_MI_CLKD_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_MI_CLKR_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);

// clang-format off

const tele_op_t op_MI_SYM_DOLLAR = MAKE_GET_SET_OP(MI.$, op_MI_SYM_DOLLAR_get, op_MI_SYM_DOLLAR_set, 1, true);
const tele_op_t op_MI_LE   = MAKE_GET_OP(MI.LE,   op_MI_LE_get,   0, true);
const tele_op_t op_MI_LN   = MAKE_GET_OP(MI.LN,   op_MI_LN_get,   0, true);
const tele_op_t op_MI_LNV  = MAKE_GET_OP(MI.LNV,  op_MI_LNV_get,  0, true);
const tele_op_t op_MI_LV   = MAKE_GET_OP(MI.LV,   op_MI_LV_get,   0, true);
const tele_op_t op_MI_LVV  = MAKE_GET_OP(MI.LVV,  op_MI_LVV_get,  0, true);
const tele_op_t op_MI_LO   = MAKE_GET_OP(MI.LO,   op_MI_LO_get,   0, true);
const tele_op_t op_MI_LC   = MAKE_GET_OP(MI.LC,   op_MI_LC_get,   0, true);
const tele_op_t op_MI_LCC  = MAKE_GET_OP(MI.LCC,  op_MI_LCC_get,  0, true);
const tele_op_t op_MI_LCCV = MAKE_GET_OP(MI.LCCV, op_MI_LCCV_get, 0, true);
const tele_op_t op_MI_NL   = MAKE_GET_OP(MI.NL,   op_MI_NL_get,   0, true);
const tele_op_t op_MI_N    = MAKE_GET_OP(MI.N,    op_MI_N_get,    0, true);
const tele_op_t op_MI_NV   = MAKE_GET_OP(MI.NV,   op_MI_NV_get,   0, true);
const tele_op_t op_MI_V    = MAKE_GET_OP(MI.V,    op_MI_V_get,    0, true);
const tele_op_t op_MI_VV   = MAKE_GET_OP(MI.VV,   op_MI_VV_get,   0, true);
const tele_op_t op_MI_OL   = MAKE_GET_OP(MI.OL,   op_MI_OL_get,   0, true);
const tele_op_t op_MI_O    = MAKE_GET_OP(MI.O,    op_MI_O_get,    0, true);
const tele_op_t op_MI_CL   = MAKE_GET_OP(MI.CL,   op_MI_CL_get,   0, true);
const tele_op_t op_MI_C    = MAKE_GET_OP(MI.C,    op_MI_C_get,    0, true);
const tele_op_t op_MI_CC   = MAKE_GET_OP(MI.CC,   op_MI_CC_get,   0, true);
const tele_op_t op_MI_CCV  = MAKE_GET_OP(MI.CCV,  op_MI_CCV_get,  0, true);
const tele_op_t op_MI_LCH  = MAKE_GET_OP(MI.LCH,  op_MI_LCH_get,  0, true);
const tele_op_t op_MI_NCH  = MAKE_GET_OP(MI.NCH,  op_MI_NCH_get,  0, true);
const tele_op_t op_MI_OCH  = MAKE_GET_OP(MI.OCH,  op_MI_OCH_get,  0, true);
const tele_op_t op_MI_CCH  = MAKE_GET_OP(MI.CCH,  op_MI_CCH_get,  0, true);
const tele_op_t op_MI_CLKR = MAKE_GET_OP(MI.CLKR, op_MI_CLKR_get, 0, false);
const tele_op_t op_MI_CLKD = MAKE_GET_SET_OP(MI.CLKD, op_MI_CLKD_get, op_MI_CLKD_set, 0, true);

// clang-format on

static void op_MI_SYM_DOLLAR_get(const void *NOTUSED(data), scene_state_t *ss,
                                 exec_state_t *NOTUSED(es),
                                 command_state_t *cs) {
    uint16_t event = cs_pop(cs);
    int16_t script = -1;

    switch (event) {
        case 0:
            script = ss->midi.on_script;
            if (script != ss->midi.off_script || script != ss->midi.cc_script ||
                script != ss->midi.clk_script ||
                script != ss->midi.start_script ||
                script != ss->midi.stop_script ||
                script != ss->midi.continue_script)
                script = -1;
            break;
        case 1: script = ss->midi.on_script; break;
        case 2: script = ss->midi.off_script; break;
        case 3: script = ss->midi.cc_script; break;
        case 4: script = ss->midi.clk_script; break;
        case 5: script = ss->midi.start_script; break;
        case 6: script = ss->midi.stop_script; break;
        case 7: script = ss->midi.continue_script; break;
        default: break;
    }

    cs_push(cs, script == -1 ? script : script + 1);
}

static void op_MI_SYM_DOLLAR_set(const void *NOTUSED(data), scene_state_t *ss,
                                 exec_state_t *NOTUSED(es),
                                 command_state_t *cs) {
    uint16_t event = cs_pop(cs);
    s16 script = cs_pop(cs) - 1;
    if (script < 0 || script > INIT_SCRIPT) script = -1;

    switch (event) {
        case 0:
            ss->midi.on_script = script;
            ss->midi.off_script = script;
            ss->midi.cc_script = script;
            ss->midi.clk_script = script;
            ss->midi.start_script = script;
            ss->midi.stop_script = script;
            ss->midi.continue_script = script;
            ss->midi.on_count = 0;
            ss->midi.off_count = 0;
            ss->midi.cc_count = 0;
            break;
        case 1:
            ss->midi.on_script = script;
            ss->midi.on_count = 0;
            break;
        case 2:
            ss->midi.off_script = script;
            ss->midi.off_count = 0;
            break;
        case 3:
            ss->midi.cc_script = script;
            ss->midi.cc_count = 0;
            break;
        case 4: ss->midi.clk_script = script; break;
        case 5: ss->midi.start_script = script; break;
        case 6: ss->midi.stop_script = script; break;
        case 7: ss->midi.continue_script = script; break;
        default: break;
    }
}

static void op_MI_LE_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->midi.last_event_type);
}

static void op_MI_LN_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->midi.last_note);
}

static void op_MI_LNV_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, table_n[ss->midi.last_note]);
}

static void op_MI_LV_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->midi.last_velocity);
}

static void op_MI_LVV_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->midi.last_velocity * 129);
}

static void op_MI_LO_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->midi.last_note);
}

static void op_MI_LC_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->midi.last_controller);
}

static void op_MI_LCC_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->midi.last_cc);
}

static void op_MI_LCCV_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->midi.last_cc * 129);
}

static void op_MI_NL_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->midi.on_count);
}

static void op_MI_N_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *es, command_state_t *cs) {
    s16 i = es_variables(es)->i;
    cs_push(cs, i < 1 || i > ss->midi.on_count ? 0 : ss->midi.note_on[i - 1]);
}

static void op_MI_NV_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *es, command_state_t *cs) {
    s16 i = es_variables(es)->i;
    cs_push(cs, i < 1 || i > ss->midi.on_count
                    ? 0
                    : table_n[ss->midi.note_on[i - 1]]);
}

static void op_MI_V_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *es, command_state_t *cs) {
    s16 i = es_variables(es)->i;
    cs_push(cs, i < 1 || i > ss->midi.on_count ? 0 : ss->midi.note_vel[i - 1]);
}

static void op_MI_VV_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *es, command_state_t *cs) {
    s16 i = es_variables(es)->i;
    cs_push(cs, i < 1 || i > ss->midi.on_count
                    ? 0
                    : ss->midi.note_vel[i - 1] * 129);
}

static void op_MI_OL_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->midi.off_count);
}

static void op_MI_O_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *es, command_state_t *cs) {
    s16 i = es_variables(es)->i;
    cs_push(cs, i < 1 || i > ss->midi.off_count ? 0 : ss->midi.note_off[i - 1]);
}

static void op_MI_CL_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->midi.cc_count);
}

static void op_MI_C_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *es, command_state_t *cs) {
    s16 i = es_variables(es)->i;
    cs_push(cs, i < 1 || i > ss->midi.cc_count ? 0 : ss->midi.cn[i - 1]);
}

static void op_MI_CC_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *es, command_state_t *cs) {
    s16 i = es_variables(es)->i;
    cs_push(cs, i < 1 || i > ss->midi.cc_count ? 0 : ss->midi.cc[i - 1]);
}

static void op_MI_CCV_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *es, command_state_t *cs) {
    s16 i = es_variables(es)->i;
    cs_push(cs, i < 1 || i > ss->midi.cc_count ? 0 : ss->midi.cc[i - 1] * 129);
}

static void op_MI_LCH_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->midi.last_channel + 1);
}

static void op_MI_NCH_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *es, command_state_t *cs) {
    s16 i = es_variables(es)->i;
    cs_push(cs, i < 1 || i > ss->midi.on_count
                    ? 0
                    : ss->midi.on_channel[i - 1] + 1);
}

static void op_MI_OCH_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *es, command_state_t *cs) {
    s16 i = es_variables(es)->i;
    cs_push(cs, i < 1 || i > ss->midi.off_count
                    ? 0
                    : ss->midi.off_channel[i - 1] + 1);
}

static void op_MI_CCH_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *es, command_state_t *cs) {
    s16 i = es_variables(es)->i;
    cs_push(cs, i < 1 || i > ss->midi.cc_count
                    ? 0
                    : ss->midi.cc_channel[i - 1] + 1);
}

static void op_MI_CLKD_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->midi.clock_div);
}

static void op_MI_CLKD_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 clock_div = cs_pop(cs);
    if (clock_div < 1 || clock_div > 24) return;
    ss->midi.clock_div = clock_div;
    reset_midi_counter();
}

static void op_MI_CLKR_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    reset_midi_counter();
}
