#include "earthsea.h"

#include "helpers.h"
#include "ii.h"
#include "teletype_io.h"

static void op_ES_CV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);

const tele_op_t op_ES_PRESET = MAKE_SIMPLE_I2C_OP(ES.PRESET, ES_PRESET);
const tele_op_t op_ES_MODE = MAKE_SIMPLE_I2C_OP(ES.MODE, ES_MODE);
const tele_op_t op_ES_CLOCK = MAKE_SIMPLE_I2C_OP(ES.CLOCK, ES_CLOCK);
const tele_op_t op_ES_RESET = MAKE_SIMPLE_I2C_OP(ES.RESET, ES_RESET);
const tele_op_t op_ES_PATTERN = MAKE_SIMPLE_I2C_OP(ES.PATTERN, ES_PATTERN);
const tele_op_t op_ES_TRANS = MAKE_SIMPLE_I2C_OP(ES.TRANS, ES_TRANS);
const tele_op_t op_ES_STOP = MAKE_SIMPLE_I2C_OP(ES.STOP, ES_STOP);
const tele_op_t op_ES_TRIPLE = MAKE_SIMPLE_I2C_OP(ES.TRIPLE, ES_TRIPLE);
const tele_op_t op_ES_MAGIC = MAKE_SIMPLE_I2C_OP(ES.MAGIC, ES_MAGIC);
const tele_op_t op_ES_CV = MAKE_GET_OP(ES.CV, op_ES_CV_get, 1, true);

static void op_ES_CV_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    uint8_t d[] = { ES_CV | II_GET, a & 0x3 };
    uint8_t addr = ES;
    tele_ii_tx(addr, d, 2);
    d[0] = 0;
    d[1] = 0;
    tele_ii_rx(addr, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}
