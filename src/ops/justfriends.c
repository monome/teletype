#include "ops/justfriends.h"

#include "helpers.h"
#include "ii.h"
#include "teletype.h"
#include "teletype_io.h"

static void mod_JF0_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command);
static void mod_JF1_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command);
static void mod_JF2_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command);
static void op_JF_TR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_JF_RMODE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_JF_RUN_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_JF_SHIFT_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_JF_VTR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_JF_MODE_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_JF_TICK_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_JF_VOX_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_JF_NOTE_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_JF_GOD_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_JF_TUNE_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_JF_QT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_JF_ADDR_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_JF_SEL_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_JF_POLY_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_JF_POLY_RESET_get(const void *data, scene_state_t *ss,
                                 exec_state_t *es, command_state_t *cs);
static void op_JF_PITCH_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_JF_SPEED_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_JF_TSC_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_JF_RAMP_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_JF_CURVE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_JF_FM_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_JF_TIME_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_JF_INTONE_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);

// clang-format off
const tele_mod_t mod_JF0          = MAKE_MOD(JF0, mod_JF0_func, 0);
const tele_mod_t mod_JF1          = MAKE_MOD(JF1, mod_JF1_func, 0);
const tele_mod_t mod_JF2          = MAKE_MOD(JF2, mod_JF2_func, 0);
const tele_op_t op_JF_TR          = MAKE_GET_OP(JF.TR        , op_JF_TR_get        , 2, false);
const tele_op_t op_JF_RMODE       = MAKE_GET_OP(JF.RMODE     , op_JF_RMODE_get     , 1, false);
const tele_op_t op_JF_RUN         = MAKE_GET_OP(JF.RUN       , op_JF_RUN_get       , 1, false);
const tele_op_t op_JF_SHIFT       = MAKE_GET_OP(JF.SHIFT     , op_JF_SHIFT_get     , 1, false);
const tele_op_t op_JF_VTR         = MAKE_GET_OP(JF.VTR       , op_JF_VTR_get       , 2, false);
const tele_op_t op_JF_MODE        = MAKE_GET_OP(JF.MODE      , op_JF_MODE_get      , 1, false);
const tele_op_t op_JF_TICK        = MAKE_GET_OP(JF.TICK      , op_JF_TICK_get      , 1, false);
const tele_op_t op_JF_VOX         = MAKE_GET_OP(JF.VOX       , op_JF_VOX_get       , 3, false);
const tele_op_t op_JF_NOTE        = MAKE_GET_OP(JF.NOTE      , op_JF_NOTE_get      , 2, false);
const tele_op_t op_JF_GOD         = MAKE_GET_OP(JF.GOD       , op_JF_GOD_get       , 1, false);
const tele_op_t op_JF_TUNE        = MAKE_GET_OP(JF.TUNE      , op_JF_TUNE_get      , 3, false);
const tele_op_t op_JF_QT          = MAKE_GET_OP(JF.QT        , op_JF_QT_get        , 1, false);
const tele_op_t op_JF_ADDR        = MAKE_GET_OP(JF.ADDR      , op_JF_ADDR_get      , 1, false);
const tele_op_t op_JF_SEL         = MAKE_GET_OP(JF.SEL       , op_JF_SEL_get       , 1, false);
const tele_op_t op_JF_POLY        = MAKE_GET_OP(JF.POLY      , op_JF_POLY_get      , 2, false);
const tele_op_t op_JF_POLY_RESET  = MAKE_GET_OP(JF.POLY.RESET, op_JF_POLY_RESET_get, 0, false);
const tele_op_t op_JF_PITCH       = MAKE_GET_OP(JF.PITCH     , op_JF_PITCH_get     , 2, false);
const tele_op_t op_JF_SPEED       = MAKE_GET_OP(JF.SPEED     , op_JF_SPEED_get     , 0, true);
const tele_op_t op_JF_TSC         = MAKE_GET_OP(JF.TSC       , op_JF_TSC_get       , 0, true);
const tele_op_t op_JF_RAMP        = MAKE_GET_OP(JF.RAMP      , op_JF_RAMP_get      , 0, true);
const tele_op_t op_JF_CURVE       = MAKE_GET_OP(JF.CURVE     , op_JF_CURVE_get     , 0, true);
const tele_op_t op_JF_FM          = MAKE_GET_OP(JF.FM        , op_JF_FM_get        , 0, true);
const tele_op_t op_JF_TIME        = MAKE_GET_OP(JF.TIME      , op_JF_TIME_get      , 0, true);
const tele_op_t op_JF_INTONE      = MAKE_GET_OP(JF.INTONE    , op_JF_INTONE_get    , 0, true);
// clang-format on

