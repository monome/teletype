#include "ops/crow.h"

#include "helpers.h"
#include "i2c.h"
#include "ii.h"
#include "teletype.h"
#include "teletype_io.h"


// helper macros for terse inline defns
#define CR_PROTO_MOD(name)                                                     \
    static void name(scene_state_t *ss, exec_state_t *es, command_state_t *cs, \
                     const tele_command_t *post_command)
#define CR_PROTO_GET(name)                                                  \
    static void name(const void *data, scene_state_t *ss, exec_state_t *es, \
                     command_state_t *cs)


// device selection ops & mods

static u8 unit = CROW_ADDR_0;

CR_PROTO_MOD(mod_CROWALL_func) {
    u8 u = unit;
    unit = CROW_ADDR_0;
    process_command(ss, es, post_command);
    unit = CROW_ADDR_1;
    process_command(ss, es, post_command);
    unit = CROW_ADDR_2;
    process_command(ss, es, post_command);
    unit = CROW_ADDR_3;
    process_command(ss, es, post_command);
    unit = u;
}
CR_PROTO_MOD(mod_CROW1_func) {
    u8 u = unit;
    unit = CROW_ADDR_0;
    process_command(ss, es, post_command);
    unit = u;
}
CR_PROTO_MOD(mod_CROW2_func) {
    u8 u = unit;
    unit = CROW_ADDR_1;
    process_command(ss, es, post_command);
    unit = u;
}
CR_PROTO_MOD(mod_CROW3_func) {
    u8 u = unit;
    unit = CROW_ADDR_2;
    process_command(ss, es, post_command);
    unit = u;
}
CR_PROTO_MOD(mod_CROW4_func) {
    u8 u = unit;
    unit = CROW_ADDR_3;
    process_command(ss, es, post_command);
    unit = u;
}
CR_PROTO_GET(op_CROW_SEL_get) {
    switch (cs_pop(cs)) {
        case 2: {
            unit = CROW_ADDR_1;
            break;
        }
        case 3: {
            unit = CROW_ADDR_2;
            break;
        }
        case 4: {
            unit = CROW_ADDR_3;
            break;
        }
        default: {
            unit = CROW_ADDR_0;
            break;
        }
    }
}


// send commands to crow

CR_PROTO_GET(op_CROW_V_get) {
    i2c_get_8_16(cs, unit, CROW_VOLTS);
}
CR_PROTO_GET(op_CROW_SLEW_get) {
    i2c_get_8_16(cs, unit, CROW_SLEW);
}
CR_PROTO_GET(op_CROW_CALL1_get) {
    i2c_get_16(cs, unit, CROW_CALL1);
}
CR_PROTO_GET(op_CROW_CALL2_get) {
    i2c_get_16_16(cs, unit, CROW_CALL2);
}
CR_PROTO_GET(op_CROW_CALL3_get) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    uint8_t d[] = { CROW_CALL3, a >> 8, a & 0xff, b >> 8,
                    b & 0xff,   c >> 8, c & 0xFF };
    tele_ii_tx(unit, d, 7);
}
CR_PROTO_GET(op_CROW_CALL4_get) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    int16_t e = cs_pop(cs);
    uint8_t d[] = { CROW_CALL4, a >> 8,   a & 0xff, b >> 8,  b & 0xff,
                    c >> 8,     c & 0xFF, e >> 8,   e & 0xFF };
    tele_ii_tx(unit, d, 9);
}
CR_PROTO_GET(op_CROW_RESET_get) {
    i2c_get_0(cs, unit, CROW_RESET);
}
CR_PROTO_GET(op_CROW_PULSE_get) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    int16_t e = cs_pop(cs);
    uint8_t d[] = { CROW_PULSE, a, b >> 8, b & 0xff, c >> 8, c & 0xFF, e };
    tele_ii_tx(unit, d, 7);
}
CR_PROTO_GET(op_CROW_AR_get) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    int16_t e = cs_pop(cs);
    uint8_t d[] = { CROW_AR, a,        b >> 8, b & 0xff,
                    c >> 8,  c & 0xFF, e >> 8, e & 0xFF };
    tele_ii_tx(unit, d, 8);
}
CR_PROTO_GET(op_CROW_LFO_get) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    int16_t e = cs_pop(cs);
    uint8_t d[] = { CROW_LFO, a,        b >> 8, b & 0xff,
                    c >> 8,   c & 0xFF, e >> 8, e & 0xFF };
    tele_ii_tx(unit, d, 8);
}


// get values from crow

