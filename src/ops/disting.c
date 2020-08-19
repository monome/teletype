#include "ops/disting.h"
#include "helpers.h"
#include "ii.h"
#include "teletype.h"
#include "teletype_io.h"

static void mod_EX1_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command);
static void mod_EX2_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command);
static void mod_EX3_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command);
static void mod_EX4_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command);
static void op_EX_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_EX_set(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_EX_PRESET_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_EX_PRESET_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_EX_SAVE_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_EX_RESET_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_EX_ALG_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_EX_ALG_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_EX_CTRL_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_EX_PARAM_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_EX_PARAM_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_EX_PV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_EX_MIN_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_EX_MAX_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_EX_REC_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_EX_PLAY_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_EX_AL_P_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_EX_AL_CLK_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_EX_M_CH_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_EX_M_CH_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_EX_M_N_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_EX_M_NO_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_EX_M_PB_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_EX_M_CC_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_EX_M_PRG_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_EX_M_CLK_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_EX_M_START_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_EX_M_STOP_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_EX_M_CONT_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_EX_SB_CH_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_EX_SB_CH_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_EX_SB_N_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_EX_SB_NO_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_EX_SB_PB_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_EX_SB_CC_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_EX_SB_PRG_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_EX_SB_CLK_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_EX_SB_START_get(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs);
static void op_EX_SB_STOP_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_EX_SB_CONT_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_EX_VOX_P_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_EX_VOX_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_EX_VOX_O_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_EX_NOTE_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_EX_NOTE_O_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_EX_ALLOFF_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_EX_T_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_EX_TV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
// clang-format off
                   
const tele_mod_t mod_EX1       = MAKE_MOD(EX1, mod_EX1_func, 0);
const tele_mod_t mod_EX2       = MAKE_MOD(EX2, mod_EX2_func, 0);
const tele_mod_t mod_EX3       = MAKE_MOD(EX3, mod_EX3_func, 0);
const tele_mod_t mod_EX4       = MAKE_MOD(EX4, mod_EX4_func, 0);
                   
const tele_op_t op_EX          = MAKE_GET_SET_OP(EX,        op_EX_get,          op_EX_set, 0, true);
const tele_op_t op_EX_PRESET   = MAKE_GET_SET_OP(EX.PRESET, op_EX_PRESET_get,   op_EX_PRESET_set, 0, true);
const tele_op_t op_EX_SAVE     = MAKE_GET_OP(EX.SAVE,       op_EX_SAVE_get,     1, false);
const tele_op_t op_EX_RESET    = MAKE_GET_OP(EX.RESET,      op_EX_RESET_get,    0, false);
const tele_op_t op_EX_ALG      = MAKE_GET_SET_OP(EX.ALG,    op_EX_ALG_get,      op_EX_ALG_set, 0, true);
const tele_op_t op_EX_CTRL     = MAKE_GET_OP(EX.CTRL,       op_EX_CTRL_get,     2, false);
const tele_op_t op_EX_PARAM    = MAKE_GET_SET_OP(EX.PARAM,  op_EX_PARAM_get,    op_EX_PARAM_set, 1, true);
const tele_op_t op_EX_PV       = MAKE_GET_OP(EX.PV,         op_EX_PV_get,       2, false);
const tele_op_t op_EX_MIN      = MAKE_GET_OP(EX.MIN,        op_EX_MIN_get,      1, true);
const tele_op_t op_EX_MAX      = MAKE_GET_OP(EX.MAX,        op_EX_MAX_get,      1, true);
const tele_op_t op_EX_REC      = MAKE_GET_OP(EX.REC,        op_EX_REC_get,      1, false);
const tele_op_t op_EX_PLAY     = MAKE_GET_OP(EX.PLAY,       op_EX_PLAY_get,     1, false);
const tele_op_t op_EX_AL_P     = MAKE_GET_OP(EX.AL.P,       op_EX_AL_P_get,     1, false);
const tele_op_t op_EX_AL_CLK   = MAKE_GET_OP(EX.AL.CLK,     op_EX_AL_CLK_get,   0, false);

const tele_op_t op_EX_M_CH     = MAKE_GET_SET_OP(EX.M.CH,   op_EX_M_CH_get,     op_EX_M_CH_set, 0, true);
const tele_op_t op_EX_M_N      = MAKE_GET_OP(EX.M.N,        op_EX_M_N_get,      2, false);
const tele_op_t op_EX_M_NO     = MAKE_GET_OP(EX.M.NO,       op_EX_M_NO_get,     1, false);
const tele_op_t op_EX_M_PB     = MAKE_GET_OP(EX.M.PB,       op_EX_M_PB_get,     1, false);
const tele_op_t op_EX_M_CC     = MAKE_GET_OP(EX.M.CC,       op_EX_M_CC_get,     2, false);
const tele_op_t op_EX_M_PRG    = MAKE_GET_OP(EX.M.PRG,      op_EX_M_PRG_get,    1, false);
const tele_op_t op_EX_M_CLK    = MAKE_GET_OP(EX.M.CLK,      op_EX_M_CLK_get,    0, false);
const tele_op_t op_EX_M_START  = MAKE_GET_OP(EX.M.START,    op_EX_M_START_get,  0, false);
const tele_op_t op_EX_M_STOP   = MAKE_GET_OP(EX.M.STOP,     op_EX_M_STOP_get,   0, false);
const tele_op_t op_EX_M_CONT   = MAKE_GET_OP(EX.M.CONT,     op_EX_M_CONT_get,   0, false);

const tele_op_t op_EX_SB_CH    = MAKE_GET_SET_OP(EX.SB.CH,  op_EX_SB_CH_get,    op_EX_SB_CH_set, 0, true);
const tele_op_t op_EX_SB_N     = MAKE_GET_OP(EX.SB.N,       op_EX_SB_N_get,     2, false);
const tele_op_t op_EX_SB_NO    = MAKE_GET_OP(EX.SB.NO,      op_EX_SB_NO_get,    1, false);
const tele_op_t op_EX_SB_PB    = MAKE_GET_OP(EX.SB.PB,      op_EX_SB_PB_get,    1, false);
const tele_op_t op_EX_SB_CC    = MAKE_GET_OP(EX.SB.CC,      op_EX_SB_CC_get,    2, false);
const tele_op_t op_EX_SB_PRG   = MAKE_GET_OP(EX.SB.PRG,     op_EX_SB_PRG_get,   1, false);
const tele_op_t op_EX_SB_CLK   = MAKE_GET_OP(EX.SB.CLK,     op_EX_SB_CLK_get,   0, false);
const tele_op_t op_EX_SB_START = MAKE_GET_OP(EX.SB.START,   op_EX_SB_START_get, 0, false);
const tele_op_t op_EX_SB_STOP  = MAKE_GET_OP(EX.SB.STOP,    op_EX_SB_STOP_get,  0, false);
const tele_op_t op_EX_SB_CONT  = MAKE_GET_OP(EX.SB.CONT,    op_EX_SB_CONT_get,  0, false);

const tele_op_t op_EX_VOX      = MAKE_GET_OP(EX.VOX,        op_EX_VOX_get,      3, false);
const tele_op_t op_EX_VOX_P    = MAKE_GET_OP(EX.VOX.P,      op_EX_VOX_P_get,    2, false);
const tele_op_t op_EX_VOX_O    = MAKE_GET_OP(EX.VOX.O,      op_EX_VOX_O_get,    1, false);
const tele_op_t op_EX_NOTE     = MAKE_GET_OP(EX.NOTE,       op_EX_NOTE_get,     2, false);
const tele_op_t op_EX_NOTE_O   = MAKE_GET_OP(EX.NOTE.O,     op_EX_NOTE_O_get,   1, false);
const tele_op_t op_EX_ALLOFF   = MAKE_GET_OP(EX.ALLOFF,     op_EX_ALLOFF_get,   0, false);

const tele_op_t op_EX_PRE = MAKE_ALIAS_OP(EX.PRE, op_EX_PRESET_get, op_EX_PRESET_set, 0, true);
const tele_op_t op_EX_A   = MAKE_ALIAS_OP(EX.A,   op_EX_ALG_get,    op_EX_ALG_set,    0, true);
const tele_op_t op_EX_P   = MAKE_ALIAS_OP(EX.P,   op_EX_PARAM_get,  op_EX_PARAM_set,  1, true);
const tele_op_t op_EX_C   = MAKE_ALIAS_OP(EX.C,   op_EX_CTRL_get,   NULL, 2, false);
const tele_op_t op_EX_V   = MAKE_ALIAS_OP(EX.V,   op_EX_VOX_get,    NULL, 3, false);
const tele_op_t op_EX_VP  = MAKE_ALIAS_OP(EX.VP,  op_EX_VOX_P_get,  NULL, 2, false);
const tele_op_t op_EX_VO  = MAKE_ALIAS_OP(EX.VO,  op_EX_VOX_O_get,  NULL, 1, false);
const tele_op_t op_EX_N   = MAKE_ALIAS_OP(EX.N,   op_EX_NOTE_get,   NULL, 2, false);
const tele_op_t op_EX_NO  = MAKE_ALIAS_OP(EX.NO,  op_EX_NOTE_O_get, NULL, 1, false);
const tele_op_t op_EX_AO  = MAKE_ALIAS_OP(EX.AO,  op_EX_ALLOFF_get, NULL, 0, false);
const tele_op_t op_EX_T   = MAKE_ALIAS_OP(EX.T,   op_EX_T_get,      NULL, 1, false);
const tele_op_t op_EX_TV  = MAKE_ALIAS_OP(EX.TV,  op_EX_TV_get,     NULL, 2, false);

// clang-format on

static u8 unit = 0;
static u8 midi_channel = 0;
static u8 sb_channel = 0;
static u8 data[4];

static inline void send1(u8 cmd) {
    data[0] = cmd;
    tele_ii_tx(DISTING_EX_1 + unit, data, 1);
}

static inline void send2(u8 cmd, u8 b1) {
    data[0] = cmd;
    data[1] = b1;
    tele_ii_tx(DISTING_EX_1 + unit, data, 2);
}

static inline void send3(u8 cmd, u8 b1, u8 b2) {
    data[0] = cmd;
    data[1] = b1;
    data[2] = b2;
    tele_ii_tx(DISTING_EX_1 + unit, data, 3);
}

static inline void send4(u8 cmd, u8 b1, u8 b2, u8 b3) {
    data[0] = cmd;
    data[1] = b1;
    data[2] = b2;
    data[3] = b3;
    tele_ii_tx(DISTING_EX_1 + unit, data, 4);
}

static void mod_EX1_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command) {
    u8 u = unit;
    unit = 0;
    process_command(ss, es, post_command);
    unit = u;
}