static u8 unit = JF_ADDR;
static u8 note_count = 1;

static void mod_JF0_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command) {
    u8 u = unit;
    process_command(ss, es, post_command);
    unit = (u == JF_ADDR) ? JF_ADDR_2 : JF_ADDR;
    process_command(ss, es, post_command);
    unit = u;
}

static void mod_JF1_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command) {
    u8 u = unit;
    unit = JF_ADDR;
    process_command(ss, es, post_command);
    unit = u;
}

static void mod_JF2_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command) {
    u8 u = unit;
    unit = JF_ADDR_2;
    process_command(ss, es, post_command);
    unit = u;
}

static void op_JF_SEL_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    if (cs_pop(cs) == 2) { unit = JF_ADDR_2; }
    else { unit = JF_ADDR; }
}

static void op_JF_TR_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    if (a == -1) {
        uint8_t d[] = { JF_TR, 0, b };
        tele_ii_tx(JF_ADDR, d, 3);
        tele_ii_tx(JF_ADDR_2, d, 3);
    }
    else if (a >= 7) {
        a = a - 6;
        uint8_t d[] = { JF_TR, a, b };
        if (unit == JF_ADDR) { tele_ii_tx(JF_ADDR_2, d, 3); }
        else { tele_ii_tx(JF_ADDR, d, 3); }
    }
    else {
        uint8_t d[] = { JF_TR, a, b };
        tele_ii_tx(unit, d, 3);
    }
}

static void op_JF_RMODE_get(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_RMODE, a };
    tele_ii_tx(unit, d, 2);
}

static void op_JF_RUN_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_RUN, a >> 8, a & 0xff };
    tele_ii_tx(unit, d, 3);
}

static void op_JF_SHIFT_get(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_SHIFT, a >> 8, a & 0xff };
    tele_ii_tx(unit, d, 3);
}

static void op_JF_VTR_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    if (a == -1) {
        uint8_t d[] = { JF_VTR, 0, b >> 8, b & 0xff };
        tele_ii_tx(JF_ADDR, d, 4);
        tele_ii_tx(JF_ADDR_2, d, 4);
    }
    else if (a >= 7) {
        a = a - 6;
        uint8_t d[] = { JF_VTR, a, b >> 8, b & 0xff };
        if (unit == JF_ADDR) { tele_ii_tx(JF_ADDR_2, d, 4); }
        else { tele_ii_tx(JF_ADDR, d, 4); }
    }
    else {
        uint8_t d[] = { JF_VTR, a, b >> 8, b & 0xff };
        tele_ii_tx(unit, d, 4);
    }
}

static void op_JF_MODE_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_MODE, a };
    tele_ii_tx(unit, d, 2);
}

static void op_JF_TICK_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_TICK, a };
    tele_ii_tx(unit, d, 2);
}

static void op_JF_VOX_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    if (a == -1) {
        uint8_t d[] = { JF_VOX, 0, b >> 8, b & 0xff, c >> 8, c & 0xff };
        tele_ii_tx(JF_ADDR, d, 6);
        tele_ii_tx(JF_ADDR_2, d, 6);
    }
    else if (a >= 7) {
        a = a - 6;
        uint8_t d[] = { JF_VOX, a, b >> 8, b & 0xff, c >> 8, c & 0xff };
        if (unit == JF_ADDR) { tele_ii_tx(JF_ADDR_2, d, 6); }
        else { tele_ii_tx(JF_ADDR, d, 6); }
    }
    else {
        uint8_t d[] = { JF_VOX, a, b >> 8, b & 0xff, c >> 8, c & 0xff };
        tele_ii_tx(unit, d, 6);
    }
}

