#include "ops/ansible.h"

#include "helpers.h"
#include "ii.h"
#include "teletype_io.h"


static void op_ANS_G_LED_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_ANS_G_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_ANS_G_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_ANS_G_P_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_ANS_A_LED_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_ANS_A_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_ANS_APP_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_ANS_APP_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);

static void op_KR_PRE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KR_PRE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KR_PAT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KR_PAT_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KR_SCALE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_KR_SCALE_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_KR_PERIOD_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_KR_PERIOD_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_KR_POS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KR_POS_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KR_L_ST_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_KR_L_ST_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_KR_L_LEN_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_KR_L_LEN_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_KR_RES_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KR_CV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_KR_MUTE_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_KR_MUTE_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_KR_TMUTE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_KR_CLK_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KR_PG_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_KR_PG_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_KR_CUE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KR_CUE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KR_DIR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KR_DIR_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KR_DUR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_ME_PRE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_ME_PRE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_ME_RES_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_ME_STOP_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_ME_SCALE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_ME_SCALE_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_ME_PERIOD_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_ME_PERIOD_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_ME_CV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);

static void op_LV_PRE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_LV_PRE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_LV_RES_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_LV_POS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_LV_POS_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_LV_L_ST_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_LV_L_ST_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_LV_L_LEN_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_LV_L_LEN_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_LV_L_DIR_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_LV_L_DIR_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_LV_CV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);

static void op_CY_PRE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_CY_PRE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_CY_RES_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_CY_POS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_CY_POS_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_CY_REV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_CY_CV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);

static void op_MID_SHIFT_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_MID_SLEW_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);

static void op_ARP_STY_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_ARP_HLD_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_ARP_RPT_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_ARP_GT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_ARP_DIV_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_ARP_RES_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_ARP_SHIFT_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_ARP_SLEW_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_ARP_FIL_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_ARP_ROT_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_ARP_ER_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);


// clang-format off
const tele_op_t op_ANS_G_LED   = MAKE_GET_OP(    ANS.G.LED  , op_ANS_G_LED_get                        , 2, true);
const tele_op_t op_ANS_G       = MAKE_GET_SET_OP(ANS.G      , op_ANS_G_get       , op_ANS_G_set       , 2, true);
const tele_op_t op_ANS_G_P     = MAKE_GET_OP(    ANS.G.P    , op_ANS_G_P_get                          , 2, false);
const tele_op_t op_ANS_A_LED   = MAKE_GET_OP(    ANS.A.LED  , op_ANS_A_LED_get                        , 2, true);
const tele_op_t op_ANS_A       = MAKE_GET_OP(    ANS.A      , op_ANS_A_get                            , 2, false);
const tele_op_t op_ANS_APP     = MAKE_GET_SET_OP(ANS.APP    , op_ANS_APP_get     , op_ANS_APP_set     , 0, true);

const tele_op_t op_KR_PRE      = MAKE_GET_SET_OP(KR.PRE     , op_KR_PRE_get      , op_KR_PRE_set      , 0, true);
const tele_op_t op_KR_PAT      = MAKE_GET_SET_OP(KR.PAT     , op_KR_PAT_get      , op_KR_PAT_set      , 0, true);
const tele_op_t op_KR_SCALE    = MAKE_GET_SET_OP(KR.SCALE   , op_KR_SCALE_get    , op_KR_SCALE_set    , 0, true);
const tele_op_t op_KR_PERIOD   = MAKE_GET_SET_OP(KR.PERIOD  , op_KR_PERIOD_get   , op_KR_PERIOD_set   , 0, true);
const tele_op_t op_KR_POS      = MAKE_GET_SET_OP(KR.POS     , op_KR_POS_get      , op_KR_POS_set      , 2, true);
const tele_op_t op_KR_L_ST     = MAKE_GET_SET_OP(KR.L.ST    , op_KR_L_ST_get     , op_KR_L_ST_set     , 2, true);
const tele_op_t op_KR_L_LEN    = MAKE_GET_SET_OP(KR.L.LEN   , op_KR_L_LEN_get    , op_KR_L_LEN_set    , 2, true);
const tele_op_t op_KR_RES      = MAKE_GET_OP    (KR.RES     , op_KR_RES_get                           , 2, false);
const tele_op_t op_KR_CV       = MAKE_GET_OP    (KR.CV      , op_KR_CV_get                            , 1, true);
const tele_op_t op_KR_MUTE     = MAKE_GET_SET_OP(KR.MUTE    , op_KR_MUTE_get     , op_KR_MUTE_set     , 1, true);
const tele_op_t op_KR_TMUTE    = MAKE_GET_OP    (KR.TMUTE   , op_KR_TMUTE_get                         , 1, false);
const tele_op_t op_KR_CLK      = MAKE_GET_OP    (KR.CLK     , op_KR_CLK_get                           , 1, false);
const tele_op_t op_KR_PG       = MAKE_GET_SET_OP(KR.PG      , op_KR_PG_get       , op_KR_PG_set       , 0, true);
const tele_op_t op_KR_CUE      = MAKE_GET_SET_OP(KR.CUE     , op_KR_CUE_get      , op_KR_CUE_set      , 0, true);
const tele_op_t op_KR_DIR      = MAKE_GET_SET_OP(KR.DIR     , op_KR_DIR_get     , op_KR_DIR_set       , 1, true);
const tele_op_t op_KR_DUR      = MAKE_GET_OP    (KR.DUR     , op_KR_DUR_get                           , 1, true);

