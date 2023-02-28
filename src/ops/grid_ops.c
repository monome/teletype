#include "ops/grid_ops.h"

#include "helpers.h"
#include "teletype.h"
#include "teletype_io.h"

#define SG ss->grid
#define GB ss->grid.button[i]
#define GBC ss->grid.button[i].common
#define GF ss->grid.fader[i]
#define GFC ss->grid.fader[i].common
#define GXY ss->grid.xypad[i]
#define GXYC ss->grid.xypad[i].common

#define GET_AND_CLAMP(value, min, max) \
    s16 value = cs_pop(cs);            \
    if (value < (s16)(min))            \
        value = (min);                 \
    else if (value > (s16)(max))       \
        value = (max);

#define GET_LEVEL(level)      \
    s16 level = cs_pop(cs);   \
    if (level < (s16)LED_OFF) \
        level = LED_OFF;      \
    else if (level > (s16)15) \
        level = 15;

#define CLAMP_X_Y_W_H(op)                                            \
    if (x < (s16)0) {                                                \
        w += x;                                                      \
        x = 0;                                                       \
    }                                                                \
    else if (x >= (s16)GRID_MAX_DIMENSION)                           \
        op;                                                          \
    if (w + x > (s16)GRID_MAX_DIMENSION) w = GRID_MAX_DIMENSION - x; \
    if (y < (s16)0) {                                                \
        h += y;                                                      \
        y = 0;                                                       \
    }                                                                \
    else if (y >= (s16)GRID_MAX_DIMENSION)                           \
        op;                                                          \
    if (h + y > (s16)GRID_MAX_DIMENSION) h = GRID_MAX_DIMENSION - y;

#define min(a, b) ((s32)(a) < (s32)(b) ? (s32)(a) : (s32)(b))
#define max(a, b) ((s32)(a) > (s32)(b) ? (s32)(a) : (s32)(b))

// clang-format off

static void grid_common_init(grid_common_t *gc);
static s32 scale(s32 a, s32 b, s32 x, s32 y, s32 value);
static void grid_rectangle(scene_state_t *ss, s16 x, s16 y, s16 w, s16 h, u8 fill, u8 border);
static void grid_init_button(scene_state_t *ss, s16 group, s16 i, s16 x, s16 y, s16 w, s16 h, s16 latch, s16 level, s16 script);
static void grid_init_fader(scene_state_t *ss, s16 group, s16 i, s16 x, s16 y, s16 w, s16 h, s16 type, s16 level, s16 script);
static s16 grid_fader_max_value(scene_state_t *ss, u16 i);
static s16 grid_fader_clamp_level(s16 level, s16 type, s16 w, s16 h);

static void op_G_RST_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_CLR_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_ROTATE_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_DIM_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_KEY_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);

