#include "ops/i2c2midi.h"

#include "helpers.h"
#include "teletype.h"
#include "teletype_io.h"

// clang-format off

static void op_I2M_PANIC_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CH_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CH_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_TIME_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_TIME_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_TIME_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_TIME_POUND_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_SHIFT_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_SHIFT_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_SHIFT_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_SHIFT_POUND_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_MIN_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_MIN_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_MAX_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_MAX_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_REP_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_REP_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_REP_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_REP_POUND_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_RAT_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_RAT_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_RAT_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_RAT_POUND_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NOTE_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NOTE_O_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NT_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_N_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NO_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NT_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_ADD_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_RM_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_CLR_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_L_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_L_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_SC_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_SC_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_B_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_INS_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_DEL_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_SET_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_INV_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_STR_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_ROT_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_REV_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_TRP_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_DIS_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_REF_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_DIR_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_VCUR_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_TCUR_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_QN_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_C_QV_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CC_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CC_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CCV_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CCV_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CC_OFF_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CC_OFF_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CC_OFF_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CC_OFF_POUND_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CC_SLEW_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CC_SLEW_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CC_SLEW_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CC_SLEW_POUND_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CC_SET_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CC_SET_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NRPN_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NRPN_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NRPN_OFF_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NRPN_OFF_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NRPN_OFF_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NRPN_OFF_POUND_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NRPN_SLEW_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NRPN_SLEW_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NRPN_SLEW_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NRPN_SLEW_POUND_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NRPN_SET_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_NRPN_SET_POUND_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_PRG_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_PB_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_AT_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CLK_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_START_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_STOP_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_CONT_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_Q_CH_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_Q_CH_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_Q_LATCH_set(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_Q_NOTE_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_Q_VEL_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_Q_CC_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_Q_LCH_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_Q_LN_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_Q_LV_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_Q_LO_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_Q_LC_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_Q_LCC_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_R_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_L_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_START_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_END_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_DIR_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_SPE_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_FB_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_NSHIFT_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_VSHIFT_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_TSHIFT_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_NOFF_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_VOFF_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_TOFF_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_CLR_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_B_MODE_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_MUTE_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);
static void op_I2M_SOLO_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);

static void op_I2M_TEST_get(const void *data, scene_state_t *ss, exec_state_t *es, command_state_t *cs);

