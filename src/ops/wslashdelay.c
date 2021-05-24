#include "ops/wslashdelay.h"

#include "helpers.h"
#include "i2c.h"
#include "ii.h"
#include "teletype_io.h"

I2C_WRITE_16(op_WS_D_FEEDBACK_write, WS_D_ADDR, WS_D_FEEDBACK)
I2C_RECV_16(op_WS_D_FEEDBACK_recv, WS_D_ADDR, WS_D_FEEDBACK)
I2C_WRITE_16(op_WS_D_MIX_write, WS_D_ADDR, WS_D_MIX)
I2C_RECV_16(op_WS_D_MIX_recv, WS_D_ADDR, WS_D_MIX)
I2C_WRITE_16(op_WS_D_LOWPASS_write, WS_D_ADDR, WS_D_LOWPASS)
I2C_RECV_16(op_WS_D_LOWPASS_recv, WS_D_ADDR, WS_D_LOWPASS)
I2C_WRITE_8(op_WS_D_FREEZE_write, WS_D_ADDR, WS_D_FREEZE)
I2C_RECV_8(op_WS_D_FREEZE_recv, WS_D_ADDR, WS_D_FREEZE)
I2C_WRITE_16(op_WS_D_TIME_write, WS_D_ADDR, WS_D_TIME)
I2C_RECV_16(op_WS_D_TIME_recv, WS_D_ADDR, WS_D_TIME)
I2C_WRITE_8_8(op_WS_D_LENGTH_write, WS_D_ADDR, WS_D_LENGTH)
I2C_WRITE_8_8(op_WS_D_POSITION_write, WS_D_ADDR, WS_D_POSITION)
I2C_WRITE_8_8(op_WS_D_CUT_write, WS_D_ADDR, WS_D_CUT)
I2C_WRITE_8(op_WS_D_FREQ_RANGE_write, WS_D_ADDR, WS_D_FREQ_RANGE)
I2C_WRITE_16(op_WS_D_RATE_write, WS_D_ADDR, WS_D_RATE)
I2C_RECV_16(op_WS_D_RATE_recv, WS_D_ADDR, WS_D_RATE)
I2C_WRITE_16(op_WS_D_FREQ_write, WS_D_ADDR, WS_D_FREQ)
I2C_RECV_16(op_WS_D_FREQ_recv, WS_D_ADDR, WS_D_FREQ)
I2C_WRITE_0(op_WS_D_CLK_write, WS_D_ADDR, WS_D_CLK)
I2C_WRITE_8_8(op_WS_D_CLK_RATIO_write, WS_D_ADDR, WS_D_CLK_RATIO)
I2C_WRITE_16(op_WS_D_PLUCK_write, WS_D_ADDR, WS_D_PLUCK)
I2C_WRITE_16(op_WS_D_MOD_RATE_write, WS_D_ADDR, WS_D_MOD_RATE)
I2C_RECV_16(op_WS_D_MOD_RATE_recv, WS_D_ADDR, WS_D_MOD_RATE)
I2C_WRITE_16(op_WS_D_MOD_AMOUNT_write, WS_D_ADDR, WS_D_MOD_AMOUNT)
I2C_RECV_16(op_WS_D_MOD_AMOUNT_recv, WS_D_ADDR, WS_D_MOD_AMOUNT)

// clang-format off
const tele_op_t op_WS_D_FEEDBACK = MAKE_GET_SET_OP(W/D.FBK, op_WS_D_FEEDBACK_recv, op_WS_D_FEEDBACK_write, 0, true);
const tele_op_t op_WS_D_MIX  = MAKE_GET_SET_OP(W/D.MIX, op_WS_D_MIX_recv, op_WS_D_MIX_write, 0, true);
const tele_op_t op_WS_D_LOWPASS = MAKE_GET_SET_OP(W/D.FILT, op_WS_D_LOWPASS_recv, op_WS_D_LOWPASS_write, 0, true);
const tele_op_t op_WS_D_FREEZE  = MAKE_GET_SET_OP(W/D.FREEZE, op_WS_D_FREEZE_recv, op_WS_D_FREEZE_write, 0, true);
const tele_op_t op_WS_D_TIME   = MAKE_GET_SET_OP(W/D.TIME, op_WS_D_TIME_recv, op_WS_D_TIME_write, 0, true);
const tele_op_t op_WS_D_LENGTH   = MAKE_GET_OP(W/D.LEN, op_WS_D_LENGTH_write, 2, false);
const tele_op_t op_WS_D_POSITION   = MAKE_GET_OP(W/D.POS, op_WS_D_POSITION_write, 2, false);
const tele_op_t op_WS_D_CUT   = MAKE_GET_OP(W/D.CUT, op_WS_D_CUT_write, 2, false);
const tele_op_t op_WS_D_FREQ_RANGE  = MAKE_GET_OP(W/D.FREQ.RNG, op_WS_D_FREQ_RANGE_write, 1, false);
const tele_op_t op_WS_D_RATE   = MAKE_GET_SET_OP(W/D.RATE, op_WS_D_RATE_recv, op_WS_D_RATE_write, 0, true);
const tele_op_t op_WS_D_FREQ   = MAKE_GET_SET_OP(W/D.FREQ, op_WS_D_FREQ_recv, op_WS_D_FREQ_write, 0, true);
const tele_op_t op_WS_D_CLK = MAKE_GET_OP(W/D.CLK, op_WS_D_CLK_write, 0, false);
const tele_op_t op_WS_D_CLK_RATIO  = MAKE_GET_OP(W/D.CLK.RATIO, op_WS_D_CLK_RATIO_write, 2, false);
const tele_op_t op_WS_D_PLUCK  = MAKE_GET_OP(W/D.PLUCK, op_WS_D_PLUCK_write, 1, false);
const tele_op_t op_WS_D_MOD_RATE  = MAKE_GET_SET_OP(W/D.MOD.RATE, op_WS_D_MOD_RATE_recv, op_WS_D_MOD_RATE_write, 0, true);
const tele_op_t op_WS_D_MOD_AMOUNT  = MAKE_GET_SET_OP(W/D.MOD.AMT, op_WS_D_MOD_AMOUNT_recv, op_WS_D_MOD_AMOUNT_write, 0, true);
// clang-format on