static void op_G_GRP_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GRP_set    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GRP_EN_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GRP_EN_set (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GRP_RST_get(const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GRP_SW_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GRP_SC_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GRP_SC_set (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GRPI_get   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);

static void op_G_LED_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_LED_set    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_LED_C_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_REC_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_RCT_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);

static void op_G_BTN_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTX_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GBT_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GBX_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTN_EN_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTN_EN_set (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTN_V_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTN_V_set  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTN_L_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTN_L_set  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTN_X_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTN_X_set  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTN_Y_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTN_Y_set  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTNI_get   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTNV_get   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTNV_set   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTNL_get   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTNL_set   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTNX_get   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTNX_set   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTNY_get   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTNY_set   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTN_SW_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_BTN_PR_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GBTN_V_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GBTN_L_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GBTN_C_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GBTN_I_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GBTN_W_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GBTN_H_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GBTN_X1_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GBTN_X2_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GBTN_Y1_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GBTN_Y2_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);

static void op_G_FDR_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDX_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GFD_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GFX_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDR_EN_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDR_EN_set (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDR_V_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDR_V_set  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDR_N_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDR_N_set  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDR_L_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDR_L_set  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDR_X_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDR_X_set  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDR_Y_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDR_Y_set  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDRI_get   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDRV_get   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDRV_set   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDRN_get   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDRN_set   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDRL_get   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDRL_set   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDRX_get   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDRX_set   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDRY_get   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDRY_set   (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_FDR_PR_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GFDR_V_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GFDR_N_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GFDR_L_get (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_GFDR_RN_get(const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);

static void op_G_XYP_get    (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_XYP_X_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_G_XYP_Y_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);

const tele_op_t op_G_RST     = MAKE_GET_OP(G.RST, op_G_RST_get, 0, false);
const tele_op_t op_G_ROTATE  = MAKE_GET_OP(G.ROTATE, op_G_ROTATE_get, 1, false);
const tele_op_t op_G_DIM     = MAKE_GET_OP(G.DIM, op_G_DIM_get, 1, false);
const tele_op_t op_G_CLR     = MAKE_GET_OP(G.CLR, op_G_CLR_get, 0, false);
const tele_op_t op_G_KEY     = MAKE_GET_OP(G.KEY, op_G_KEY_get, 3, false);

const tele_op_t op_G_GRP     = MAKE_GET_SET_OP(G.GRP, op_G_GRP_get, op_G_GRP_set, 0, true);
const tele_op_t op_G_GRP_EN  = MAKE_GET_SET_OP(G.GRP.EN, op_G_GRP_EN_get, op_G_GRP_EN_set, 1, true);
const tele_op_t op_G_GRP_RST = MAKE_GET_OP(G.GRP.RST, op_G_GRP_RST_get, 1, false);
const tele_op_t op_G_GRP_SW  = MAKE_GET_OP(G.GRP.SW, op_G_GRP_SW_get, 1, false);
const tele_op_t op_G_GRP_SC  = MAKE_GET_SET_OP(G.GRP.SC, op_G_GRP_SC_get, op_G_GRP_SC_set, 1, true);
const tele_op_t op_G_GRPI    = MAKE_GET_OP(G.GRPI, op_G_GRPI_get, 0, true);

const tele_op_t op_G_LED     = MAKE_GET_SET_OP(G.LED, op_G_LED_get, op_G_LED_set, 2, true);
const tele_op_t op_G_LED_C   = MAKE_GET_OP(G.LED.C, op_G_LED_C_get, 2, false);
const tele_op_t op_G_REC     = MAKE_GET_OP(G.REC, op_G_REC_get, 6, false);
const tele_op_t op_G_RCT     = MAKE_GET_OP(G.RCT, op_G_RCT_get, 6, false);

const tele_op_t op_G_BTN     = MAKE_GET_OP(G.BTN, op_G_BTN_get, 8, false);
const tele_op_t op_G_BTX     = MAKE_GET_OP(G.BTX, op_G_BTX_get, 10, false);
const tele_op_t op_G_GBT     = MAKE_GET_OP(G.GBT, op_G_GBT_get, 9, false);
const tele_op_t op_G_GBX     = MAKE_GET_OP(G.GBX, op_G_GBX_get, 11, false);
const tele_op_t op_G_BTN_EN  = MAKE_GET_SET_OP(G.BTN.EN, op_G_BTN_EN_get, op_G_BTN_EN_set, 1, true);
const tele_op_t op_G_BTN_V   = MAKE_GET_SET_OP(G.BTN.V, op_G_BTN_V_get, op_G_BTN_V_set, 1, true);
const tele_op_t op_G_BTN_L   = MAKE_GET_SET_OP(G.BTN.L, op_G_BTN_L_get, op_G_BTN_L_set, 1, true);
const tele_op_t op_G_BTN_X   = MAKE_GET_SET_OP(G.BTN.X, op_G_BTN_X_get, op_G_BTN_X_set, 1, true);
const tele_op_t op_G_BTN_Y   = MAKE_GET_SET_OP(G.BTN.Y, op_G_BTN_Y_get, op_G_BTN_Y_set, 1, true);
const tele_op_t op_G_BTNI    = MAKE_GET_OP(G.BTNI, op_G_BTNI_get, 0, true);
const tele_op_t op_G_BTNV    = MAKE_GET_SET_OP(G.BTNV, op_G_BTNV_get, op_G_BTNV_set, 0, true);
const tele_op_t op_G_BTNL    = MAKE_GET_SET_OP(G.BTNL, op_G_BTNL_get, op_G_BTNL_set, 0, true);
const tele_op_t op_G_BTNX    = MAKE_GET_SET_OP(G.BTNX, op_G_BTNX_get, op_G_BTNX_set, 0, true);
const tele_op_t op_G_BTNY    = MAKE_GET_SET_OP(G.BTNY, op_G_BTNY_get, op_G_BTNY_set, 0, true);
const tele_op_t op_G_BTN_SW  = MAKE_GET_OP(G.BTN.SW, op_G_BTN_SW_get, 1, false);
const tele_op_t op_G_BTN_PR  = MAKE_GET_OP(G.BTN.PR, op_G_BTN_PR_get, 2, false);
const tele_op_t op_G_GBTN_V  = MAKE_GET_OP(G.GBTN.V, op_G_GBTN_V_get, 2, false);
const tele_op_t op_G_GBTN_L  = MAKE_GET_OP(G.GBTN.L, op_G_GBTN_L_get, 3, false);
const tele_op_t op_G_GBTN_C  = MAKE_GET_OP(G.GBTN.C, op_G_GBTN_C_get, 1, true);
const tele_op_t op_G_GBTN_I  = MAKE_GET_OP(G.GBTN.I, op_G_GBTN_I_get, 2, true);
const tele_op_t op_G_GBTN_W  = MAKE_GET_OP(G.GBTN.W, op_G_GBTN_W_get, 1, true);
const tele_op_t op_G_GBTN_H  = MAKE_GET_OP(G.GBTN.H, op_G_GBTN_H_get, 1, true);
const tele_op_t op_G_GBTN_X1  = MAKE_GET_OP(G.GBTN.X1, op_G_GBTN_X1_get, 1, true);
const tele_op_t op_G_GBTN_X2  = MAKE_GET_OP(G.GBTN.X2, op_G_GBTN_X2_get, 1, true);
const tele_op_t op_G_GBTN_Y1  = MAKE_GET_OP(G.GBTN.Y1, op_G_GBTN_Y1_get, 1, true);
const tele_op_t op_G_GBTN_Y2  = MAKE_GET_OP(G.GBTN.Y2, op_G_GBTN_Y2_get, 1, true);

const tele_op_t op_G_FDR     = MAKE_GET_OP(G.FDR, op_G_FDR_get, 8, false);
const tele_op_t op_G_FDX     = MAKE_GET_OP(G.FDX, op_G_FDX_get, 10, false);
const tele_op_t op_G_GFD     = MAKE_GET_OP(G.GFD, op_G_GFD_get, 9, false);
const tele_op_t op_G_GFX     = MAKE_GET_OP(G.GFX, op_G_GFX_get, 11, false);
const tele_op_t op_G_FDR_EN  = MAKE_GET_SET_OP(G.FDR.EN, op_G_FDR_EN_get, op_G_FDR_EN_set, 1, true);
const tele_op_t op_G_FDR_V   = MAKE_GET_SET_OP(G.FDR.V, op_G_FDR_V_get, op_G_FDR_V_set, 1, true);
const tele_op_t op_G_FDR_N   = MAKE_GET_SET_OP(G.FDR.N, op_G_FDR_N_get, op_G_FDR_N_set, 1, true);
const tele_op_t op_G_FDR_L   = MAKE_GET_SET_OP(G.FDR.L, op_G_FDR_L_get, op_G_FDR_L_set, 1, true);
const tele_op_t op_G_FDR_X   = MAKE_GET_SET_OP(G.FDR.X, op_G_FDR_X_get, op_G_FDR_X_set, 1, true);
const tele_op_t op_G_FDR_Y   = MAKE_GET_SET_OP(G.FDR.Y, op_G_FDR_Y_get, op_G_FDR_Y_set, 1, true);
const tele_op_t op_G_FDRI    = MAKE_GET_OP(G.FDRI, op_G_FDRI_get, 0, true);
const tele_op_t op_G_FDRV    = MAKE_GET_SET_OP(G.FDRV, op_G_FDRV_get, op_G_FDRV_set, 0, true);
const tele_op_t op_G_FDRN    = MAKE_GET_SET_OP(G.FDRN, op_G_FDRN_get, op_G_FDRN_set, 0, true);
const tele_op_t op_G_FDRL    = MAKE_GET_SET_OP(G.FDRL, op_G_FDRL_get, op_G_FDRL_set, 0, true);
const tele_op_t op_G_FDRX    = MAKE_GET_SET_OP(G.FDRX, op_G_FDRX_get, op_G_FDRX_set, 0, true);
const tele_op_t op_G_FDRY    = MAKE_GET_SET_OP(G.FDRY, op_G_FDRY_get, op_G_FDRY_set, 0, true);
const tele_op_t op_G_FDR_PR  = MAKE_GET_OP(G.FDR.PR, op_G_FDR_PR_get, 2, false);
const tele_op_t op_G_GFDR_V  = MAKE_GET_OP(G.GFDR.V, op_G_GFDR_V_get, 2, false);
const tele_op_t op_G_GFDR_N  = MAKE_GET_OP(G.GFDR.N, op_G_GFDR_N_get, 2, false);
const tele_op_t op_G_GFDR_L  = MAKE_GET_OP(G.GFDR.L, op_G_GFDR_L_get, 3, false);
const tele_op_t op_G_GFDR_RN = MAKE_GET_OP(G.GFDR.RN, op_G_GFDR_RN_get, 3, false);

const tele_op_t op_G_XYP     = MAKE_GET_OP(G.XYP, op_G_XYP_get, 7, false);
const tele_op_t op_G_XYP_X   = MAKE_GET_OP(G.XYP.X, op_G_XYP_X_get, 1, true);
const tele_op_t op_G_XYP_Y   = MAKE_GET_OP(G.XYP.Y, op_G_XYP_Y_get, 1, true);

// clang-format on

static void op_G_RST_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es),
                         command_state_t *NOTUSED(cs)) {
    SG.rotate = 0;
    SG.dim = 0;

    SG.current_group = 0;
    SG.latest_group = 0;
    SG.latest_button = 0;
    SG.latest_fader = 0;

    for (u8 i = 0; i < GRID_MAX_DIMENSION; i++)
        for (u8 j = 0; j < GRID_MAX_DIMENSION; j++) SG.leds[i][j] = LED_OFF;

    for (u8 i = 0; i < GRID_GROUP_COUNT; i++) {
        SG.group[i].enabled = true;
        SG.group[i].script = -1;
        SG.group[i].fader_min = 0;
        SG.group[i].fader_max = 16383;
    }

    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++) {
        grid_common_init(&(GBC));
        GB.latch = 0;
        GB.state = 0;
    }

    for (u8 i = 0; i < GRID_FADER_COUNT; i++) {
        grid_common_init(&(GFC));
        GF.type = FADER_CH_BAR;
        GF.value = 0;
        GF.slide = 0;
    }

    for (u8 i = 0; i < GRID_XYPAD_COUNT; i++) {
        grid_common_init(&(GXYC));
        GXY.value_x = 0;
        GXY.value_y = 0;
    }

    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_CLR_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es),
                         command_state_t *NOTUSED(cs)) {
    for (u8 i = 0; i < GRID_MAX_DIMENSION; i++)
        for (u8 j = 0; j < GRID_MAX_DIMENSION; j++) SG.leds[i][j] = LED_OFF;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_ROTATE_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 rotate = cs_pop(cs);
    SG.rotate = rotate != 0;
    SG.scr_dirty = SG.grid_dirty = SG.clear_held = 1;
}