static void mod_EX2_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command) {
    u8 u = unit;
    unit = 1;
    process_command(ss, es, post_command);
    unit = u;
}

static void mod_EX3_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command) {
    u8 u = unit;
    unit = 2;
    process_command(ss, es, post_command);
    unit = u;
}

static void mod_EX4_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command) {
    u8 u = unit;
    unit = 3;
    process_command(ss, es, post_command);
    unit = u;
}

static void op_EX_get(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, unit + 1);
}

static void op_EX_set(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 u = cs_pop(cs) - 1;
    if (u < 0 || u > 3) return;
    unit = u;
}

static void op_EX_PRESET_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    send1(0x43);

    data[0] = data[1] = 0;
    tele_ii_rx(DISTING_EX_1 + unit, data, 2);

    cs_push(cs, (data[0] << 8) + data[1]);
}

static void op_EX_PRESET_set(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 preset = cs_pop(cs);
    send3(0x40, preset >> 8, preset);
}

static void op_EX_SAVE_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 preset = cs_pop(cs);
    send3(0x41, preset >> 8, preset);
}

static void op_EX_RESET_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    send1(0x42);
}

static void op_EX_ALG_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    send1(0x45);

    data[0] = 0;
    tele_ii_rx(DISTING_EX_1 + unit, data, 1);
    cs_push(cs, data[0]);
}

