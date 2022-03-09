#include "ops/hardware.h"

#include "helpers.h"
#include "ii.h"
#include "teletype_io.h"

static void op_CV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_CV_set(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_CV_SLEW_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_CV_SLEW_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_CV_OFF_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_CV_OFF_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_CV_CAL_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs);
static void op_CV_CAL_RESET_set(const void *NOTUSED(data), scene_state_t *ss,
                                exec_state_t *NOTUSED(es), command_state_t *cs);
static void op_IN_get(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *cs);
static void op_IN_SCALE_set(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs);
static void op_IN_CAL_MIN_set(const void *NOTUSED(data), scene_state_t *ss,
                              exec_state_t *NOTUSED(es), command_state_t *cs);
static void op_IN_CAL_MAX_set(const void *NOTUSED(data), scene_state_t *ss,
                              exec_state_t *NOTUSED(es), command_state_t *cs);
static void op_IN_CAL_RESET_set(const void *NOTUSED(data), scene_state_t *ss,
                                exec_state_t *NOTUSED(es), command_state_t *cs);
static void op_PARAM_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs);
static void op_PARAM_SCALE_set(const void *NOTUSED(data), scene_state_t *ss,
                               exec_state_t *NOTUSED(es), command_state_t *cs);
static void op_PARAM_CAL_MIN_set(const void *NOTUSED(data), scene_state_t *ss,
                                 exec_state_t *NOTUSED(es),
                                 command_state_t *cs);
static void op_PARAM_CAL_MAX_set(const void *NOTUSED(data), scene_state_t *ss,
                                 exec_state_t *NOTUSED(es),
                                 command_state_t *cs);
static void op_PARAM_CAL_RESET_set(const void *NOTUSED(data), scene_state_t *ss,
                                   exec_state_t *NOTUSED(es),
                                   command_state_t *cs);
static void op_TR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_TR_set(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_TR_POL_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_TR_POL_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_TR_TIME_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_TR_TIME_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_TR_TOG_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_TR_PULSE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_CV_GET_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_CV_SET_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_MUTE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_MUTE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_STATE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_DEVICE_FLIP_get(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs);
static void op_LIVE_OFF_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_LIVE_DASH_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_LIVE_GRID_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_LIVE_VARS_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_PRINT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_PRINT_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);