const tele_op_t op_I2M_PANIC           = MAKE_GET_OP(I2M.PANIC, op_I2M_PANIC_get, 0, false);
const tele_op_t op_I2M_CH              = MAKE_GET_SET_OP(I2M.CH, op_I2M_CH_get, op_I2M_CH_set, 0, true);
const tele_op_t op_I2M_POUND           = MAKE_ALIAS_OP(I2M.#, op_I2M_CH_get, op_I2M_CH_set, 0, true);
const tele_op_t op_I2M_TIME            = MAKE_GET_SET_OP(I2M.TIME, op_I2M_TIME_get, op_I2M_TIME_set, 0, true);
const tele_op_t op_I2M_T               = MAKE_ALIAS_OP(I2M.T, op_I2M_TIME_get, op_I2M_TIME_set, 0, true);
const tele_op_t op_I2M_T_POUND         = MAKE_GET_SET_OP(I2M.T#, op_I2M_TIME_POUND_get, op_I2M_TIME_POUND_set, 1, true);
const tele_op_t op_I2M_SHIFT           = MAKE_GET_SET_OP(I2M.SHIFT, op_I2M_SHIFT_get, op_I2M_SHIFT_set, 0, true);
const tele_op_t op_I2M_S               = MAKE_ALIAS_OP(I2M.S, op_I2M_SHIFT_get, op_I2M_SHIFT_set, 0, true);
const tele_op_t op_I2M_S_POUND         = MAKE_GET_SET_OP(I2M.S#, op_I2M_SHIFT_POUND_get, op_I2M_SHIFT_POUND_set, 1, true);
const tele_op_t op_I2M_MIN             = MAKE_GET_OP(I2M.MIN, op_I2M_MIN_get, 2, false);
const tele_op_t op_I2M_MIN_POUND       = MAKE_GET_OP(I2M.MIN#, op_I2M_MIN_POUND_get, 3, false);
const tele_op_t op_I2M_MAX             = MAKE_GET_OP(I2M.MAX, op_I2M_MAX_get, 2, false);
const tele_op_t op_I2M_MAX_POUND       = MAKE_GET_OP(I2M.MAX#, op_I2M_MAX_POUND_get, 3, false);
const tele_op_t op_I2M_REP             = MAKE_GET_SET_OP(I2M.REP, op_I2M_REP_get, op_I2M_REP_set, 0, true);
const tele_op_t op_I2M_REP_POUND       = MAKE_GET_SET_OP(I2M.REP#, op_I2M_REP_POUND_get, op_I2M_REP_POUND_set, 1, true);
const tele_op_t op_I2M_RAT             = MAKE_GET_SET_OP(I2M.RAT, op_I2M_RAT_get, op_I2M_RAT_set, 0, true);
const tele_op_t op_I2M_RAT_POUND       = MAKE_GET_SET_OP(I2M.RAT#, op_I2M_RAT_POUND_get, op_I2M_RAT_POUND_set, 1, true);
const tele_op_t op_I2M_NOTE            = MAKE_GET_OP(I2M.NOTE, op_I2M_NOTE_get, 2, false);
const tele_op_t op_I2M_N               = MAKE_ALIAS_OP(I2M.N, op_I2M_NOTE_get, NULL, 2, false);
const tele_op_t op_I2M_NOTE_O          = MAKE_GET_OP(I2M.NOTE.O, op_I2M_NOTE_O_get, 1, false);
const tele_op_t op_I2M_NO              = MAKE_ALIAS_OP(I2M.NO, op_I2M_NOTE_O_get, NULL, 1, false);
const tele_op_t op_I2M_NT              = MAKE_GET_OP(I2M.NT, op_I2M_NT_get, 3, false);
const tele_op_t op_I2M_N_POUND         = MAKE_GET_OP(I2M.N#, op_I2M_N_POUND_get, 3, false);
const tele_op_t op_I2M_NO_POUND        = MAKE_GET_OP(I2M.NO#, op_I2M_NO_POUND_get, 2, false);
const tele_op_t op_I2M_NT_POUND        = MAKE_GET_OP(I2M.NT#, op_I2M_NT_POUND_get, 4, false);
const tele_op_t op_I2M_C               = MAKE_GET_OP(I2M.C, op_I2M_C_get, 3, false);
const tele_op_t op_I2M_CHORD           = MAKE_ALIAS_OP(I2M.CHORD, op_I2M_C_get, NULL, 3, false);
const tele_op_t op_I2M_C_POUND         = MAKE_GET_OP(I2M.C#, op_I2M_C_POUND_get, 4, false);
const tele_op_t op_I2M_C_ADD           = MAKE_GET_OP(I2M.C.ADD, op_I2M_C_ADD_get, 2, false);
const tele_op_t op_I2M_C_PLUS          = MAKE_ALIAS_OP(I2M.C+, op_I2M_C_ADD_get, NULL, 2, false);
const tele_op_t op_I2M_C_RM            = MAKE_GET_OP(I2M.C.RM, op_I2M_C_RM_get, 2, false);
const tele_op_t op_I2M_C_MINUS         = MAKE_ALIAS_OP(I2M.C-, op_I2M_C_RM_get, NULL, 2, false);
const tele_op_t op_I2M_C_CLR           = MAKE_GET_OP(I2M.C.CLR, op_I2M_C_CLR_get, 1, false);
const tele_op_t op_I2M_C_L             = MAKE_GET_SET_OP(I2M.C.L, op_I2M_C_L_get, op_I2M_C_L_set, 1, true);
const tele_op_t op_I2M_C_B             = MAKE_GET_OP(I2M.C.B, op_I2M_C_B_get, 2, false);
const tele_op_t op_I2M_C_SC            = MAKE_GET_SET_OP(I2M.C.SC, op_I2M_C_SC_get, op_I2M_C_SC_set, 1, true);
const tele_op_t op_I2M_C_INS           = MAKE_GET_OP(I2M.C.INS, op_I2M_C_INS_get, 3, false);
const tele_op_t op_I2M_C_DEL           = MAKE_GET_OP(I2M.C.DEL, op_I2M_C_DEL_get, 2, false);
const tele_op_t op_I2M_C_SET           = MAKE_GET_OP(I2M.C.SET, op_I2M_C_SET_get, 3, false);
const tele_op_t op_I2M_C_INV           = MAKE_GET_OP(I2M.C.INV, op_I2M_C_INV_get, 2, false);
const tele_op_t op_I2M_C_STR           = MAKE_GET_OP(I2M.C.STR, op_I2M_C_STR_get, 2, false);
const tele_op_t op_I2M_C_ROT           = MAKE_GET_OP(I2M.C.ROT, op_I2M_C_ROT_get, 2, false);
const tele_op_t op_I2M_C_REV           = MAKE_GET_OP(I2M.C.REV, op_I2M_C_REV_get, 2, false);
const tele_op_t op_I2M_C_TRP           = MAKE_GET_OP(I2M.C.TRP, op_I2M_C_TRP_get, 2, false);
const tele_op_t op_I2M_C_DIS           = MAKE_GET_OP(I2M.C.DIS, op_I2M_C_DIS_get, 3, false);
const tele_op_t op_I2M_C_REF           = MAKE_GET_OP(I2M.C.REF, op_I2M_C_REF_get, 3, false);
const tele_op_t op_I2M_C_DIR           = MAKE_GET_OP(I2M.C.DIR, op_I2M_C_DIR_get, 2, false);
const tele_op_t op_I2M_C_VCUR          = MAKE_GET_OP(I2M.C.VCUR, op_I2M_C_VCUR_get, 4, false);
const tele_op_t op_I2M_C_VTILDE        = MAKE_ALIAS_OP(I2M.C.V~, op_I2M_C_VCUR_get, NULL, 4, false);
const tele_op_t op_I2M_C_TCUR          = MAKE_GET_OP(I2M.C.TCUR, op_I2M_C_TCUR_get, 4, false);
const tele_op_t op_I2M_C_TTILDE        = MAKE_ALIAS_OP(I2M.C.T~, op_I2M_C_TCUR_get, NULL, 4, false);
const tele_op_t op_I2M_C_QN            = MAKE_GET_OP(I2M.C.QN, op_I2M_C_QN_get, 3, true);
const tele_op_t op_I2M_C_QV            = MAKE_GET_OP(I2M.C.QV, op_I2M_C_QV_get, 3, true);
const tele_op_t op_I2M_CC              = MAKE_GET_OP(I2M.CC, op_I2M_CC_get, 2, false);
const tele_op_t op_I2M_CC_POUND        = MAKE_GET_OP(I2M.CC#, op_I2M_CC_POUND_get, 3, false);
const tele_op_t op_I2M_CCV             = MAKE_GET_OP(I2M.CCV, op_I2M_CCV_get, 2, false);
const tele_op_t op_I2M_CCV_POUND       = MAKE_GET_OP(I2M.CCV#, op_I2M_CCV_POUND_get, 3, false);
const tele_op_t op_I2M_CC_OFF          = MAKE_GET_SET_OP(I2M.CC.OFF, op_I2M_CC_OFF_get, op_I2M_CC_OFF_set, 1, true);
const tele_op_t op_I2M_CC_OFF_POUND    = MAKE_GET_SET_OP(I2M.CC.OFF#, op_I2M_CC_OFF_POUND_get, op_I2M_CC_OFF_POUND_set, 2, true);
const tele_op_t op_I2M_CC_SLEW         = MAKE_GET_SET_OP(I2M.CC.SLEW, op_I2M_CC_SLEW_get, op_I2M_CC_SLEW_set, 1, true);
const tele_op_t op_I2M_CC_SLEW_POUND   = MAKE_GET_SET_OP(I2M.CC.SLEW#, op_I2M_CC_SLEW_POUND_get, op_I2M_CC_SLEW_POUND_set, 2, true);
const tele_op_t op_I2M_CC_SET          = MAKE_GET_OP(I2M.CC.SET, op_I2M_CC_SET_get, 2, false);
const tele_op_t op_I2M_CC_SET_POUND    = MAKE_GET_OP(I2M.CC.SET#, op_I2M_CC_SET_POUND_get, 3, false);
const tele_op_t op_I2M_NRPN            = MAKE_GET_OP(I2M.NRPN, op_I2M_NRPN_get, 3, false);
const tele_op_t op_I2M_NRPN_POUND      = MAKE_GET_OP(I2M.NRPN#, op_I2M_NRPN_POUND_get, 4, false);
const tele_op_t op_I2M_NRPN_OFF        = MAKE_GET_SET_OP(I2M.NRPN.OFF, op_I2M_NRPN_OFF_get, op_I2M_NRPN_OFF_set, 2, true);
const tele_op_t op_I2M_NRPN_OFF_POUND  = MAKE_GET_SET_OP(I2M.NRPN.OFF#, op_I2M_NRPN_OFF_POUND_get, op_I2M_NRPN_OFF_POUND_set, 3, true);
const tele_op_t op_I2M_NRPN_SLEW       = MAKE_GET_SET_OP(I2M.NRPN.SLEW, op_I2M_NRPN_SLEW_get, op_I2M_NRPN_SLEW_set, 2, true);
const tele_op_t op_I2M_NRPN_SLEW_POUND = MAKE_GET_SET_OP(I2M.NRPN.SLEW#, op_I2M_NRPN_SLEW_POUND_get, op_I2M_NRPN_SLEW_POUND_set, 3, true);
const tele_op_t op_I2M_NRPN_SET        = MAKE_GET_OP(I2M.NRPN.SET, op_I2M_NRPN_SET_get, 3, false);
const tele_op_t op_I2M_NRPN_SET_POUND  = MAKE_GET_OP(I2M.NRPN.SET#, op_I2M_NRPN_SET_POUND_get, 4, false);
const tele_op_t op_I2M_PRG             = MAKE_GET_OP(I2M.PRG, op_I2M_PRG_get, 1, false);
const tele_op_t op_I2M_PB              = MAKE_GET_OP(I2M.PB, op_I2M_PB_get, 1, false);
const tele_op_t op_I2M_AT              = MAKE_GET_OP(I2M.AT, op_I2M_AT_get, 1, false);
const tele_op_t op_I2M_CLK             = MAKE_GET_OP(I2M.CLK, op_I2M_CLK_get, 0, false);
const tele_op_t op_I2M_START           = MAKE_GET_OP(I2M.START, op_I2M_START_get, 0, false);
const tele_op_t op_I2M_STOP            = MAKE_GET_OP(I2M.STOP, op_I2M_STOP_get, 0, false);
const tele_op_t op_I2M_CONT            = MAKE_GET_OP(I2M.CONT, op_I2M_CONT_get, 0, false);
const tele_op_t op_I2M_Q_CH            = MAKE_GET_SET_OP(I2M.Q.CH, op_I2M_Q_CH_get, op_I2M_Q_CH_set, 0, true);
const tele_op_t op_I2M_Q_POUND         = MAKE_ALIAS_OP(I2M.Q.#, op_I2M_Q_CH_get, op_I2M_Q_CH_set, 0, true);
const tele_op_t op_I2M_Q_LATCH         = MAKE_GET_OP(I2M.Q.LATCH, op_I2M_Q_LATCH_set, 1, false);
const tele_op_t op_I2M_Q_NOTE          = MAKE_GET_OP(I2M.Q.NOTE, op_I2M_Q_NOTE_get, 1, true);
const tele_op_t op_I2M_Q_N             = MAKE_ALIAS_OP(I2M.Q.N, op_I2M_Q_NOTE_get, NULL, 1, true);
const tele_op_t op_I2M_Q_VEL           = MAKE_GET_OP(I2M.Q.VEL, op_I2M_Q_VEL_get, 1, true);
const tele_op_t op_I2M_Q_V             = MAKE_ALIAS_OP(I2M.Q.V, op_I2M_Q_VEL_get, NULL, 1, true);
const tele_op_t op_I2M_Q_CC            = MAKE_GET_OP(I2M.Q.CC, op_I2M_Q_CC_get, 1, true);
const tele_op_t op_I2M_Q_LCH           = MAKE_GET_OP(I2M.Q.LCH, op_I2M_Q_LCH_get, 0, true);
const tele_op_t op_I2M_Q_LN            = MAKE_GET_OP(I2M.Q.LN, op_I2M_Q_LN_get, 0, true);
const tele_op_t op_I2M_Q_LV            = MAKE_GET_OP(I2M.Q.LV, op_I2M_Q_LV_get, 0, true);
const tele_op_t op_I2M_Q_LO            = MAKE_GET_OP(I2M.Q.LO, op_I2M_Q_LO_get, 0, true);
const tele_op_t op_I2M_Q_LC            = MAKE_GET_OP(I2M.Q.LC, op_I2M_Q_LC_get, 0, true);
const tele_op_t op_I2M_Q_LCC           = MAKE_GET_OP(I2M.Q.LCC, op_I2M_Q_LCC_get, 0, true);
const tele_op_t op_I2M_B_R             = MAKE_GET_OP(I2M.B.R, op_I2M_B_R_get, 1, false);
const tele_op_t op_I2M_B_L             = MAKE_GET_OP(I2M.B.L, op_I2M_B_L_get, 1, false);
const tele_op_t op_I2M_B_START         = MAKE_GET_OP(I2M.B.START, op_I2M_B_START_get, 1, false);
const tele_op_t op_I2M_B_END           = MAKE_GET_OP(I2M.B.END, op_I2M_B_END_get, 1, false);
const tele_op_t op_I2M_B_DIR           = MAKE_GET_OP(I2M.B.DIR, op_I2M_B_DIR_get, 1, false);
const tele_op_t op_I2M_B_SPE           = MAKE_GET_OP(I2M.B.SPE, op_I2M_B_SPE_get, 1, false);
const tele_op_t op_I2M_B_FB            = MAKE_GET_OP(I2M.B.FB, op_I2M_B_FB_get, 1, false);
const tele_op_t op_I2M_B_NSHIFT        = MAKE_GET_OP(I2M.B.NSHIFT, op_I2M_B_NSHIFT_get, 1, false);
const tele_op_t op_I2M_B_VSHIFT        = MAKE_GET_OP(I2M.B.VSHIFT, op_I2M_B_VSHIFT_get, 1, false);
const tele_op_t op_I2M_B_TSHIFT        = MAKE_GET_OP(I2M.B.TSHIFT, op_I2M_B_TSHIFT_get, 1, false);
const tele_op_t op_I2M_B_NOFF          = MAKE_GET_OP(I2M.B.NOFF, op_I2M_B_NOFF_get, 1, false);
const tele_op_t op_I2M_B_VOFF          = MAKE_GET_OP(I2M.B.VOFF, op_I2M_B_VOFF_get, 1, false);
const tele_op_t op_I2M_B_TOFF          = MAKE_GET_OP(I2M.B.TOFF, op_I2M_B_TOFF_get, 1, false);
const tele_op_t op_I2M_B_CLR           = MAKE_GET_OP(I2M.B.CLR, op_I2M_B_CLR_get, 0, false);
const tele_op_t op_I2M_B_MODE          = MAKE_GET_OP(I2M.B.MODE, op_I2M_B_MODE_get, 1, false);
const tele_op_t op_I2M_MUTE            = MAKE_GET_OP(I2M.MUTE, op_I2M_MUTE_get, 2, false);
const tele_op_t op_I2M_SOLO            = MAKE_GET_OP(I2M.SOLO, op_I2M_SOLO_get, 2, false);

const tele_op_t op_I2M_TEST            = MAKE_GET_OP(I2M.TEST, op_I2M_TEST_get, 2, false);

// clang-format on

#define I2C2MIDI 0x3F
#define MAX_CHANNEL 32

static u8 midi_channel = 0, q_channel = 0;
static u8 d[7];

#define SEND_CMD(cmd) \
    d[0] = cmd;       \
    tele_ii_tx(I2C2MIDI, d, 1);

#define SEND_B1(cmd, b) \
    d[0] = cmd;         \
    d[1] = b;           \
    tele_ii_tx(I2C2MIDI, d, 2);

#define SEND_B2(cmd, b1, b2) \
    d[0] = cmd;              \
    d[1] = b1;               \
    d[2] = b2;               \
    tele_ii_tx(I2C2MIDI, d, 3);

#define SEND_B3(cmd, b1, b2, b3) \
    d[0] = cmd;                  \
    d[1] = b1;                   \
    d[2] = b2;                   \
    d[3] = b3;                   \
    tele_ii_tx(I2C2MIDI, d, 4);

#define SEND_B4(cmd, b1, b2, b3, b4) \
    d[0] = cmd;                      \
    d[1] = b1;                       \
    d[2] = b2;                       \
    d[3] = b3;                       \
    d[4] = b4;                       \
    tele_ii_tx(I2C2MIDI, d, 5);

#define SEND_B5(cmd, b1, b2, b3, b4, b5) \
    d[0] = cmd;                          \
    d[1] = b1;                           \
    d[2] = b2;                           \
    d[3] = b3;                           \
    d[4] = b4;                           \
    d[5] = b5;                           \
    tele_ii_tx(I2C2MIDI, d, 6);

#define SEND_B6(cmd, b1, b2, b3, b4, b5, b6) \
    d[0] = cmd;                              \
    d[1] = b1;                               \
    d[2] = b2;                               \
    d[3] = b3;                               \
    d[4] = b4;                               \
    d[5] = b5;                               \
    d[6] = b6;                               \
    tele_ii_tx(I2C2MIDI, d, 7);

#define RECEIVE_AND_PUSH_S8     \
    d[0] = 0;                   \
    tele_ii_rx(I2C2MIDI, d, 1); \
    cs_push(cs, (s8)d[0]);

#define RECEIVE_AND_PUSH_S16    \
    d[0] = d[1] = 0;            \
    tele_ii_rx(I2C2MIDI, d, 2); \
    cs_push(cs, (d[0] << 8) + d[1]);

#define RECEIVE_AND_PUSH_S16_7  \
    d[0] = d[1] = 0;            \
    tele_ii_rx(I2C2MIDI, d, 2); \
    cs_push(cs, (d[0] << 7) + d[1]);

#define RETURN_IF_OUT_OF_RANGE(value, min, max) \
    if ((value) < min || (value) > max) return;

#define CLAMP_TO_RANGE(value, min, max) \
    if (value < min)                    \
        value = min;                    \
    else if (value > max)               \
        value = max;


// implementation

static void op_I2M_PANIC_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    SEND_CMD(22);
}

static void op_I2M_CH_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    cs_push(cs, midi_channel + 1);
}

static void op_I2M_CH_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    s16 channel = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    midi_channel = channel - 1;
}

static void op_I2M_TIME_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    SEND_B1(1, midi_channel + 1);
    RECEIVE_AND_PUSH_S16;
}

static void op_I2M_TIME_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 time = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(time, 0, 32767);
    SEND_B3(2, midi_channel + 1, time >> 8, time & 0xff);
}

static void op_I2M_TIME_POUND_get(const void *data, scene_state_t *ss,
                                  exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    if (channel < 1 || channel > MAX_CHANNEL) {
        cs_push(cs, 0);
        return;
    }
    SEND_B1(1, channel);
    RECEIVE_AND_PUSH_S16;
}

static void op_I2M_TIME_POUND_set(const void *data, scene_state_t *ss,
                                  exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 time = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 0, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(time, 0, 32767);
    SEND_B3(2, channel, time >> 8, time & 0xff);
}

static void op_I2M_SHIFT_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    SEND_B1(3, midi_channel + 1);
    RECEIVE_AND_PUSH_S8;
}

static void op_I2M_SHIFT_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 shift = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(shift, -127, 127);
    SEND_B2(4, midi_channel + 1, shift);
}

static void op_I2M_SHIFT_POUND_get(const void *data, scene_state_t *ss,
                                   exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    if (channel < 1 || channel > MAX_CHANNEL) {
        cs_push(cs, 0);
        return;
    }
    SEND_B1(3, channel);
    RECEIVE_AND_PUSH_S8;
}

static void op_I2M_SHIFT_POUND_set(const void *data, scene_state_t *ss,
                                   exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 shift = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 0, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(shift, -127, 127);
    SEND_B2(4, channel, shift);
}

static void op_I2M_MIN_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    s16 note = cs_pop(cs);
    s16 mode = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(note, 0, 127);
    SEND_B3(10, midi_channel + 1, note, mode);
}

static void op_I2M_MIN_POUND_get(const void *data, scene_state_t *ss,
                                 exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 note = cs_pop(cs);
    s16 mode = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 0, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(note, 0, 127);
    SEND_B3(10, channel, note, mode);
}

static void op_I2M_MAX_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    s16 note = cs_pop(cs);
    s16 mode = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(note, 0, 127);
    SEND_B3(12, midi_channel + 1, note, mode);
}

static void op_I2M_MAX_POUND_get(const void *data, scene_state_t *ss,
                                 exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 note = cs_pop(cs);
    s16 mode = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 0, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(note, 0, 127);
    SEND_B3(12, channel, note, mode);
}

static void op_I2M_REP_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    SEND_B1(5, midi_channel + 1);
    RECEIVE_AND_PUSH_S16;
}

static void op_I2M_REP_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    s16 rep = cs_pop(cs);
    SEND_B3(6, midi_channel + 1, rep >> 8, rep & 0xff);
}

static void op_I2M_REP_POUND_get(const void *data, scene_state_t *ss,
                                 exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    if (channel < 1 || channel > MAX_CHANNEL) {
        cs_push(cs, 0);
        return;
    }
    SEND_B1(5, channel);
    RECEIVE_AND_PUSH_S16;
}

static void op_I2M_REP_POUND_set(const void *data, scene_state_t *ss,
                                 exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 rep = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 0, MAX_CHANNEL);
    SEND_B3(6, channel, rep >> 8, rep & 0xff);
}

static void op_I2M_RAT_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    SEND_B1(7, midi_channel + 1);
    RECEIVE_AND_PUSH_S16;
}

static void op_I2M_RAT_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    s16 rat = cs_pop(cs);
    SEND_B3(8, midi_channel + 1, rat >> 8, rat & 0xff);
}

static void op_I2M_RAT_POUND_get(const void *data, scene_state_t *ss,
                                 exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    if (channel < 1 || channel > MAX_CHANNEL) {
        cs_push(cs, 0);
        return;
    }
    SEND_B1(7, channel);
    RECEIVE_AND_PUSH_S16;
}

static void op_I2M_RAT_POUND_set(const void *data, scene_state_t *ss,
                                 exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 rat = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 0, MAX_CHANNEL);
    SEND_B3(8, channel, rat >> 8, rat & 0xff);
}

static void op_I2M_NOTE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 note = cs_pop(cs);
    s16 velocity = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(note, 0, 127);
    CLAMP_TO_RANGE(velocity, 0, 127);
    SEND_B3(20, midi_channel, note, velocity);
}

static void op_I2M_NOTE_O_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    s16 note = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(note, 0, 127);
    SEND_B2(21, midi_channel, note);
}

static void op_I2M_NT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    s16 note = cs_pop(cs);
    s16 velocity = cs_pop(cs);
    s16 duration = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(note, 0, 127);
    RETURN_IF_OUT_OF_RANGE(duration, 0, 32767);
    CLAMP_TO_RANGE(velocity, 0, 127);
    SEND_B5(23, midi_channel, note, velocity, duration >> 8, duration & 0xff);
}

static void op_I2M_N_POUND_get(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 note = cs_pop(cs);
    s16 velocity = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(note, 0, 127);
    CLAMP_TO_RANGE(velocity, 0, 127);
    SEND_B3(20, channel - 1, note, velocity);
}

static void op_I2M_NO_POUND_get(const void *data, scene_state_t *ss,
                                exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 note = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(note, 0, 127);
    SEND_B2(21, channel - 1, note);
}

static void op_I2M_NT_POUND_get(const void *data, scene_state_t *ss,
                                exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 note = cs_pop(cs);
    s16 velocity = cs_pop(cs);
    s16 duration = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(note, 0, 127);
    RETURN_IF_OUT_OF_RANGE(duration, 0, 32767);
    CLAMP_TO_RANGE(velocity, 0, 127);
    SEND_B5(23, channel - 1, note, velocity, duration >> 8, duration & 0xff);
}

static void op_I2M_C_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 note = cs_pop(cs);
    s16 velocity = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(note, 0, 127);
    CLAMP_TO_RANGE(velocity, 0, 127);
    SEND_B4(30, midi_channel, chord, note, velocity);
}

static void op_I2M_C_ADD_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 note = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(note, -127, 127);
    SEND_B2(31, chord, note);
}

static void op_I2M_C_POUND_get(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 chord = cs_pop(cs);
    s16 note = cs_pop(cs);
    s16 velocity = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(note, 0, 127);
    CLAMP_TO_RANGE(velocity, 0, 127);
    SEND_B4(30, channel - 1, chord, note, velocity);
}

static void op_I2M_C_RM_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 note = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(note, -127, 127);
    SEND_B2(32, chord, note);
}

