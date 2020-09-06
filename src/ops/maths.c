#include "ops/maths.h"

#include <stdlib.h>  // abs
#include "random.h"

#include "chaos.h"
#include "euclidean/euclidean.h"
#include "helpers.h"
#include "table.h"

static void op_ADD_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_SUB_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_MUL_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_DIV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_MOD_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_RAND_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_RRAND_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_R_get(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_R_MIN_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_R_MIN_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_R_MAX_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_R_MAX_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_TOSS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_MIN_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_MAX_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_LIM_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_WRAP_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_QT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_QT_S_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_QT_CS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_QT_B_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_AVG_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_EQ_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_NE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_LT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_GT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_LTE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_GTE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_NZ_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_EZ_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_RSH_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_LSH_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_RROT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_LROT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_EXP_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_ABS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_SGN_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_AND_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_OR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_JI_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_SCALE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_N_get(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_VN_get(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_N_S_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_N_C_get(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_N_CS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_V_get(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_VV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_ER_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_NR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_BPM_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_BIT_OR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_BIT_AND_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_BIT_NOT_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_BIT_XOR_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_BSET_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_BGET_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_BCLR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_BTOG_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_CHAOS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_CHAOS_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_CHAOS_R_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_CHAOS_R_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_CHAOS_ALG_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_CHAOS_ALG_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_TIF_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);

// clang-format off
const tele_op_t op_ADD   = MAKE_GET_OP(ADD     , op_ADD_get     , 2, true);
const tele_op_t op_SUB   = MAKE_GET_OP(SUB     , op_SUB_get     , 2, true);
const tele_op_t op_MUL   = MAKE_GET_OP(MUL     , op_MUL_get     , 2, true);
const tele_op_t op_DIV   = MAKE_GET_OP(DIV     , op_DIV_get     , 2, true);
const tele_op_t op_MOD   = MAKE_GET_OP(MOD     , op_MOD_get     , 2, true);
const tele_op_t op_RAND  = MAKE_GET_OP(RAND    , op_RAND_get    , 1, true);
const tele_op_t op_RND   = MAKE_GET_OP(RND     , op_RAND_get    , 1, true);
const tele_op_t op_RRAND = MAKE_GET_OP(RRAND   , op_RRAND_get   , 2, true);
const tele_op_t op_RRND  = MAKE_GET_OP(RRND    , op_RRAND_get   , 2, true);
const tele_op_t op_R     = MAKE_GET_OP(R       , op_R_get       , 0, true);
const tele_op_t op_R_MIN = MAKE_GET_SET_OP(R.MIN, op_R_MIN_get, op_R_MIN_set, 0, true);
const tele_op_t op_R_MAX = MAKE_GET_SET_OP(R.MAX, op_R_MAX_get, op_R_MAX_set, 0, true);
const tele_op_t op_TOSS  = MAKE_GET_OP(TOSS    , op_TOSS_get    , 0, true);
const tele_op_t op_MIN   = MAKE_GET_OP(MIN     , op_MIN_get     , 2, true);
const tele_op_t op_MAX   = MAKE_GET_OP(MAX     , op_MAX_get     , 2, true);
const tele_op_t op_LIM   = MAKE_GET_OP(LIM     , op_LIM_get     , 3, true);
const tele_op_t op_WRAP  = MAKE_GET_OP(WRAP    , op_WRAP_get    , 3, true);
const tele_op_t op_WRP   = MAKE_GET_OP(WRP     , op_WRAP_get    , 3, true);
const tele_op_t op_QT    = MAKE_GET_OP(QT      , op_QT_get      , 2, true);
const tele_op_t op_QT_S  = MAKE_GET_OP(QT.S    , op_QT_S_get    , 3, true);
const tele_op_t op_QT_CS = MAKE_GET_OP(QT.CS   , op_QT_CS_get   , 5, true);
const tele_op_t op_QT_B  = MAKE_GET_OP(QT.B    , op_QT_B_get    , 3, true);
const tele_op_t op_AVG   = MAKE_GET_OP(AVG     , op_AVG_get     , 2, true);
const tele_op_t op_EQ    = MAKE_GET_OP(EQ      , op_EQ_get      , 2, true);
const tele_op_t op_NE    = MAKE_GET_OP(NE      , op_NE_get      , 2, true);
const tele_op_t op_LT    = MAKE_GET_OP(LT      , op_LT_get      , 2, true);
const tele_op_t op_GT    = MAKE_GET_OP(GT      , op_GT_get      , 2, true);
const tele_op_t op_LTE   = MAKE_GET_OP(LTE     , op_LTE_get     , 2, true);
const tele_op_t op_GTE   = MAKE_GET_OP(GTE     , op_GTE_get     , 2, true);
const tele_op_t op_NZ    = MAKE_GET_OP(NZ      , op_NZ_get      , 1, true);
const tele_op_t op_EZ    = MAKE_GET_OP(EZ      , op_EZ_get      , 1, true);
const tele_op_t op_RSH   = MAKE_GET_OP(RSH     , op_RSH_get     , 2, true);
const tele_op_t op_LSH   = MAKE_GET_OP(LSH     , op_LSH_get     , 2, true);
const tele_op_t op_RROT  = MAKE_GET_OP(RROT    , op_RROT_get    , 2, true);
const tele_op_t op_LROT  = MAKE_GET_OP(LROT    , op_LROT_get    , 2, true);
const tele_op_t op_EXP   = MAKE_GET_OP(EXP     , op_EXP_get     , 1, true);
const tele_op_t op_ABS   = MAKE_GET_OP(ABS     , op_ABS_get     , 1, true);
const tele_op_t op_SGN   = MAKE_GET_OP(SGN     , op_SGN_get     , 1, true);
const tele_op_t op_AND   = MAKE_GET_OP(AND     , op_AND_get     , 2, true);
const tele_op_t op_OR    = MAKE_GET_OP(OR      , op_OR_get      , 2, true);
const tele_op_t op_JI    = MAKE_GET_OP(JI      , op_JI_get      , 2, true);
const tele_op_t op_SCALE = MAKE_GET_OP(SCALE   , op_SCALE_get   , 5, true);
const tele_op_t op_SCL   = MAKE_GET_OP(SCL     , op_SCALE_get   , 5, true);
const tele_op_t op_N     = MAKE_GET_OP(N       , op_N_get       , 1, true);
const tele_op_t op_VN    = MAKE_GET_OP(VN      , op_VN_get      , 1, true);
const tele_op_t op_N_S   = MAKE_GET_OP(N.S      , op_N_S_get    , 3, true);
const tele_op_t op_N_C   = MAKE_GET_OP(N.C      , op_N_C_get    , 3, true);
const tele_op_t op_N_CS  = MAKE_GET_OP(N.CS     , op_N_CS_get   , 4, true);
const tele_op_t op_V     = MAKE_GET_OP(V       , op_V_get       , 1, true);
const tele_op_t op_VV    = MAKE_GET_OP(VV      , op_VV_get      , 1, true);
const tele_op_t op_ER    = MAKE_GET_OP(ER      , op_ER_get      , 3, true);
const tele_op_t op_NR    = MAKE_GET_OP(NR      , op_NR_get      , 4, true);
const tele_op_t op_BPM   = MAKE_GET_OP(BPM     , op_BPM_get     , 1, true);
const tele_op_t op_BIT_OR  = MAKE_GET_OP(|, op_BIT_OR_get  , 2, true);
const tele_op_t op_BIT_AND = MAKE_GET_OP(&, op_BIT_AND_get, 2, true);
const tele_op_t op_BIT_NOT  = MAKE_GET_OP(~, op_BIT_NOT_get  , 1, true);
const tele_op_t op_BIT_XOR = MAKE_GET_OP(^, op_BIT_XOR_get, 2, true);
const tele_op_t op_BSET  = MAKE_GET_OP(BSET    , op_BSET_get    , 2, true);
const tele_op_t op_BGET  = MAKE_GET_OP(BGET    , op_BGET_get    , 2, true);
const tele_op_t op_BCLR  = MAKE_GET_OP(BCLR    , op_BCLR_get    , 2, true);
const tele_op_t op_BTOG  = MAKE_GET_OP(BTOG    , op_BTOG_get    , 2, true);
const tele_op_t op_CHAOS   = MAKE_GET_SET_OP(CHAOS,   op_CHAOS_get,   op_CHAOS_set, 0, true);
const tele_op_t op_CHAOS_R = MAKE_GET_SET_OP(CHAOS.R, op_CHAOS_R_get, op_CHAOS_R_set, 0, true);
const tele_op_t op_CHAOS_ALG = MAKE_GET_SET_OP(CHAOS.ALG, op_CHAOS_ALG_get, op_CHAOS_ALG_set, 0, true);
const tele_op_t op_TIF = MAKE_GET_OP(?, op_TIF_get, 3, true);

const tele_op_t op_XOR   = MAKE_ALIAS_OP(XOR, op_NE_get, NULL, 2, true);

const tele_op_t op_SYM_PLUS               = MAKE_ALIAS_OP(+ ,  op_ADD_get,  NULL, 2, true);
const tele_op_t op_SYM_DASH               = MAKE_ALIAS_OP(- ,  op_SUB_get,  NULL, 2, true);
const tele_op_t op_SYM_STAR               = MAKE_ALIAS_OP(* ,  op_MUL_get,  NULL, 2, true);
const tele_op_t op_SYM_FORWARD_SLASH      = MAKE_ALIAS_OP(/ ,  op_DIV_get,  NULL, 2, true);
const tele_op_t op_SYM_PERCENTAGE         = MAKE_ALIAS_OP(% ,  op_MOD_get,  NULL, 2, true);
const tele_op_t op_SYM_EQUAL_x2           = MAKE_ALIAS_OP(==,  op_EQ_get ,  NULL, 2, true);
const tele_op_t op_SYM_EXCLAMATION_EQUAL  = MAKE_ALIAS_OP(!=,  op_NE_get ,  NULL, 2, true);
const tele_op_t op_SYM_LEFT_ANGLED        = MAKE_ALIAS_OP(< ,  op_LT_get ,  NULL, 2, true);
const tele_op_t op_SYM_RIGHT_ANGLED       = MAKE_ALIAS_OP(> ,  op_GT_get ,  NULL, 2, true);
const tele_op_t op_SYM_LEFT_ANGLED_EQUAL  = MAKE_ALIAS_OP(<=,  op_LTE_get,  NULL, 2, true);
const tele_op_t op_SYM_RIGHT_ANGLED_EQUAL = MAKE_ALIAS_OP(>=,  op_GTE_get,  NULL, 2, true);
const tele_op_t op_SYM_EXCLAMATION        = MAKE_ALIAS_OP(! ,  op_EZ_get ,  NULL, 1, true);
const tele_op_t op_SYM_LEFT_ANGLED_x2     = MAKE_ALIAS_OP(<<,  op_LSH_get,  NULL, 2, true);
const tele_op_t op_SYM_RIGHT_ANGLED_x2    = MAKE_ALIAS_OP(>>,  op_RSH_get,  NULL, 2, true);
const tele_op_t op_SYM_LEFT_ANGLED_x3     = MAKE_ALIAS_OP(<<<, op_LROT_get, NULL, 2, true);
const tele_op_t op_SYM_RIGHT_ANGLED_x3    = MAKE_ALIAS_OP(>>>, op_RROT_get, NULL, 2, true);
const tele_op_t op_SYM_AMPERSAND_x2       = MAKE_ALIAS_OP(&&,  op_AND_get,  NULL, 2, true);
const tele_op_t op_SYM_PIPE_x2            = MAKE_ALIAS_OP(||,  op_OR_get ,  NULL, 2, true);
// clang-format on


static void op_ADD_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, cs_pop(cs) + cs_pop(cs));
}

static void op_SUB_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, cs_pop(cs) - cs_pop(cs));
}