// clang-format off
const tele_op_t op_CV       = MAKE_GET_SET_OP(CV      , op_CV_get      , op_CV_set     , 1, true);
const tele_op_t op_CV_OFF   = MAKE_GET_SET_OP(CV.OFF  , op_CV_OFF_get  , op_CV_OFF_set , 1, true);
const tele_op_t op_CV_SLEW  = MAKE_GET_SET_OP(CV.SLEW , op_CV_SLEW_get , op_CV_SLEW_set, 1, true);
const tele_op_t op_CV_CAL   = MAKE_GET_OP(CV.CAL , op_CV_CAL_set, 3, true);
const tele_op_t op_CV_CAL_RESET = MAKE_GET_OP(CV.CAL.RESET , op_CV_CAL_RESET_set, 1, true);
const tele_op_t op_IN       = MAKE_GET_OP    (IN      , op_IN_get      , 0, true);
const tele_op_t op_IN_SCALE = MAKE_GET_OP    (IN.SCALE, op_IN_SCALE_set, 2, false);
const tele_op_t op_PARAM    = MAKE_GET_OP    (PARAM   , op_PARAM_get   , 0, true);
const tele_op_t op_PARAM_SCALE = MAKE_GET_OP (PARAM.SCALE, op_PARAM_SCALE_set, 2, false);
const tele_op_t op_PRM      = MAKE_ALIAS_OP  (PRM     , op_PARAM_get   , NULL,           0, true);
const tele_op_t op_TR       = MAKE_GET_SET_OP(TR      , op_TR_get      , op_TR_set     , 1, true);
const tele_op_t op_TR_POL   = MAKE_GET_SET_OP(TR.POL  , op_TR_POL_get  , op_TR_POL_set , 1, true);
const tele_op_t op_TR_TIME  = MAKE_GET_SET_OP(TR.TIME , op_TR_TIME_get , op_TR_TIME_set, 1, true);
const tele_op_t op_TR_TOG   = MAKE_GET_OP    (TR.TOG  , op_TR_TOG_get  , 1, false);
const tele_op_t op_TR_PULSE = MAKE_GET_OP    (TR.PULSE, op_TR_PULSE_get, 1, false);
const tele_op_t op_TR_P     = MAKE_ALIAS_OP  (TR.P    , op_TR_PULSE_get, NULL, 1, false);
const tele_op_t op_CV_GET   = MAKE_GET_OP    (CV.GET  , op_CV_GET_get  , 1, true);
const tele_op_t op_CV_SET   = MAKE_GET_OP    (CV.SET  , op_CV_SET_get  , 2, false);
const tele_op_t op_MUTE     = MAKE_GET_SET_OP(MUTE    , op_MUTE_get    , op_MUTE_set   , 1, true);
const tele_op_t op_STATE    = MAKE_GET_OP    (STATE   , op_STATE_get   , 1, true );
const tele_op_t op_IN_CAL_MIN    = MAKE_GET_OP (IN.CAL.MIN, op_IN_CAL_MIN_set, 0, true);
const tele_op_t op_IN_CAL_MAX    = MAKE_GET_OP (IN.CAL.MAX, op_IN_CAL_MAX_set, 0, true);
const tele_op_t op_IN_CAL_RESET  = MAKE_GET_OP (IN.CAL.RESET, op_IN_CAL_RESET_set, 0, false);
const tele_op_t op_PARAM_CAL_MIN = MAKE_GET_OP (PARAM.CAL.MIN, op_PARAM_CAL_MIN_set, 0, true);
const tele_op_t op_PARAM_CAL_MAX = MAKE_GET_OP (PARAM.CAL.MAX, op_PARAM_CAL_MAX_set, 0, true);
const tele_op_t op_PARAM_CAL_RESET  = MAKE_GET_OP (PARAM.CAL.RESET, op_PARAM_CAL_RESET_set, 0, false);
const tele_op_t op_DEVICE_FLIP   = MAKE_GET_OP (DEVICE.FLIP, op_DEVICE_FLIP_get, 0, false);
const tele_op_t op_LIVE_OFF      = MAKE_GET_OP (LIVE.OFF, op_LIVE_OFF_get, 0, false);
const tele_op_t op_LIVE_O        = MAKE_ALIAS_OP (LIVE.O, op_LIVE_OFF_get, NULL, 0, false);
const tele_op_t op_LIVE_DASH     = MAKE_GET_OP (LIVE.DASH, op_LIVE_DASH_get, 1, false);
const tele_op_t op_LIVE_D        = MAKE_ALIAS_OP (LIVE.D, op_LIVE_DASH_get, NULL, 1, false);
const tele_op_t op_LIVE_GRID     = MAKE_GET_OP (LIVE.GRID, op_LIVE_GRID_get, 0, false);
const tele_op_t op_LIVE_G        = MAKE_ALIAS_OP (LIVE.G, op_LIVE_GRID_get, NULL, 0, false);
const tele_op_t op_LIVE_VARS     = MAKE_GET_OP (LIVE.VARS, op_LIVE_VARS_get, 0, false);
const tele_op_t op_LIVE_V        = MAKE_ALIAS_OP (LIVE.V, op_LIVE_VARS_get, NULL, 0, false);
const tele_op_t op_PRINT         = MAKE_GET_SET_OP (PRINT, op_PRINT_get, op_PRINT_set, 1, true);
const tele_op_t op_PRT           = MAKE_ALIAS_OP (PRT, op_PRINT_get, op_PRINT_set, 1, true);
// clang-format on

static void op_CV_get(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    if (a < 0)
        cs_push(cs, 0);
    else if (a < 4)
        cs_push(cs, ss->variables.cv[a]);
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_CV | II_GET, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 2);
        d[0] = 0;
        d[1] = 0;
        tele_ii_rx(addr, d, 2);
        cs_push(cs, (d[0] << 8) + d[1]);
    }
    else
        cs_push(cs, 0);
}

static void op_CV_set(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    b = normalise_value(0, 16383, 0, b);
    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        ss->variables.cv[a] = b;
        tele_cv(a, b, 1);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_CV, a & 0x3, b >> 8, b & 0xff };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);

        tele_ii_tx(addr, d, 4);
    }
}

static void op_CV_SLEW_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    if (a < 0)
        cs_push(cs, 0);
    else if (a < 4)
        cs_push(cs, ss->variables.cv_slew[a]);
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_CV_SLEW | II_GET, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 2);
        d[0] = 0;
        d[1] = 0;
        tele_ii_rx(addr, d, 2);
        cs_push(cs, (d[0] << 8) + d[1]);
    }
    else
        cs_push(cs, 0);
}