static void op_I2M_C_CLR_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    SEND_B1(33, chord);
}

static void op_I2M_C_L_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    SEND_B1(34, chord);
    RECEIVE_AND_PUSH_S8;
}

static void op_I2M_C_L_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 length = cs_pop(cs);
    SEND_B2(35, chord, length);
}

static void op_I2M_C_SC_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    SEND_B1(157, chord);
    RECEIVE_AND_PUSH_S8;
}

static void op_I2M_C_SC_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 scaleChord = cs_pop(cs);
    SEND_B2(158, chord, scaleChord);
}

static void op_I2M_C_B_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 binary = cs_pop(cs);
    SEND_B3(159, chord, binary >> 8, binary & 0xff);
}

static void op_I2M_C_INS_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 index = cs_pop(cs);
    s16 note = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(note, -127, 127);
    SEND_B3(152, chord, index, note);
}

static void op_I2M_C_DEL_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 index = cs_pop(cs);
    SEND_B2(153, chord, index);
}

static void op_I2M_C_SET_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 index = cs_pop(cs);
    s16 note = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(note, 0, 127);
    SEND_B3(154, chord, index, note);
}

static void op_I2M_C_INV_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 inversion = cs_pop(cs);
    SEND_B2(37, chord, inversion);
}

static void op_I2M_C_STR_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 strum = cs_pop(cs);
    CLAMP_TO_RANGE(strum, 0, 32767);
    SEND_B3(151, chord, strum >> 8, strum & 0xff);
}