const tele_op_t op_ME_PRE      = MAKE_GET_SET_OP(ME.PRE     , op_ME_PRE_get      , op_ME_PRE_set      , 0, true);
const tele_op_t op_ME_RES      = MAKE_GET_OP    (ME.RES     , op_ME_RES_get                           , 1, false);
const tele_op_t op_ME_STOP     = MAKE_GET_OP    (ME.STOP    , op_ME_STOP_get                          , 1, false);
const tele_op_t op_ME_SCALE    = MAKE_GET_SET_OP(ME.SCALE   , op_ME_SCALE_get    , op_ME_SCALE_set    , 0, true);
const tele_op_t op_ME_PERIOD   = MAKE_GET_SET_OP(ME.PERIOD  , op_ME_PERIOD_get   , op_ME_PERIOD_set   , 0, true);
const tele_op_t op_ME_CV       = MAKE_GET_OP    (ME.CV      , op_ME_CV_get                            , 1, true);

const tele_op_t op_LV_PRE      = MAKE_GET_SET_OP(LV.PRE     , op_LV_PRE_get      , op_LV_PRE_set      , 0, true);
const tele_op_t op_LV_RES      = MAKE_GET_OP    (LV.RES     , op_LV_RES_get                           , 1, false);
const tele_op_t op_LV_POS      = MAKE_GET_SET_OP(LV.POS     , op_LV_POS_get      , op_LV_POS_set      , 0, true);
const tele_op_t op_LV_L_ST     = MAKE_GET_SET_OP(LV.L.ST    , op_LV_L_ST_get     , op_LV_L_ST_set     , 0, true);
const tele_op_t op_LV_L_LEN    = MAKE_GET_SET_OP(LV.L.LEN   , op_LV_L_LEN_get    , op_LV_L_LEN_set    , 0, true);
const tele_op_t op_LV_L_DIR    = MAKE_GET_SET_OP(LV.L.DIR   , op_LV_L_DIR_get    , op_LV_L_DIR_set    , 0, true);
const tele_op_t op_LV_CV       = MAKE_GET_OP    (LV.CV      , op_LV_CV_get                            , 1, true);

const tele_op_t op_CY_PRE      = MAKE_GET_SET_OP(CY.PRE     , op_CY_PRE_get      , op_CY_PRE_set      , 0, true);
const tele_op_t op_CY_RES      = MAKE_GET_OP    (CY.RES     , op_CY_RES_get                           , 1, false);
const tele_op_t op_CY_POS      = MAKE_GET_SET_OP(CY.POS     , op_CY_POS_get      , op_CY_POS_set      , 1, true);
const tele_op_t op_CY_REV      = MAKE_GET_OP    (CY.REV     , op_CY_REV_get                           , 1, false);
const tele_op_t op_CY_CV       = MAKE_GET_OP    (CY.CV      , op_CY_CV_get                            , 1, true);

const tele_op_t op_MID_SHIFT   = MAKE_GET_OP(MID.SHIFT      , op_MID_SHIFT_get                        , 1, false);
const tele_op_t op_MID_SLEW    = MAKE_GET_OP(MID.SLEW       , op_MID_SLEW_get                         , 1, false);