static void op_G_DIM_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    GET_AND_CLAMP(dim, 0, 14);
    SG.dim = dim;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_KEY_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs) {
    s16 x = cs_pop(cs);
    s16 y = cs_pop(cs);
    s16 action = cs_pop(cs);
    if (x < 0 || y < 0 || x >= GRID_MAX_DIMENSION || y >= GRID_MAX_DIMENSION)
        return;
    grid_key_press(x, y, action != 0);
}

static void op_G_GRP_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, SG.current_group);
}

static void op_G_GRP_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    if (group < (s16)0 || group >= (s16)GRID_GROUP_COUNT) return;
    SG.current_group = group;
    SG.scr_dirty = 1;
}

static void op_G_GRP_EN_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    cs_push(cs, group < (s16)0 || group >= (s16)GRID_GROUP_COUNT
                    ? 0
                    : SG.group[group].enabled);
}

static void op_G_GRP_EN_set(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    s16 en = cs_pop(cs);
    if (group < (s16)0 || group >= (s16)GRID_GROUP_COUNT) return;
    SG.group[group].enabled = en != 0;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_GRP_RST_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    if (group < (s16)0 || group >= (s16)GRID_GROUP_COUNT) return;

    SG.group[group].enabled = true;
    SG.group[group].script = -1;
    SG.group[group].fader_min = 0;
    SG.group[group].fader_max = 16383;

    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++)
        if (GBC.group == group) {
            grid_common_init(&(GBC));
            GB.latch = 0;
            GB.state = 0;
        }

    for (u8 i = 0; i < GRID_FADER_COUNT; i++)
        if (GFC.group == group) {
            grid_common_init(&(GFC));
            GF.type = FADER_CH_BAR;
            GF.value = 0;
            GF.slide = 0;
        }

    for (u8 i = 0; i < GRID_XYPAD_COUNT; i++)
        if (GXYC.group == group) {
            grid_common_init(&(GXYC));
            GXY.value_x = 0;
            GXY.value_y = 0;
        }

    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_GRP_SW_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    if (group < (s16)0 || group >= (s16)GRID_GROUP_COUNT) return;

    for (u8 i = 0; i < GRID_GROUP_COUNT; i++) SG.group[i].enabled = false;
    SG.group[group].enabled = true;

    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_GRP_SC_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    cs_push(cs, group < (s16)0 || group >= (s16)GRID_GROUP_COUNT
                    ? -1
                    : SG.group[group].script + 1);
}

static void op_G_GRP_SC_set(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    s16 script = cs_pop(cs) - 1;

    if (group < (s16)0 || group >= (s16)GRID_GROUP_COUNT) return;
    if (script < 0 || script > INIT_SCRIPT) script = -1;

    SG.group[group].script = script;
}

static void op_G_GRPI_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, SG.latest_group);
}

static void op_G_LED_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 x = cs_pop(cs);
    s16 y = cs_pop(cs);

    if (x < (s16)0 || x >= (s16)GRID_MAX_DIMENSION)
        cs_push(cs, LED_OFF);
    else if (y < (s16)0 || y >= (s16)GRID_MAX_DIMENSION)
        cs_push(cs, LED_OFF);
    else
        cs_push(cs, SG.leds[x][y]);
}