static void op_MUL_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int32_t r = cs_pop(cs);
    r *= cs_pop(cs);
    if (r > INT16_MAX) r = INT16_MAX;
    if (r < INT16_MIN) r = INT16_MIN;
    cs_push(cs, (int16_t)r);
}

static void op_DIV_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t out = b != 0 ? a / b : 0;
    cs_push(cs, out);
}

static void op_MOD_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t out = b != 0 ? a % b : 0;
    cs_push(cs, out);
}

static void op_RAND_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    random_state_t *r = &ss->rand_states.s.rand.rand;

    if (a < 0)
        cs_push(cs, -(random_next(r) % (1 - a)));
    else if (a == 32767)
        cs_push(cs, random_next(r));
    else
        cs_push(cs, random_next(r) % (a + 1));
}

static int16_t push_random(int16_t a, int16_t b, scene_state_t *ss) {
    int16_t min, max;
    random_state_t *r = &ss->rand_states.s.rand.rand;

    if (a < b) {
        min = a;
        max = b;
    }
    else {
        min = b;
        max = a;
    }
    int32_t range = max - min + 1;
    if (range == 0 || min == max) return min;

    int32_t rrand = (int32_t)random_next(r);
    rrand = rrand % range + min;
    return rrand;
}

static void op_RRAND_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a, b;
    a = cs_pop(cs);
    b = cs_pop(cs);
    cs_push(cs, push_random(a, b, ss));
}


