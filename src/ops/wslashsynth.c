#include "ops/wslashsynth.h"

#include "helpers.h"
#include "i2c.h"
#include "ii.h"
#include "teletype_io.h"

I2C_GET_8_16(op_WS_S_VEL_get, WS_ADDR_6, WS_S_VEL)
I2C_GET_8_16(op_WS_S_PITCH_get, WS_ADDR_6, WS_S_PITCH)

I2C_GET_8_16_16(op_WS_S_VOX_get, WS_ADDR_6, WS_S_VOX)
I2C_GET_16_16(op_WS_S_NOTE_get, WS_ADDR_6, WS_S_NOTE)

I2C_GET_8(op_WS_S_AR_MODE_get, WS_ADDR_6, WS_S_AR_MODE)

I2C_GET_16(op_WS_S_CURVE_get, WS_ADDR_6, WS_S_CURVE)
I2C_GET_16(op_WS_S_RAMP_get, WS_ADDR_6, WS_S_RAMP)

I2C_GET_16(op_WS_S_FM_INDEX_get, WS_ADDR_6, WS_S_FM_INDEX)
I2C_GET_16(op_WS_S_FM_ENV_get, WS_ADDR_6, WS_S_FM_ENV)
I2C_GET_16_16(op_WS_S_FM_RATIO_get, WS_ADDR_6, WS_S_FM_RATIO)

I2C_GET_16(op_WS_S_LPG_TIME_get, WS_ADDR_6, WS_S_LPG_TIME)
I2C_GET_16(op_WS_S_LPG_SYMMETRY_get, WS_ADDR_6, WS_S_LPG_SYMMETRY)

I2C_GET_8_8(op_WS_S_PATCH_get, WS_ADDR_6, WS_S_PATCH)
I2C_GET_8(op_WS_S_VOICES_get, WS_ADDR_6, WS_S_VOICES)

// clang-format off
const tele_op_t op_WS_S_PITCH  = MAKE_GET_OP(W/S.PITCH, op_WS_S_PITCH_get, 2, false);
const tele_op_t op_WS_S_VEL  = MAKE_GET_OP(W/S.VEL, op_WS_S_VEL_get, 2, false);

const tele_op_t op_WS_S_VOX  = MAKE_GET_OP(W/S.VOX, op_WS_S_VOX_get, 3, false);
const tele_op_t op_WS_S_NOTE  = MAKE_GET_OP(W/S.NOTE, op_WS_S_NOTE_get, 2, false);

const tele_op_t op_WS_S_AR_MODE  = MAKE_GET_OP(W/S.AR.MODE , op_WS_S_AR_MODE_get, 1, false);

const tele_op_t op_WS_S_CURVE  = MAKE_GET_OP(W/S.CURVE , op_WS_S_CURVE_get, 1, false);
const tele_op_t op_WS_S_RAMP  = MAKE_GET_OP(W/S.RAMP , op_WS_S_RAMP_get, 1, false);

const tele_op_t op_WS_S_FM_INDEX  = MAKE_GET_OP(W/S.FM.INDEX , op_WS_S_FM_INDEX_get, 1, false);
const tele_op_t op_WS_S_FM_ENV  = MAKE_GET_OP(W/S.FM.ENV , op_WS_S_FM_ENV_get, 1, false);
const tele_op_t op_WS_S_FM_RATIO  = MAKE_GET_OP(W/S.FM.RATIO , op_WS_S_FM_RATIO_get, 2, false);

const tele_op_t op_WS_S_LPG_TIME  = MAKE_GET_OP(W/S.LPG.TIME , op_WS_S_LPG_TIME_get, 1, false);
const tele_op_t op_WS_S_LPG_SYMMETRY  = MAKE_GET_OP(W/S.LPG.SYM , op_WS_S_LPG_SYMMETRY_get, 1, false);

const tele_op_t op_WS_S_PATCH  = MAKE_GET_OP(W/S.PATCH , op_WS_S_PATCH_get, 2, false);
const tele_op_t op_WS_S_VOICES  = MAKE_GET_OP(W/S.VOICES , op_WS_S_VOICES_get, 1, false);

// clang-format on
