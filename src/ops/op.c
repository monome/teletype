#include "ops/op.h"

#include <stddef.h>  // offsetof

#include "helpers.h"
#include "ops/ansible.h"
#include "ops/controlflow.h"
#include "ops/delay.h"
#include "ops/disting.h"
#include "ops/earthsea.h"
#include "ops/er301.h"
#include "ops/fader.h"
#include "ops/grid_ops.h"
#include "ops/hardware.h"
#include "ops/i2c.h"
#include "ops/init.h"
#include "ops/justfriends.h"
#include "ops/maths.h"
#include "ops/matrixarchate.h"
#include "ops/meadowphysics.h"
#include "ops/metronome.h"
#include "ops/midi.h"
#include "ops/orca.h"
#include "ops/patterns.h"
#include "ops/queue.h"
#include "ops/seed.h"
#include "ops/stack.h"
#include "ops/telex.h"
#include "ops/turtle.h"
#include "ops/variables.h"
#include "ops/whitewhale.h"
#include "ops/wslash.h"
#include "ops/wslashdelay.h"
#include "ops/wslashsynth.h"
#include "ops/wslashtape.h"
#include "teletype_io.h"
#include "ops/arc_ops.h"
/////////////////////////////////////////////////////////////////
// OPS //////////////////////////////////////////////////////////