static void op_CV_SLEW_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    b = normalise_value(1, 32767, 0, b);  // min slew = 1
    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        ss->variables.cv_slew[a] = b;
        tele_cv_slew(a, b);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_CV_SLEW, a & 0x3, b >> 8, b & 0xff };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 4);
    }
}

static void op_CV_OFF_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    if (a < 0)
        cs_push(cs, 0);
    else if (a < 4)
        cs_push(cs, ss->variables.cv_off[a]);
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_CV_OFF | II_GET, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 2);
        d[0] = 0;
        d[1] = 0;
        tele_ii_rx(addr, d, 2);
        cs_push(cs, (d[0] << 8) + d[1]);
    }
    else
        cs_push(cs, 0);
}

static void op_CV_OFF_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        ss->variables.cv_off[a] = b;
        tele_cv_off(a, b);
        tele_cv(a, ss->variables.cv[a], 1);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_CV_OFF, a & 0x3, b >> 8, b & 0xff };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 4);
    }
}

static void op_CV_CAL_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t n = cs_pop(cs);
    int16_t vv1v = cs_pop(cs);
    int16_t vv3v = cs_pop(cs);
    n -= 1;
    if (n < 0 || n > 3) { return; }

    // todo: calc these from vv1v and vv3v
    int16_t min = 0;
    int16_t max = 16383;

    ss_set_cv_scale(ss, n, min, max);
}

static void op_CV_CAL_RESET_set(const void *NOTUSED(data), scene_state_t *ss,
                                exec_state_t *NOTUSED(es),
                                command_state_t *cs) {
    int16_t n = cs_pop(cs);
    n -= 1;
    if (n < 0 || n > 3) { return; }

    ss_set_cv_scale(ss, n, 0, 16383);
}


static void op_IN_get(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    tele_update_adc(0);
    cs_push(cs, ss_get_in(ss));
}

static void op_IN_SCALE_set(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t min = cs_pop(cs);
    int16_t max = cs_pop(cs);
    ss_set_in_scale(ss, min, max);
}

static void op_IN_CAL_MIN_set(const void *NOTUSED(data), scene_state_t *ss,
                              exec_state_t *NOTUSED(es), command_state_t *cs) {
    ss_set_in_min(ss, ss->variables.in);
    cs_push(cs, ss->variables.in);
}

static void op_IN_CAL_MAX_set(const void *NOTUSED(data), scene_state_t *ss,
                              exec_state_t *NOTUSED(es), command_state_t *cs) {
    ss_set_in_max(ss, ss->variables.in);
    cs_push(cs, ss->variables.in);
}

static void op_IN_CAL_RESET_set(const void *NOTUSED(data), scene_state_t *ss,
                                exec_state_t *NOTUSED(es),
                                command_state_t *NOTUSED(cs)) {
    ss_reset_in_cal(ss);
}


static void op_PARAM_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    tele_update_adc(0);
    cs_push(cs, ss_get_param(ss));
}

static void op_PARAM_SCALE_set(const void *NOTUSED(data), scene_state_t *ss,
                               exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t min = cs_pop(cs);
    int16_t max = cs_pop(cs);
    ss_set_param_scale(ss, min, max);
}

static void op_PARAM_CAL_MIN_set(const void *NOTUSED(data), scene_state_t *ss,
                                 exec_state_t *NOTUSED(es),
                                 command_state_t *cs) {
    ss_set_param_min(ss, ss->variables.param);
    cs_push(cs, ss->variables.param);
}

static void op_PARAM_CAL_MAX_set(const void *NOTUSED(data), scene_state_t *ss,
                                 exec_state_t *NOTUSED(es),
                                 command_state_t *cs) {
    ss_set_param_max(ss, ss->variables.param);
    cs_push(cs, ss->variables.param);
}

static void op_PARAM_CAL_RESET_set(const void *NOTUSED(data), scene_state_t *ss,
                                   exec_state_t *NOTUSED(es),
                                   command_state_t *cs) {
    ss_reset_param_cal(ss);
}

static void op_TR_get(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    if (a < 0)
        cs_push(cs, 0);
    else if (a < 4)
        cs_push(cs, ss->variables.tr[a]);
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR | II_GET, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 2);
        d[0] = 0;
        tele_ii_rx(addr, d, 1);
        cs_push(cs, d[0]);
    }
    else
        cs_push(cs, 0);
}