const tele_op_t op_ARP_STY     = MAKE_GET_OP(ARP.STY        , op_ARP_STY_get                          , 1, false);
const tele_op_t op_ARP_HLD     = MAKE_GET_OP(ARP.HLD        , op_ARP_HLD_get                          , 1, false);
const tele_op_t op_ARP_RPT     = MAKE_GET_OP(ARP.RPT        , op_ARP_RPT_get                          , 3, false);
const tele_op_t op_ARP_GT      = MAKE_GET_OP(ARP.GT         , op_ARP_GT_get                           , 2, false);
const tele_op_t op_ARP_DIV     = MAKE_GET_OP(ARP.DIV        , op_ARP_DIV_get                          , 2, false);
const tele_op_t op_ARP_RES     = MAKE_GET_OP(ARP.RES        , op_ARP_RES_get                          , 1, false);
const tele_op_t op_ARP_SHIFT   = MAKE_GET_OP(ARP.SHIFT      , op_ARP_SHIFT_get                        , 2, false);
const tele_op_t op_ARP_SLEW    = MAKE_GET_OP(ARP.SLEW       , op_ARP_SLEW_get                         , 2, false);
const tele_op_t op_ARP_FIL     = MAKE_GET_OP(ARP.FIL        , op_ARP_FIL_get                          , 2, false);
const tele_op_t op_ARP_ROT     = MAKE_GET_OP(ARP.ROT        , op_ARP_ROT_get                          , 2, false);
const tele_op_t op_ARP_ER      = MAKE_GET_OP(ARP.ER         , op_ARP_ER_get                           , 4, false);
// clang-format on


static void op_ANS_G_LED_get(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t x = cs_pop(cs);
    int16_t y = cs_pop(cs);

    uint8_t d[] = { II_GRID_LED | II_GET, x, y };
    tele_ii_tx(II_KR_ADDR, d, 3);
    tele_ii_tx(II_MP_ADDR, d, 3);
    tele_ii_tx(ES, d, 3);

    d[0] = 0;
    tele_ii_rx(II_KR_ADDR, d, 1);
    tele_ii_rx(II_MP_ADDR, d, 1);
    tele_ii_rx(ES, d, 1);
    cs_push(cs, d[0]);
}

static void op_ANS_G_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs) {
    int16_t x = cs_pop(cs);
    int16_t y = cs_pop(cs);

    uint8_t d[] = { II_GRID_KEY | II_GET, x, y };
    tele_ii_tx(II_KR_ADDR, d, 4);
    tele_ii_tx(II_MP_ADDR, d, 4);
    tele_ii_tx(ES, d, 4);

    d[0] = 0;
    tele_ii_rx(II_KR_ADDR, d, 1);
    tele_ii_rx(II_MP_ADDR, d, 1);
    tele_ii_rx(ES, d, 1);
    cs_push(cs, d[0]);
}

static void op_ANS_G_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t x = cs_pop(cs);
    int16_t y = cs_pop(cs);
    int16_t z = cs_pop(cs);

    uint8_t d[] = { II_GRID_KEY, x, y, z };
    tele_ii_tx(II_KR_ADDR, d, 4);
    tele_ii_tx(II_MP_ADDR, d, 4);
    tele_ii_tx(ES, d, 4);
}

static void op_ANS_G_P_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t x = cs_pop(cs);
    int16_t y = cs_pop(cs);

    uint8_t d[] = { II_GRID_KEY, x, y, 1 };
    tele_ii_tx(II_KR_ADDR, d, 4);
    tele_ii_tx(II_MP_ADDR, d, 4);
    tele_ii_tx(ES, d, 4);
    d[3] = 0;
    tele_ii_tx(II_KR_ADDR, d, 4);
    tele_ii_tx(II_MP_ADDR, d, 4);
    tele_ii_tx(ES, d, 4);
}

static void op_ANS_A_LED_get(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t n = cs_pop(cs);
    int16_t i = cs_pop(cs);

    uint8_t d[] = { II_ARC_LED | II_GET, n, i };
    tele_ii_tx(II_LV_ADDR, d, 3);
    tele_ii_tx(II_CY_ADDR, d, 3);
    d[0] = 0;
    tele_ii_rx(II_LV_ADDR, d, 1);
    tele_ii_rx(II_CY_ADDR, d, 1);
    cs_push(cs, d[0]);
}

