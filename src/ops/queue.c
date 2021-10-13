#include "ops/queue.h"
#include <stdlib.h>  // rand
#include "helpers.h"
#include "state.h"
#include "teletype_io.h"

static void op_Q_get(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_Q_set(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_Q_AVG_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_AVG_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_N_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_Q_N_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_Q_CLR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_CLR_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_GRW_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_GRW_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_SUM_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_MIN_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_MIN_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_MAX_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_MAX_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_RND_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_RND_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_SRT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_SRT_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_REV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_SH_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_Q_SH_set(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_Q_ADD_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_ADD_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_SUB_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_SUB_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_MUL_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_MUL_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_DIV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_DIV_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_MOD_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_MOD_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_POP_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_REM_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_FND_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_I_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_Q_I_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_Q_2P_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_Q_2P_set(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_Q_P2_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_Q_P2_set(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);


const tele_op_t op_Q = MAKE_GET_SET_OP(Q, op_Q_get, op_Q_set, 0, true);
const tele_op_t op_Q_AVG =
    MAKE_GET_SET_OP(Q.AVG, op_Q_AVG_get, op_Q_AVG_set, 0, true);
const tele_op_t op_Q_N = MAKE_GET_SET_OP(Q.N, op_Q_N_get, op_Q_N_set, 0, true);
const tele_op_t op_Q_CLR =
    MAKE_GET_SET_OP(Q.CLR, op_Q_CLR_get, op_Q_CLR_set, 0, false);
const tele_op_t op_Q_GRW =
    MAKE_GET_SET_OP(Q.GRW, op_Q_GRW_get, op_Q_GRW_set, 0, true);
const tele_op_t op_Q_SUM = MAKE_GET_OP(Q.SUM, op_Q_SUM_get, 0, true);
const tele_op_t op_Q_MIN =
    MAKE_GET_SET_OP(Q.MIN, op_Q_MIN_get, op_Q_MIN_set, 0, true);
const tele_op_t op_Q_MAX =
    MAKE_GET_SET_OP(Q.MAX, op_Q_MAX_get, op_Q_MAX_set, 0, true);
const tele_op_t op_Q_RND =
    MAKE_GET_SET_OP(Q.RND, op_Q_RND_get, op_Q_RND_set, 0, true);
const tele_op_t op_Q_SRT =
    MAKE_GET_SET_OP(Q.SRT, op_Q_SRT_get, op_Q_SRT_set, 0, false);
const tele_op_t op_Q_REV = MAKE_GET_OP(Q.REV, op_Q_REV_get, 0, false);
const tele_op_t op_Q_SH =
    MAKE_GET_SET_OP(Q.SH, op_Q_SH_get, op_Q_SH_set, 0, false);
const tele_op_t op_Q_ADD =
    MAKE_GET_SET_OP(Q.ADD, op_Q_ADD_get, op_Q_ADD_set, 1, false);
const tele_op_t op_Q_SUB =
    MAKE_GET_SET_OP(Q.SUB, op_Q_SUB_get, op_Q_SUB_set, 1, false);
const tele_op_t op_Q_MUL =
    MAKE_GET_SET_OP(Q.MUL, op_Q_MUL_get, op_Q_MUL_set, 1, false);
const tele_op_t op_Q_DIV =
    MAKE_GET_SET_OP(Q.DIV, op_Q_DIV_get, op_Q_DIV_set, 1, false);
const tele_op_t op_Q_MOD =
    MAKE_GET_SET_OP(Q.MOD, op_Q_MOD_get, op_Q_MOD_set, 1, false);
const tele_op_t op_Q_POP = MAKE_GET_OP(Q.POP, op_Q_POP_get, 0, true);
const tele_op_t op_Q_REM = MAKE_GET_OP(Q.REM, op_Q_REM_get, 1, true);
const tele_op_t op_Q_FND = MAKE_GET_OP(Q.FND, op_Q_FND_get, 1, true);
const tele_op_t op_Q_I = MAKE_GET_SET_OP(Q.I, op_Q_I_get, op_Q_I_set, 1, true);
const tele_op_t op_Q_2P =
    MAKE_GET_SET_OP(Q.2P,  op_Q_2P_get,  op_Q_2P_set, 0, false);
const tele_op_t op_Q_P2 =
    MAKE_GET_SET_OP(Q.P2,  op_Q_P2_get,  op_Q_P2_set, 0, false);

int16_t q_find(int16_t needle, int16_t haystack[], int8_t end) {
    for (int8_t i = 0; i < end; i++) {
        if (needle == haystack[i]) { return i; }
    }
    return -1;
}

static void op_Q_get(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    cs_push(cs, q[q_n - 1]);
    if (ss->variables.q_grow && ss->variables.q_n > 1) { ss->variables.q_n--; }
}

static void op_Q_set(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    for (int8_t i = Q_LENGTH - 1; i > 0; i--) { q[i] = q[i - 1]; }
    q[0] = cs_pop(cs);
    if (ss->variables.q_grow && ss->variables.q_n < Q_LENGTH) {
        ss->variables.q_n++;
    }
}

static void op_Q_AVG_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int32_t avg = 0;
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    if (q_n == 0)
        cs_push(cs, 0);
    else {
        for (int8_t i = 0; i < q_n; i++) { avg += q[i]; }
        avg = (avg * 2) / q_n;
        if (avg % 2) avg += 1;
        cs_push(cs, (int16_t)(avg / 2));
    }
}

static void op_Q_AVG_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t *q = ss->variables.q;
    for (int8_t i = 0; i < Q_LENGTH; i++) { q[i] = a; }
}

static void op_Q_N_get(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->variables.q_n);
}

static void op_Q_N_set(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    if (a < 1)
        a = 1;
    else if (a > Q_LENGTH)
        a = Q_LENGTH;
    ss->variables.q_n = a;
}


static void op_Q_CLR_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    ss->variables.q_n = 1;
    for (int8_t i = 0; i < Q_LENGTH; i++) { q[i] = 0; }
}