CR_PROTO_GET(op_CROW_IN_get) {
    u8 d[] = { CROW_IN, cs_pop(cs) };
    tele_ii_tx(unit, d, 2);
    u8 r[2];
    tele_ii_rx(unit, r, 2);
    cs_push(cs, (r[0] << 8) + r[1]);
}
CR_PROTO_GET(op_CROW_OUT_get) {
    u8 d[] = { CROW_OUT, cs_pop(cs) };
    tele_ii_tx(unit, d, 2);
    u8 r[2];
    tele_ii_rx(unit, r, 2);
    cs_push(cs, (r[0] << 8) + r[1]);
}
CR_PROTO_GET(op_CROW_Q0_get) {
    u8 d[] = { CROW_QUERY0 };
    tele_ii_tx(unit, d, 1);
    u8 r[2];
    tele_ii_rx(unit, r, 2);
    cs_push(cs, (r[0] << 8) + r[1]);
}
CR_PROTO_GET(op_CROW_Q1_get) {
    u16 a = cs_pop(cs);
    u8 d[] = { CROW_QUERY1, a >> 8, a & 0xFF };
    tele_ii_tx(unit, d, 3);
    u8 r[2];
    tele_ii_rx(unit, r, 2);
    cs_push(cs, (r[0] << 8) + r[1]);
}
CR_PROTO_GET(op_CROW_Q2_get) {
    u16 a = cs_pop(cs);
    u16 b = cs_pop(cs);
    u8 d[] = { CROW_QUERY2, a >> 8, a & 0xFF, b >> 8, b & 0xFF };
    tele_ii_tx(unit, d, 5);
    u8 r[2];
    tele_ii_rx(unit, r, 2);
    cs_push(cs, (r[0] << 8) + r[1]);
}
CR_PROTO_GET(op_CROW_Q3_get) {
    u16 a = cs_pop(cs);
    u16 b = cs_pop(cs);
    u16 c = cs_pop(cs);
    u8 d[] = {
        CROW_QUERY2, a >> 8, a & 0xFF, b >> 8, b & 0xFF, c >> 8, c & 0xFF
    };
    tele_ii_tx(unit, d, 7);
    u8 r[2];
    tele_ii_rx(unit, r, 2);
    cs_push(cs, (r[0] << 8) + r[1]);
}

#undef CR_PROTO_GET
#undef CR_PROTO_MOD


// clang-format off
const tele_mod_t mod_CROWN    = MAKE_MOD(CROWN, mod_CROWALL_func, 0);
const tele_mod_t mod_CROW1    = MAKE_MOD(CROW1, mod_CROW1_func, 0);
const tele_mod_t mod_CROW2    = MAKE_MOD(CROW2, mod_CROW2_func, 0);
const tele_mod_t mod_CROW3    = MAKE_MOD(CROW3, mod_CROW3_func, 0);
const tele_mod_t mod_CROW4    = MAKE_MOD(CROW4, mod_CROW4_func, 0);
const tele_op_t op_CROW_SEL   = MAKE_GET_OP(CROW.SEL   , op_CROW_SEL_get    , 1, false);
const tele_op_t op_CROW_V     = MAKE_GET_OP(CROW.V     , op_CROW_V_get      , 2, false);
const tele_op_t op_CROW_SLEW  = MAKE_GET_OP(CROW.SLEW  , op_CROW_SLEW_get   , 2, false);
const tele_op_t op_CROW_C1    = MAKE_GET_OP(CROW.C1    , op_CROW_CALL1_get  , 1, false);
const tele_op_t op_CROW_C2    = MAKE_GET_OP(CROW.C2    , op_CROW_CALL2_get  , 2, false);
const tele_op_t op_CROW_C3    = MAKE_GET_OP(CROW.C3    , op_CROW_CALL3_get  , 3, false);
const tele_op_t op_CROW_C4    = MAKE_GET_OP(CROW.C4    , op_CROW_CALL4_get  , 4, false);
const tele_op_t op_CROW_RST   = MAKE_GET_OP(CROW.RST   , op_CROW_RESET_get  , 0, false);
const tele_op_t op_CROW_PULSE = MAKE_GET_OP(CROW.PULSE , op_CROW_PULSE_get  , 4, false);
const tele_op_t op_CROW_AR    = MAKE_GET_OP(CROW.AR    , op_CROW_AR_get     , 4, false);
const tele_op_t op_CROW_LFO   = MAKE_GET_OP(CROW.LFO   , op_CROW_LFO_get    , 4, false);
const tele_op_t op_CROW_IN    = MAKE_GET_OP(CROW.IN    , op_CROW_IN_get     , 1, true);
const tele_op_t op_CROW_OUT   = MAKE_GET_OP(CROW.OUT   , op_CROW_OUT_get    , 1, true);
const tele_op_t op_CROW_Q0    = MAKE_GET_OP(CROW.Q0    , op_CROW_Q0_get     , 0, true);
const tele_op_t op_CROW_Q1    = MAKE_GET_OP(CROW.Q1    , op_CROW_Q1_get     , 1, true);
const tele_op_t op_CROW_Q2    = MAKE_GET_OP(CROW.Q2    , op_CROW_Q2_get     , 2, true);
const tele_op_t op_CROW_Q3    = MAKE_GET_OP(CROW.Q3    , op_CROW_Q3_get     , 3, true);
// clang-format on