static void op_I2M_C_REV_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 rev = cs_pop(cs);
    SEND_B2(39, chord, rev);
}

static void op_I2M_C_ROT_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 rot = cs_pop(cs);
    SEND_B2(156, chord, rot);
}

static void op_I2M_C_TRP_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 transpose = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(transpose, -127, 127);
    SEND_B2(160, chord, transpose);
}

static void op_I2M_C_DIS_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 distort = cs_pop(cs);
    s16 anchor = cs_pop(cs);
    SEND_B3(161, chord, distort, anchor);
}

static void op_I2M_C_REF_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 value = cs_pop(cs);
    s16 point = cs_pop(cs);
    SEND_B3(162, chord, value, point);
}

static void op_I2M_C_DIR_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 dir = cs_pop(cs);
    SEND_B2(165, chord, dir);
}

static void op_I2M_C_VCUR_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 curve = cs_pop(cs);
    s16 start = cs_pop(cs);
    s16 end = cs_pop(cs);
    SEND_B6(163, chord, curve, start >> 8, start & 0xff, end >> 8, end & 0xff);
}

static void op_I2M_C_TCUR_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 curve = cs_pop(cs);
    s16 start = cs_pop(cs);
    s16 end = cs_pop(cs);
    SEND_B6(164, chord, curve, start >> 8, start & 0xff, end >> 8, end & 0xff);
}

