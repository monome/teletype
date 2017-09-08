// clang-format off

#ifndef _OP_ENUM_H_
#define _OP_ENUM_H_

// This file has been autogenerated by 'utils/op_enums.py'

typedef enum {
    E_OP_A,
    E_OP_B,
    E_OP_C,
    E_OP_D,
    E_OP_DRUNK,
    E_OP_DRUNK_MAX,
    E_OP_DRUNK_MIN,
    E_OP_DRUNK_WRAP,
    E_OP_FLIP,
    E_OP_I,
    E_OP_O,
    E_OP_O_INC,
    E_OP_O_MAX,
    E_OP_O_MIN,
    E_OP_O_WRAP,
    E_OP_T,
    E_OP_TIME,
    E_OP_TIME_ACT,
    E_OP_X,
    E_OP_Y,
    E_OP_Z,
    E_OP_M,
    E_OP_M_SYM_EXCLAMATION,
    E_OP_M_ACT,
    E_OP_M_RESET,
    E_OP_P_N,
    E_OP_P,
    E_OP_PN,
    E_OP_P_L,
    E_OP_PN_L,
    E_OP_P_WRAP,
    E_OP_PN_WRAP,
    E_OP_P_START,
    E_OP_PN_START,
    E_OP_P_END,
    E_OP_PN_END,
    E_OP_P_I,
    E_OP_PN_I,
    E_OP_P_HERE,
    E_OP_PN_HERE,
    E_OP_P_NEXT,
    E_OP_PN_NEXT,
    E_OP_P_PREV,
    E_OP_PN_PREV,
    E_OP_P_INS,
    E_OP_PN_INS,
    E_OP_P_RM,
    E_OP_PN_RM,
    E_OP_P_PUSH,
    E_OP_PN_PUSH,
    E_OP_P_POP,
    E_OP_PN_POP,
    E_OP_Q,
    E_OP_Q_AVG,
    E_OP_Q_N,
    E_OP_CV,
    E_OP_CV_OFF,
    E_OP_CV_SLEW,
    E_OP_IN,
    E_OP_PARAM,
    E_OP_PRM,
    E_OP_TR,
    E_OP_TR_POL,
    E_OP_TR_TIME,
    E_OP_TR_TOG,
    E_OP_TR_PULSE,
    E_OP_TR_P,
    E_OP_CV_SET,
    E_OP_MUTE,
    E_OP_STATE,
    E_OP_ADD,
    E_OP_SUB,
    E_OP_MUL,
    E_OP_DIV,
    E_OP_MOD,
    E_OP_RAND,
    E_OP_RRAND,
    E_OP_TOSS,
    E_OP_MIN,
    E_OP_MAX,
    E_OP_LIM,
    E_OP_WRAP,
    E_OP_QT,
    E_OP_AVG,
    E_OP_EQ,
    E_OP_NE,
    E_OP_LT,
    E_OP_GT,
    E_OP_LTE,
    E_OP_GTE,
    E_OP_NZ,
    E_OP_EZ,
    E_OP_RSH,
    E_OP_LSH,
    E_OP_EXP,
    E_OP_ABS,
    E_OP_AND,
    E_OP_OR,
    E_OP_JI,
    E_OP_SCALE,
    E_OP_N,
    E_OP_V,
    E_OP_VV,
    E_OP_ER,
    E_OP_MSPB,
    E_OP_XOR,
    E_OP_SYM_PLUS,
    E_OP_SYM_DASH,
    E_OP_SYM_STAR,
    E_OP_SYM_FORWARD_SLASH,
    E_OP_SYM_PERCENTAGE,
    E_OP_SYM_EQUAL_x2,
    E_OP_SYM_EXCLAMATION_EQUAL,
    E_OP_SYM_LEFT_ANGLED,
    E_OP_SYM_RIGHT_ANGLED,
    E_OP_SYM_LEFT_ANGLED_EQUAL,
    E_OP_SYM_RIGHT_ANGLED_EQUAL,
    E_OP_SYM_EXCLAMATION,
    E_OP_SYM_LEFT_ANGLED_x2,
    E_OP_SYM_RIGHT_ANGLED_x2,
    E_OP_SYM_AMPERSAND_x2,
    E_OP_SYM_PIPE_x2,
    E_OP_S_ALL,
    E_OP_S_POP,
    E_OP_S_CLR,
    E_OP_S_L,
    E_OP_SCRIPT,
    E_OP_KILL,
    E_OP_SCENE,
    E_OP_DEL_CLR,
    E_OP_WW_PRESET,
    E_OP_WW_POS,
    E_OP_WW_SYNC,
    E_OP_WW_START,
    E_OP_WW_END,
    E_OP_WW_PMODE,
    E_OP_WW_PATTERN,
    E_OP_WW_QPATTERN,
    E_OP_WW_MUTE1,
    E_OP_WW_MUTE2,
    E_OP_WW_MUTE3,
    E_OP_WW_MUTE4,
    E_OP_WW_MUTEA,
    E_OP_WW_MUTEB,
    E_OP_MP_PRESET,
    E_OP_MP_RESET,
    E_OP_MP_STOP,
    E_OP_ES_PRESET,
    E_OP_ES_MODE,
    E_OP_ES_CLOCK,
    E_OP_ES_RESET,
    E_OP_ES_PATTERN,
    E_OP_ES_TRANS,
    E_OP_ES_STOP,
    E_OP_ES_TRIPLE,
    E_OP_ES_MAGIC,
    E_OP_OR_TRK,
    E_OP_OR_CLK,
    E_OP_OR_DIV,
    E_OP_OR_PHASE,
    E_OP_OR_RST,
    E_OP_OR_WGT,
    E_OP_OR_MUTE,
    E_OP_OR_SCALE,
    E_OP_OR_BANK,
    E_OP_OR_PRESET,
    E_OP_OR_RELOAD,
    E_OP_OR_ROTS,
    E_OP_OR_ROTW,
    E_OP_OR_GRST,
    E_OP_OR_CVA,
    E_OP_OR_CVB,
    E_OP_KR_PRE,
    E_OP_KR_PAT,
    E_OP_KR_SCALE,
    E_OP_KR_PERIOD,
    E_OP_KR_POS,
    E_OP_KR_L_ST,
    E_OP_KR_L_LEN,
    E_OP_KR_RES,
    E_OP_ME_PRE,
    E_OP_ME_RES,
    E_OP_ME_STOP,
    E_OP_ME_SCALE,
    E_OP_ME_PERIOD,
    E_OP_LV_PRE,
    E_OP_LV_RES,
    E_OP_LV_POS,
    E_OP_LV_L_ST,
    E_OP_LV_L_LEN,
    E_OP_LV_L_DIR,
    E_OP_LV_CV,
    E_OP_CY_PRE,
    E_OP_CY_RES,
    E_OP_CY_POS,
    E_OP_CY_REV,
    E_OP_CY_CV,
    E_OP_MID_SHIFT,
    E_OP_MID_SLEW,
    E_OP_ARP_STY,
    E_OP_ARP_HLD,
    E_OP_ARP_RPT,
    E_OP_ARP_GT,
    E_OP_ARP_DIV,
    E_OP_ARP_RES,
    E_OP_ARP_SHIFT,
    E_OP_ARP_SLEW,
    E_OP_ARP_FIL,
    E_OP_ARP_ROT,
    E_OP_ARP_ER,
    E_OP_JF_TR,
    E_OP_JF_RMODE,
    E_OP_JF_RUN,
    E_OP_JF_SHIFT,
    E_OP_JF_VTR,
    E_OP_JF_MODE,
    E_OP_JF_TICK,
    E_OP_JF_VOX,
    E_OP_JF_NOTE,
    E_OP_JF_GOD,
    E_OP_JF_TUNE,
    E_OP_JF_QT,
    E_OP_TO_TR,
    E_OP_TO_TR_TOG,
    E_OP_TO_TR_PULSE,
    E_OP_TO_TR_TIME,
    E_OP_TO_TR_TIME_S,
    E_OP_TO_TR_TIME_M,
    E_OP_TO_TR_POL,
    E_OP_TO_KILL,
    E_OP_TO_TR_PULSE_DIV,
    E_OP_TO_TR_PULSE_MUTE,
    E_OP_TO_TR_M_MUL,
    E_OP_TO_M,
    E_OP_TO_M_S,
    E_OP_TO_M_M,
    E_OP_TO_M_BPM,
    E_OP_TO_M_ACT,
    E_OP_TO_M_SYNC,
    E_OP_TO_M_COUNT,
    E_OP_TO_TR_M,
    E_OP_TO_TR_M_S,
    E_OP_TO_TR_M_M,
    E_OP_TO_TR_M_BPM,
    E_OP_TO_TR_M_ACT,
    E_OP_TO_TR_M_SYNC,
    E_OP_TO_TR_WIDTH,
    E_OP_TO_TR_M_COUNT,
    E_OP_TO_CV,
    E_OP_TO_CV_SLEW,
    E_OP_TO_CV_SLEW_S,
    E_OP_TO_CV_SLEW_M,
    E_OP_TO_CV_SET,
    E_OP_TO_CV_OFF,
    E_OP_TO_CV_QT,
    E_OP_TO_CV_QT_SET,
    E_OP_TO_CV_N,
    E_OP_TO_CV_N_SET,
    E_OP_TO_CV_SCALE,
    E_OP_TO_CV_LOG,
    E_OP_TO_CV_INIT,
    E_OP_TO_TR_INIT,
    E_OP_TO_INIT,
    E_OP_TO_TR_P,
    E_OP_TO_TR_P_DIV,
    E_OP_TO_TR_P_MUTE,
    E_OP_TO_OSC,
    E_OP_TO_OSC_SET,
    E_OP_TO_OSC_QT,
    E_OP_TO_OSC_QT_SET,
    E_OP_TO_OSC_FQ,
    E_OP_TO_OSC_FQ_SET,
    E_OP_TO_OSC_N,
    E_OP_TO_OSC_N_SET,
    E_OP_TO_OSC_LFO,
    E_OP_TO_OSC_LFO_SET,
    E_OP_TO_OSC_WAVE,
    E_OP_TO_OSC_SYNC,
    E_OP_TO_OSC_PHASE,
    E_OP_TO_OSC_WIDTH,
    E_OP_TO_OSC_RECT,
    E_OP_TO_OSC_SLEW,
    E_OP_TO_OSC_SLEW_S,
    E_OP_TO_OSC_SLEW_M,
    E_OP_TO_OSC_SCALE,
    E_OP_TO_OSC_CYC,
    E_OP_TO_OSC_CYC_S,
    E_OP_TO_OSC_CYC_M,
    E_OP_TO_OSC_CYC_SET,
    E_OP_TO_OSC_CYC_S_SET,
    E_OP_TO_OSC_CYC_M_SET,
    E_OP_TO_OSC_CTR,
    E_OP_TO_ENV_ACT,
    E_OP_TO_ENV_ATT,
    E_OP_TO_ENV_ATT_S,
    E_OP_TO_ENV_ATT_M,
    E_OP_TO_ENV_DEC,
    E_OP_TO_ENV_DEC_S,
    E_OP_TO_ENV_DEC_M,
    E_OP_TO_ENV_TRIG,
    E_OP_TO_ENV_EOR,
    E_OP_TO_ENV_EOC,
    E_OP_TO_ENV_LOOP,
    E_OP_TI_PARAM,
    E_OP_TI_PARAM_QT,
    E_OP_TI_PARAM_N,
    E_OP_TI_PARAM_SCALE,
    E_OP_TI_PARAM_MAP,
    E_OP_TI_IN,
    E_OP_TI_IN_QT,
    E_OP_TI_IN_N,
    E_OP_TI_IN_SCALE,
    E_OP_TI_IN_MAP,
    E_OP_TI_PARAM_CALIB,
    E_OP_TI_IN_CALIB,
    E_OP_TI_STORE,
    E_OP_TI_RESET,
    E_OP_TI_PARAM_INIT,
    E_OP_TI_IN_INIT,
    E_OP_TI_INIT,
    E_OP_TI_PRM,
    E_OP_TI_PRM_QT,
    E_OP_TI_PRM_N,
    E_OP_TI_PRM_SCALE,
    E_OP_TI_PRM_MAP,
    E_OP_TI_PRM_INIT,
    E_OP__LENGTH,
} tele_op_idx_t;

typedef enum {
    E_MOD_IF,
    E_MOD_ELIF,
    E_MOD_ELSE,
    E_MOD_L,
    E_MOD_PROB,
    E_MOD_DEL,
    E_MOD_S,
    E_MOD__LENGTH,
} tele_mod_idx_t;

#endif