static void op_G_LED_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 x = cs_pop(cs);
    s16 y = cs_pop(cs);
    GET_LEVEL(level);

    if (x < (s16)0 || x >= (s16)GRID_MAX_DIMENSION) return;
    if (y < (s16)0 || y >= (s16)GRID_MAX_DIMENSION) return;

    SG.leds[x][y] = level;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_LED_C_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 x = cs_pop(cs);
    s16 y = cs_pop(cs);

    if (x < (s16)0 || x >= (s16)GRID_MAX_DIMENSION) return;
    if (y < (s16)0 || y >= (s16)GRID_MAX_DIMENSION) return;

    SG.leds[x][y] = LED_OFF;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_REC_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 x = cs_pop(cs);
    s16 y = cs_pop(cs);
    s16 w = cs_pop(cs);
    s16 h = cs_pop(cs);
    GET_LEVEL(fill);
    GET_LEVEL(border);
    grid_rectangle(ss, x, y, w, h, fill, border);
}

static void op_G_RCT_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 x1 = cs_pop(cs);
    s16 y1 = cs_pop(cs);
    s16 x2 = cs_pop(cs);
    s16 y2 = cs_pop(cs);
    GET_LEVEL(fill);
    GET_LEVEL(border);
    grid_rectangle(ss, x1, y1, x2 - x1 + 1, y2 - y1 + 1, fill, border);
}

static void op_G_BTN_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 x = cs_pop(cs);
    s16 y = cs_pop(cs);
    s16 w = cs_pop(cs);
    s16 h = cs_pop(cs);
    s16 latch = cs_pop(cs);
    s16 level = cs_pop(cs);
    s16 script = cs_pop(cs) - 1;
    grid_init_button(ss, SG.current_group, i, x, y, w, h, latch, level, script);
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_GBT_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    s16 i = cs_pop(cs);
    s16 x = cs_pop(cs);
    s16 y = cs_pop(cs);
    s16 w = cs_pop(cs);
    s16 h = cs_pop(cs);
    s16 latch = cs_pop(cs);
    s16 level = cs_pop(cs);
    s16 script = cs_pop(cs) - 1;
    grid_init_button(ss, group, i, x, y, w, h, latch, level, script);
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_BTX_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 id = cs_pop(cs);
    s16 _x = cs_pop(cs);
    s16 _y = cs_pop(cs);
    s16 _w = cs_pop(cs);
    s16 _h = cs_pop(cs);
    s16 latch = cs_pop(cs);
    s16 level = cs_pop(cs);
    s16 script = cs_pop(cs) - 1;
    s16 count_x = cs_pop(cs);
    s16 count_y = cs_pop(cs);

    if (count_x <= (s16)0) return;
    if (count_x > (s16)GRID_MAX_DIMENSION) count_x = GRID_MAX_DIMENSION;
    if (count_y <= (s16)0) return;
    if (count_y > (s16)GRID_MAX_DIMENSION) count_y = GRID_MAX_DIMENSION;

    u16 i;
    s16 x, y, w, h;
    for (s16 cy = 0; cy < count_y; cy++)
        for (s16 cx = 0; cx < count_x; cx++) {
            i = id + cy * count_x + cx;
            x = _x + _w * cx;
            w = _w;
            y = _y + _h * cy;
            h = _h;
            grid_init_button(ss, SG.current_group, i, x, y, w, h, latch, level,
                             script);
        }

    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_GBX_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    s16 id = cs_pop(cs);
    s16 _x = cs_pop(cs);
    s16 _y = cs_pop(cs);
    s16 _w = cs_pop(cs);
    s16 _h = cs_pop(cs);
    s16 latch = cs_pop(cs);
    s16 level = cs_pop(cs);
    s16 script = cs_pop(cs) - 1;
    s16 count_x = cs_pop(cs);
    s16 count_y = cs_pop(cs);

    if (count_x <= (s16)0) return;
    if (count_x > (s16)GRID_MAX_DIMENSION) count_x = GRID_MAX_DIMENSION;
    if (count_y <= (s16)0) return;
    if (count_y > (s16)GRID_MAX_DIMENSION) count_y = GRID_MAX_DIMENSION;

    u16 i;
    s16 x, y, w, h;
    for (s16 cy = 0; cy < count_y; cy++)
        for (s16 cx = 0; cx < count_x; cx++) {
            i = id + cy * count_x + cx;
            x = _x + _w * cx;
            w = _w;
            y = _y + _h * cy;
            h = _h;
            grid_init_button(ss, group, i, x, y, w, h, latch, level, script);
        }

    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_BTN_EN_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    cs_push(cs, i < (s16)0 || i >= (s16)GRID_BUTTON_COUNT ? 0 : GBC.enabled);
}

static void op_G_BTN_EN_set(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 en = cs_pop(cs);

    if (i < (s16)0 || i >= (s16)GRID_BUTTON_COUNT) return;
    GBC.enabled = en;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_BTN_V_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    cs_push(cs, i < (s16)0 || i >= (s16)GRID_BUTTON_COUNT ? 0 : GB.state);
}

static void op_G_BTN_V_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 value = cs_pop(cs);
    if (i < (s16)0 || i >= (s16)GRID_BUTTON_COUNT) return;
    GB.state = value != 0;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_BTN_L_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    cs_push(cs, i < (s16)0 || i >= (s16)GRID_BUTTON_COUNT ? 0 : GBC.level);
}

static void op_G_BTN_L_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    GET_LEVEL(level);
    if (i < (s16)0 || i >= (s16)GRID_BUTTON_COUNT) return;
    GBC.level = level;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_BTN_X_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    cs_push(cs, i < (s16)0 || i >= (s16)GRID_BUTTON_COUNT ? 0 : GBC.x);
}

static void op_G_BTN_X_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 x = cs_pop(cs);

    if (i < (s16)0 || i >= (s16)GRID_BUTTON_COUNT) return;
    s16 y = GBC.y;
    s16 w = GBC.w;
    s16 h = GBC.h;
    CLAMP_X_Y_W_H(return );

    GBC.x = x;
    GBC.y = y;
    GBC.w = w;
    GBC.h = h;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_BTN_Y_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    cs_push(cs, i < (s16)0 || i >= (s16)GRID_BUTTON_COUNT ? 0 : GBC.y);
}