// If you edit this array, you need to run 'utils/op_enums.py' to update the
// values in 'op_enum.h' so that the order matches.
const tele_op_t *tele_ops[E_OP__LENGTH] = {
    // variables
    &op_A, &op_B, &op_C, &op_D, &op_DRUNK, &op_DRUNK_MAX, &op_DRUNK_MIN,
    &op_DRUNK_WRAP, &op_FLIP, &op_I, &op_O, &op_O_INC, &op_O_MAX, &op_O_MIN,
    &op_O_WRAP, &op_T, &op_TIME, &op_TIME_ACT, &op_LAST, &op_X, &op_Y, &op_Z,
    &op_J, &op_K,

    // init
    &op_INIT, &op_INIT_SCENE, &op_INIT_SCRIPT, &op_INIT_SCRIPT_ALL, &op_INIT_P,
    &op_INIT_P_ALL, &op_INIT_CV, &op_INIT_CV_ALL, &op_INIT_TR, &op_INIT_TR_ALL,
    &op_INIT_DATA, &op_INIT_TIME,

    // turtle
    &op_TURTLE, &op_TURTLE_X, &op_TURTLE_Y, &op_TURTLE_MOVE, &op_TURTLE_F,
    &op_TURTLE_FX1, &op_TURTLE_FY1, &op_TURTLE_FX2, &op_TURTLE_FY2,
    &op_TURTLE_SPEED, &op_TURTLE_DIR, &op_TURTLE_STEP, &op_TURTLE_BUMP,
    &op_TURTLE_WRAP, &op_TURTLE_BOUNCE, &op_TURTLE_SCRIPT, &op_TURTLE_SHOW,

    // metronome
    &op_M, &op_M_SYM_EXCLAMATION, &op_M_ACT, &op_M_RESET,

    // patterns
    &op_P_N, &op_P, &op_PN, &op_P_L, &op_PN_L, &op_P_WRAP, &op_PN_WRAP,
    &op_P_START, &op_PN_START, &op_P_END, &op_PN_END, &op_P_I, &op_PN_I,
    &op_P_HERE, &op_PN_HERE, &op_P_NEXT, &op_PN_NEXT, &op_P_PREV, &op_PN_PREV,
    &op_P_INS, &op_PN_INS, &op_P_RM, &op_PN_RM, &op_P_PUSH, &op_PN_PUSH,
    &op_P_POP, &op_PN_POP, &op_P_MIN, &op_PN_MIN, &op_P_MAX, &op_PN_MAX,
    &op_P_SHUF, &op_PN_SHUF, &op_P_REV, &op_PN_REV, &op_P_ROT, &op_PN_ROT,
    &op_P_RND, &op_PN_RND, &op_P_ADD, &op_PN_ADD, &op_P_SUB, &op_PN_SUB,
    &op_P_ADDW, &op_PN_ADDW, &op_P_SUBW, &op_PN_SUBW,

    // queue
    &op_Q, &op_Q_AVG, &op_Q_N,

    // hardware
    &op_CV, &op_CV_OFF, &op_CV_SLEW, &op_IN, &op_IN_SCALE, &op_PARAM,
    &op_PARAM_SCALE, &op_IN_CAL_MIN, &op_IN_CAL_MAX, &op_IN_CAL_RESET,
    &op_PARAM_CAL_MIN, &op_PARAM_CAL_MAX, &op_PARAM_CAL_RESET, &op_PRM, &op_TR,
    &op_TR_POL, &op_TR_TIME, &op_TR_TOG, &op_TR_PULSE, &op_TR_P, &op_CV_SET,
    &op_MUTE, &op_STATE, &op_DEVICE_FLIP,

    // maths
    &op_ADD, &op_SUB, &op_MUL, &op_DIV, &op_MOD, &op_RAND, &op_RND, &op_RRAND,
    &op_RRND, &op_R, &op_R_MIN, &op_R_MAX, &op_TOSS, &op_MIN, &op_MAX, &op_LIM,
    &op_WRAP, &op_WRP, &op_QT, &op_QT_S, &op_QT_CS, &op_QT_B, &op_AVG, &op_EQ,
    &op_NE, &op_LT, &op_GT, &op_LTE, &op_GTE, &op_NZ, &op_EZ, &op_RSH, &op_LSH,
    &op_LROT, &op_RROT, &op_EXP, &op_ABS, &op_SGN, &op_AND, &op_OR, &op_JI,
    &op_SCALE, &op_SCL, &op_N, &op_VN, &op_HZ, &op_N_S, &op_N_C, &op_N_CS,
    &op_N_B, &op_N_BX, &op_V, &op_VV, &op_ER, &op_NR, &op_BPM, &op_BIT_OR,
    &op_BIT_AND, &op_BIT_NOT, &op_BIT_XOR, &op_BSET, &op_BGET, &op_BCLR,
    &op_BTOG, &op_BREV, &op_XOR, &op_CHAOS, &op_CHAOS_R, &op_CHAOS_ALG,
    &op_SYM_PLUS, &op_SYM_DASH, &op_SYM_STAR, &op_SYM_FORWARD_SLASH,
    &op_SYM_PERCENTAGE, &op_SYM_EQUAL_x2, &op_SYM_EXCLAMATION_EQUAL,
    &op_SYM_LEFT_ANGLED, &op_SYM_RIGHT_ANGLED, &op_SYM_LEFT_ANGLED_EQUAL,
    &op_SYM_RIGHT_ANGLED_EQUAL, &op_SYM_EXCLAMATION, &op_SYM_LEFT_ANGLED_x2,
    &op_SYM_RIGHT_ANGLED_x2, &op_SYM_LEFT_ANGLED_x3, &op_SYM_RIGHT_ANGLED_x3,
    &op_SYM_AMPERSAND_x2, &op_SYM_PIPE_x2, &op_TIF,

    // stack
    &op_S_ALL, &op_S_POP, &op_S_CLR, &op_S_L,

    // controlflow
    &op_SCRIPT, &op_SYM_DOLLAR, &op_SCRIPT_POL, &op_SYM_DOLLAR_POL, &op_KILL,
    &op_SCENE, &op_SCENE_G, &op_BREAK, &op_BRK, &op_SYNC,

    // delay
    &op_DEL_CLR,

    // i2c
    &op_IIA, &op_IIS, &op_IIS1, &op_IIS2, &op_IIS3, &op_IISB1, &op_IISB2,
    &op_IISB3, &op_IIQ, &op_IIQ1, &op_IIQ2, &op_IIQ3, &op_IIQB1, &op_IIQB2,
    &op_IIQB3, &op_IIB, &op_IIB1, &op_IIB2, &op_IIB3, &op_IIBB1, &op_IIBB2,
    &op_IIBB3,

    // whitewhale
    &op_WW_PRESET, &op_WW_POS, &op_WW_SYNC, &op_WW_START, &op_WW_END,
    &op_WW_PMODE, &op_WW_PATTERN, &op_WW_QPATTERN, &op_WW_MUTE1, &op_WW_MUTE2,
    &op_WW_MUTE3, &op_WW_MUTE4, &op_WW_MUTEA, &op_WW_MUTEB,

    // meadowphysics
    &op_MP_PRESET, &op_MP_RESET, &op_MP_STOP,

    // earthsea
    &op_ES_PRESET, &op_ES_MODE, &op_ES_CLOCK, &op_ES_RESET, &op_ES_PATTERN,
    &op_ES_TRANS, &op_ES_STOP, &op_ES_TRIPLE, &op_ES_MAGIC, &op_ES_CV,

    // orca
    &op_OR_TRK, &op_OR_CLK, &op_OR_DIV, &op_OR_PHASE, &op_OR_RST, &op_OR_WGT,
    &op_OR_MUTE, &op_OR_SCALE, &op_OR_BANK, &op_OR_PRESET, &op_OR_RELOAD,
    &op_OR_ROTS, &op_OR_ROTW, &op_OR_GRST, &op_OR_CVA, &op_OR_CVB,

    // ansible
    &op_ANS_G_LED, &op_ANS_G, &op_ANS_G_P, &op_ANS_A_LED, &op_ANS_A,
    &op_ANS_APP, &op_KR_PRE, &op_KR_PAT, &op_KR_SCALE, &op_KR_PERIOD,
    &op_KR_POS, &op_KR_L_ST, &op_KR_L_LEN, &op_KR_RES, &op_KR_CV, &op_KR_MUTE,
    &op_KR_TMUTE, &op_KR_CLK, &op_KR_PG, &op_KR_CUE, &op_KR_DIR, &op_KR_DUR,
    &op_ME_PRE, &op_ME_RES, &op_ME_STOP, &op_ME_SCALE, &op_ME_PERIOD, &op_ME_CV,
    &op_LV_PRE, &op_LV_RES, &op_LV_POS, &op_LV_L_ST, &op_LV_L_LEN, &op_LV_L_DIR,
    &op_LV_CV, &op_CY_PRE, &op_CY_RES, &op_CY_POS, &op_CY_REV, &op_CY_CV,
    &op_MID_SHIFT, &op_MID_SLEW, &op_ARP_STY, &op_ARP_HLD, &op_ARP_RPT,
    &op_ARP_GT, &op_ARP_DIV, &op_ARP_RES, &op_ARP_SHIFT, &op_ARP_SLEW,
    &op_ARP_FIL, &op_ARP_ROT, &op_ARP_ER,

    // justfriends
    &op_JF_TR, &op_JF_RMODE, &op_JF_RUN, &op_JF_SHIFT, &op_JF_VTR, &op_JF_MODE,
    &op_JF_TICK, &op_JF_VOX, &op_JF_NOTE, &op_JF_GOD, &op_JF_TUNE, &op_JF_QT,
    &op_JF_PITCH, &op_JF_ADDR, &op_JF_SPEED, &op_JF_TSC, &op_JF_RAMP,
    &op_JF_CURVE, &op_JF_FM, &op_JF_TIME, &op_JF_INTONE, &op_JF_POLY,
    &op_JF_POLY_RESET, &op_JF_SEL,

    // W/
    &op_WS_PLAY, &op_WS_REC, &op_WS_CUE, &op_WS_LOOP,

    // W/S
    &op_WS_S_PITCH, &op_WS_S_VEL, &op_WS_S_VOX, &op_WS_S_NOTE, &op_WS_S_AR_MODE,
    &op_WS_S_LPG_TIME, &op_WS_S_LPG_SYMMETRY, &op_WS_S_CURVE, &op_WS_S_RAMP,
    &op_WS_S_FM_INDEX, &op_WS_S_FM_RATIO, &op_WS_S_FM_ENV, &op_WS_S_VOICES,
    &op_WS_S_PATCH,

    // W/D
    &op_WS_D_FEEDBACK, &op_WS_D_MIX, &op_WS_D_LOWPASS, &op_WS_D_FREEZE,
    &op_WS_D_TIME, &op_WS_D_LENGTH, &op_WS_D_POSITION, &op_WS_D_CUT,
    &op_WS_D_FREQ_RANGE, &op_WS_D_RATE, &op_WS_D_FREQ, &op_WS_D_CLK,
    &op_WS_D_CLK_RATIO, &op_WS_D_PLUCK, &op_WS_D_MOD_RATE, &op_WS_D_MOD_AMOUNT,

    // W/T
    &op_WS_T_RECORD, &op_WS_T_PLAY, &op_WS_T_REV, &op_WS_T_SPEED, &op_WS_T_FREQ,
    &op_WS_T_PRE_LEVEL, &op_WS_T_MONITOR_LEVEL, &op_WS_T_REC_LEVEL,
    &op_WS_T_HEAD_ORDER, &op_WS_T_LOOP_START, &op_WS_T_LOOP_END,
    &op_WS_T_LOOP_ACTIVE, &op_WS_T_LOOP_SCALE, &op_WS_T_LOOP_NEXT,
    &op_WS_T_TIMESTAMP, &op_WS_T_SEEK,

    // telex
    &op_TO_TR, &op_TO_TR_TOG, &op_TO_TR_PULSE, &op_TO_TR_TIME, &op_TO_TR_TIME_S,
    &op_TO_TR_TIME_M, &op_TO_TR_POL, &op_TO_KILL,

    &op_TO_TR_PULSE_DIV, &op_TO_TR_PULSE_MUTE, &op_TO_TR_M_MUL,

    &op_TO_M, &op_TO_M_S, &op_TO_M_M, &op_TO_M_BPM, &op_TO_M_ACT, &op_TO_M_SYNC,
    &op_TO_M_COUNT,

    &op_TO_TR_M, &op_TO_TR_M_S, &op_TO_TR_M_M, &op_TO_TR_M_BPM, &op_TO_TR_M_ACT,
    &op_TO_TR_M_SYNC, &op_TO_TR_WIDTH, &op_TO_TR_M_COUNT,

    &op_TO_CV, &op_TO_CV_SLEW, &op_TO_CV_SLEW_S, &op_TO_CV_SLEW_M,
    &op_TO_CV_SET, &op_TO_CV_OFF, &op_TO_CV_QT, &op_TO_CV_QT_SET, &op_TO_CV_N,
    &op_TO_CV_N_SET, &op_TO_CV_SCALE, &op_TO_CV_LOG,

    &op_TO_CV_INIT, &op_TO_TR_INIT, &op_TO_INIT,

    &op_TO_TR_P, &op_TO_TR_P_DIV, &op_TO_TR_P_MUTE,

    &op_TO_OSC, &op_TO_OSC_SET, &op_TO_OSC_QT, &op_TO_OSC_QT_SET, &op_TO_OSC_FQ,
    &op_TO_OSC_FQ_SET, &op_TO_OSC_N, &op_TO_OSC_N_SET, &op_TO_OSC_LFO,
    &op_TO_OSC_LFO_SET, &op_TO_OSC_WAVE, &op_TO_OSC_SYNC, &op_TO_OSC_PHASE,
    &op_TO_OSC_WIDTH, &op_TO_OSC_RECT, &op_TO_OSC_SLEW, &op_TO_OSC_SLEW_S,
    &op_TO_OSC_SLEW_M, &op_TO_OSC_SCALE, &op_TO_OSC_CYC, &op_TO_OSC_CYC_S,
    &op_TO_OSC_CYC_M, &op_TO_OSC_CYC_SET, &op_TO_OSC_CYC_S_SET,
    &op_TO_OSC_CYC_M_SET, &op_TO_OSC_CTR,

    &op_TO_ENV_ACT, &op_TO_ENV_ATT, &op_TO_ENV_ATT_S, &op_TO_ENV_ATT_M,
    &op_TO_ENV_DEC, &op_TO_ENV_DEC_S, &op_TO_ENV_DEC_M, &op_TO_ENV_TRIG,
    &op_TO_ENV_EOR, &op_TO_ENV_EOC, &op_TO_ENV_LOOP,

    &op_TO_ENV, &op_TO_CV_CALIB, &op_TO_CV_RESET,

    &op_TI_PARAM, &op_TI_PARAM_QT, &op_TI_PARAM_N, &op_TI_PARAM_SCALE,
    &op_TI_PARAM_MAP, &op_TI_IN, &op_TI_IN_QT, &op_TI_IN_N, &op_TI_IN_SCALE,
    &op_TI_IN_MAP, &op_TI_PARAM_CALIB, &op_TI_IN_CALIB, &op_TI_STORE,
    &op_TI_RESET,

    &op_TI_PARAM_INIT, &op_TI_IN_INIT, &op_TI_INIT,

    &op_TI_PRM, &op_TI_PRM_QT, &op_TI_PRM_N, &op_TI_PRM_SCALE, &op_TI_PRM_MAP,
    &op_TI_PRM_INIT,

    // fader
    &op_FADER, &op_FADER_SCALE, &op_FADER_CAL_MIN, &op_FADER_CAL_MAX,
    &op_FADER_CAL_RESET, &op_FB, &op_FB_S, &op_FB_C_MIN, &op_FB_C_MAX,
    &op_FB_C_R,

    // ER301
    &op_SC_TR, &op_SC_TR_TOG, &op_SC_TR_PULSE, &op_SC_TR_TIME, &op_SC_TR_POL,
    &op_SC_CV, &op_SC_CV_SLEW, &op_SC_CV_SET, &op_SC_CV_OFF, &op_SC_TR_P,

    // grid
    &op_G_RST, &op_G_CLR, &op_G_ROTATE, &op_G_DIM, &op_G_KEY, &op_G_GRP,
    &op_G_GRP_EN, &op_G_GRP_RST, &op_G_GRP_SW, &op_G_GRP_SC, &op_G_GRPI,
    &op_G_LED, &op_G_LED_C, &op_G_REC, &op_G_RCT, &op_G_BTN, &op_G_BTX,
    &op_G_GBT, &op_G_GBX, &op_G_BTN_EN, &op_G_BTN_V, &op_G_BTN_L, &op_G_BTN_X,
    &op_G_BTN_Y, &op_G_BTNI, &op_G_BTNV, &op_G_BTNL, &op_G_BTNX, &op_G_BTNY,
    &op_G_BTN_SW, &op_G_BTN_PR, &op_G_GBTN_V, &op_G_GBTN_L, &op_G_FDR,
    &op_G_FDX, &op_G_GFD, &op_G_GFX, &op_G_FDR_EN, &op_G_FDR_V, &op_G_FDR_N,
    &op_G_FDR_L, &op_G_FDR_X, &op_G_FDR_Y, &op_G_FDRI, &op_G_FDRV, &op_G_FDRN,
    &op_G_FDRL, &op_G_FDRX, &op_G_FDRY, &op_G_FDR_PR, &op_G_GFDR_V,
    &op_G_GFDR_N, &op_G_GFDR_L, &op_G_GFDR_RN, &op_G_XYP, &op_G_XYP_X,
    &op_G_XYP_Y, &op_G_GBTN_C, &op_G_GBTN_I, &op_G_GBTN_W, &op_G_GBTN_H,
    &op_G_GBTN_X1, &op_G_GBTN_X2, &op_G_GBTN_Y1, &op_G_GBTN_Y2,

    // arc_cycles
    &op_ARC_METRO, &op_ARC_SYNC,&op_ARC_RST,

    // matrixarchate
    &op_MA_SELECT, &op_MA_STEP, &op_MA_RESET, &op_MA_PGM, &op_MA_ON, &op_MA_PON,
    &op_MA_OFF, &op_MA_POFF, &op_MA_SET, &op_MA_PSET, &op_MA_COL, &op_MA_PCOL,
    &op_MA_ROW, &op_MA_PROW, &op_MA_CLR, &op_MA_PCLR,

    // disting ex
    &op_EX, &op_EX_PRESET, &op_EX_PRE, &op_EX_SAVE, &op_EX_RESET, &op_EX_ALG,
    &op_EX_A, &op_EX_CTRL, &op_EX_C, &op_EX_PARAM, &op_EX_P, &op_EX_PV,
    &op_EX_MIN, &op_EX_MAX, &op_EX_REC, &op_EX_PLAY, &op_EX_AL_P, &op_EX_AL_CLK,
    &op_EX_M_CH, &op_EX_M_N, &op_EX_M_NO, &op_EX_M_PB, &op_EX_M_CC,
    &op_EX_M_PRG, &op_EX_M_CLK, &op_EX_M_START, &op_EX_M_STOP, &op_EX_M_CONT,
    &op_EX_SB_CH, &op_EX_SB_N, &op_EX_SB_NO, &op_EX_SB_PB, &op_EX_SB_CC,
    &op_EX_SB_PRG, &op_EX_SB_CLK, &op_EX_SB_START, &op_EX_SB_STOP,
    &op_EX_SB_CONT, &op_EX_VOX_P, &op_EX_VP, &op_EX_VOX, &op_EX_V, &op_EX_VOX_O,
    &op_EX_VO, &op_EX_NOTE, &op_EX_N, &op_EX_NOTE_O, &op_EX_NO, &op_EX_ALLOFF,
    &op_EX_AO, &op_EX_T, &op_EX_TV, &op_EX_LP_REC, &op_EX_LP_PLAY,
    &op_EX_LP_REV, &op_EX_LP_DOWN, &op_EX_LP_CLR, &op_EX_LP, &op_EX_LP_DOWNQ,
    &op_EX_LP_REVQ,

    // seed
    &op_SEED, &op_RAND_SEED, &op_SYM_RAND_SD, &op_SYM_R_SD, &op_TOSS_SEED,
    &op_SYM_TOSS_SD, &op_PROB_SEED, &op_SYM_PROB_SD, &op_DRUNK_SEED,
    &op_SYM_DRUNK_SD, &op_P_SEED, &op_SYM_P_SD,

    &op_MI_SYM_DOLLAR, &op_MI_LN, &op_MI_LNV, &op_MI_LV, &op_MI_LVV, &op_MI_LO,
    &op_MI_LC, &op_MI_LCC, &op_MI_LCCV, &op_MI_NL, &op_MI_N, &op_MI_NV,
    &op_MI_V, &op_MI_VV, &op_MI_OL, &op_MI_O, &op_MI_CL, &op_MI_C, &op_MI_CC,
    &op_MI_CCV, &op_MI_LCH, &op_MI_NCH, &op_MI_OCH, &op_MI_CCH, &op_MI_LE,
    &op_MI_CLKD, &op_MI_CLKR
};

