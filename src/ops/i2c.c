#include "ops/i2c.h"

#include <stdarg.h>

#include "helpers.h"
#include "teletype_io.h"

static void op_IIA_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_IIA_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_IIS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_IIS1_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_IIS2_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_IIS3_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_IISB1_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_IISB2_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_IISB3_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_IIQ_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_IIQ1_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_IIQ2_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_IIQ3_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_IIQB1_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_IIQB2_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_IIQB3_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_IIB_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_IIB1_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_IIB2_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_IIB3_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_IIBB1_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_IIBB2_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_IIBB3_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);

const tele_op_t op_IIA = MAKE_GET_SET_OP(IIA, op_IIA_get, op_IIA_set, 0, true);
const tele_op_t op_IIS = MAKE_GET_OP(IIS, op_IIS_get, 1, false);
const tele_op_t op_IIS1 = MAKE_GET_OP(IIS1, op_IIS1_get, 2, false);
const tele_op_t op_IIS2 = MAKE_GET_OP(IIS2, op_IIS2_get, 3, false);
const tele_op_t op_IIS3 = MAKE_GET_OP(IIS3, op_IIS3_get, 4, false);
const tele_op_t op_IISB1 = MAKE_GET_OP(IISB1, op_IISB1_get, 2, false);
const tele_op_t op_IISB2 = MAKE_GET_OP(IISB2, op_IISB2_get, 3, false);
const tele_op_t op_IISB3 = MAKE_GET_OP(IISB3, op_IISB3_get, 4, false);
const tele_op_t op_IIQ = MAKE_GET_OP(IIQ, op_IIQ_get, 1, true);
const tele_op_t op_IIQ1 = MAKE_GET_OP(IIQ1, op_IIQ1_get, 2, true);
const tele_op_t op_IIQ2 = MAKE_GET_OP(IIQ2, op_IIQ2_get, 3, true);
const tele_op_t op_IIQ3 = MAKE_GET_OP(IIQ3, op_IIQ3_get, 4, true);
const tele_op_t op_IIQB1 = MAKE_GET_OP(IIQB1, op_IIQB1_get, 2, true);
const tele_op_t op_IIQB2 = MAKE_GET_OP(IIQB2, op_IIQB2_get, 3, true);
const tele_op_t op_IIQB3 = MAKE_GET_OP(IIQB3, op_IIQB3_get, 4, true);
const tele_op_t op_IIB = MAKE_GET_OP(IIB, op_IIB_get, 1, true);
const tele_op_t op_IIB1 = MAKE_GET_OP(IIB1, op_IIB1_get, 2, true);
const tele_op_t op_IIB2 = MAKE_GET_OP(IIB2, op_IIB2_get, 3, true);
const tele_op_t op_IIB3 = MAKE_GET_OP(IIB3, op_IIB3_get, 4, true);
const tele_op_t op_IIBB1 = MAKE_GET_OP(IIBB1, op_IIBB1_get, 2, true);
const tele_op_t op_IIBB2 = MAKE_GET_OP(IIBB2, op_IIBB2_get, 3, true);
const tele_op_t op_IIBB3 = MAKE_GET_OP(IIBB3, op_IIBB3_get, 4, true);

static void send_words(scene_state_t *ss, command_state_t *cs, uint8_t count) {
    uint8_t length = (count << 1) + 1;
    uint8_t d[length];
    d[0] = cs_pop(cs) & 0xff;

    uint8_t index = 1;
    uint16_t value;

    for (uint8_t i = 0; i < count; i++) {
        value = cs_pop(cs);
        d[index++] = value >> 8;
        d[index++] = value & 0xff;
    }

    if (ss->i2c_op_address == -1) return;
    tele_ii_tx(ss->i2c_op_address, d, length);
}

static void send_bytes(scene_state_t *ss, command_state_t *cs, uint8_t count) {
    uint8_t length = count + 1;
    uint8_t d[length];
    d[0] = cs_pop(cs) & 0xff;

    uint8_t index = 1;
    uint16_t value;

    for (uint8_t i = 0; i < count; i++) {
        value = cs_pop(cs);
        d[index++] = value & 0xff;
    }

    if (ss->i2c_op_address == -1) return;
    tele_ii_tx(ss->i2c_op_address, d, length);
}

static void query_word(scene_state_t *ss, command_state_t *cs) {
    if (ss->i2c_op_address == -1) {
        cs_push(cs, 0);
        return;
    }

    uint8_t buffer[2] = { 0 };
    tele_ii_rx(ss->i2c_op_address, buffer, 2);
    int16_t value = (buffer[0] << 8) + buffer[1];
    cs_push(cs, value);
}