static void op_G_BTN_Y_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 y = cs_pop(cs);

    if (i < (s16)0 || i >= (s16)GRID_BUTTON_COUNT) return;
    s16 x = GBC.x;
    s16 w = GBC.w;
    s16 h = GBC.h;
    CLAMP_X_Y_W_H(return );

    GBC.x = x;
    GBC.y = y;
    GBC.w = w;
    GBC.h = h;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_BTNI_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, SG.latest_button);
}

static void op_G_BTNV_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, SG.button[SG.latest_button].state);
}

static void op_G_BTNV_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 value = cs_pop(cs);
    SG.button[SG.latest_button].state = value != 0;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_BTNL_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, SG.button[SG.latest_button].common.level);
}

static void op_G_BTNL_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    GET_LEVEL(level);
    SG.button[SG.latest_button].common.level = level;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_BTNX_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, SG.button[SG.latest_button].common.x);
}

static void op_G_BTNX_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 x = cs_pop(cs);
    u16 i = SG.latest_button;

    s16 y = GBC.y;
    s16 w = GBC.w;
    s16 h = GBC.h;
    CLAMP_X_Y_W_H(return );

    GBC.x = x;
    GBC.y = y;
    GBC.w = w;
    GBC.h = h;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_BTNY_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, SG.button[SG.latest_button].common.y);
}

static void op_G_BTNY_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 y = cs_pop(cs);
    u16 i = SG.latest_button;

    s16 x = GBC.x;
    s16 w = GBC.w;
    s16 h = GBC.h;
    CLAMP_X_Y_W_H(return );

    GBC.x = x;
    GBC.y = y;
    GBC.w = w;
    GBC.h = h;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_BTN_SW_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 id = cs_pop(cs);
    if (id < (s16)0 || id >= (s16)GRID_BUTTON_COUNT) return;

    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++)
        if (GBC.group == SG.button[id].common.group) GB.state = 0;

    SG.button[id].state = 1;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_BTN_PR_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 action = cs_pop(cs);

    if (i < (s16)0 || i >= (s16)GRID_BUTTON_COUNT) return;

    GB.state = GB.latch ? !GB.state : action != 0;
    SG.latest_button = i;
    SG.latest_group = GBC.group;

    if (GBC.script != -1) {
        es_push(es);
        if (!es->overflow) run_script_with_exec_state(ss, es, GBC.script);
        es_pop(es);
    }

    if (SG.group[GBC.group].script != -1) {
        es_push(es);
        if (!es->overflow)
            run_script_with_exec_state(ss, es, SG.group[GBC.group].script);
        es_pop(es);
    }

    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_GBTN_V_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    s16 value = cs_pop(cs);

    if (group < (s16)0 || group > (s16)GRID_GROUP_COUNT) return;
    value = value != 0;

    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++)
        if (GBC.group == group) GB.state = value;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_GBTN_L_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    GET_LEVEL(odd);
    GET_LEVEL(even);

    if (group < (s16)0 || group > (s16)GRID_GROUP_COUNT) return;

    u8 is_odd = 0;
    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++)
        if (GBC.group == group) {
            GBC.level = is_odd ? odd : even;
            is_odd = !is_odd;
        }
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_GBTN_C_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    if (group < (s16)0 || group > (s16)GRID_GROUP_COUNT) {
        cs_push(cs, 0);
        return;
    }

    s16 count = 0;
    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++)
        if (GBC.group == group && GB.state) count++;
    cs_push(cs, count);
}

static void op_G_GBTN_I_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    s16 index = cs_pop(cs);
    if (group < (s16)0 || group > (s16)GRID_GROUP_COUNT) {
        cs_push(cs, -1);
        return;
    }

    s16 id = -1;
    s16 count = -1;
    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++) {
        if (GBC.group == group && GB.state) count++;
        if (count == index) {
            id = i;
            break;
        }
    }

    cs_push(cs, id);
}

static void op_G_GBTN_W_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    if (group < (s16)0 || group > (s16)GRID_GROUP_COUNT) {
        cs_push(cs, 0);
        return;
    }

    s16 x1 = 32767, x2 = -32768;
    u8 atleastone = 0;
    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++) {
        if (GBC.group == group && GB.state) {
            if (GBC.x < x1) x1 = GBC.x;
            if (GBC.x > x2) x2 = GBC.x;
            atleastone = 1;
        }
    }

    cs_push(cs, atleastone ? x2 - x1 + 1 : 0);
}

static void op_G_GBTN_H_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    if (group < (s16)0 || group > (s16)GRID_GROUP_COUNT) {
        cs_push(cs, 0);
        return;
    }

    s16 y1 = 32767, y2 = -32768;
    u8 atleastone = 0;
    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++) {
        if (GBC.group == group && GB.state) {
            if (GBC.y < y1) y1 = GBC.y;
            if (GBC.y > y2) y2 = GBC.y;
            atleastone = 1;
        }
    }

    cs_push(cs, atleastone ? y2 - y1 + 1 : 0);
}

static void op_G_GBTN_X1_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    if (group < (s16)0 || group > (s16)GRID_GROUP_COUNT) {
        cs_push(cs, -1);
        return;
    }

    s16 x1 = 32767;
    u8 atleastone = 0;
    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++) {
        if (GBC.group == group && GB.state) {
            if (GBC.x < x1) x1 = GBC.x;
            atleastone = 1;
        }
    }

    cs_push(cs, atleastone ? x1 : -1);
}

static void op_G_GBTN_X2_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    if (group < (s16)0 || group > (s16)GRID_GROUP_COUNT) {
        cs_push(cs, -1);
        return;
    }

    s16 x2 = -32768;
    u8 atleastone = 0;
    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++) {
        if (GBC.group == group && GB.state) {
            if (GBC.x > x2) x2 = GBC.x;
            atleastone = 1;
        }
    }

    cs_push(cs, atleastone ? x2 : -1);
}

static void op_G_GBTN_Y1_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    if (group < (s16)0 || group > (s16)GRID_GROUP_COUNT) {
        cs_push(cs, -1);
        return;
    }

    s16 y1 = 32767;
    u8 atleastone = 0;
    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++) {
        if (GBC.group == group && GB.state) {
            if (GBC.y < y1) y1 = GBC.y;
            atleastone = 1;
        }
    }

    cs_push(cs, atleastone ? y1 : -1);
}

