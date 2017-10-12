#include "ops/op.h"

#include <stddef.h>  // offsetof

#include "helpers.h"
#include "teletype_io.h"

#include "ops/ansible.h"
#include "ops/controlflow.h"
#include "ops/delay.h"
#include "ops/earthsea.h"
#include "ops/hardware.h"
#include "ops/justfriends.h"
#include "ops/maths.h"
#include "ops/meadowphysics.h"
#include "ops/metronome.h"
#include "ops/orca.h"
#include "ops/patterns.h"
#include "ops/queue.h"
#include "ops/stack.h"
#include "ops/telex.h"
#include "ops/turtle.h"
#include "ops/variables.h"
#include "ops/whitewhale.h"


/////////////////////////////////////////////////////////////////
// OPS //////////////////////////////////////////////////////////

// If you edit this array, you need to run 'utils/op_enums.py' to update the
// values in 'op_enum.h' so that the order matches.
const tele_op_t *tele_ops[E_OP__LENGTH] = {
    // variables
    &op_A, &op_B, &op_C, &op_D, &op_DRUNK, &op_DRUNK_MAX, &op_DRUNK_MIN,
    &op_DRUNK_WRAP, &op_FLIP, &op_I, &op_O, &op_O_INC, &op_O_MAX, &op_O_MIN,
    &op_O_WRAP, &op_T, &op_TIME, &op_TIME_ACT, &op_LAST, &op_X, &op_Y, &op_Z,

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
    &op_P_POP, &op_PN_POP,

    // queue
    &op_Q, &op_Q_AVG, &op_Q_N,

    // hardware
    &op_CV, &op_CV_OFF, &op_CV_SLEW, &op_IN, &op_PARAM, &op_PRM, &op_TR,
    &op_TR_POL, &op_TR_TIME, &op_TR_TOG, &op_TR_PULSE, &op_TR_P, &op_CV_SET,
    &op_MUTE, &op_STATE,

    // maths
    &op_ADD, &op_SUB, &op_MUL, &op_DIV, &op_MOD, &op_RAND, &op_RRAND, &op_TOSS,
    &op_MIN, &op_MAX, &op_LIM, &op_WRAP, &op_QT, &op_AVG, &op_EQ, &op_NE,
    &op_LT, &op_GT, &op_LTE, &op_GTE, &op_NZ, &op_EZ, &op_RSH, &op_LSH, &op_EXP,
    &op_ABS, &op_AND, &op_OR, &op_JI, &op_SCALE, &op_N, &op_V, &op_VV, &op_ER,
    &op_BPM, &op_XOR, &op_SYM_PLUS, &op_SYM_DASH, &op_SYM_STAR,
    &op_SYM_FORWARD_SLASH, &op_SYM_PERCENTAGE, &op_SYM_EQUAL_x2,
    &op_SYM_EXCLAMATION_EQUAL, &op_SYM_LEFT_ANGLED, &op_SYM_RIGHT_ANGLED,
    &op_SYM_LEFT_ANGLED_EQUAL, &op_SYM_RIGHT_ANGLED_EQUAL, &op_SYM_EXCLAMATION,
    &op_SYM_LEFT_ANGLED_x2, &op_SYM_RIGHT_ANGLED_x2, &op_SYM_AMPERSAND_x2,
    &op_SYM_PIPE_x2,

    // stack
    &op_S_ALL, &op_S_POP, &op_S_CLR, &op_S_L,

    // controlflow
    &op_SCRIPT, &op_KILL, &op_SCENE, &op_BREAK, &op_BRK, &op_SYNC,

    // delay
    &op_DEL_CLR,

    // whitewhale
    &op_WW_PRESET, &op_WW_POS, &op_WW_SYNC, &op_WW_START, &op_WW_END,
    &op_WW_PMODE, &op_WW_PATTERN, &op_WW_QPATTERN, &op_WW_MUTE1, &op_WW_MUTE2,
    &op_WW_MUTE3, &op_WW_MUTE4, &op_WW_MUTEA, &op_WW_MUTEB,

    // meadowphysics
    &op_MP_PRESET, &op_MP_RESET, &op_MP_STOP,

    // earthsea
    &op_ES_PRESET, &op_ES_MODE, &op_ES_CLOCK, &op_ES_RESET, &op_ES_PATTERN,
    &op_ES_TRANS, &op_ES_STOP, &op_ES_TRIPLE, &op_ES_MAGIC,

    // orca
    &op_OR_TRK, &op_OR_CLK, &op_OR_DIV, &op_OR_PHASE, &op_OR_RST, &op_OR_WGT,
    &op_OR_MUTE, &op_OR_SCALE, &op_OR_BANK, &op_OR_PRESET, &op_OR_RELOAD,
    &op_OR_ROTS, &op_OR_ROTW, &op_OR_GRST, &op_OR_CVA, &op_OR_CVB,

    // ansible
    &op_KR_PRE, &op_KR_PAT, &op_KR_SCALE, &op_KR_PERIOD, &op_KR_POS,
    &op_KR_L_ST, &op_KR_L_LEN, &op_KR_RES, &op_ME_PRE, &op_ME_RES, &op_ME_STOP,
    &op_ME_SCALE, &op_ME_PERIOD, &op_LV_PRE, &op_LV_RES, &op_LV_POS,
    &op_LV_L_ST, &op_LV_L_LEN, &op_LV_L_DIR, &op_LV_CV, &op_CY_PRE, &op_CY_RES,
    &op_CY_POS, &op_CY_REV, &op_CY_CV, &op_MID_SHIFT, &op_MID_SLEW, &op_ARP_STY,
    &op_ARP_HLD, &op_ARP_RPT, &op_ARP_GT, &op_ARP_DIV, &op_ARP_RES,
    &op_ARP_SHIFT, &op_ARP_SLEW, &op_ARP_FIL, &op_ARP_ROT, &op_ARP_ER,

    // justfriends
    &op_JF_TR, &op_JF_RMODE, &op_JF_RUN, &op_JF_SHIFT, &op_JF_VTR, &op_JF_MODE,
    &op_JF_TICK, &op_JF_VOX, &op_JF_NOTE, &op_JF_GOD, &op_JF_TUNE, &op_JF_QT,

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

    &op_TI_PARAM, &op_TI_PARAM_QT, &op_TI_PARAM_N, &op_TI_PARAM_SCALE,
    &op_TI_PARAM_MAP, &op_TI_IN, &op_TI_IN_QT, &op_TI_IN_N, &op_TI_IN_SCALE,
    &op_TI_IN_MAP, &op_TI_PARAM_CALIB, &op_TI_IN_CALIB, &op_TI_STORE,
    &op_TI_RESET,

    &op_TI_PARAM_INIT, &op_TI_IN_INIT, &op_TI_INIT,

    &op_TI_PRM, &op_TI_PRM_QT, &op_TI_PRM_N, &op_TI_PRM_SCALE, &op_TI_PRM_MAP,
    &op_TI_PRM_INIT
};

/////////////////////////////////////////////////////////////////
// MODS /////////////////////////////////////////////////////////

const tele_mod_t *tele_mods[E_MOD__LENGTH] = {
    // controlflow
    &mod_IF, &mod_ELIF, &mod_ELSE, &mod_L, &mod_W, &mod_EVERY, &mod_SKIP,
    &mod_OTHER, &mod_PROB,

    // delay
    &mod_DEL,

    // stack
    &mod_S

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