static void op_ANS_A_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t n = cs_pop(cs);
    int16_t delta = cs_pop(cs);

    uint8_t d[] = { II_ARC_ENC, n, delta };
    tele_ii_tx(II_LV_ADDR, d, 3);
    tele_ii_tx(II_CY_ADDR, d, 3);
}

static void op_ANS_APP_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { II_ANSIBLE_APP | II_GET };
    tele_ii_tx(II_ANSIBLE_ADDR, d, 1);
    tele_ii_tx(II_LV_ADDR, d, 1);
    tele_ii_tx(II_CY_ADDR, d, 1);
    tele_ii_tx(II_MP_ADDR, d, 1);
    tele_ii_tx(II_KR_ADDR, d, 1);
    tele_ii_tx(II_MID_ADDR, d, 1);
    tele_ii_tx(II_ARP_ADDR, d, 1);
    tele_ii_tx(ES, d, 1);

    d[0] = 0;
    tele_ii_rx(II_ANSIBLE_ADDR, d, 1);
    tele_ii_rx(II_LV_ADDR, d, 1);
    tele_ii_rx(II_CY_ADDR, d, 1);
    tele_ii_rx(II_KR_ADDR, d, 1);
    tele_ii_rx(II_MP_ADDR, d, 1);
    tele_ii_rx(II_MID_ADDR, d, 1);
    tele_ii_rx(II_ARP_ADDR, d, 1);
    tele_ii_rx(ES, d, 1);
    cs_push(cs, d[0]);
}

static void op_ANS_APP_set(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t n = cs_pop(cs);

    uint8_t d[] = { II_ANSIBLE_APP, n };
    tele_ii_tx(II_ANSIBLE_ADDR, d, 2);
    tele_ii_tx(II_LV_ADDR, d, 2);
    tele_ii_tx(II_CY_ADDR, d, 2);
    tele_ii_tx(II_KR_ADDR, d, 2);
    tele_ii_tx(II_MP_ADDR, d, 2);
    tele_ii_tx(II_MID_ADDR, d, 2);
    tele_ii_tx(II_ARP_ADDR, d, 2);
    tele_ii_tx(ES, d, 2);
}

static void op_KR_PRE_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_KR_PRESET, a };
    tele_ii_tx(II_KR_ADDR, d, 2);
}

static void op_KR_PRE_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { II_KR_PRESET | II_GET };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_PAT_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_KR_PATTERN, a };
    tele_ii_tx(II_KR_ADDR, d, 2);
}

static void op_KR_PAT_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { II_KR_PATTERN | II_GET };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_SCALE_set(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_KR_SCALE, a };
    tele_ii_tx(II_KR_ADDR, d, 2);
}

static void op_KR_SCALE_get(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { II_KR_SCALE | II_GET };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_PERIOD_set(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_KR_PERIOD, a >> 8, a & 0xff };
    tele_ii_tx(II_KR_ADDR, d, 3);
}

static void op_KR_PERIOD_get(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { II_KR_PERIOD | II_GET, 0 };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    d[1] = 0;
    tele_ii_rx(addr, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}

static void op_KR_POS_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    uint8_t d[] = { II_KR_POS, a, b, c };
    tele_ii_tx(II_KR_ADDR, d, 4);
}

static void op_KR_POS_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_KR_POS | II_GET, a, b };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx(addr, d, 3);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_L_ST_set(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    uint8_t d[] = { II_KR_LOOP_ST, a, b, c };
    tele_ii_tx(II_KR_ADDR, d, 4);
}

static void op_KR_L_ST_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_KR_LOOP_ST | II_GET, a, b };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx(addr, d, 3);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_L_LEN_set(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    uint8_t d[] = { II_KR_LOOP_LEN, a, b, c };
    tele_ii_tx(II_KR_ADDR, d, 4);
}

static void op_KR_L_LEN_get(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_KR_LOOP_LEN | II_GET, a, b };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx(addr, d, 3);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_RES_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_KR_RESET, a, b };
    tele_ii_tx(II_KR_ADDR, d, 3);
}