static void op_EX_ALG_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 algo = cs_pop(cs);
    send2(0x44, algo);
}

static void op_EX_CTRL_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 controller = cs_pop(cs);
    u16 value = cs_pop(cs);
    send4(0x11, controller, value >> 8, value);
}

static void op_EX_PARAM_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 param = cs_pop(cs);
    send2(0x48, param);

    data[0] = data[1] = 0;
    tele_ii_rx(DISTING_EX_1 + unit, data, 2);
    u16 value = (data[0] << 8) + data[1];
    cs_push(cs, (s16)value);
}

static void op_EX_PARAM_set(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 param = cs_pop(cs);
    u16 value = cs_pop(cs);
    send4(0x46, param, value >> 8, value);
}

static void op_EX_PV_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 param = cs_pop(cs);
    u16 value = cs_pop(cs);
    send4(0x47, param, value >> 8, value);
}

static void op_EX_MIN_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 param = cs_pop(cs);
    send2(0x49, param);

    data[0] = data[1] = 0;
    tele_ii_rx(DISTING_EX_1 + unit, data, 2);
    u16 value = (data[0] << 8) + data[1];
    cs_push(cs, (s16)value);
}

static void op_EX_MAX_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 param = cs_pop(cs);
    send2(0x4A, param);

    data[0] = data[1] = 0;
    tele_ii_rx(DISTING_EX_1 + unit, data, 2);
    u16 value = (data[0] << 8) + data[1];
    cs_push(cs, (s16)value);
}