static void op_R_get(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, push_random(ss->variables.r_min, ss->variables.r_max, ss));
}

static void op_R_MIN_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->variables.r_min);
}

static void op_R_MIN_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    ss->variables.r_min = cs_pop(cs);
}

static void op_R_MAX_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->variables.r_max);
}

static void op_R_MAX_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    ss->variables.r_max = cs_pop(cs);
}

static void op_TOSS_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    random_state_t *r = &ss->rand_states.s.toss.rand;
    cs_push(cs, random_next(r) & 1);
}

static void op_MIN_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a, b;
    a = cs_pop(cs);
    b = cs_pop(cs);
    if (b > a)
        cs_push(cs, a);
    else
        cs_push(cs, b);
}

static void op_MAX_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a, b;
    a = cs_pop(cs);
    b = cs_pop(cs);
    if (a > b)
        cs_push(cs, a);
    else
        cs_push(cs, b);
}

static void op_LIM_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a, b, i;
    i = cs_pop(cs);
    a = cs_pop(cs);
    b = cs_pop(cs);
    if (i < a)
        cs_push(cs, a);
    else if (i > b)
        cs_push(cs, b);
    else
        cs_push(cs, i);
}

static void op_WRAP_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a, b, i, c;
    i = cs_pop(cs);
    a = cs_pop(cs);
    b = cs_pop(cs);
    if (a < b) {
        c = b - a + 1;
        while (i >= b) i -= c;
        while (i < a) i += c;
    }
    else {
        c = a - b + 1;
        while (i >= a) i -= c;
        while (i < b) i += c;
    }
    cs_push(cs, i);
}