static void op_KR_CV_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    uint8_t d[] = { II_KR_CV | II_GET, a & 0x3 };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx(addr, d, 2);
    d[0] = 0;
    d[1] = 0;
    tele_ii_rx(addr, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}

static void op_KR_MUTE_set(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_KR_MUTE, a, b };
    tele_ii_tx(II_KR_ADDR, d, 3);
}

static void op_KR_MUTE_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_KR_MUTE | II_GET, a };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx(addr, d, 2);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_TMUTE_get(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_KR_TMUTE, a };
    tele_ii_tx(II_KR_ADDR, d, 2);
}

static void op_KR_CLK_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_KR_CLK, a };
    tele_ii_tx(II_KR_ADDR, d, 2);
}


static void op_KR_PG_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { II_KR_PAGE | II_GET };
    tele_ii_tx(II_KR_ADDR, d, 1);

    d[0] = 0;
    tele_ii_rx(II_KR_ADDR, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_PG_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t n = cs_pop(cs);

    uint8_t d[] = { II_KR_PAGE, n };
    tele_ii_tx(II_KR_ADDR, d, 2);
}

static void op_KR_CUE_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { II_KR_CUE | II_GET };
    tele_ii_tx(II_KR_ADDR, d, 1);

    d[0] = 0;
    tele_ii_rx(II_KR_ADDR, d, 1);
    cs_push(cs, (int8_t)d[0]);
}

static void op_KR_CUE_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t pat = cs_pop(cs);

    uint8_t d[] = { II_KR_CUE, pat };
    tele_ii_tx(II_KR_ADDR, d, 2);
}

static void op_KR_DIR_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t n = cs_pop(cs);
    uint8_t d[] = { II_KR_DIR | II_GET, n };
    tele_ii_tx(II_KR_ADDR, d, 2);

    d[0] = 0;
    tele_ii_rx(II_KR_ADDR, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_DIR_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t n = cs_pop(cs);
    int16_t x = cs_pop(cs);

    uint8_t d[] = { II_KR_DIR, n, x };
    tele_ii_tx(II_KR_ADDR, d, 3);
}

static void op_KR_DUR_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    uint8_t d[] = { II_KR_DURATION | II_GET, a & 0x3 };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx(addr, d, 2);
    d[0] = 0;
    d[1] = 0;
    tele_ii_rx(addr, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}


static void op_ME_PRE_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_MP_PRESET, a };
    tele_ii_tx(II_MP_ADDR, d, 2);
}

static void op_ME_PRE_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { II_MP_PRESET | II_GET };
    uint8_t addr = II_MP_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_ME_RES_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_MP_RESET, a };
    tele_ii_tx(II_MP_ADDR, d, 2);
}

static void op_ME_STOP_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_MP_STOP, a };
    tele_ii_tx(II_MP_ADDR, d, 2);
}

static void op_ME_SCALE_set(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_MP_SCALE, a };
    tele_ii_tx(II_MP_ADDR, d, 2);
}

static void op_ME_SCALE_get(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { II_MP_SCALE | II_GET };
    uint8_t addr = II_MP_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_ME_PERIOD_set(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_MP_PERIOD, a >> 8, a & 0xff };
    tele_ii_tx(II_MP_ADDR, d, 3);
}

static void op_ME_PERIOD_get(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { II_MP_PERIOD | II_GET, 0 };
    uint8_t addr = II_MP_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    d[1] = 0;
    tele_ii_rx(addr, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}

static void op_ME_CV_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    uint8_t d[] = { II_MP_CV | II_GET, a & 0x3 };
    uint8_t addr = II_MP_ADDR;
    tele_ii_tx(addr, d, 2);
    d[0] = 0;
    d[1] = 0;
    tele_ii_rx(addr, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}

static void op_LV_PRE_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_LV_PRESET, a };
    tele_ii_tx(II_LV_ADDR, d, 2);
}

static void op_LV_PRE_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { II_LV_PRESET | II_GET };
    uint8_t addr = II_LV_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_LV_RES_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_LV_RESET, a };
    tele_ii_tx(II_LV_ADDR, d, 2);
}

static void op_LV_POS_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_LV_POS, a };
    tele_ii_tx(II_LV_ADDR, d, 2);
}

static void op_LV_POS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    uint8_t d[] = { II_LV_POS | II_GET };
    uint8_t addr = II_LV_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_LV_L_ST_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_LV_L_ST, a };
    tele_ii_tx(II_LV_ADDR, d, 2);
}

