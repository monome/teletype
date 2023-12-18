#include "ops/wslashsynth.h"

#include "helpers.h"
#include "i2c.h"
#include "ii.h"
#include "ops/wslash_shared.h"
#include "teletype_io.h"

#define WS_ADDR (wslash_unit == 2 ? WS_S_ADDR_2 : WS_S_ADDR)

I2C_WRITE_8_16(op_WS_S_VEL_write, WS_ADDR, WS_S_VEL)
I2C_WRITE_8_16(op_WS_S_PITCH_write, WS_ADDR, WS_S_PITCH)

I2C_WRITE_8_16_16(op_WS_S_VOX_write, WS_ADDR, WS_S_VOX)
I2C_WRITE_16_16(op_WS_S_NOTE_write, WS_ADDR, WS_S_NOTE)

I2C_WRITE_8(op_WS_S_AR_MODE_write, WS_ADDR, WS_S_AR_MODE)
I2C_RECV_8(op_WS_S_AR_MODE_recv, WS_ADDR, WS_S_AR_MODE)

I2C_WRITE_16(op_WS_S_CURVE_write, WS_ADDR, WS_S_CURVE)
I2C_RECV_16(op_WS_S_CURVE_recv, WS_ADDR, WS_S_CURVE)
I2C_WRITE_16(op_WS_S_RAMP_write, WS_ADDR, WS_S_RAMP)
I2C_RECV_16(op_WS_S_RAMP_recv, WS_ADDR, WS_S_RAMP)

I2C_WRITE_16(op_WS_S_FM_INDEX_write, WS_ADDR, WS_S_FM_INDEX)
I2C_RECV_16(op_WS_S_FM_INDEX_recv, WS_ADDR, WS_S_FM_INDEX)
I2C_WRITE_16(op_WS_S_FM_ENV_write, WS_ADDR, WS_S_FM_ENV)
I2C_RECV_16(op_WS_S_FM_ENV_recv, WS_ADDR, WS_S_FM_ENV)
I2C_WRITE_16_16(op_WS_S_FM_RATIO_write, WS_ADDR, WS_S_FM_RATIO)

I2C_WRITE_16(op_WS_S_LPG_TIME_write, WS_ADDR, WS_S_LPG_TIME)
I2C_RECV_16(op_WS_S_LPG_TIME_recv, WS_ADDR, WS_S_LPG_TIME)
I2C_WRITE_16(op_WS_S_LPG_SYMMETRY_write, WS_ADDR, WS_S_LPG_SYMMETRY)
I2C_RECV_16(op_WS_S_LPG_SYMMETRY_recv, WS_ADDR, WS_S_LPG_SYMMETRY)

I2C_WRITE_8_8(op_WS_S_PATCH_write, WS_ADDR, WS_S_PATCH)
I2C_WRITE_8(op_WS_S_VOICES_write, WS_ADDR, WS_S_VOICES)
I2C_RECV_8(op_WS_S_VOICES_recv, WS_ADDR, WS_S_VOICES)

static void op_WS_S_POLY_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_WS_S_POLY_RESET_get(const void *data, scene_state_t *ss,
                                   exec_state_t *es, command_state_t *cs);

// clang-format off

const tele_op_t op_WS_S_PITCH  = MAKE_GET_OP(W/S.PITCH, op_WS_S_PITCH_write, 2, false);
const tele_op_t op_WS_S_VEL  = MAKE_GET_OP(W/S.VEL, op_WS_S_VEL_write, 2, false);

const tele_op_t op_WS_S_VOX  = MAKE_GET_OP(W/S.VOX, op_WS_S_VOX_write, 3, false);
const tele_op_t op_WS_S_NOTE  = MAKE_GET_OP(W/S.NOTE, op_WS_S_NOTE_write, 2, false);

const tele_op_t op_WS_S_POLY       = MAKE_GET_OP(W/S.POLY      , op_WS_S_POLY_get      , 2, false);
const tele_op_t op_WS_S_POLY_RESET = MAKE_GET_OP(W/S.POLY.RESET, op_WS_S_POLY_RESET_get, 0, false);

const tele_op_t op_WS_S_AR_MODE  = MAKE_GET_SET_OP(W/S.AR.MODE , op_WS_S_AR_MODE_recv, op_WS_S_AR_MODE_write, 0, true);

const tele_op_t op_WS_S_CURVE  = MAKE_GET_SET_OP(W/S.CURVE , op_WS_S_CURVE_recv, op_WS_S_CURVE_write, 0, true);
const tele_op_t op_WS_S_RAMP  = MAKE_GET_SET_OP(W/S.RAMP , op_WS_S_RAMP_recv, op_WS_S_RAMP_write, 0, true);

const tele_op_t op_WS_S_FM_INDEX  = MAKE_GET_SET_OP(W/S.FM.INDEX , op_WS_S_FM_INDEX_recv, op_WS_S_FM_INDEX_write, 0, true);
const tele_op_t op_WS_S_FM_ENV  = MAKE_GET_SET_OP(W/S.FM.ENV , op_WS_S_FM_ENV_recv, op_WS_S_FM_ENV_write, 0, true);
const tele_op_t op_WS_S_FM_RATIO  = MAKE_GET_OP(W/S.FM.RATIO , op_WS_S_FM_RATIO_write, 2, false);

const tele_op_t op_WS_S_LPG_TIME  = MAKE_GET_SET_OP(W/S.LPG.TIME , op_WS_S_LPG_TIME_recv, op_WS_S_LPG_TIME_write, 0, true);
const tele_op_t op_WS_S_LPG_SYMMETRY  = MAKE_GET_SET_OP(W/S.LPG.SYM , op_WS_S_LPG_SYMMETRY_recv, op_WS_S_LPG_SYMMETRY_write, 0, true);

const tele_op_t op_WS_S_PATCH  = MAKE_GET_OP(W/S.PATCH , op_WS_S_PATCH_write, 2, false);
const tele_op_t op_WS_S_VOICES  = MAKE_GET_SET_OP(W/S.VOICES , op_WS_S_VOICES_recv, op_WS_S_VOICES_write, 0, true);

// clang-format on

static u8 note_count = 1;

static void op_WS_S_POLY_get(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pitch = cs_pop(cs);
    int16_t vel = cs_pop(cs);
    uint8_t d[] = { WS_S_NOTE, pitch >> 8, pitch & 0xff, vel >> 8, vel & 0xff };

    if (note_count < 4)
        tele_ii_tx(wslash_unit == 1 ? WS_S_ADDR : WS_S_ADDR_2, d, 5);
    else
        tele_ii_tx(wslash_unit == 2 ? WS_S_ADDR : WS_S_ADDR_2, d, 5);

    note_count = (note_count + 1) & 7;
}

static void op_WS_S_POLY_RESET_get(const void *NOTUSED(data),
                                   scene_state_t *NOTUSED(ss),
                                   exec_state_t *NOTUSED(es),
                                   command_state_t *NOTUSED(cs)) {
    note_count = 0;
}