static void op_QT_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    // this rounds negative numbers rather than quantize (choose closer)
    int16_t a, b, c, d, e;
    b = cs_pop(cs);
    a = cs_pop(cs);

    if (a == 0) {
        cs_push(cs, 0);
        return;
    }

    c = b / a;
    d = c * a;
    e = (c + 1) * a;

    if (abs(b - d) < abs(b - e))
        cs_push(cs, d);
    else
        cs_push(cs, e);
}

static void op_QT_S_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
	
	int16_t note_in = cs_pop(cs);
    int16_t root = cs_pop(cs);
    int16_t scale = cs_pop(cs) % 9;
    if (scale < 0) scale = 9 + scale;
	
	note_in = note_in - root;	
	int16_t octave = (note_in >= 0) ? (note_in / 12) : ((note_in + 1) / 12 - 1);

	if (note_in >= 0) {
		note_in = note_in % 12;
	}
	else {
		if (note_in % 12 == 0) {
			note_in = 0;
		}
		else {
			note_in = 12 + (note_in % 12);
		}
	}

	int16_t note_out = 0;
	
	for (uint8_t i = 0; i < 7; i++) {
		if (note_in >= table_n_s[scale][i]) note_out = table_n_s[scale][i];
	}
	
	cs_push(cs, normalise_value(-127, 127, 0, 12 * octave + note_out + root));
}

static void op_QT_CS_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
	
	int16_t note_in = cs_pop(cs);
    int16_t root = cs_pop(cs);
    int16_t scale = cs_pop(cs) % 9;
    if (scale < 0) scale = 9 + scale;
	
    int16_t degree = cs_pop(cs) - 1;
	int16_t degree_octave = (degree >= 0) ? (degree / 7) : ((degree + 1) / 7 - 1);
	if (degree >= 0) {
		degree = degree % 7;
	}
	else {
		if (degree % 7 == 0) {
			degree = 0;
		}
		else {
			degree = 7 + (degree % 7);
		}
	}
	
	int16_t voices = normalise_value(1, 7, 0, cs_pop(cs));
	
	note_in = note_in - root;	
	int16_t octave = (note_in >= 0) ? (note_in / 12) : ((note_in + 1) / 12 - 1);

	if (note_in >= 0) {
		note_in = note_in % 12;
	}
	else {
		if (note_in % 12 == 0) {
			note_in = 0;
		}
		else {
			note_in = 12 + (note_in % 12);
		}
	}

	bool quant = false;
	int16_t max_n_s_val = 0;
	int16_t n_s_val;
	int16_t note_out = 0;
	int16_t dix;
	
	for (int8_t i = 0; i < voices; i++) {
		dix = (2 * i + degree) % 7;
		n_s_val = table_n_s[scale][dix];
		if (n_s_val > max_n_s_val) max_n_s_val = n_s_val;
		if ((note_in >= n_s_val) && (note_out <= n_s_val)){
			note_out = n_s_val;
			quant = true;
		}
	}
	
	if (!quant) note_out = max_n_s_val - 12;
	
	cs_push(cs, normalise_value(-127, 127, 0, 12 * (octave + degree_octave) + note_out + root));
}