static void op_I2M_C_QN_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 note = cs_pop(cs);
    s16 index = cs_pop(cs);
    if (note < 0 || note > 127) {
        cs_push(cs, 0);
        return;
    }

    SEND_B3(166, chord, note, index);
    d[0] = d[1] = 0;
    tele_ii_rx(I2C2MIDI, d, 2);
    s16 qn = (d[0] << 8) | d[1];
    cs_push(cs, qn);
}

static void op_I2M_C_QV_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 chord = cs_pop(cs);
    s16 velocity = cs_pop(cs);
    s16 index = cs_pop(cs);
    if (velocity < 0 || velocity > 127) {
        cs_push(cs, 0);
        return;
    }

    SEND_B3(167, chord, velocity, index);
    d[0] = d[1] = 0;
    tele_ii_rx(I2C2MIDI, d, 2);
    s16 qv = (d[0] << 8) | d[1];
    cs_push(cs, qv);
}

static void op_I2M_CC_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    s16 controller = cs_pop(cs);
    s16 cc = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(controller, 0, 127);
    CLAMP_TO_RANGE(cc, 0, 127);
    cc *= 129;
    SEND_B4(40, midi_channel, controller, cc >> 7, cc & 0x7f);
}

static void op_I2M_CC_POUND_get(const void *data, scene_state_t *ss,
                                exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 controller = cs_pop(cs);
    s16 cc = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(controller, 0, 127);
    CLAMP_TO_RANGE(cc, 0, 127);
    cc *= 129;
    SEND_B4(40, channel - 1, controller, cc >> 7, cc & 0x7f);
}