static void op_Q_CLR_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    ss->variables.q_n = 1;
    for (int8_t i = 0; i < Q_LENGTH; i++) { q[i] = 0; }
    q[0] = cs_pop(cs);
}

static void op_Q_GRW_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->variables.q_grow);
}

static void op_Q_GRW_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    ss->variables.q_grow = cs_pop(cs);
    ss->variables.q_grow = ss->variables.q_grow < 1 ? 0 : 1;
    if (!ss->variables.q_grow && ss->variables.q_n < 1) {
        ss->variables.q_n = 1;
    }
}

static void op_Q_SUM_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t sum = 0;
    for (int8_t i = 0; i < q_n; i++) { sum += q[i]; }
    cs_push(cs, sum);
}

static void op_Q_MIN_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t min = INT16_MAX;
    for (int8_t i = 0; i < q_n; i++) {
        if (q[i] < min) { min = q[i]; }
    }
    cs_push(cs, min);
}


static void op_Q_MIN_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t min = cs_pop(cs);
    for (int8_t i = 0; i < q_n; i++) {
        if (q[i] < min) { q[i] = min; }
    }
}


static void op_Q_MAX_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t max = INT16_MIN;
    for (int8_t i = 0; i < q_n; i++) {
        if (q[i] > max) { max = q[i]; }
    }
    cs_push(cs, max);
}

static void op_Q_MAX_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t max = cs_pop(cs);
    for (int8_t i = 0; i < q_n; i++) {
        if (q[i] > max) { q[i] = max; }
    }
}


static void op_Q_RND_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    cs_push(cs, q[rand() % q_n]);
}


static void op_Q_RND_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t rnd = cs_pop(cs);
    int16_t tmp;
    int8_t a, b;

    if (rnd > 0) {
        // all elements random between 0 and rnd
        for (int8_t i = 0; i < q_n; i++) { q[i] = rand() % rnd; }
    }
    else if (rnd < 0) {
        // switch random elements rnd nb times
        rnd = rnd < (-3 * q_n) ? (-3 * q_n) : rnd;  // not more than 3*q_n times
        for (int16_t i = rnd; i < 0; i++) {
            a = rand() % q_n;
            b = rand() % q_n;
            tmp = q[a];
            q[a] = q[b];
            q[b] = tmp;
        }
    }
    else {
        // what to to with rnd = 0????
    }
}

