#include "ops/wslashtape.h"

#include "helpers.h"
#include "i2c.h"
#include "ii.h"
#include "teletype_io.h"

I2C_GET_8(op_WS_T_RECORD_get, WS_T_ADDR, WS_T_RECORD)
I2C_GET_8(op_WS_T_PLAY_get, WS_T_ADDR, WS_T_PLAY)
I2C_GET_0(op_WS_T_REV_get, WS_T_ADDR, WS_T_REV)
I2C_GET_16_16(op_WS_T_SPEED_get, WS_T_ADDR, WS_T_SPEED)
I2C_GET_16(op_WS_T_FREQ_get, WS_T_ADDR, WS_T_FREQ)
I2C_GET_16(op_WS_T_PRE_LEVEL_get, WS_T_ADDR, WS_T_PRE_LEVEL)
I2C_GET_16(op_WS_T_MONITOR_LEVEL_get, WS_T_ADDR, WS_T_MONITOR_LEVEL)
I2C_GET_16(op_WS_T_REC_LEVEL_get, WS_T_ADDR, WS_T_REC_LEVEL)
I2C_GET_8(op_WS_T_HEAD_ORDER_get, WS_T_ADDR, WS_T_HEAD_ORDER)
I2C_GET_0(op_WS_T_LOOP_START_get, WS_T_ADDR, WS_T_LOOP_START)
I2C_GET_0(op_WS_T_LOOP_END_get, WS_T_ADDR, WS_T_LOOP_END)
I2C_GET_8(op_WS_T_LOOP_ACTIVE_get, WS_T_ADDR, WS_T_LOOP_ACTIVE)
I2C_GET_8(op_WS_T_LOOP_SCALE_get, WS_T_ADDR, WS_T_LOOP_SCALE)
I2C_GET_8(op_WS_T_LOOP_NEXT_get, WS_T_ADDR, WS_T_LOOP_NEXT)
I2C_GET_16_16(op_WS_T_TIMESTAMP_get, WS_T_ADDR, WS_T_TIMESTAMP)
I2C_GET_16_16(op_WS_T_SEEK_get, WS_T_ADDR, WS_T_SEEK)

// clang-format off
const tele_op_t op_WS_T_RECORD = MAKE_GET_OP(W/T.REC, op_WS_T_RECORD_get, 1, false);
const tele_op_t op_WS_T_PLAY   = MAKE_GET_OP(W/T.PLAY, op_WS_T_PLAY_get, 1, false);
const tele_op_t op_WS_T_REV = MAKE_GET_OP(W/T.REV, op_WS_T_REV_get, 0, false);
const tele_op_t op_WS_T_SPEED   = MAKE_GET_OP(W/T.SPEED, op_WS_T_SPEED_get, 2, false);
const tele_op_t op_WS_T_FREQ   = MAKE_GET_OP(W/T.FREQ, op_WS_T_FREQ_get, 1, false);
const tele_op_t op_WS_T_PRE_LEVEL  = MAKE_GET_OP(W/T.PRE.LVL, op_WS_T_PRE_LEVEL_get, 1, false);
const tele_op_t op_WS_T_MONITOR_LEVEL  = MAKE_GET_OP(W/T.MONITOR.LVL, op_WS_T_MONITOR_LEVEL_get, 1, false);
const tele_op_t op_WS_T_REC_LEVEL  = MAKE_GET_OP(W/T.REC.LVL, op_WS_T_REC_LEVEL_get, 1, false);
const tele_op_t op_WS_T_HEAD_ORDER  = MAKE_GET_OP(W/T.HEAD.ORDER, op_WS_T_HEAD_ORDER_get, 1, false);
const tele_op_t op_WS_T_LOOP_START = MAKE_GET_OP(W/T.LOOP.START, op_WS_T_LOOP_START_get, 0, false);
const tele_op_t op_WS_T_LOOP_END  = MAKE_GET_OP(W/T.LOOP.END, op_WS_T_LOOP_END_get, 0, false);
const tele_op_t op_WS_T_LOOP_ACTIVE  = MAKE_GET_OP(W/T.LOOP.ACTIVE, op_WS_T_LOOP_ACTIVE_get, 1, false);
const tele_op_t op_WS_T_LOOP_SCALE  = MAKE_GET_OP(W/T.LOOP.SCALE, op_WS_T_LOOP_SCALE_get, 1, false);
const tele_op_t op_WS_T_LOOP_NEXT  = MAKE_GET_OP(W/T.LOOP.NEXT, op_WS_T_LOOP_NEXT_get, 1, false);
const tele_op_t op_WS_T_TIMESTAMP   = MAKE_GET_OP(W/T.TIME, op_WS_T_TIMESTAMP_get, 2, false);
const tele_op_t op_WS_T_SEEK  = MAKE_GET_OP(W/T.SEEK, op_WS_T_SEEK_get, 2, false);
// clang-format on