static void op_QT_B_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
	
	int16_t note_in = cs_pop(cs);
    int16_t root = cs_pop(cs);
    int16_t mask = cs_pop(cs);
	
	note_in = note_in - root;
	int16_t octave = (note_in >= 0) ? (note_in / 12) : ((note_in + 1) / 12 - 1);

	if (note_in >= 0) {
		note_in = note_in % 12;
	}
	else {
		if (note_in % 12 == 0) {
			note_in = 0;
		}
		else {
			note_in = 12 + (note_in % 12);
		}
	}

	bool quant = false;
	int16_t b_val = 0;
	int16_t note_out = 0;
	
	for (uint8_t i=0; i<=11; i++) {
		if ((mask >> i) & 1) {
			b_val = i;
			if	(note_in >= b_val) {
				note_out = b_val;
				quant = true;
			}
		}
	}
	
	if (!quant) note_out = b_val - 12;
	note_out = 12 * octave + note_out + root;
	
	cs_push(cs, normalise_value(-127, 127, 0, note_out));
}

static void op_AVG_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int32_t ret = (((int32_t)cs_pop(cs) * 2) + ((int32_t)cs_pop(cs) * 2)) / 2;
    if (ret % 2) ret += 1;
    cs_push(cs, (int16_t)(ret / 2));
}

static void op_EQ_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, cs_pop(cs) == cs_pop(cs));
}

static void op_NE_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, cs_pop(cs) != cs_pop(cs));
}

static void op_LT_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, cs_pop(cs) < cs_pop(cs));
}

static void op_GT_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, cs_pop(cs) > cs_pop(cs));
}

static void op_LTE_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, cs_pop(cs) <= cs_pop(cs));
}

static void op_GTE_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, cs_pop(cs) >= cs_pop(cs));
}

static void op_NZ_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, cs_pop(cs) != 0);
}

static void op_EZ_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, cs_pop(cs) == 0);
}

static void op_RSH_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t x = cs_pop(cs);
    int16_t n = cs_pop(cs);
    cs_push(cs, (n > 0) ? (x >> n) : (x << -n));
}

static void op_LSH_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t x = cs_pop(cs);
    int16_t n = cs_pop(cs);
    cs_push(cs, (n > 0) ? (x << n) : (x >> -n));
}

static uint16_t rrot(uint16_t x, uint8_t n) {
    return (x >> n) | (x << (16 - n));
}

static uint16_t lrot(uint16_t x, uint8_t n) {
    return (x << n) | (x >> (16 - n));
}

typedef union { int16_t si; uint16_t ui; } bits16;

static void op_RROT_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    bits16 u;
    u.si = cs_pop(cs);
    int16_t n = cs_pop(cs) % 16;
    u.ui = (n > 0) ? rrot(u.ui, n) : lrot(u.ui, -n);
    cs_push(cs, u.si);
}

static void op_LROT_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    bits16 u;
    u.si = cs_pop(cs);
    int16_t n = cs_pop(cs) % 16;
    u.ui = (n > 0) ? lrot(u.ui, n) : rrot(u.ui, -n);
    cs_push(cs, u.si);
}

static void op_EXP_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    if (a > 16383)
        a = 16383;
    else if (a < -16383)
        a = -16383;

    a = a >> 6;

    if (a < 0) {
        a = -a;
        cs_push(cs, -table_exp[a]);
    }
    else
        cs_push(cs, table_exp[a]);
}

static void op_ABS_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);

    if (a < 0)
        cs_push(cs, -a);
    else
        cs_push(cs, a);
}

static void op_SGN_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    cs_push(cs, (a > 0) ? 1 : ((a < 0) ? -1 : 0));
}

static void op_AND_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    cs_push(cs, (a > 0) && (b > 0));
}

static void op_OR_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    cs_push(cs, (a > 0) || (b > 0));
}