static void op_I2M_CCV_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    s16 controller = cs_pop(cs);
    s16 cc = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(controller, 0, 127);
    CLAMP_TO_RANGE(cc, 0, 16383);
    SEND_B4(40, midi_channel, controller, cc >> 7, cc & 0x7f);
}

static void op_I2M_CCV_POUND_get(const void *data, scene_state_t *ss,
                                 exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 controller = cs_pop(cs);
    s16 cc = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(controller, 0, 127);
    CLAMP_TO_RANGE(cc, 0, 16383);
    SEND_B4(40, channel - 1, controller, cc >> 7, cc & 0x7f);
}

static void op_I2M_CC_OFF_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    s16 controller = cs_pop(cs);
    if (controller < 0 || controller > 127) { cs_push(cs, 0); }
    else {
        SEND_B2(41, midi_channel, controller);
        d[0] = d[1] = 0;
        tele_ii_rx(I2C2MIDI, d, 2);
        s16 offset = (d[0] << 8) | d[1];
        offset = (offset << 1) / 129;
        offset = (offset >> 1) + (offset & 1);
        cs_push(cs, offset);
    }
}

static void op_I2M_CC_OFF_set(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    s16 controller = cs_pop(cs);
    s16 offset = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(controller, 0, 127);
    CLAMP_TO_RANGE(offset, -127, 127);
    offset *= 129;
    SEND_B4(42, midi_channel, controller, offset >> 8, offset & 0xff);
}

static void op_I2M_CC_OFF_POUND_get(const void *data, scene_state_t *ss,
                                    exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 controller = cs_pop(cs);
    if (controller < 0 || controller > 127 || channel < 1 ||
        channel > MAX_CHANNEL) {
        cs_push(cs, 0);
    }
    else {
        SEND_B2(41, channel - 1, controller);
        d[0] = d[1] = 0;
        tele_ii_rx(I2C2MIDI, d, 2);
        s16 offset = (d[0] << 8) | d[1];
        offset = (offset << 1) / 129;
        offset = (offset >> 1) + (offset & 1);
        cs_push(cs, offset);
    }
}

static void op_I2M_CC_OFF_POUND_set(const void *data, scene_state_t *ss,
                                    exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 controller = cs_pop(cs);
    s16 offset = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(controller, 0, 127);
    CLAMP_TO_RANGE(offset, -127, 127);
    offset *= 129;
    SEND_B4(42, channel - 1, controller, offset >> 8, offset & 0xff);
}

static void op_I2M_CC_SLEW_get(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs) {
    s16 controller = cs_pop(cs);
    if (controller < 0 || controller > 127) { cs_push(cs, 0); }
    else {
        SEND_B2(43, midi_channel, controller);
        RECEIVE_AND_PUSH_S16;
    }
}

static void op_I2M_CC_SLEW_set(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs) {
    s16 controller = cs_pop(cs);
    s16 slew = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(controller, 0, 127);
    RETURN_IF_OUT_OF_RANGE(slew, 0, 32767);
    SEND_B4(44, midi_channel, controller, slew >> 8, slew & 0xff);
}

static void op_I2M_CC_SLEW_POUND_get(const void *data, scene_state_t *ss,
                                     exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 controller = cs_pop(cs);
    if (controller < 0 || controller > 127 || channel < 1 ||
        channel > MAX_CHANNEL) {
        cs_push(cs, 0);
    }
    else {
        SEND_B2(43, channel - 1, controller);
        RECEIVE_AND_PUSH_S16;
    }
}