static void op_Q_SRT_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int8_t min_idx = 0;
    int16_t tmp, min;

    for (int8_t i = 0; i < q_n; i++) {
        min = INT16_MAX;
        for (int8_t j = i; j < q_n; j++) {
            if (q[j] < min) {
                min = q[j];
                min_idx = j;
            }
        }
        tmp = q[min_idx];
        q[min_idx] = q[i];
        q[i] = tmp;
    }
}


static void op_Q_SRT_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int8_t min_idx = 0;
    int16_t tmp, min;
    int16_t bound = cs_pop(cs);
    int8_t lo, hi;
    if (bound > 0) {
        lo = 0;
        hi = bound > q_n ? q_n : bound;
    }
    else if (bound < 0) {
        hi = q_n;
        lo = -bound > q_n ? q_n : -bound;
    }
    else {
        lo = 0;
        hi = q_n;
    }
    for (int8_t i = lo; i < hi; i++) {
        min = INT16_MAX;
        for (int8_t j = i; j < hi; j++) {
            if (q[j] < min) {
                min = q[j];
                min_idx = j;
            }
        }
        tmp = q[min_idx];
        q[min_idx] = q[i];
        q[i] = tmp;
    }
}

static void op_Q_REV_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t tmp;

    for (int8_t i = 0; i < ((q_n - 0) / 2); i++) {
        tmp = q[i];
        q[i] = q[q_n - 1 - i];
        q[q_n - 1 - i] = tmp;
    }
}


static void op_Q_SH_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t tmp = q[q_n - 1];

    for (int8_t i = q_n - 1; i >= 0; i--) { q[i] = q[i - 1]; }
    q[0] = tmp;
}


static void op_Q_SH_set(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t nb_shifts = cs_pop(cs);
    int16_t tmp[q_n];
    if (nb_shifts > 0) { nb_shifts = nb_shifts % q_n; }
    else if (nb_shifts < 0) {
        nb_shifts = q_n - (-nb_shifts % q_n);
    }

    if (!nb_shifts) { return; }

    for (int8_t i = 0; i < q_n; i++) { tmp[i] = q[i]; }

    for (int8_t i = 0; i < q_n; i++) { q[(i + nb_shifts) % q_n] = tmp[i]; }
}

static void op_Q_ADD_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t add = cs_pop(cs);

    for (int8_t i = 0; i < q_n; i++) { q[i] = q[i] + add; }
}

static void op_Q_ADD_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t add = cs_pop(cs);
    int8_t i = cs_pop(cs);
    i = i < 0 ? 0 : i;
    i = i > q_n - 1 ? q_n - 1 : i;
    q[i] = q[i] + add;
}

static void op_Q_SUB_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t sub = cs_pop(cs);

    for (int8_t i = 0; i < q_n; i++) { q[i] = q[i] - sub; }
}

static void op_Q_SUB_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t sub = cs_pop(cs);
    int8_t i = cs_pop(cs);
    i = i < 0 ? 0 : i;
    i = i > q_n - 1 ? q_n - 1 : i;
    q[i] = q[i] - sub;
}

static void op_Q_MUL_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t mul = cs_pop(cs);

    for (int8_t i = 0; i < q_n; i++) { q[i] = q[i] * mul; }
}

static void op_Q_MUL_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t mul = cs_pop(cs);
    int8_t i = cs_pop(cs);
    i = i < 0 ? 0 : i;
    i = i > q_n - 1 ? q_n - 1 : i;
    q[i] = q[i] * mul;
}

static void op_Q_DIV_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t div = cs_pop(cs);
    if (div != 0) {
        for (int8_t i = 0; i < q_n; i++) { q[i] = q[i] / div; }
    }
}

