#ifndef _OPS_MATHS_H_
#define _OPS_MATHS_H_

#include "ops/op.h"

extern const tele_op_t op_ADD;
extern const tele_op_t op_SUB;
extern const tele_op_t op_MUL;
extern const tele_op_t op_DIV;
extern const tele_op_t op_MOD;
extern const tele_op_t op_RAND;
extern const tele_op_t op_RND;
extern const tele_op_t op_RRAND;
extern const tele_op_t op_RRND;
extern const tele_op_t op_R;
extern const tele_op_t op_R_MIN;
extern const tele_op_t op_R_MAX;
extern const tele_op_t op_TOSS;
extern const tele_op_t op_MIN;
extern const tele_op_t op_MAX;
extern const tele_op_t op_LIM;
extern const tele_op_t op_WRAP;
extern const tele_op_t op_WRP;
extern const tele_op_t op_QT;
extern const tele_op_t op_AVG;
extern const tele_op_t op_EQ;
extern const tele_op_t op_NE;
extern const tele_op_t op_LT;
extern const tele_op_t op_GT;
extern const tele_op_t op_LTE;
extern const tele_op_t op_GTE;
extern const tele_op_t op_NZ;
extern const tele_op_t op_EZ;
extern const tele_op_t op_RSH;
extern const tele_op_t op_LSH;
extern const tele_op_t op_RROT;
extern const tele_op_t op_LROT;
extern const tele_op_t op_EXP;
extern const tele_op_t op_ABS;
extern const tele_op_t op_SGN;
extern const tele_op_t op_AND;
extern const tele_op_t op_OR;
extern const tele_op_t op_JI;
extern const tele_op_t op_SCALE;
extern const tele_op_t op_SCL;
extern const tele_op_t op_N;
extern const tele_op_t op_V;
extern const tele_op_t op_VV;
extern const tele_op_t op_ER;
extern const tele_op_t op_NR;
extern const tele_op_t op_SN;
extern const tele_op_t op_CN;
extern const tele_op_t op_CSN;
extern const tele_op_t op_BPM;
extern const tele_op_t op_BIT_OR;
extern const tele_op_t op_BIT_AND;
extern const tele_op_t op_BIT_NOT;
extern const tele_op_t op_BIT_XOR;
extern const tele_op_t op_BSET;
extern const tele_op_t op_BGET;
extern const tele_op_t op_BCLR;
extern const tele_op_t op_CHAOS;
extern const tele_op_t op_CHAOS_R;
extern const tele_op_t op_CHAOS_ALG;
extern const tele_op_t op_TIF;  // ternary if

extern const tele_op_t op_XOR;  // XOR alias NE

extern const tele_op_t op_SYM_PLUS;                // + alias ADD
extern const tele_op_t op_SYM_DASH;                // - alias SUB
extern const tele_op_t op_SYM_STAR;                // * alias MUL
extern const tele_op_t op_SYM_FORWARD_SLASH;       // / alias DIV
extern const tele_op_t op_SYM_PERCENTAGE;          // % alias MOD
extern const tele_op_t op_SYM_EQUAL_x2;            // == alias EQ
extern const tele_op_t op_SYM_EXCLAMATION_EQUAL;   // != alias NE
extern const tele_op_t op_SYM_LEFT_ANGLED;         // < alias LT
extern const tele_op_t op_SYM_RIGHT_ANGLED;        // > alias GT
extern const tele_op_t op_SYM_LEFT_ANGLED_EQUAL;   // <= alias LTE
extern const tele_op_t op_SYM_RIGHT_ANGLED_EQUAL;  // >= alias GT
extern const tele_op_t op_SYM_EXCLAMATION;         // ! alias NZ
extern const tele_op_t op_SYM_LEFT_ANGLED_x2;      // << alias LSH
extern const tele_op_t op_SYM_RIGHT_ANGLED_x2;     // >> alias RSH
extern const tele_op_t op_SYM_LEFT_ANGLED_x3;      // <<< alias LROT
extern const tele_op_t op_SYM_RIGHT_ANGLED_x3;     // >>> alias RROT
extern const tele_op_t op_SYM_AMPERSAND_x2;        // && alias AND
extern const tele_op_t op_SYM_PIPE_x2;             // || alias OR
extern const tele_op_t op_SYM_HASH;                // # alias NR

#endif
