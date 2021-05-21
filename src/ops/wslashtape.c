#include "ops/wslashtape.h"

#include "helpers.h"
#include "i2c.h"
#include "ii.h"
#include "teletype_io.h"

I2C_WRITE_8(op_WS_T_RECORD_write, WS_T_ADDR, WS_T_RECORD)
I2C_RECV_8(op_WS_T_RECORD_recv, WS_T_ADDR, WS_T_RECORD)
I2C_WRITE_8(op_WS_T_PLAY_write, WS_T_ADDR, WS_T_PLAY)
I2C_RECV_8(op_WS_T_PLAY_recv, WS_T_ADDR, WS_T_PLAY)
I2C_WRITE_0(op_WS_T_REV_write, WS_T_ADDR, WS_T_REV)
I2C_WRITE_16_16(op_WS_T_SPEED_write, WS_T_ADDR, WS_T_SPEED)
I2C_WRITE_16(op_WS_T_FREQ_write, WS_T_ADDR, WS_T_FREQ)
I2C_RECV_16(op_WS_T_FREQ_recv, WS_T_ADDR, WS_T_FREQ)
I2C_WRITE_16(op_WS_T_PRE_LEVEL_write, WS_T_ADDR, WS_T_PRE_LEVEL)
I2C_RECV_16(op_WS_T_PRE_LEVEL_recv, WS_T_ADDR, WS_T_PRE_LEVEL)
I2C_WRITE_16(op_WS_T_MONITOR_LEVEL_write, WS_T_ADDR, WS_T_MONITOR_LEVEL)
I2C_RECV_16(op_WS_T_MONITOR_LEVEL_recv, WS_T_ADDR, WS_T_MONITOR_LEVEL)
I2C_WRITE_16(op_WS_T_REC_LEVEL_write, WS_T_ADDR, WS_T_REC_LEVEL)
I2C_RECV_16(op_WS_T_REC_LEVEL_recv, WS_T_ADDR, WS_T_REC_LEVEL)
I2C_WRITE_8(op_WS_T_HEAD_ORDER_write, WS_T_ADDR, WS_T_HEAD_ORDER)
I2C_RECV_8(op_WS_T_HEAD_ORDER_recv, WS_T_ADDR, WS_T_HEAD_ORDER)
I2C_WRITE_0(op_WS_T_LOOP_START_write, WS_T_ADDR, WS_T_LOOP_START)
I2C_WRITE_0(op_WS_T_LOOP_END_write, WS_T_ADDR, WS_T_LOOP_END)
I2C_WRITE_8(op_WS_T_LOOP_ACTIVE_write, WS_T_ADDR, WS_T_LOOP_ACTIVE)
I2C_WRITE_8(op_WS_T_LOOP_SCALE_write, WS_T_ADDR, WS_T_LOOP_SCALE)
I2C_RECV_8(op_WS_T_LOOP_SCALE_recv, WS_T_ADDR, WS_T_LOOP_SCALE)
I2C_WRITE_8(op_WS_T_LOOP_NEXT_write, WS_T_ADDR, WS_T_LOOP_NEXT)
I2C_WRITE_16_16(op_WS_T_TIMESTAMP_write, WS_T_ADDR, WS_T_TIMESTAMP)
I2C_WRITE_16_16(op_WS_T_SEEK_write, WS_T_ADDR, WS_T_SEEK)
I2C_WRITE_0(op_WS_T_CLEARTAPE_write, WS_T_ADDR, WS_T_CLEARTAPE)

// clang-format off
const tele_op_t op_WS_T_RECORD          = MAKE_GET_SET_OP(W/T.REC           , op_WS_T_RECORD_recv       , op_WS_T_RECORD_write          , 0, true);
const tele_op_t op_WS_T_PLAY            = MAKE_GET_SET_OP(W/T.PLAY          , op_WS_T_PLAY_recv         , op_WS_T_PLAY_write            , 0, true);
const tele_op_t op_WS_T_FREQ            = MAKE_GET_SET_OP(W/T.FREQ          , op_WS_T_FREQ_recv         , op_WS_T_FREQ_write            , 0, true);
const tele_op_t op_WS_T_PRE_LEVEL       = MAKE_GET_SET_OP(W/T.ERASE.LVL     , op_WS_T_PRE_LEVEL_recv    , op_WS_T_PRE_LEVEL_write       , 0, true);
const tele_op_t op_WS_T_MONITOR_LEVEL   = MAKE_GET_SET_OP(W/T.MONITOR.LVL   , op_WS_T_MONITOR_LEVEL_recv, op_WS_T_MONITOR_LEVEL_write   , 0, true);
const tele_op_t op_WS_T_REC_LEVEL       = MAKE_GET_SET_OP(W/T.REC.LVL       , op_WS_T_REC_LEVEL_recv    , op_WS_T_REC_LEVEL_write       , 0, true);
const tele_op_t op_WS_T_HEAD_ORDER      = MAKE_GET_SET_OP(W/T.ECHOMODE      , op_WS_T_HEAD_ORDER_recv   , op_WS_T_HEAD_ORDER_write      , 0, true);
const tele_op_t op_WS_T_LOOP_SCALE      = MAKE_GET_SET_OP(W/T.LOOP.SCALE    , op_WS_T_LOOP_SCALE_recv   , op_WS_T_LOOP_SCALE_write      , 0, true);
const tele_op_t op_WS_T_REV             = MAKE_GET_OP(W/T.REV           , op_WS_T_REV_write         , 0, false);
const tele_op_t op_WS_T_SPEED           = MAKE_GET_OP(W/T.SPEED         , op_WS_T_SPEED_write       , 2, false);
const tele_op_t op_WS_T_LOOP_START      = MAKE_GET_OP(W/T.LOOP.START    , op_WS_T_LOOP_START_write  , 0, false);
const tele_op_t op_WS_T_LOOP_END        = MAKE_GET_OP(W/T.LOOP.END      , op_WS_T_LOOP_END_write    , 0, false);
const tele_op_t op_WS_T_LOOP_ACTIVE     = MAKE_GET_OP(W/T.LOOP.ACTIVE   , op_WS_T_LOOP_ACTIVE_write , 1, false);
const tele_op_t op_WS_T_LOOP_NEXT       = MAKE_GET_OP(W/T.LOOP.NEXT     , op_WS_T_LOOP_NEXT_write   , 1, false);
const tele_op_t op_WS_T_TIMESTAMP       = MAKE_GET_OP(W/T.TIME          , op_WS_T_TIMESTAMP_write   , 2, false);
const tele_op_t op_WS_T_SEEK            = MAKE_GET_OP(W/T.SEEK          , op_WS_T_SEEK_write        , 2, false);
const tele_op_t op_WS_T_CLEARTAPE       = MAKE_GET_OP(W/T.CLEARTAPE     , op_WS_T_CLEARTAPE_write   , 0, false);
// clang-format on