static void op_I2M_CC_SLEW_POUND_set(const void *data, scene_state_t *ss,
                                     exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 controller = cs_pop(cs);
    s16 slew = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(controller, 0, 127);
    RETURN_IF_OUT_OF_RANGE(slew, 0, 32767);
    SEND_B4(44, channel - 1, controller, slew >> 8, slew & 0xff);
}

static void op_I2M_CC_SET_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    s16 controller = cs_pop(cs);
    s16 cc = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(controller, 0, 127);
    CLAMP_TO_RANGE(cc, 0, 127);
    cc *= 129;
    SEND_B4(45, midi_channel, controller, cc >> 7, cc & 0x7f);
}

static void op_I2M_CC_SET_POUND_get(const void *data, scene_state_t *ss,
                                    exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 controller = cs_pop(cs);
    s16 cc = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(controller, 0, 127);
    CLAMP_TO_RANGE(cc, 0, 127);
    cc *= 129;
    SEND_B4(45, channel - 1, controller, cc >> 7, cc & 0x7f);
}

static void op_I2M_NRPN_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 msb = cs_pop(cs);
    s16 lsb = cs_pop(cs);
    s16 value = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(msb, 0, 127);
    RETURN_IF_OUT_OF_RANGE(lsb, 0, 127);
    CLAMP_TO_RANGE(value, 0, 16384);
    SEND_B5(50, midi_channel, msb, lsb, value >> 7, value & 0x7f);
}

static void op_I2M_NRPN_POUND_get(const void *data, scene_state_t *ss,
                                  exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 msb = cs_pop(cs);
    s16 lsb = cs_pop(cs);
    s16 value = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(msb, 0, 127);
    RETURN_IF_OUT_OF_RANGE(lsb, 0, 127);
    CLAMP_TO_RANGE(value, 0, 16384);
    SEND_B5(50, channel - 1, msb, lsb, value >> 7, value & 0x7f);
}

static void op_I2M_NRPN_OFF_get(const void *data, scene_state_t *ss,
                                exec_state_t *es, command_state_t *cs) {
    s16 msb = cs_pop(cs);
    s16 lsb = cs_pop(cs);
    if (msb < 0 || msb > 127 || lsb < 0 || lsb > 127) { cs_push(cs, 0); }
    else {
        SEND_B3(51, midi_channel, msb, lsb);
        RECEIVE_AND_PUSH_S16;
    }
}

static void op_I2M_NRPN_OFF_set(const void *data, scene_state_t *ss,
                                exec_state_t *es, command_state_t *cs) {
    s16 msb = cs_pop(cs);
    s16 lsb = cs_pop(cs);
    s16 offset = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(msb, 0, 127);
    RETURN_IF_OUT_OF_RANGE(lsb, 0, 127);
    CLAMP_TO_RANGE(offset, -16384, 16384);
    SEND_B5(52, midi_channel, msb, lsb, offset >> 8, offset & 0xff);
}

static void op_I2M_NRPN_OFF_POUND_get(const void *data, scene_state_t *ss,
                                      exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 msb = cs_pop(cs);
    s16 lsb = cs_pop(cs);
    if (msb < 0 || msb > 127 || lsb < 0 || lsb > 127 || channel < 1 ||
        channel > MAX_CHANNEL) {
        cs_push(cs, 0);
    }
    else {
        SEND_B3(51, channel - 1, msb, lsb);
        RECEIVE_AND_PUSH_S16;
    }
}

static void op_I2M_NRPN_OFF_POUND_set(const void *data, scene_state_t *ss,
                                      exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 msb = cs_pop(cs);
    s16 lsb = cs_pop(cs);
    s16 offset = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(msb, 0, 127);
    RETURN_IF_OUT_OF_RANGE(lsb, 0, 127);
    CLAMP_TO_RANGE(offset, -16384, 16384);
    SEND_B5(52, channel - 1, msb, lsb, offset >> 8, offset & 0xff);
}

static void op_I2M_NRPN_SLEW_get(const void *data, scene_state_t *ss,
                                 exec_state_t *es, command_state_t *cs) {
    s16 msb = cs_pop(cs);
    s16 lsb = cs_pop(cs);
    if (msb < 0 || msb > 127 || lsb < 0 || lsb > 127) { cs_push(cs, 0); }
    else {
        SEND_B3(53, midi_channel, msb, lsb);
        RECEIVE_AND_PUSH_S16;
    }
}

static void op_I2M_NRPN_SLEW_set(const void *data, scene_state_t *ss,
                                 exec_state_t *es, command_state_t *cs) {
    s16 msb = cs_pop(cs);
    s16 lsb = cs_pop(cs);
    s16 slew = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(msb, 0, 127);
    RETURN_IF_OUT_OF_RANGE(lsb, 0, 127);
    RETURN_IF_OUT_OF_RANGE(slew, 0, 32767);
    SEND_B5(54, midi_channel, msb, lsb, slew >> 8, slew & 0xff);
}

static void op_I2M_NRPN_SLEW_POUND_get(const void *data, scene_state_t *ss,
                                       exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 msb = cs_pop(cs);
    s16 lsb = cs_pop(cs);
    if (msb < 0 || msb > 127 || lsb < 0 || lsb > 127 || channel < 1 ||
        channel > MAX_CHANNEL) {
        cs_push(cs, 0);
    }
    else {
        SEND_B3(53, channel - 1, msb, lsb);
        RECEIVE_AND_PUSH_S16;
    }
}

static void op_I2M_NRPN_SLEW_POUND_set(const void *data, scene_state_t *ss,
                                       exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 msb = cs_pop(cs);
    s16 lsb = cs_pop(cs);
    s16 slew = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(msb, 0, 127);
    RETURN_IF_OUT_OF_RANGE(lsb, 0, 127);
    RETURN_IF_OUT_OF_RANGE(slew, 0, 32767);
    SEND_B5(54, channel - 1, msb, lsb, slew >> 8, slew & 0xff);
}

