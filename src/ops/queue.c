#include "ops/queue.h"

#include "helpers.h"

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

const tele_op_t op_Q = MAKE_GET_SET_OP(Q, op_Q_get, op_Q_set, 0, true);
const tele_op_t op_Q_AVG =
    MAKE_GET_SET_OP(Q.AVG, op_Q_AVG_get, op_Q_AVG_set, 0, true);
const tele_op_t op_Q_N = MAKE_GET_SET_OP(Q.N, op_Q_N_get, op_Q_N_set, 0, true);

static void op_Q_get(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    cs_push(cs, q[q_n - 1]);
}

static void op_Q_set(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t *q = ss->variables.q;
    for (int16_t i = Q_LENGTH - 1; i > 0; i--) { q[i] = q[i - 1]; }
    q[0] = cs_pop(cs);
}

static void op_Q_AVG_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int32_t avg = 0;
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    for (int16_t i = 0; i < q_n; i++) {
        avg += q[i];
    }
    avg = (avg * 2) / q_n;
    if (avg % 2)
        avg += 1;

    cs_push(cs, (int16_t)(avg / 2));
}

static void op_Q_AVG_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t *q = ss->variables.q;
    for (int16_t i = 0; i < Q_LENGTH; i++) { q[i] = a; }
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