static void op_LV_L_ST_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_LV_L_ST | II_GET };
    uint8_t addr = II_LV_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_LV_L_LEN_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_LV_L_LEN, a };
    tele_ii_tx(II_LV_ADDR, d, 2);
}

static void op_LV_L_LEN_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_LV_L_LEN | II_GET };
    uint8_t addr = II_LV_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_LV_L_DIR_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_LV_L_DIR, a };
    tele_ii_tx(II_LV_ADDR, d, 2);
}

static void op_LV_L_DIR_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_LV_L_DIR | II_GET };
    uint8_t addr = II_LV_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_LV_CV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    uint8_t d[] = { II_LV_CV | II_GET, a & 0x3 };
    uint8_t addr = II_LV_ADDR;
    tele_ii_tx(addr, d, 2);
    d[0] = 0;
    d[1] = 0;
    tele_ii_rx(addr, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}

static void op_CY_PRE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_CY_PRESET, a };
    tele_ii_tx(II_CY_ADDR, d, 2);
}

static void op_CY_PRE_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { II_CY_PRESET | II_GET };
    uint8_t addr = II_CY_ADDR;
    tele_ii_tx(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_CY_RES_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_CY_RESET, a };
    tele_ii_tx(II_CY_ADDR, d, 2);
}

static void op_CY_POS_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_CY_POS, a, b };
    tele_ii_tx(II_CY_ADDR, d, 3);
}

static void op_CY_POS_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_CY_POS | II_GET, a };
    uint8_t addr = II_CY_ADDR;
    tele_ii_tx(addr, d, 2);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_CY_REV_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_CY_REV, a };
    tele_ii_tx(II_CY_ADDR, d, 2);
}

static void op_CY_CV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    uint8_t d[] = { II_CY_CV | II_GET, a & 0x3 };
    uint8_t addr = II_CY_ADDR;
    tele_ii_tx(addr, d, 2);
    d[0] = 0;
    d[1] = 0;
    tele_ii_rx(addr, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}

static void op_MID_SHIFT_get(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_MID_SHIFT, a >> 8, a & 0xff };
    tele_ii_tx(II_MID_ADDR, d, 3);
}

static void op_MID_SLEW_get(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_MID_SLEW, a >> 8, a & 0xff };
    tele_ii_tx(II_MID_ADDR, d, 3);
}

static void op_ARP_STY_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_ARP_STYLE, a };
    tele_ii_tx(II_ARP_ADDR, d, 2);
}

static void op_ARP_HLD_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_ARP_HOLD, a & 0xff };
    tele_ii_tx(II_ARP_ADDR, d, 2);
}

static void op_ARP_RPT_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    uint8_t d[] = { II_ARP_RPT, a, b, c >> 8, c & 0xff };
    tele_ii_tx(II_ARP_ADDR, d, 5);
}

static void op_ARP_GT_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_ARP_GATE, a & 0xff, b & 0xff };
    tele_ii_tx(II_ARP_ADDR, d, 3);
}

static void op_ARP_DIV_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_ARP_DIV, a & 0xff, b & 0xff };
    tele_ii_tx(II_ARP_ADDR, d, 3);
}

static void op_ARP_RES_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_ARP_RESET, a };
    tele_ii_tx(II_ARP_ADDR, d, 2);
}

static void op_ARP_SHIFT_get(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_ARP_SHIFT, a, b >> 8, b & 0xff };
    tele_ii_tx(II_ARP_ADDR, d, 4);
}

static void op_ARP_SLEW_get(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_ARP_SLEW, a, b >> 8, b & 0xff };
    tele_ii_tx(II_ARP_ADDR, d, 4);
}

static void op_ARP_FIL_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_ARP_FILL, a, b };
    tele_ii_tx(II_ARP_ADDR, d, 3);
}

static void op_ARP_ROT_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_ARP_ROT, a, b >> 8, b & 0xff };
    tele_ii_tx(II_ARP_ADDR, d, 4);
}

static void op_ARP_ER_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    int16_t e = cs_pop(cs);
    uint8_t d[] = { II_ARP_ER, a, b, c, e >> 8, e & 0xff };
    tele_ii_tx(II_ARP_ADDR, d, 6);
}