static void op_G_GBTN_Y2_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    if (group < (s16)0 || group > (s16)GRID_GROUP_COUNT) {
        cs_push(cs, -1);
        return;
    }

    s16 y2 = -32768;
    u8 atleastone = 0;
    for (u16 i = 0; i < GRID_BUTTON_COUNT; i++) {
        if (GBC.group == group && GB.state) {
            if (GBC.y > y2) y2 = GBC.y;
            atleastone = 1;
        }
    }

    cs_push(cs, atleastone ? y2 : -1);
}

static void op_G_FDR_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 x = cs_pop(cs);
    s16 y = cs_pop(cs);
    s16 w = cs_pop(cs);
    s16 h = cs_pop(cs);
    s16 type = cs_pop(cs);
    s16 level = cs_pop(cs);
    s16 script = cs_pop(cs) - 1;
    grid_init_fader(ss, SG.current_group, i, x, y, w, h, type, level, script);
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_GFD_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    s16 i = cs_pop(cs);
    s16 x = cs_pop(cs);
    s16 y = cs_pop(cs);
    s16 w = cs_pop(cs);
    s16 h = cs_pop(cs);
    s16 type = cs_pop(cs);
    s16 level = cs_pop(cs);
    s16 script = cs_pop(cs) - 1;
    grid_init_fader(ss, group, i, x, y, w, h, type, level, script);
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_FDX_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 id = cs_pop(cs);
    s16 x = cs_pop(cs);
    s16 y = cs_pop(cs);
    s16 w = cs_pop(cs);
    s16 h = cs_pop(cs);
    s16 type = cs_pop(cs);
    s16 level = cs_pop(cs);
    s16 script = cs_pop(cs) - 1;
    s16 count_x = cs_pop(cs);
    s16 count_y = cs_pop(cs);

    if (count_x <= (s16)0) return;
    if (count_x > (s16)GRID_MAX_DIMENSION) count_x = GRID_MAX_DIMENSION;
    if (count_y <= (s16)0) return;
    if (count_y > (s16)GRID_MAX_DIMENSION) count_y = GRID_MAX_DIMENSION;

    for (u16 cy = 0; cy < count_y; cy++)
        for (u16 cx = 0; cx < count_x; cx++)
            grid_init_fader(ss, SG.current_group, id + cy * count_x + cx,
                            x + w * cx, y + h * cy, w, h, type, level, script);

    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_GFX_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    s16 id = cs_pop(cs);
    s16 x = cs_pop(cs);
    s16 y = cs_pop(cs);
    s16 w = cs_pop(cs);
    s16 h = cs_pop(cs);
    s16 type = cs_pop(cs);
    s16 level = cs_pop(cs);
    s16 script = cs_pop(cs) - 1;
    s16 count_x = cs_pop(cs);
    s16 count_y = cs_pop(cs);

    if (count_x <= (s16)0) return;
    if (count_x > (s16)GRID_MAX_DIMENSION) count_x = GRID_MAX_DIMENSION;
    if (count_y <= (s16)0) return;
    if (count_y > (s16)GRID_MAX_DIMENSION) count_y = GRID_MAX_DIMENSION;

    for (u16 cy = 0; cy < count_y; cy++)
        for (u16 cx = 0; cx < count_x; cx++)
            grid_init_fader(ss, group, id + cy * count_x + cx, x + w * cx,
                            y + h * cy, w, h, type, level, script);

    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_FDR_EN_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    cs_push(cs, i < (s16)0 || i >= (s16)GRID_FADER_COUNT ? 0 : GFC.enabled);
}

static void op_G_FDR_EN_set(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 en = cs_pop(cs);

    if (i < (s16)0 || i >= (s16)GRID_FADER_COUNT) return;
    GFC.enabled = en;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_FDR_V_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    if (i < (s16)0 || i >= (s16)GRID_FADER_COUNT) {
        cs_push(cs, 0);
        return;
    }

    s16 value =
        scale(0, grid_fader_max_value(ss, i), SG.group[GFC.group].fader_min,
              SG.group[GFC.group].fader_max, GF.value);
    cs_push(cs, value);
}

static void op_G_FDR_V_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 value = cs_pop(cs);

    if (i < (s16)0 || i >= (s16)GRID_FADER_COUNT) return;
    if (value < SG.group[GFC.group].fader_min)
        value = SG.group[GFC.group].fader_min;
    else if (value > SG.group[GFC.group].fader_max)
        value = SG.group[GFC.group].fader_max;

    GF.value =
        scale(SG.group[GFC.group].fader_min, SG.group[GFC.group].fader_max, 0,
              grid_fader_max_value(ss, i), value);
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_FDR_N_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    cs_push(cs, i < (s16)0 || i >= (s16)GRID_FADER_COUNT ? 0 : GF.value);
}

static void op_G_FDR_N_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 value = cs_pop(cs);

    if (i < (s16)0 || i >= (s16)GRID_FADER_COUNT) return;

    s16 maxvalue = grid_fader_max_value(ss, i);
    if (value < (s16)0)
        value = 0;
    else if (value > maxvalue)
        value = maxvalue;

    GF.value = value;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_FDR_L_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    cs_push(cs, i < (s16)0 || i >= (s16)GRID_FADER_COUNT ? 0 : GFC.level);
}

static void op_G_FDR_L_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 level = cs_pop(cs);
    if (i < (s16)0 || i >= (s16)GRID_FADER_COUNT) return;

    level = grid_fader_clamp_level(level, GF.type, GFC.w, GFC.h);
    if (GF.type > FADER_COARSE)
        GF.value = scale(0, GFC.level, 0, level, GF.value);
    GFC.level = level;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_FDR_X_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    cs_push(cs, i < (s16)0 || i >= (s16)GRID_FADER_COUNT ? 0 : GFC.x);
}

static void op_G_FDR_X_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 x = cs_pop(cs);

    if (i < (s16)0 || i >= (s16)GRID_FADER_COUNT) return;
    s16 y = GFC.y;
    s16 w = GFC.w;
    s16 h = GFC.h;
    CLAMP_X_Y_W_H(return );

    GFC.x = x;
    GFC.y = y;
    GFC.w = w;
    GFC.h = h;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_FDR_Y_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    cs_push(cs, i < (s16)0 || i >= (s16)GRID_FADER_COUNT ? 0 : GFC.y);
}