static void op_Q_DIV_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t div = cs_pop(cs);
    int8_t i = cs_pop(cs);
    if (div != 0) {
        i = i < 0 ? 0 : i;
        i = i > q_n - 1 ? q_n - 1 : i;
        q[i] = q[i] / div;
    }
}

static void op_Q_MOD_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t mod = cs_pop(cs);
    if (mod != 0) {
        for (int8_t i = 0; i < q_n; i++) { q[i] = q[i] % mod; }
    }
}

static void op_Q_MOD_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    int16_t mod = cs_pop(cs);
    int8_t i = cs_pop(cs);
    if (mod != 0) {
        i = i < 0 ? 0 : i;
        i = i > q_n - 1 ? q_n - 1 : i;
        q[i] = q[i] % mod;
    }
}


static void op_Q_POP_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    cs_push(cs, q[0]);
    if (ss->variables.q_grow) {
        if (ss->variables.q_n > 1) { ss->variables.q_n--; }
        for (int8_t i = 0; i < Q_LENGTH - 1; i++) { q[i] = q[i + 1]; }
        q[Q_LENGTH - 1] = 0;
    }
}

static void op_Q_REM_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int8_t needle = cs_pop(cs);
    int8_t idx = q_find(needle, q, ss->variables.q_n);
    if (idx != -1) {
        if (ss->variables.q_n > 1) { ss->variables.q_n--; }
        for (int8_t i = idx; i < Q_LENGTH - 1; i++) { q[i] = q[i + 1]; }
        q[Q_LENGTH - 1] = 0;
    }
    cs_push(cs, idx);
}

static void op_Q_FND_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int8_t needle = cs_pop(cs);
    cs_push(cs, q_find(needle, q, ss->variables.q_n));
}

static void op_Q_I_get(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int8_t i = cs_pop(cs);
    i = i < 0 ? 0 : i;
    i = i > Q_LENGTH - 1 ? Q_LENGTH - 1 : i;
    cs_push(cs, q[i]);
}

static void op_Q_I_set(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int8_t i = cs_pop(cs);
    int16_t value = cs_pop(cs);
    i = i < 0 ? 0 : i;
    i = i > Q_LENGTH - 1 ? Q_LENGTH - 1 : i;
    q[i] = value;
}

static void op_Q_2P_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t pn = ss->variables.p_n;
    int8_t end_at = PATTERN_LENGTH < Q_LENGTH ? PATTERN_LENGTH : Q_LENGTH;
    for (int8_t i = 0; i < end_at; i++) { ss_set_pattern_val(ss, pn, i, q[i]); }
    tele_pattern_updated();
}

static void op_Q_2P_set(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t pn = cs_pop(cs);
    pn = pn < 0 ? 0 : pn;
    pn = pn > PATTERN_COUNT - 1 ? PATTERN_COUNT - 1 : pn;
    int8_t end_at = PATTERN_LENGTH < Q_LENGTH ? PATTERN_LENGTH : Q_LENGTH;
    for (int8_t i = 0; i < end_at; i++) { ss_set_pattern_val(ss, pn, i, q[i]); }
    tele_pattern_updated();
}

static void op_Q_P2_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t pn = ss->variables.p_n;
    int8_t end_at = PATTERN_LENGTH < Q_LENGTH ? PATTERN_LENGTH : Q_LENGTH;
    for (int8_t i = 0; i < end_at; i++) {
        q[i] = ss_get_pattern_val(ss, pn, i);
    }
}

static void op_Q_P2_set(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t pn = cs_pop(cs);
    pn = pn < 0 ? 0 : pn;
    pn = pn > PATTERN_COUNT - 1 ? PATTERN_COUNT - 1 : pn;
    int8_t end_at = PATTERN_LENGTH < Q_LENGTH ? PATTERN_LENGTH : Q_LENGTH;
    for (int8_t i = 0; i < end_at; i++) {
        q[i] = ss_get_pattern_val(ss, pn, i);
    }
}