static void op_JI_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    const uint8_t prime[6] = { 2, 3, 5, 7, 11, 13 };
    const int16_t ji_const[6] = { 6554, 10388, 15218, 18399, 22673, 24253 };
    int32_t result = 0;
    int16_t n = abs(cs_pop(cs));
    int16_t d = abs(cs_pop(cs));

    /* code for generation of ji_const

     int16_t ji_find_prime_constant( uint16_t prime ) {
        float r = 1638.0 * logf( (float)prime ) / log( 2.0 );
        r *= 4.0;                       // this corresponds to the inverse of
     the bitshift applied at rounding & scaling
        return( (int16_t)( r + 0.5 ) );
     }
     */

    if (n == 0 || d == 0) {  // early return if zeroes
        cs_push(cs, 0);
        return;
    }

    for (uint8_t p = 0; p <= 6; p++) {  // find num factors
        if (n == 1) { break; }          // succeed if all primes found
        if (p == 6) {                   // failed to find solution
            cs_push(cs, 0);
            return;
        }
        int32_t quotient = n / prime[p];
        while (n == quotient * prime[p]) {  // match
            result += ji_const[p];          // ADD for numerator
            n = quotient;
            quotient = n / prime[p];
        }
    }
    for (uint8_t p = 0; p <= 6; p++) {  // denom
        if (d == 1) { break; }          // succeed if all primes found
        if (p == 6) {                   // failed to find solution
            cs_push(cs, 0);
            return;
        }
        int32_t quotient = d / prime[p];
        while (d == quotient * prime[p]) {  // match
            result -= ji_const[p];          // SUBTRACT for denominator
            d = quotient;
            quotient = d / prime[p];
        }
    }
    result = (result + 2) >> 2;  // round & scale
    cs_push(cs, result);
}

static void op_SCALE_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int32_t a, b, x, y, i;
    a = cs_pop(cs);
    b = cs_pop(cs);
    x = cs_pop(cs);
    y = cs_pop(cs);
    i = cs_pop(cs);

    if ((b - a) == 0) {
        cs_push(cs, 0);
        return;
    }

    int32_t result = (i - a) * (y - x) * 2 / (b - a);
    result = result / 2 + (result & 1);  // rounding

    cs_push(cs, result + x);
}

static void op_N_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);

    if (a < 0) {
        if (a < -127) a = -127;
        a = -a;
        cs_push(cs, -table_n[a]);
    }
    else {
        if (a > 127) a = 127;
        cs_push(cs, table_n[a]);
    }
}

static void op_VN_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v_in = cs_pop(cs);
	int16_t n_out = 0;

    if (v_in < 0) {
		v_in = -v_in;
		for (int16_t i = 127; i >= 0; i--) {
			if (v_in <= table_n[i]) n_out = i;
		}
        cs_push(cs, -n_out);
    }
    else {
		for (int16_t i = 0; i <= 127; i++) {
			if (v_in >= table_n[i]) n_out = i;
		}
        cs_push(cs, n_out);
    }
}

static void op_V_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    if (a > 10)
        a = 10;
    else if (a < -10)
        a = -10;

    if (a < 0) {
        a = -a;
        cs_push(cs, -table_v[a]);
    }
    else
        cs_push(cs, table_v[a]);
}

static void op_VV_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t negative = 1;
    int16_t a = cs_pop(cs);
    if (a < 0) {
        negative = -1;
        a = -a;
    }
    if (a > 1000) a = 1000;
    cs_push(cs, negative * (table_v[a / 100] + table_vv[a % 100]));
}

static void op_ER_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t fill = cs_pop(cs);
    int16_t len = cs_pop(cs);
    int16_t step = cs_pop(cs);
    cs_push(cs, euclidean(fill, len, step));
}

static void op_NR_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t prime = cs_pop(cs) % 32;
    if (prime < 0)
        prime = 32 + prime;
    uint16_t rhythm = (uint16_t)table_nr[prime];
    int16_t mask = cs_pop(cs) % 4;
    if (mask < 0)
        mask = 4 + mask;
    int16_t factor = cs_pop(cs) % 17;
    if (factor < 0)
        factor = 17 + factor;
    int16_t step = cs_pop(cs) % 16;
    if (step < 0)
        step = 16 + step;
    if (mask == 1)
        rhythm = rhythm & 0x0F0F;
    else if (mask == 2)
        rhythm = rhythm & 0xF003;
    else if (mask == 3)
        rhythm = rhythm & 0x1F0;
    uint32_t modified = rhythm * factor;
    uint16_t final = (uint16_t)((modified & 0xFFFF) | (modified >> 16));
    int16_t bit_status = (final >> (15 - step)) & 1;
    cs_push(cs, bit_status);
}