static void query_byte(scene_state_t *ss, command_state_t *cs) {
    if (ss->i2c_op_address == -1) {
        cs_push(cs, 0);
        return;
    }

    uint8_t buffer[1] = { 0 };
    tele_ii_rx(ss->i2c_op_address, buffer, 1);
    int16_t value = buffer[0];
    cs_push(cs, value);
}

static void op_IIA_get(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->i2c_op_address);
}

static void op_IIA_set(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    u16 address = cs_pop(cs);
    ss->i2c_op_address = address > 0x7f ? -1 : address;
}

static void op_IIS_get(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_words(ss, cs, 0);
}

static void op_IIS1_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_words(ss, cs, 1);
}

static void op_IIS2_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_words(ss, cs, 2);
}

static void op_IIS3_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_words(ss, cs, 3);
}

static void op_IISB1_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_bytes(ss, cs, 1);
}

static void op_IISB2_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_bytes(ss, cs, 2);
}

static void op_IISB3_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_bytes(ss, cs, 3);
}

static void op_IIQ_get(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_words(ss, cs, 0);
    query_word(ss, cs);
}

static void op_IIQ1_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_words(ss, cs, 1);
    query_word(ss, cs);
}

static void op_IIQ2_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_words(ss, cs, 2);
    query_word(ss, cs);
}

static void op_IIQ3_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_words(ss, cs, 3);
    query_word(ss, cs);
}

static void op_IIQB1_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_bytes(ss, cs, 1);
    query_word(ss, cs);
}

static void op_IIQB2_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_bytes(ss, cs, 2);
    query_word(ss, cs);
}

static void op_IIQB3_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_bytes(ss, cs, 3);
    query_word(ss, cs);
}

static void op_IIB_get(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_words(ss, cs, 0);
    query_byte(ss, cs);
}

static void op_IIB1_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_words(ss, cs, 1);
    query_byte(ss, cs);
}

static void op_IIB2_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_words(ss, cs, 2);
    query_byte(ss, cs);
}

static void op_IIB3_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_words(ss, cs, 3);
    query_byte(ss, cs);
}

static void op_IIBB1_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_bytes(ss, cs, 1);
    query_byte(ss, cs);
}

static void op_IIBB2_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_bytes(ss, cs, 2);
    query_byte(ss, cs);
}

static void op_IIBB3_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    send_bytes(ss, cs, 3);
    query_byte(ss, cs);
}

void i2c_write_0(command_state_t *cs, uint8_t addr, uint8_t cmd) {
    uint8_t d[] = { cmd };
    tele_ii_tx(addr, d, 1);
}

void i2c_write_8(command_state_t *cs, uint8_t addr, uint8_t cmd) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { cmd, (uint8_t)(a & 0xff) };
    tele_ii_tx(addr, d, 2);
}

void i2c_write_8_8(command_state_t *cs, uint8_t addr, uint8_t cmd) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { cmd, (uint8_t)(a & 0xff), (uint8_t)(b & 0xff) };
    tele_ii_tx(addr, d, 3);
}

void i2c_write_8_16(command_state_t *cs, uint8_t addr, uint8_t cmd) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { cmd, (uint8_t)(a & 0xff), b >> 8, b & 0xff };
    tele_ii_tx(addr, d, 4);
}

void i2c_write_8_16_16(command_state_t *cs, uint8_t addr, uint8_t cmd) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    uint8_t d[] = {
        cmd, (uint8_t)(a & 0xff), b >> 8, b & 0xff, c >> 8, c & 0xff
    };
    tele_ii_tx(addr, d, 6);
}

void i2c_write_16(command_state_t *cs, uint8_t addr, uint8_t cmd) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { cmd, a >> 8, a & 0xff };
    tele_ii_tx(addr, d, 3);
}

void i2c_write_16_16(command_state_t *cs, uint8_t addr, uint8_t cmd) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { cmd, a >> 8, a & 0xff, b >> 8, b & 0xff };
    tele_ii_tx(addr, d, 5);
}

void i2c_write_32(command_state_t *cs, uint8_t addr, uint8_t cmd) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { cmd, a >> 8, a & 0xff, 0,
                    0 };  // currently used only for w/s.t which uses to last
                          // bytes to pass subseconds precission
    tele_ii_tx(addr, d, 5);
}

void i2c_recv_8(command_state_t *cs, uint8_t addr, uint8_t cmd) {
    i2c_write_0(cs, addr, cmd + 0x80);
    uint8_t buffer[1] = { 0 };
    tele_ii_rx(addr, buffer, 1);
    int16_t value = buffer[0];
    cs_push(cs, value);
}

void i2c_recv_16(command_state_t *cs, uint8_t addr, uint8_t cmd) {
    i2c_write_0(cs, addr, cmd + 0x80);
    uint8_t buffer[2] = { 0 };
    tele_ii_rx(addr, buffer, 2);
    int16_t value = (buffer[0] << 8) + buffer[1];
    cs_push(cs, value);
}