static void op_I2M_NRPN_SET_get(const void *data, scene_state_t *ss,
                                exec_state_t *es, command_state_t *cs) {
    s16 msb = cs_pop(cs);
    s16 lsb = cs_pop(cs);
    s16 value = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(msb, 0, 127);
    RETURN_IF_OUT_OF_RANGE(lsb, 0, 127);
    CLAMP_TO_RANGE(value, -16384, 16384);
    SEND_B5(55, midi_channel, msb, lsb, value >> 7, value & 0x7f);
}

static void op_I2M_NRPN_SET_POUND_get(const void *data, scene_state_t *ss,
                                      exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 msb = cs_pop(cs);
    s16 lsb = cs_pop(cs);
    s16 value = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    RETURN_IF_OUT_OF_RANGE(msb, 0, 127);
    RETURN_IF_OUT_OF_RANGE(lsb, 0, 127);
    CLAMP_TO_RANGE(value, -16384, 16384);
    SEND_B5(55, channel - 1, msb, lsb, value >> 7, value & 0x7f);
}

static void op_I2M_PRG_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    s16 prg = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(prg, 0, 127);
    SEND_B2(60, midi_channel, prg);
}

static void op_I2M_PB_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    s16 pb = cs_pop(cs);
    CLAMP_TO_RANGE(pb, -8192, 8191);
    SEND_B3(61, midi_channel, pb >> 8, pb & 0xff);
}

static void op_I2M_AT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    s16 at = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(at, 0, 127);
    SEND_B2(62, midi_channel, at);
}

static void op_I2M_CLK_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    SEND_CMD(63);
}

static void op_I2M_START_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    SEND_CMD(64);
}

static void op_I2M_STOP_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    SEND_CMD(65);
}

static void op_I2M_CONT_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    SEND_CMD(66);
}

static void op_I2M_Q_CH_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    cs_push(cs, q_channel + 1);
}

static void op_I2M_Q_CH_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 1, MAX_CHANNEL);
    q_channel = channel - 1;
}

static void op_I2M_Q_LATCH_set(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs) {
    s16 latch = cs_pop(cs);
    SEND_B1(100, latch);
}

static void op_I2M_Q_NOTE_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    s16 index = cs_pop(cs);
    SEND_B2(110, q_channel, index);
    RECEIVE_AND_PUSH_S8;
}

static void op_I2M_Q_VEL_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 index = cs_pop(cs);
    SEND_B2(111, q_channel, index);
    RECEIVE_AND_PUSH_S8;
}

static void op_I2M_Q_CC_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 controller = cs_pop(cs);

    if (controller < 0 || controller > 127) { cs_push(cs, 0); }
    else {
        SEND_B2(120, q_channel, controller);
        RECEIVE_AND_PUSH_S8;
    }
}

static void op_I2M_Q_LCH_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    SEND_CMD(130);
    RECEIVE_AND_PUSH_S8;
}

static void op_I2M_Q_LN_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    SEND_CMD(131);
    RECEIVE_AND_PUSH_S8;
}

static void op_I2M_Q_LV_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    SEND_CMD(132);
    RECEIVE_AND_PUSH_S8;
}

static void op_I2M_Q_LO_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    SEND_CMD(133);
    RECEIVE_AND_PUSH_S8;
}

static void op_I2M_Q_LC_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    SEND_CMD(134);
    RECEIVE_AND_PUSH_S8;
}

static void op_I2M_Q_LCC_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    SEND_CMD(135);
    RECEIVE_AND_PUSH_S8;
}

static void op_I2M_B_R_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    s16 on = cs_pop(cs);
    SEND_B1(180, on ? 1 : 0);
}

static void op_I2M_B_L_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    s16 length = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(length, 0, 32767);
    SEND_B2(181, length >> 8, length & 0xff);
}

static void op_I2M_B_START_get(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs) {
    s16 start = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(start, 0, 32767);
    SEND_B2(182, start >> 8, start & 0xff);
}

static void op_I2M_B_END_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 end = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(end, 0, 32767);
    SEND_B2(183, end >> 8, end & 0xff);
}

static void op_I2M_B_DIR_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 dir = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(dir, 0, 2);
    SEND_B1(184, dir);
}

static void op_I2M_B_SPE_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    s16 speed = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(speed, 1, 32767);
    SEND_B2(185, speed >> 8, speed & 0xff);
}

static void op_I2M_B_FB_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 fb = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(fb, 0, 255);
    SEND_B1(186, fb);
}

static void op_I2M_B_NSHIFT_get(const void *data, scene_state_t *ss,
                                exec_state_t *es, command_state_t *cs) {
    s16 nshift = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(nshift, -127, 127);
    SEND_B1(187, nshift);
}

static void op_I2M_B_VSHIFT_get(const void *data, scene_state_t *ss,
                                exec_state_t *es, command_state_t *cs) {
    s16 vshift = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(vshift, -127, 127);
    SEND_B1(188, vshift);
}

static void op_I2M_B_TSHIFT_get(const void *data, scene_state_t *ss,
                                exec_state_t *es, command_state_t *cs) {
    s16 tshift = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(tshift, -16384, 16383);
    SEND_B2(189, tshift >> 8, tshift & 0xff);
}

static void op_I2M_B_NOFF_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    s16 noff = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(noff, -127, 127);
    SEND_B1(190, noff);
}

static void op_I2M_B_VOFF_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    s16 voff = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(voff, -127, 127);
    SEND_B1(191, voff);
}

static void op_I2M_B_TOFF_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    s16 toff = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(toff, -16384, 16383);
    SEND_B2(192, toff >> 8, toff & 0xff);
}

static void op_I2M_B_CLR_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    SEND_CMD(193);
}

static void op_I2M_B_MODE_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    s16 mode = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(mode, 0, 1);
    SEND_B1(194, mode);
}

static void op_I2M_MUTE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 value = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 0, MAX_CHANNEL);
    SEND_B2(13, channel, value);
}

static void op_I2M_SOLO_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 channel = cs_pop(cs);
    s16 value = cs_pop(cs);
    RETURN_IF_OUT_OF_RANGE(channel, 0, MAX_CHANNEL);
    SEND_B2(14, channel, value);
}

static void op_I2M_TEST_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 value1 = cs_pop(cs);
    s16 value2 = cs_pop(cs);
    SEND_B3(255, value1, value2 >> 8, value2 & 0xff);
}