static void op_G_FDR_Y_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 y = cs_pop(cs);

    if (i < (s16)0 || i >= (s16)GRID_FADER_COUNT) return;
    s16 x = GFC.x;
    s16 w = GFC.w;
    s16 h = GFC.h;
    CLAMP_X_Y_W_H(return );

    GFC.x = x;
    GFC.y = y;
    GFC.w = w;
    GFC.h = h;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_FDRI_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, SG.latest_fader);
}

static void op_G_FDRV_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    u8 i = SG.latest_fader;
    s16 value =
        scale(0, grid_fader_max_value(ss, i), SG.group[GFC.group].fader_min,
              SG.group[GFC.group].fader_max, GF.value);
    cs_push(cs, value);
}

static void op_G_FDRV_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 value = cs_pop(cs);
    s16 i = SG.latest_fader;

    if (value < SG.group[GFC.group].fader_min)
        value = SG.group[GFC.group].fader_min;
    else if (value > SG.group[GFC.group].fader_max)
        value = SG.group[GFC.group].fader_max;

    GF.value =
        scale(SG.group[GFC.group].fader_min, SG.group[GFC.group].fader_max, 0,
              grid_fader_max_value(ss, i), value);
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_FDRN_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, SG.fader[SG.latest_fader].value);
}

static void op_G_FDRN_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 value = cs_pop(cs);
    s16 i = SG.latest_fader;

    s16 maxvalue = grid_fader_max_value(ss, i);
    if (value < (s16)0)
        value = 0;
    else if (value > maxvalue)
        value = maxvalue;

    GF.value = value;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_FDRL_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, SG.fader[SG.latest_fader].common.level);
}

static void op_G_FDRL_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 level = cs_pop(cs);
    u16 i = SG.latest_fader;

    level = grid_fader_clamp_level(level, GF.type, GFC.w, GFC.h);
    if (GF.type > FADER_COARSE)
        GF.value = scale(0, GFC.level, 0, level, GF.value);
    GFC.level = level;

    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_FDRX_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, SG.fader[SG.latest_fader].common.x);
}

static void op_G_FDRX_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 x = cs_pop(cs);
    s16 i = SG.latest_fader;

    s16 y = GFC.y;
    s16 w = GFC.w;
    s16 h = GFC.h;
    CLAMP_X_Y_W_H(return );

    GFC.x = x;
    GFC.y = y;
    GFC.w = w;
    GFC.h = h;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_FDRY_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, SG.fader[SG.latest_fader].common.y);
}

static void op_G_FDRY_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 y = cs_pop(cs);
    s16 i = SG.latest_fader;

    s16 x = GFC.x;
    s16 w = GFC.w;
    s16 h = GFC.h;
    CLAMP_X_Y_W_H(return );

    GFC.x = x;
    GFC.y = y;
    GFC.w = w;
    GFC.h = h;
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_FDR_PR_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 value = cs_pop(cs) - 1;

    if (i < (s16)0 || i >= (s16)GRID_FADER_COUNT) return;

    s16 maxvalue = grid_fader_max_value(ss, i);
    if (value < (s16)0)
        value = 0;
    else if (value > maxvalue)
        value = maxvalue;

    GF.value = value;
    SG.latest_fader = i;
    SG.latest_group = GFC.group;

    if (GFC.script != -1) {
        es_push(es);
        if (!es->overflow) run_script_with_exec_state(ss, es, GFC.script);
        es_pop(es);
    }

    if (SG.group[GFC.group].script != -1) {
        es_push(es);
        if (!es->overflow)
            run_script_with_exec_state(ss, es, SG.group[GFC.group].script);
        es_pop(es);
    }

    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_GFDR_V_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    s16 value = cs_pop(cs);

    if (group < (s16)0 || group > (s16)GRID_GROUP_COUNT) return;
    if (value < SG.group[group].fader_min)
        value = SG.group[group].fader_min;
    else if (value > SG.group[group].fader_max)
        value = SG.group[group].fader_max;

    for (u16 i = 0; i < GRID_FADER_COUNT; i++)
        if (GFC.group == group)
            GF.value =
                scale(SG.group[group].fader_min, SG.group[group].fader_max, 0,
                      grid_fader_max_value(ss, i), value);

    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_GFDR_N_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    s16 value = cs_pop(cs);

    if (group < (s16)0 || group > (s16)GRID_GROUP_COUNT) return;
    if (value < (s16)0) value = 0;

    for (u16 i = 0; i < GRID_FADER_COUNT; i++)
        if (GFC.group == group)
            GF.value = min(grid_fader_max_value(ss, i), value);
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_GFDR_L_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    s16 odd = cs_pop(cs);
    s16 even = cs_pop(cs);

    if (group < (s16)0 || group > (s16)GRID_GROUP_COUNT) return;

    u8 is_odd = 0;
    s16 level;
    for (u16 i = 0; i < GRID_FADER_COUNT; i++)
        if (GFC.group == group) {
            level = grid_fader_clamp_level(is_odd ? odd : even, GF.type, GFC.w,
                                           GFC.h);
            if (GF.type > FADER_COARSE)
                GF.value = scale(0, GFC.level, 0, level, GF.value);
            GFC.level = level;
            is_odd = !is_odd;
        }
    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_GFDR_RN_get(const void *NOTUSED(data), scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 group = cs_pop(cs);
    s16 min = cs_pop(cs);
    s16 max = cs_pop(cs);

    if (group < (s16)0 || group > (s16)GRID_GROUP_COUNT) return;
    SG.group[group].fader_min = min;
    SG.group[group].fader_max = max;
}

static void op_G_XYP_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    s16 x = cs_pop(cs);
    s16 y = cs_pop(cs);
    s16 w = cs_pop(cs);
    s16 h = cs_pop(cs);
    GET_LEVEL(level);
    s16 script = cs_pop(cs) - 1;

    if (i < (s16)0 || i >= (s16)GRID_XYPAD_COUNT) return;
    if (script < 0 || script > INIT_SCRIPT) script = -1;
    CLAMP_X_Y_W_H(return );

    GXYC.enabled = true;
    GXYC.group = SG.current_group;
    GXYC.x = x;
    GXYC.y = y;
    GXYC.w = w;
    GXYC.h = h;
    GXYC.level = level;
    GXYC.script = script;
    GXY.value_x = 0;
    GXY.value_y = 0;

    SG.scr_dirty = SG.grid_dirty = 1;
}