static void op_EX_REC_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    send2(0x4B, cs_pop(cs) ? 1 : 0);
}

static void op_EX_PLAY_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    send2(0x4C, cs_pop(cs) ? 1 : 0);
}

static void op_EX_AL_P_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 pitch = cs_pop(cs);
    send3(0x4D, pitch >> 8, pitch);
}

static void op_EX_AL_CLK_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    send1(0x4E);
}

static void op_EX_M_CH_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, midi_channel + 1);
}

static void op_EX_M_CH_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 ch = cs_pop(cs) - 1;
    if (ch < 0 || ch > 15) return;
    midi_channel = ch;
}

static void op_EX_M_N_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 note = cs_pop(cs);
    u16 velocity = cs_pop(cs);
    if (note > 127) return;
    if (velocity > 127) velocity = 127;
    send4(0x4F, 0x90 + midi_channel, note, velocity);
}

static void op_EX_M_NO_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 note = cs_pop(cs);
    if (note > 127) return;
    send4(0x4F, 0x80 + midi_channel, note, 0);
}

static void op_EX_M_PB_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 bend = cs_pop(cs);
    send4(0x4F, 0xE0 + midi_channel, bend, bend >> 8);
}

static void op_EX_M_CC_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 controller = cs_pop(cs);
    u16 value = cs_pop(cs);
    if (controller > 127) return;
    if (value > 127) value = 127;
    send4(0x4F, 0xB0 + midi_channel, controller, value);
}

static void op_EX_M_PRG_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 program = cs_pop(cs);
    if (program > 127) return;
    send3(0x4F, 0xC0 + midi_channel, program);
}

static void op_EX_M_CLK_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    send3(0x4F, 0xF8 + midi_channel, 0xF8);
}

static void op_EX_M_START_get(const void *NOTUSED(data), scene_state_t *ss,
                              exec_state_t *NOTUSED(es), command_state_t *cs) {
    send3(0x4F, 0xFA + midi_channel, 0xFA);
}

static void op_EX_M_STOP_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    send3(0x4F, 0xFC + midi_channel, 0xFC);
}

static void op_EX_M_CONT_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    send3(0x4F, 0xFB + midi_channel, 0xFB);
}