static void op_N_S_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t root = cs_pop(cs);
    int16_t scale = cs_pop(cs) % 9;
    if (scale < 0) {
        scale = 9 + scale;
    }
    int16_t degree = (cs_pop(cs) - 1) % 7;
    if (degree < 0) {
        degree = 7 + degree;
    }
    int16_t transpose = table_n_s[scale][degree];
    if (root < 0) {
        if (root < -127) root = -127;
        root = -root;
        cs_push(cs, -table_n[root + transpose]);
    }
    else {
        if (root > 127) root = 127;
        cs_push(cs, table_n[root + transpose]);
    }
}

static void op_N_C_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t root = cs_pop(cs);
    int16_t chord = cs_pop(cs) % 13;
    if (chord < 0) {
        chord = 13 + chord;
    }
    int16_t component = cs_pop(cs) % 4;
    if (component < 0) {
        component = 4 + component;
    }
    int16_t transpose = table_n_c[chord][component];
    if (root < 0) {
        if (root < -127) root = -127;
        root = -root;
        cs_push(cs, -table_n[root + transpose]);
    }
    else {
        if (root > 127) root = 127;
        cs_push(cs, table_n[root + transpose]);
    }
}

static void op_N_CS_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t root = cs_pop(cs);
    int16_t scale = cs_pop(cs) % 9;
    if (scale < 0) {
        scale = 9 + scale;
    }
    int16_t scl_deg = (cs_pop(cs) - 1) % 7;
    if (scl_deg < 0) {
        scl_deg = 7 + scl_deg;
    }
    int16_t scl_trans = table_n_s[scale][scl_deg];
    int16_t ch_deg = cs_pop(cs) % 4;
    if (ch_deg < 0) {
        ch_deg = 4 + ch_deg;
    }
    int16_t ch_trans = table_n_c[table_n_cs[scale][scl_deg]][ch_deg];
    if (root < 0) {
        if (root < -127) root = -127;
        root = -root;
        cs_push(cs, -table_n[root + scl_trans + ch_trans]);
    }
    else {
        if (root > 127) root = 127;
        cs_push(cs, table_n[root + scl_trans + ch_trans]);
    }
}

static void op_BPM_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint32_t ret;
    if (a < 2) a = 2;
    if (a > 1000) a = 1000;
    ret = ((((uint32_t)(1 << 31)) / ((a << 20) / 60)) * 1000) >> 11;
    cs_push(cs, (int16_t)ret);
}

static void op_BIT_OR_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    cs_push(cs, a | b);
}

static void op_BIT_AND_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    cs_push(cs, a & b);
}

static void op_BIT_NOT_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    cs_push(cs, ~a);
}

static void op_BIT_XOR_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    cs_push(cs, a ^ b);
}

static void op_BSET_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v = cs_pop(cs);
    int16_t b = cs_pop(cs);
    cs_push(cs, v | (1 << b));
}

static void op_BGET_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v = cs_pop(cs);
    int16_t b = cs_pop(cs);
    cs_push(cs, (v >> b) & 1);
}

static void op_BCLR_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v = cs_pop(cs);
    int16_t b = cs_pop(cs);
    cs_push(cs, v & ~(1 << b));
}

static void op_BTOG_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t v = cs_pop(cs);
    int16_t b = cs_pop(cs);
    if ((v >> b) & 1) cs_push(cs, v & ~(1 << b));
	else cs_push(cs, v | (1 << b));
}

static void op_CHAOS_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, chaos_get_val());
}

static void op_CHAOS_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    chaos_set_val(cs_pop(cs));
}

static void op_CHAOS_R_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, chaos_get_r());
}

static void op_CHAOS_R_set(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    chaos_set_r(cs_pop(cs));
}

static void op_CHAOS_ALG_get(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, chaos_get_alg());
}

static void op_CHAOS_ALG_set(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    chaos_set_alg(cs_pop(cs));
}

static void op_TIF_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 condition = cs_pop(cs);
    s16 a = cs_pop(cs);
    s16 b = cs_pop(cs);
    cs_push(cs, condition ? a : b);
}