static void op_JF_NOTE_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { JF_NOTE, a >> 8, a & 0xff, b >> 8, b & 0xff };
    tele_ii_tx(unit, d, 5);
}

static void op_JF_POLY_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { JF_NOTE, a >> 8, a & 0xff, b >> 8, b & 0xff };
    if (note_count < 7) {
        tele_ii_tx(unit, d, 5);
        note_count++;
    }
    else {
        if (unit == JF_ADDR) { tele_ii_tx(JF_ADDR_2, d, 5); }
        else { tele_ii_tx(JF_ADDR, d, 5); }
        note_count++;
        if (note_count > 12) { note_count = 1; }
    }
}

static void op_JF_POLY_RESET_get(const void *NOTUSED(data),
                                 scene_state_t *NOTUSED(ss),
                                 exec_state_t *NOTUSED(es),
                                 command_state_t *NOTUSED(cs)) {
    note_count = 1;
}

static void op_JF_GOD_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_GOD, a };
    tele_ii_tx(unit, d, 2);
}

static void op_JF_TUNE_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    if (a == -1) {
        uint8_t d[] = { JF_TUNE, 0, b, c };
        tele_ii_tx(JF_ADDR, d, 4);
        tele_ii_tx(JF_ADDR_2, d, 4);
    }
    else if (a >= 7) {
        a = a - 6;
        uint8_t d[] = { JF_TUNE, a, b, c };
        if (unit == JF_ADDR) { tele_ii_tx(JF_ADDR_2, d, 4); }
        else { tele_ii_tx(JF_ADDR, d, 4); }
    }
    else {
        uint8_t d[] = { JF_TUNE, a, b, c };
        tele_ii_tx(unit, d, 4);
    }
}

static void op_JF_QT_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_QT, a };
    tele_ii_tx(unit, d, 2);
}

static void op_JF_ADDR_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_ADDRESS, a };
    tele_ii_tx(unit, d, 2);
}

static void op_JF_PITCH_get(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    if (a == -1) {
        uint8_t d[] = { JF_PITCH, 0, b >> 8, b & 0xff };
        tele_ii_tx(JF_ADDR, d, 4);
        tele_ii_tx(JF_ADDR_2, d, 4);
    }
    else if (a >= 7) {
        a = a - 6;
        uint8_t d[] = { JF_PITCH, a, b >> 8, b & 0xff };
        if (unit == JF_ADDR) { tele_ii_tx(JF_ADDR_2, d, 6); }
        else { tele_ii_tx(JF_ADDR, d, 6); }
    }
    else {
        uint8_t d[] = { JF_PITCH, a, b >> 8, b & 0xff };
        tele_ii_tx(unit, d, 6);
    }
}

static void op_JF_SPEED_get(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { JF_SPEED | II_GET };
    tele_ii_tx(unit, d, 1);
    d[0] = 0;
    tele_ii_rx(unit, d, 1);
    cs_push(cs, d[0]);
}

static void op_JF_TSC_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { JF_TSC | II_GET };
    tele_ii_tx(unit, d, 1);
    d[0] = 0;
    tele_ii_rx(unit, d, 1);
    cs_push(cs, d[0]);
}

static void op_JF_RAMP_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { JF_RAMP | II_GET };
    tele_ii_tx(unit, d, 1);
    d[0] = 0;
    tele_ii_rx(unit, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}

static void op_JF_CURVE_get(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { JF_CURVE | II_GET };
    tele_ii_tx(unit, d, 1);
    d[0] = 0;
    tele_ii_rx(unit, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}

static void op_JF_FM_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { JF_FM | II_GET };
    tele_ii_tx(unit, d, 1);
    d[0] = 0;
    tele_ii_rx(unit, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}

static void op_JF_TIME_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { JF_TIME | II_GET };
    tele_ii_tx(unit, d, 1);
    d[0] = 0;
    tele_ii_rx(unit, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}

static void op_JF_INTONE_get(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t d[] = { JF_INTONE | II_GET };
    tele_ii_tx(unit, d, 1);
    d[0] = 0;
    tele_ii_rx(unit, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}