static void op_EX_SB_CH_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, sb_channel + 1);
}

static void op_EX_SB_CH_set(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 ch = cs_pop(cs) - 1;
    if (ch < 0 || ch > 15) return;
    sb_channel = ch;
}

static void op_EX_SB_N_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 note = cs_pop(cs);
    u16 velocity = cs_pop(cs);
    if (note > 127) return;
    if (velocity > 127) velocity = 127;
    send4(0x50, 0x90 + sb_channel, note, velocity);
}

static void op_EX_SB_NO_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 note = cs_pop(cs);
    if (note > 127) return;
    send4(0x50, 0x80 + sb_channel, note, 0);
}

static void op_EX_SB_PB_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 bend = cs_pop(cs);
    send4(0x50, 0xE0 + sb_channel, bend, bend >> 8);
}

static void op_EX_SB_CC_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 controller = cs_pop(cs);
    u16 value = cs_pop(cs);
    if (controller > 127) return;
    if (value > 127) value = 127;
    send4(0x50, 0xB0 + sb_channel, controller, value);
}

static void op_EX_SB_PRG_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 program = cs_pop(cs);
    if (program > 127) return;
    send3(0x50, 0xC0 + sb_channel, program);
}

static void op_EX_SB_CLK_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    send3(0x50, 0xF8 + sb_channel, 0xF8);
}

static void op_EX_SB_START_get(const void *NOTUSED(data), scene_state_t *ss,
                               exec_state_t *NOTUSED(es), command_state_t *cs) {
    send3(0x50, 0xFA + sb_channel, 0xFA);
}

static void op_EX_SB_STOP_get(const void *NOTUSED(data), scene_state_t *ss,
                              exec_state_t *NOTUSED(es), command_state_t *cs) {
    send3(0x50, 0xFC + sb_channel, 0xFC);
}

static void op_EX_SB_CONT_get(const void *NOTUSED(data), scene_state_t *ss,
                              exec_state_t *NOTUSED(es), command_state_t *cs) {
    send3(0x50, 0xFB + sb_channel, 0xFB);
}

static void op_EX_VOX_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 voice = cs_pop(cs) - 1;
    s16 pitch = cs_pop(cs);
    u16 velocity = cs_pop(cs);
    if (voice < 0) return;

    send4(0x51, voice, (u16)pitch >> 8, pitch);
    send4(0x52, voice, velocity >> 8, velocity);
}

static void op_EX_VOX_P_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 voice = cs_pop(cs) - 1;
    s16 pitch = cs_pop(cs);
    if (voice < 0) return;

    send4(0x51, voice, pitch >> 8, pitch);
}

static void op_EX_VOX_O_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 voice = cs_pop(cs) - 1;
    if (voice < -1) return;

    if (voice == -1)
        send1(0x57);
    else
        send2(0x53, voice);
}

static u8 calculate_note(s16 pitch) {
    s32 note = (((s32)pitch * 120) / 16384) + 48;
    if (note < 0) note = 0;
    else if (note > 127) note = 127;
    return (u8)note;
}

static void op_EX_NOTE_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 pitch = cs_pop(cs);
    u16 velocity = cs_pop(cs);
    u8 note = calculate_note(pitch);

    send2(0x56, note);
    send4(0x54, note, (u16)pitch >> 8, pitch);
    send4(0x55, note, velocity >> 8, velocity);
}

static void op_EX_NOTE_O_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 pitch = cs_pop(cs);
    u8 note = calculate_note(pitch);

    send2(0x56, note);
}

static void op_EX_ALLOFF_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    send1(0x57);
}

static void op_EX_T_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 voice = cs_pop(cs) - 1;
    if (voice < 0) return;

    u16 velocity = 8192;
    send4(0x52, voice, velocity >> 8, velocity);
}

static void op_EX_TV_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 voice = cs_pop(cs) - 1;
    u16 velocity = cs_pop(cs);
    if (voice < 0) return;

    send4(0x52, voice, velocity >> 8, velocity);
}