/////////////////////////////////////////////////////////////////
// MODS /////////////////////////////////////////////////////////

const tele_mod_t *tele_mods[E_MOD__LENGTH] = {
    // controlflow
    &mod_IF, &mod_ELIF, &mod_ELSE, &mod_L, &mod_W, &mod_EVERY, &mod_SKIP,
    &mod_OTHER, &mod_PROB,

    // delay
    &mod_DEL, &mod_DEL_X, &mod_DEL_R, &mod_DEL_G, &mod_DEL_B,

    // pattern
    &mod_P_MAP, &mod_PN_MAP,

    // stack
    &mod_S,

    // disting ex
    &mod_EX1, &mod_EX2, &mod_EX3, &mod_EX4,

    // just friends
    &mod_JF0, &mod_JF1, &mod_JF2
};

/////////////////////////////////////////////////////////////////
// HELPERS //////////////////////////////////////////////////////

void op_peek_i16(const void *data, scene_state_t *ss, exec_state_t *NOTUSED(es),
                 command_state_t *cs) {
    char *base = (char *)ss;
    size_t offset = (size_t)data;
    int16_t *ptr = (int16_t *)(base + offset);
    cs_push(cs, *ptr);
}

void op_poke_i16(const void *data, scene_state_t *ss, exec_state_t *NOTUSED(es),
                 command_state_t *cs) {
    char *base = (char *)ss;
    size_t offset = (size_t)data;
    int16_t *ptr = (int16_t *)(base + offset);
    *ptr = cs_pop(cs);
    tele_vars_updated();
}

void op_simple_i2c(const void *data, scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t message = (intptr_t)data;
    int16_t value = cs_pop(cs);

    uint8_t address = message & 0xF0;
    uint8_t message_type = message & 0xFF;

    uint8_t buffer[3] = { message_type, value >> 8, value & 0xFF };

    tele_ii_tx(address, buffer, 3);
}