static void op_G_XYP_X_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    cs_push(cs, i < (s16)0 || i >= (s16)GRID_XYPAD_COUNT ? 0 : GXY.value_x);
}

static void op_G_XYP_Y_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    s16 i = cs_pop(cs);
    cs_push(cs, i < (s16)0 || i >= (s16)GRID_XYPAD_COUNT ? 0 : GXY.value_y);
}

void grid_common_init(grid_common_t *gc) {
    gc->enabled = false;
    gc->group = 0;
    gc->x = gc->y = 0;
    gc->w = gc->h = 1;
    gc->level = 5;
    gc->script = -1;
}

// helpers

s32 scale(s32 a, s32 b, s32 x, s32 y, s32 value) {
    if (a == b) return x;
    s32 result = (value - a) * (y - x) * 2 / (b - a);
    result = (result / 2) + (result & 1);  // rounding
    return result + x;
}

void grid_rectangle(scene_state_t *ss, s16 x, s16 y, s16 w, s16 h, u8 fill,
                    u8 border) {
    for (u16 col = max(0, x + (s32)1);
         col < min(GRID_MAX_DIMENSION, x + w - (s32)1); col++)
        for (u16 row = max(0, y + (s32)1);
             row < min(GRID_MAX_DIMENSION, y + h - (s32)1); row++)
            SG.leds[col][row] = fill;

    if (y >= (s16)0 && y < (s16)GRID_MAX_DIMENSION)
        for (u16 col = max(0, x); col < min(GRID_MAX_DIMENSION, (s32)x + w);
             col++)
            SG.leds[col][y] = border;

    s16 row = y + h - (s16)1;
    if (row >= (s16)0 && row < (s16)GRID_MAX_DIMENSION)
        for (u16 col = max(0, x); col < min(GRID_MAX_DIMENSION, (s32)x + w);
             col++)
            SG.leds[col][row] = border;

    if (x >= (s16)0 && x < (s16)GRID_MAX_DIMENSION)
        for (u16 row = max(0, y); row < min(GRID_MAX_DIMENSION, (s32)y + h);
             row++)
            SG.leds[x][row] = border;

    s16 col = x + w - 1;
    if (col >= (s16)0 && col < (s16)GRID_MAX_DIMENSION)
        for (u16 row = max(0, y); row < min(GRID_MAX_DIMENSION, (s32)y + h);
             row++)
            SG.leds[col][row] = border;

    SG.scr_dirty = SG.grid_dirty = 1;
}

static void grid_init_button(scene_state_t *ss, s16 group, s16 i, s16 x, s16 y,
                             s16 w, s16 h, s16 latch, s16 level, s16 script) {
    if (group < (s16)0 || group >= (s16)GRID_GROUP_COUNT) return;
    if (i < (s16)0 || i >= (s16)GRID_BUTTON_COUNT) return;

    if (x >= (s16)GRID_MAX_DIMENSION || y >= (s16)GRID_MAX_DIMENSION) return;
    if (x < (s16)0) {
        w += x;
        x = 0;
    }
    if (w + x > (s16)GRID_MAX_DIMENSION) w = GRID_MAX_DIMENSION - x;
    if (y < (s16)0) {
        h += y;
        y = 0;
    }
    if (h + y > (s16)GRID_MAX_DIMENSION) h = GRID_MAX_DIMENSION - y;
    if (w == 0 || h == 0) return;

    if (level < (s16)LED_OFF)
        level = LED_OFF;
    else if (level > (s16)15)
        level = 15;

    if (script < 0 || script > INIT_SCRIPT) script = -1;

    GBC.enabled = true;
    GBC.group = group;
    GBC.x = x;
    GBC.y = y;
    GBC.w = w;

    GBC.h = h;
    GBC.level = level;
    GBC.script = script;
    GB.latch = latch != 0;
    if (!GB.latch) GB.state = 0;
}

static void grid_init_fader(scene_state_t *ss, s16 group, s16 i, s16 x, s16 y,
                            s16 w, s16 h, s16 type, s16 level, s16 script) {
    if (group < (s16)0 || group >= (s16)GRID_GROUP_COUNT) return;
    if (i < (s16)0 || i >= (s16)GRID_FADER_COUNT) return;

    if (x >= (s16)GRID_MAX_DIMENSION || y >= (s16)GRID_MAX_DIMENSION) return;
    if (x < (s16)0) {
        w += x;
        x = 0;
    }
    if (w + x > (s16)GRID_MAX_DIMENSION) w = GRID_MAX_DIMENSION - x;
    if (y < (s16)0) {
        h += y;
        y = 0;
    }
    if (h + y > (s16)GRID_MAX_DIMENSION) h = GRID_MAX_DIMENSION - y;
    if (w == 0 || h == 0) return;

    if (type < FADER_CH_BAR || type > FADER_FV_DOT) type = FADER_CH_BAR;

    level = grid_fader_clamp_level(level, type, w, h);

    if (script < 0 || script > INIT_SCRIPT) script = -1;

    GFC.enabled = true;
    GFC.group = group;
    GFC.x = x;
    GFC.y = y;
    GFC.w = w;
    GFC.h = h;
    GFC.level = level;
    GFC.script = script;
    GF.type = type;
}

static s16 grid_fader_max_value(scene_state_t *ss, u16 i) {
    switch (GF.type) {
        case FADER_CH_BAR:
        case FADER_CH_DOT: return GFC.w - 1;
        case FADER_CV_BAR:
        case FADER_CV_DOT: return GFC.h - 1;
        case FADER_FH_BAR:
        case FADER_FV_BAR:
        case FADER_FH_DOT:
        case FADER_FV_DOT: return GFC.level;
    }
    return 0;
}

static s16 grid_fader_clamp_level(s16 level, s16 type, s16 w, s16 h) {
    if (type > FADER_COARSE) {
        u8 size = (type == FADER_FH_BAR || type == FADER_FH_DOT) ? w : h;
        s16 maxlevel = ((size - 2) << 4) - 1;
        if (level < 0 || size < 3) return 0;
        if (level > maxlevel) return maxlevel;
    }
    else {
        if (level < (s16)LED_OFF) return LED_OFF;
        if (level > (s16)15) return 15;
    }
    return level;
}