static void op_TR_set(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        ss->variables.tr[a] = b != 0;
        tele_tr(a, b);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR, a & 0x3, b };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 3);
    }
}

static void op_TR_POL_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    if (a < 0)
        cs_push(cs, 0);
    else if (a < 4)
        cs_push(cs, ss->variables.tr_pol[a]);
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR_POL | II_GET, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 2);
        d[0] = 0;
        tele_ii_rx(addr, d, 1);
        cs_push(cs, d[0]);
    }
    else
        cs_push(cs, 0);
}

static void op_TR_POL_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    a--;
    if (a < 0)
        return;
    else if (a < 4) { ss->variables.tr_pol[a] = b > 0; }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR_POL, a & 0x3, b > 0 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 3);
    }
}

static void op_TR_TIME_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    if (a < 0)
        cs_push(cs, 0);
    else if (a < 4)
        cs_push(cs, ss->variables.tr_time[a]);
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR_TIME | II_GET, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 2);
        d[0] = 0;
        d[1] = 0;
        tele_ii_rx(addr, d, 2);
        cs_push(cs, (d[0] << 8) + d[1]);
    }
    else
        cs_push(cs, 0);
}

static void op_TR_TIME_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    if (b < 0) b = 0;
    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        ss->variables.tr_time[a] = b;
        tele_tr_pulse_time(a, b);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR_TIME, a & 0x3, b >> 8, b & 0xff };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 4);
    }
}

static void op_TR_TOG_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        if (ss->variables.tr[a])
            ss->variables.tr[a] = 0;
        else
            ss->variables.tr[a] = 1;
        tele_tr(a, ss->variables.tr[a]);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR_TOG, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 2);
    }
}

static void op_TR_PULSE_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        int16_t time = ss->variables.tr_time[a];  // pulse time
        if (time <= 0) return;  // if time <= 0 don't do anything
        ss->variables.tr[a] = ss->variables.tr_pol[a];
        tele_tr(a, ss->variables.tr[a]);
        tele_tr_pulse(a, time);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR_PULSE, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 2);
    }
}

static void op_CV_GET_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint8_t i = cs_pop(cs) - 1;
    cs_push(cs, i < 4 ? tele_get_cv(i) : 0);
}

static void op_CV_SET_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);

    if (b < 0)
        b = 0;
    else if (b > 16383)
        b = 16383;

    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        ss->variables.cv[a] = b;
        tele_cv(a, b, 0);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_CV_SET, a & 0x3, b >> 8, b & 0xff };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 4);
    }
}

static void op_MUTE_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs) - 1;
    if (a >= 0 && a < TRIGGER_INPUTS) { cs_push(cs, ss_get_mute(ss, a)); }
    else { cs_push(cs, 0); }
}

static void op_MUTE_set(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs) - 1;
    bool b = cs_pop(cs) > 0;
    if (a >= 0 && a < TRIGGER_INPUTS) { ss_set_mute(ss, a, b); }
}

static void op_STATE_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    a--;
    if (a < 0)
        cs_push(cs, 0);
    else if (a < 8)
        cs_push(cs, tele_get_input_state(a));
    else if (a < 24) {
        uint8_t d[] = { II_ANSIBLE_INPUT | II_GET, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 8) >> 2) << 1);
        tele_ii_tx(addr, d, 2);
        d[0] = 0;
        tele_ii_rx(addr, d, 1);
        cs_push(cs, d[0]);
    }
    else
        cs_push(cs, 0);
}

static void op_DEVICE_FLIP_get(const void *NOTUSED(data),
                               scene_state_t *NOTUSED(ss),
                               exec_state_t *NOTUSED(es), command_state_t *cs) {
    device_flip();
}

static void op_LIVE_OFF_get(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    set_live_submode(SUB_MODE_OFF);
}

static void op_LIVE_DASH_get(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    select_dash_screen(cs_pop(cs) - 1);
}

static void op_LIVE_GRID_get(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    set_live_submode(SUB_MODE_GRID);
}

static void op_LIVE_VARS_get(const void *NOTUSED(data),
                             scene_state_t *NOTUSED(ss),
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    set_live_submode(SUB_MODE_VARS);
}

static void op_PRINT_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t index = cs_pop(cs);
    cs_push(cs, get_dashboard_value(index - 1));
}

static void op_PRINT_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t index = cs_pop(cs);
    int16_t value = cs_pop(cs);
    print_dashboard_value(index - 1, value);
}
