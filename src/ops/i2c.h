#ifndef _OPS_I2C_H_
#define _OPS_I2C_H_

#include "ops/op.h"

extern const tele_op_t op_IIA;
extern const tele_op_t op_IIS;
extern const tele_op_t op_IIS1;
extern const tele_op_t op_IIS2;
extern const tele_op_t op_IIS3;
extern const tele_op_t op_IISB1;
extern const tele_op_t op_IISB2;
extern const tele_op_t op_IISB3;
extern const tele_op_t op_IIQ;
extern const tele_op_t op_IIQ1;
extern const tele_op_t op_IIQ2;
extern const tele_op_t op_IIQ3;
extern const tele_op_t op_IIQB1;
extern const tele_op_t op_IIQB2;
extern const tele_op_t op_IIQB3;
extern const tele_op_t op_IIB;
extern const tele_op_t op_IIB1;
extern const tele_op_t op_IIB2;
extern const tele_op_t op_IIB3;
extern const tele_op_t op_IIBB1;
extern const tele_op_t op_IIBB2;
extern const tele_op_t op_IIBB3;

extern void i2c_get_0(command_state_t *cs, uint8_t addr, uint8_t cmd);
extern void i2c_get_8(command_state_t *cs, uint8_t addr, uint8_t cmd);
extern void i2c_get_8_8(command_state_t *cs, uint8_t addr, uint8_t cmd);
extern void i2c_get_8_16(command_state_t *cs, uint8_t addr, uint8_t cmd);
extern void i2c_get_8_16_16(command_state_t *cs, uint8_t addr, uint8_t cmd);
extern void i2c_get_16(command_state_t *cs, uint8_t addr, uint8_t cmd);
extern void i2c_get_16_16(command_state_t *cs, uint8_t addr, uint8_t cmd);
extern void i2c_get_32(command_state_t *cs, uint8_t addr, uint8_t cmd);

#define I2C_GET(name, addr, cmd, fn)                                        \
    static void name(const void *NOTUSED(data), scene_state_t *NOTUSED(ss), \
                     exec_state_t *NOTUSED(es), command_state_t *cs) {      \
        fn(cs, addr, cmd);                                                  \
    }

#define I2C_GET_0(name, addr, cmd) I2C_GET(name, addr, cmd, i2c_get_0)

#define I2C_GET_8(name, addr, cmd) I2C_GET(name, addr, cmd, i2c_get_8)

#define I2C_GET_8_8(name, addr, cmd) I2C_GET(name, addr, cmd, i2c_get_8_8)

#define I2C_GET_8_16(name, addr, cmd) I2C_GET(name, addr, cmd, i2c_get_8_16)

#define I2C_GET_8_16_16(name, addr, cmd) \
    I2C_GET(name, addr, cmd, i2c_get_8_16_16)

#define I2C_GET_16(name, addr, cmd) I2C_GET(name, addr, cmd, i2c_get_16)

#define I2C_GET_16_16(name, addr, cmd) I2C_GET(name, addr, cmd, i2c_get_16_16)

#define I2C_GET_32(name, addr, cmd) I2C_GET(name, addr, cmd, i2c_get_32)
#endif
