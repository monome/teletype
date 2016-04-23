#include <ctype.h>   // isdigit
#include <stddef.h>  // offsetof
#include <stdint.h>  // types
#include <stdio.h>   // printf
#include <stdlib.h>  // rand, strtol
#include <string.h>

#include "euclidean/euclidean.h"
#include "ii.h"
#include "table.h"
#include "teletype.h"
#include "util.h"

#ifdef SIM
#define DBG printf("%s", dbg);
#else
#define DBG print_dbg(dbg);
#endif

// http://stackoverflow.com/questions/3599160/unused-parameter-warnings-in-c-code
// Needs to be named NOTUSED to void conflict with UNUSED from
// libavr32/asf/avr32/utils/compiler.h
// Probably should put this macro somewhere else
#ifdef __GNUC__
#define NOTUSED(x) UNUSED_##x __attribute__((__unused__))
#else
#define NOTUSED(x) UNUSED_##x
#endif

static const char *errordesc[] = { "OK",
                                   WELCOME,
                                   "UNKNOWN WORD",
                                   "COMMAND TOO LONG",
                                   "NOT ENOUGH PARAMS",
                                   "TOO MANY PARAMS",
                                   "MOD NOT ALLOWED HERE",
                                   "EXTRA SEPARATOR",
                                   "NEED SEPARATOR",
                                   "BAD SEPARATOR",
                                   "MOVE LEFT" };

const char *tele_error(error_t e) {
    return errordesc[e];
}

// static char dbg[32];
static char pcmd[32];


char error_detail[16];

uint8_t mutes[8];

tele_pattern_t tele_patterns[4];

static char condition;

static tele_command_t tele_stack[TELE_STACK_SIZE];
static uint8_t tele_stack_top;

volatile update_metro_t update_metro;
volatile update_tr_t update_tr;
volatile update_cv_t update_cv;
volatile update_cv_slew_t update_cv_slew;
volatile update_delay_t update_delay;
volatile update_s_t update_s;
volatile update_cv_off_t update_cv_off;
volatile update_ii_t update_ii;
volatile update_scene_t update_scene;
volatile update_pi_t update_pi;
volatile update_kill_t update_kill;
volatile update_mute_t update_mute;
volatile update_input_t update_input;

volatile run_script_t run_script;

volatile uint8_t input_states[8];

const char *to_v(int16_t);

/////////////////////////////////////////////////////////////////
// STATE ////////////////////////////////////////////////////////

// eventually these will not be global variables
static scene_state_t scene_state = {
    // variables that haven't been explicitly initialised, will be set to 0
    .variables = {.a = 1,
                  .b = 2,
                  .c = 3,
                  .cv_slew = { 1, 1, 1, 1 },
                  .d = 4,
                  .drunk_min = 0,
                  .drunk_max = 255,
                  .m = 1000,
                  .m_act = 1,
                  .o_inc = 1,
                  .o_min = 0,
                  .o_max = 63,
                  .o_wrap = 1,
                  .q_n = 1,
                  .time_act = 1,
                  .tr_pol = { 1, 1, 1, 1 },
                  .tr_time = { 100, 100, 100, 100 } }
};
static exec_state_t exec_state;
static command_state_t command_state;

/////////////////////////////////////////////////////////////////
// STACK ////////////////////////////////////////////////////////

static int16_t pop(void);
static void push(int16_t);

static int16_t top;
static int16_t stack[STACK_SIZE];

int16_t pop() {
    top--;
    // sprintf(dbg,"\r\npop %d", stack[top]);
    return stack[top];
}

void push(int16_t data) {
    stack[top] = data;
    // sprintf(dbg,"\r\npush %d", stack[top]);
    top++;
}


/////////////////////////////////////////////////////////////////
// VARS ARRAYS //////////////////////////////////////////////////

// ENUM IN HEADER

static void op_M_get(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_M_set(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_M_ACT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_M_ACT_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_P_N_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_P_N_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_P_L_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_P_L_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_P_I_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_P_I_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_P_HERE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_P_HERE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_P_NEXT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_P_NEXT_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_P_PREV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_P_PREV_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_P_WRAP_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_P_WRAP_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_P_START_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_P_START_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_P_END_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_P_END_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_O_get(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_O_set(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_DRUNK_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_DRUNK_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_get(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_Q_set(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_Q_N_get(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_Q_N_set(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_Q_AVG_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_Q_AVG_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_SCENE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_SCENE_set(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_FLIP_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_FLIP_set(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);


static void op_M_get(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(ss->variables.m);
}

static void op_M_set(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t m = pop();
    if (m < 10) m = 10;
    ss->variables.m = m;
    (*update_metro)(m, ss->variables.m_act, 0);
}

static void op_M_ACT_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es),
                         command_state_t *NOTUSED(cs)) {
    push(ss->variables.m_act);
}

static void op_M_ACT_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es),
                         command_state_t *NOTUSED(cs)) {
    int16_t m_act = pop();
    if (m_act != 0) m_act = 1;
    ss->variables.m_act = m_act;
    (*update_metro)(ss->variables.m, m_act, 0);
}

static void op_P_N_get(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es),
                       command_state_t *NOTUSED(cs)) {
    push(ss->variables.p_n);
}

static void op_P_N_set(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es),
                       command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    if (a < 0)
        a = 0;
    else if (a > 3)
        a = 3;
    ss->variables.p_n = a;
}

static void op_P_L_get(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es),
                       command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    push(tele_patterns[pn].l);
}

static void op_P_L_set(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es),
                       command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    int16_t a = pop();
    if (a < 0)
        tele_patterns[pn].l = 0;
    else if (a > 63)
        tele_patterns[pn].l = 63;
    else
        tele_patterns[pn].l = a;
}

static void op_P_I_get(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es),
                       command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    push(tele_patterns[pn].i);
}

static void op_P_I_set(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es),
                       command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    int16_t a = pop();
    if (a < 0)
        tele_patterns[pn].i = 0;
    else if (a > tele_patterns[pn].l)
        tele_patterns[pn].i = tele_patterns[pn].l;
    else
        tele_patterns[pn].i = a;
    (*update_pi)();
}

static void op_P_HERE_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es),
                          command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    push(tele_patterns[pn].v[tele_patterns[pn].i]);
}

static void op_P_HERE_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es),
                          command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    int16_t a = pop();
    tele_patterns[pn].v[tele_patterns[pn].i] = a;
}

static void op_P_NEXT_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es),
                          command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    if ((tele_patterns[pn].i == (tele_patterns[pn].l - 1)) ||
        (tele_patterns[pn].i == tele_patterns[pn].end)) {
        if (tele_patterns[pn].wrap)
            tele_patterns[pn].i = tele_patterns[pn].start;
    }
    else
        tele_patterns[pn].i++;

    if (tele_patterns[pn].i > tele_patterns[pn].l) tele_patterns[pn].i = 0;

    push(tele_patterns[pn].v[tele_patterns[pn].i]);

    (*update_pi)();
}

static void op_P_NEXT_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es),
                          command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    if ((tele_patterns[pn].i == (tele_patterns[pn].l - 1)) ||
        (tele_patterns[pn].i == tele_patterns[pn].end)) {
        if (tele_patterns[pn].wrap)
            tele_patterns[pn].i = tele_patterns[pn].start;
    }
    else
        tele_patterns[pn].i++;

    if (tele_patterns[pn].i > tele_patterns[pn].l) tele_patterns[pn].i = 0;

    int16_t a = pop();
    tele_patterns[pn].v[tele_patterns[pn].i] = a;

    (*update_pi)();
}

static void op_P_PREV_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es),
                          command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    if ((tele_patterns[pn].i == 0) ||
        (tele_patterns[pn].i == tele_patterns[pn].start)) {
        if (tele_patterns[pn].wrap) {
            if (tele_patterns[pn].end < tele_patterns[pn].l)
                tele_patterns[pn].i = tele_patterns[pn].end;
            else
                tele_patterns[pn].i = tele_patterns[pn].l - 1;
        }
    }
    else
        tele_patterns[pn].i--;

    push(tele_patterns[pn].v[tele_patterns[pn].i]);

    (*update_pi)();
}

static void op_P_PREV_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es),
                          command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    if ((tele_patterns[pn].i == 0) ||
        (tele_patterns[pn].i == tele_patterns[pn].start)) {
        if (tele_patterns[pn].wrap) {
            if (tele_patterns[pn].end < tele_patterns[pn].l)
                tele_patterns[pn].i = tele_patterns[pn].end;
            else
                tele_patterns[pn].i = tele_patterns[pn].l - 1;
        }
    }
    else
        tele_patterns[pn].i--;

    int16_t a = pop();
    tele_patterns[pn].v[tele_patterns[pn].i] = a;

    (*update_pi)();
}

static void op_P_WRAP_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es),
                          command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    push(tele_patterns[pn].wrap);
}

static void op_P_WRAP_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es),
                          command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    int16_t a = pop();
    if (a < 0)
        tele_patterns[pn].wrap = 0;
    else if (a > 1)
        tele_patterns[pn].wrap = 1;
    else
        tele_patterns[pn].wrap = a;
}

static void op_P_START_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es),
                           command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    push(tele_patterns[pn].start);
}

static void op_P_START_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es),
                           command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    int16_t a = pop();
    if (a < 0)
        tele_patterns[pn].start = 0;
    else if (a > 63)
        tele_patterns[pn].start = 63;
    else
        tele_patterns[pn].start = a;
}

static void op_P_END_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es),
                         command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    push(tele_patterns[pn].end);
}

static void op_P_END_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es),
                         command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    int16_t a = pop();
    if (a < 0)
        tele_patterns[pn].end = 0;
    else if (a > 63)
        tele_patterns[pn].end = 63;
    else
        tele_patterns[pn].end = a;
}

int16_t normalise_value(int16_t min, int16_t max, int16_t wrap, int16_t value);
int16_t normalise_value(int16_t min, int16_t max, int16_t wrap, int16_t value) {
    if (value >= min && value <= max) return value;

    if (wrap) {
        if (value < min)
            return max;
        else
            return min;
    }
    else {
        if (value < min)
            return min;
        else
            return max;
    }
}

static void op_O_get(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t min = ss->variables.o_min;
    int16_t max = ss->variables.o_max;
    int16_t wrap = ss->variables.o_wrap;

    // restrict current_value to (wrapped) bounds
    int16_t current_value = normalise_value(min, max, wrap, ss->variables.o);
    push(current_value);

    // calculate new value
    ss->variables.o =
        normalise_value(min, max, wrap, current_value + ss->variables.o_inc);
}

static void op_O_set(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    ss->variables.o = pop();
}

static void op_DRUNK_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es),
                         command_state_t *NOTUSED(cs)) {
    int16_t min = ss->variables.drunk_min;
    int16_t max = ss->variables.drunk_max;
    int16_t wrap = ss->variables.drunk_wrap;

    // restrict current_value to (wrapped) bounds
    int16_t current_value =
        normalise_value(min, max, wrap, ss->variables.drunk);
    push(current_value);

    // calculate new value
    int16_t new_value = current_value + (rand() % 3) - 1;
    ss->variables.drunk = normalise_value(min, max, wrap, new_value);
}

static void op_DRUNK_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es),
                         command_state_t *NOTUSED(cs)) {
    ss->variables.drunk = pop();
}

static void op_Q_get(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    push(q[q_n - 1]);
}

static void op_Q_set(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t *q = ss->variables.q;
    for (int16_t i = Q_LENGTH - 1; i > 0; i--) { q[i] = q[i - 1]; }
    q[0] = pop();
}

static void op_Q_N_get(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es),
                       command_state_t *NOTUSED(cs)) {
    push(ss->variables.q_n);
}

static void op_Q_N_set(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es),
                       command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    if (a < 1)
        a = 1;
    else if (a > Q_LENGTH)
        a = Q_LENGTH;
    ss->variables.q_n = a;
}

static void op_Q_AVG_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es),
                         command_state_t *NOTUSED(cs)) {
    int16_t avg = 0;
    int16_t *q = ss->variables.q;
    int16_t q_n = ss->variables.q_n;
    for (int16_t i = 0; i < q_n; i++) { avg += q[i]; }
    avg /= q_n;

    push(avg);
}

static void op_Q_AVG_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es),
                         command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    int16_t *q = ss->variables.q;
    for (int16_t i = 0; i < Q_LENGTH; i++) { q[i] = a; }
}

static void op_SCENE_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es),
                         command_state_t *NOTUSED(cs)) {
    push(ss->variables.scene);
}

static void op_SCENE_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es),
                         command_state_t *NOTUSED(cs)) {
    int16_t scene = pop();
    ss->variables.scene = scene;
    (*update_scene)(scene);
}

static void op_FLIP_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es),
                        command_state_t *NOTUSED(cs)) {
    int16_t flip = ss->variables.flip;
    push(flip);
    ss->variables.flip = flip == 0;
}

static void op_FLIP_set(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es),
                        command_state_t *NOTUSED(cs)) {
    ss->variables.flip = pop() != 0;
}


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

static void op_CV_get(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    a = normalise_value(0, CV_COUNT - 1, 0, a - 1);
    push(ss->variables.cv[a]);
}

static void op_CV_set(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    int16_t b = pop();
    a = normalise_value(0, CV_COUNT - 1, 0, a - 1);
    b = normalise_value(0, 16383, 0, b);
    ss->variables.cv[a] = b;
    (*update_cv)(a, b, 1);
}

static void op_CV_SLEW_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es),
                           command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    a = normalise_value(0, CV_COUNT - 1, 0, a - 1);
    push(ss->variables.cv_slew[a]);
}

static void op_CV_SLEW_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es),
                           command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    int16_t b = pop();
    a = normalise_value(0, CV_COUNT - 1, 0, a - 1);
    b = normalise_value(1, 32767, 0, b);  // min slew = 1
    ss->variables.cv_slew[a] = b;
    (*update_cv_slew)(a, b);
}

static void op_CV_OFF_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es),
                          command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    a = normalise_value(0, CV_COUNT - 1, 0, a - 1);
    push(ss->variables.cv_off[a]);
}

static void op_CV_OFF_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es),
                          command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    int16_t b = pop();
    a = normalise_value(0, CV_COUNT - 1, 0, a - 1);
    ss->variables.cv_off[a] = b;
    (*update_cv_off)(a, b);
    (*update_cv)(a, ss->variables.cv[a], 1);
}

static void op_TR_get(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    a = normalise_value(0, TR_COUNT - 1, 0, a - 1);
    push(ss->variables.tr[a]);
}

static void op_TR_set(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    int16_t b = pop();
    a = normalise_value(0, TR_COUNT - 1, 0, a - 1);
    ss->variables.tr[a] = b != 0;
    (*update_tr)(a, b);
}

static void op_TR_POL_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es),
                          command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    a = normalise_value(0, TR_COUNT - 1, 0, a - 1);
    push(ss->variables.tr_pol[a]);
}

static void op_TR_POL_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es),
                          command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    int16_t b = pop();
    a = normalise_value(0, TR_COUNT - 1, 0, a - 1);
    ss->variables.tr_pol[a] = b > 0;
}

static void op_TR_TIME_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es),
                           command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    a = normalise_value(0, TR_COUNT - 1, 0, a - 1);
    push(ss->variables.tr_time[a]);
}

static void op_TR_TIME_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es),
                           command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    int16_t b = pop();
    a = normalise_value(0, TR_COUNT - 1, 0, a - 1);
    if (b < 0) b = 0;
    ss->variables.tr_time[a] = b;
}

/////////////////////////////////////////////////////////////////
// DELAY ////////////////////////////////////////////////////////

static tele_command_t delay_c[TELE_D_SIZE];
static int16_t delay_t[TELE_D_SIZE];
static uint8_t delay_count;
static int16_t tr_pulse[4];

static void process_delays(uint8_t);
void clear_delays(void);

static void process_delays(uint8_t v) {
    for (int16_t i = 0; i < TELE_D_SIZE; i++) {
        if (delay_t[i]) {
            delay_t[i] -= v;
            if (delay_t[i] <= 0) {
                // sprintf(dbg,"\r\ndelay %d", i);
                // DBG
                process(&delay_c[i]);
                delay_t[i] = 0;
                delay_count--;
                if (delay_count == 0) (*update_delay)(0);
            }
        }
    }

    for (int16_t i = 0; i < 4; i++) {
        if (tr_pulse[i]) {
            tr_pulse[i] -= v;
            if (tr_pulse[i] <= 0) {
                tr_pulse[i] = 0;
                scene_state.variables.tr[i] =
                    scene_state.variables.tr_pol[i] == 0;
                (*update_tr)(i, scene_state.variables.tr[i]);
            }
        }
    }
}

void clear_delays(void) {
    for (int16_t i = 0; i < 4; i++) tr_pulse[i] = 0;

    for (int16_t i = 0; i < TELE_D_SIZE; i++) { delay_t[i] = 0; }

    delay_count = 0;

    tele_stack_top = 0;

    (*update_delay)(0);
    (*update_s)(0);
}


/////////////////////////////////////////////////////////////////
// MOD //////////////////////////////////////////////////////////

static void mod_PROB(tele_command_t *);
static void mod_DEL(tele_command_t *);
static void mod_S(tele_command_t *);
static void mod_IF(tele_command_t *);
static void mod_ELIF(tele_command_t *);
static void mod_ELSE(tele_command_t *);
static void mod_L(tele_command_t *);

#define MAKEMOD(name, params, doc) \
    { #name, mod_##name, params, doc }
#define MODS 7
static const tele_mod_t tele_mods[MODS] = {
    MAKEMOD(PROB, 1, "PROBABILITY TO CONTINUE EXECUTING LINE"),
    MAKEMOD(DEL, 1, "DELAY THIS COMMAND"),
    MAKEMOD(S, 0, "ADD COMMAND TO STACK"),
    MAKEMOD(IF, 1, "IF CONDITION FOR COMMAND"),
    MAKEMOD(ELIF, 1, "ELSE IF"),
    MAKEMOD(ELSE, 0, "ELSE"),
    MAKEMOD(L, 2, "LOOPED COMMAND WITH ITERATION")
};


void mod_PROB(tele_command_t *c) {
    int16_t a = pop();

    tele_command_t cc;
    if (rand() % 101 < a) {
        cc.l = c->l - c->separator - 1;
        cc.separator = -1;
        memcpy(cc.data, &c->data[c->separator + 1], cc.l * sizeof(tele_data_t));
        // sprintf(dbg,"\r\nsub-length: %d", cc.l);
        process(&cc);
    }
}
void mod_DEL(tele_command_t *c) {
    int16_t i = 0;
    int16_t a = pop();

    if (a < 1) a = 1;

    while (delay_t[i] != 0 && i != TELE_D_SIZE) i++;

    if (i < TELE_D_SIZE) {
        delay_count++;
        if (delay_count == 1) (*update_delay)(1);
        delay_t[i] = a;

        delay_c[i].l = c->l - c->separator - 1;
        delay_c[i].separator = -1;

        memcpy(delay_c[i].data, &c->data[c->separator + 1],
               delay_c[i].l * sizeof(tele_data_t));
    }
}
void mod_S(tele_command_t *c) {
    if (tele_stack_top < TELE_STACK_SIZE) {
        tele_stack[tele_stack_top].l = c->l - c->separator - 1;
        memcpy(tele_stack[tele_stack_top].data, &c->data[c->separator + 1],
               tele_stack[tele_stack_top].l * sizeof(tele_data_t));
        tele_stack[tele_stack_top].separator = -1;
        tele_stack_top++;
        if (tele_stack_top == 1) (*update_s)(1);
    }
}
void mod_IF(tele_command_t *c) {
    condition = false;
    tele_command_t cc;
    if (pop()) {
        condition = true;
        cc.l = c->l - c->separator - 1;
        cc.separator = -1;
        memcpy(cc.data, &c->data[c->separator + 1], cc.l * sizeof(tele_data_t));
        // sprintf(dbg,"\r\nsub-length: %d", cc.l);
        process(&cc);
    }
}
void mod_ELIF(tele_command_t *c) {
    tele_command_t cc;
    if (!condition) {
        if (pop()) {
            condition = true;
            cc.l = c->l - c->separator - 1;
            cc.separator = -1;
            memcpy(cc.data, &c->data[c->separator + 1],
                   cc.l * sizeof(tele_data_t));
            // sprintf(dbg,"\r\nsub-length: %d", cc.l);
            process(&cc);
        }
    }
}
void mod_ELSE(tele_command_t *c) {
    tele_command_t cc;
    if (!condition) {
        condition = true;
        cc.l = c->l - c->separator - 1;
        cc.separator = -1;
        memcpy(cc.data, &c->data[c->separator + 1], cc.l * sizeof(tele_data_t));
        // sprintf(dbg,"\r\nsub-length: %d", cc.l);
        process(&cc);
    }
}
void mod_L(tele_command_t *c) {
    tele_command_t cc;
    cc.l = c->l - c->separator - 1;
    cc.separator = -1;
    memcpy(cc.data, &c->data[c->separator + 1], cc.l * sizeof(tele_data_t));

    int16_t a = pop();
    int16_t b = pop();
    int16_t loop_size = a < b ? b - a : a - b;

    for (int16_t i = 0; i <= loop_size; i++) {
        scene_state.variables.i = a < b ? a + i : a - i;
        process(&cc);
    }
}


/////////////////////////////////////////////////////////////////
// OPS //////////////////////////////////////////////////////////

static void op_CONSTANT(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_PEEK_I16(const void *data, scene_state_t *ss,
                        exec_state_t *NOTUSED(es),
                        command_state_t *NOTUSED(cs));
static void op_POKE_I16(const void *data, scene_state_t *ss,
                        exec_state_t *NOTUSED(es),
                        command_state_t *NOTUSED(cs));

static void op_ADD(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_SUB(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_MUL(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_DIV(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_MOD(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_RAND(const void *data, scene_state_t *ss, exec_state_t *es,
                    command_state_t *cs);
static void op_RRAND(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_TOSS(const void *data, scene_state_t *ss, exec_state_t *es,
                    command_state_t *cs);
static void op_MIN(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_MAX(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_LIM(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_WRAP(const void *data, scene_state_t *ss, exec_state_t *es,
                    command_state_t *cs);
static void op_QT(const void *data, scene_state_t *ss, exec_state_t *es,
                  command_state_t *cs);
static void op_AVG(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_EQ(const void *data, scene_state_t *ss, exec_state_t *es,
                  command_state_t *cs);
static void op_NE(const void *data, scene_state_t *ss, exec_state_t *es,
                  command_state_t *cs);
static void op_LT(const void *data, scene_state_t *ss, exec_state_t *es,
                  command_state_t *cs);
static void op_GT(const void *data, scene_state_t *ss, exec_state_t *es,
                  command_state_t *cs);
static void op_NZ(const void *data, scene_state_t *ss, exec_state_t *es,
                  command_state_t *cs);
static void op_EZ(const void *data, scene_state_t *ss, exec_state_t *es,
                  command_state_t *cs);
static void op_TR_TOG(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_N(const void *data, scene_state_t *ss, exec_state_t *es,
                 command_state_t *cs);
static void op_S_ALL(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_S_POP(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_S_CLR(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_DEL_CLR(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_M_RESET(const void *data, scene_state_t *ss, exec_state_t *es,
                       command_state_t *cs);
static void op_V(const void *data, scene_state_t *ss, exec_state_t *es,
                 command_state_t *cs);
static void op_VV(const void *data, scene_state_t *ss, exec_state_t *es,
                  command_state_t *cs);
static void op_P_get(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_P_set(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_P_INS(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_P_RM(const void *data, scene_state_t *ss, exec_state_t *es,
                    command_state_t *cs);
static void op_P_PUSH(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_P_POP(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_PN_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_PN_set(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_TR_PULSE(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_II(const void *data, scene_state_t *ss, exec_state_t *es,
                  command_state_t *cs);
static void op_RSH(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_LSH(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_S_L(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_CV_SET(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_EXP(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_ABS(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_AND(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_OR(const void *data, scene_state_t *ss, exec_state_t *es,
                  command_state_t *cs);
static void op_XOR(const void *data, scene_state_t *ss, exec_state_t *es,
                   command_state_t *cs);
static void op_JI(const void *data, scene_state_t *ss, exec_state_t *es,
                  command_state_t *cs);
static void op_SCRIPT(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_KILL(const void *data, scene_state_t *ss, exec_state_t *es,
                    command_state_t *cs);
static void op_MUTE(const void *data, scene_state_t *ss, exec_state_t *es,
                    command_state_t *cs);
static void op_UNMUTE(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_SCALE(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_STATE(const void *data, scene_state_t *ss, exec_state_t *es,
                     command_state_t *cs);
static void op_ER(const void *data, scene_state_t *ss, exec_state_t *es,
                  command_state_t *cs);


// Get only ops
#define MAKE_GET_OP(n, g, p, r, d)                                    \
    {                                                                 \
        .name = #n, .get = g, .set = NULL, .params = p, .returns = r, \
        .data = NULL, .doc = d                                        \
    }

// Get & set ops
#define MAKE_GET_SET_OP(n, g, s, p, r, d)                          \
    {                                                              \
        .name = #n, .get = g, .set = s, .params = p, .returns = r, \
        .data = NULL, .doc = d                                     \
    }

// Constant Ops
static void op_CONSTANT(const void *data, scene_state_t *NOTUSED(ss),
                        exec_state_t *NOTUSED(es),
                        command_state_t *NOTUSED(cs)) {
    push((intptr_t)data);
}

#define MAKE_CONSTANT_OP(n, v, d)                                 \
    {                                                             \
        .name = #n, .get = op_CONSTANT, .set = NULL, .params = 0, \
        .returns = 1, .data = (void *)v, .doc = d                 \
    }

// Variables, peek & poke
static void op_PEEK_I16(const void *data, scene_state_t *ss,
                        exec_state_t *NOTUSED(es),
                        command_state_t *NOTUSED(cs)) {
    char *base = (char *)ss;
    size_t offset = (size_t)data;
    int16_t *ptr = (int16_t *)(base + offset);
    push(*ptr);
}

static void op_POKE_I16(const void *data, scene_state_t *ss,
                        exec_state_t *NOTUSED(es),
                        command_state_t *NOTUSED(cs)) {
    char *base = (char *)ss;
    size_t offset = (size_t)data;
    int16_t *ptr = (int16_t *)(base + offset);
    *ptr = pop();
}

#define MAKE_SIMPLE_VARIABLE_OP(n, v, d)                                   \
    {                                                                      \
        .name = #n, .get = op_PEEK_I16, .set = op_POKE_I16, .params = 0,   \
        .returns = 1, .data = (void *)offsetof(scene_state_t, v), .doc = d \
    }

#define OPS 145
// clang-format off
static const tele_op_t tele_ops[OPS] = {
    //                    var  member       docs
    MAKE_SIMPLE_VARIABLE_OP(A         , variables.a         , "A"                       ),
    MAKE_SIMPLE_VARIABLE_OP(B         , variables.b         , "B"                       ),
    MAKE_SIMPLE_VARIABLE_OP(C         , variables.c         , "C"                       ),
    MAKE_SIMPLE_VARIABLE_OP(D         , variables.d         , "D"                       ),
    MAKE_SIMPLE_VARIABLE_OP(DRUNK.MAX , variables.drunk_max , "DRUNK.MAX"               ),
    MAKE_SIMPLE_VARIABLE_OP(DRUNK.MIN , variables.drunk_min , "DRUNK.MIN"               ),
    MAKE_SIMPLE_VARIABLE_OP(DRUNK.WRAP, variables.drunk_wrap, "DRUNK.WRAP"              ),
    MAKE_SIMPLE_VARIABLE_OP(I         , variables.i         , "I: GETS OVERWRITTEN BY L"),
    MAKE_SIMPLE_VARIABLE_OP(IN        , variables.in        , "IN                      "),
    MAKE_SIMPLE_VARIABLE_OP(O.INC     , variables.o_inc     , "O.DIR"                   ),
    MAKE_SIMPLE_VARIABLE_OP(O.MAX     , variables.o_max     , "O.MAX"                   ),
    MAKE_SIMPLE_VARIABLE_OP(O.MIN     , variables.o_min     , "O.MIN"                   ),
    MAKE_SIMPLE_VARIABLE_OP(O.WRAP    , variables.o_wrap    , "O.WRAP"                  ),
    MAKE_SIMPLE_VARIABLE_OP(PARAM     , variables.param     , "PARAM"                   ),
    MAKE_SIMPLE_VARIABLE_OP(T         , variables.t         , "T"                       ),
    MAKE_SIMPLE_VARIABLE_OP(TIME      , variables.time      , "TIME"                    ),
    MAKE_SIMPLE_VARIABLE_OP(TIME.ACT  , variables.time_act  , "TIME.ACT"                ),
    MAKE_SIMPLE_VARIABLE_OP(X         , variables.x         , "X"                       ),
    MAKE_SIMPLE_VARIABLE_OP(Y         , variables.y         , "Y"                       ),
    MAKE_SIMPLE_VARIABLE_OP(Z         , variables.z         , "Z"                       ),

    //          op        get fn   inputs output docs
    MAKE_GET_OP(ADD     , op_ADD     , 2, true , "[A B] ADD A TO B"                          ),
    MAKE_GET_OP(SUB     , op_SUB     , 2, true , "[A B] SUBTRACT B FROM A"                   ),
    MAKE_GET_OP(MUL     , op_MUL     , 2, true , "[A B] MULTIPLY TWO VALUES"                 ),
    MAKE_GET_OP(DIV     , op_DIV     , 2, true , "[A B] DIVIDE FIRST BY SECOND"              ),
    MAKE_GET_OP(MOD     , op_MOD     , 2, true , "[A B] MOD FIRST BY SECOND"                 ),
    MAKE_GET_OP(RAND    , op_RAND    , 1, true , "[A] RETURN RANDOM NUMBER UP TO A"          ),
    MAKE_GET_OP(RRAND   , op_RRAND   , 2, true , "[A B] RETURN RANDOM NUMBER BETWEEN A AND B"),
    MAKE_GET_OP(TOSS    , op_TOSS    , 0, true , "RETURN RANDOM STATE"                       ),
    MAKE_GET_OP(MIN     , op_MIN     , 2, true , "RETURN LESSER OF TWO VALUES"               ),
    MAKE_GET_OP(MAX     , op_MAX     , 2, true , "RETURN GREATER OF TWO VALUES"              ),
    MAKE_GET_OP(LIM     , op_LIM     , 3, true , "[A B C] LIMIT C TO RANGE A TO B"           ),
    MAKE_GET_OP(WRAP    , op_WRAP    , 3, true , "[A B C] WRAP C WITHIN RANGE A TO B"        ),
    MAKE_GET_OP(QT      , op_QT      , 2, true , "[A B] QUANTIZE A TO STEP SIZE B"           ),
    MAKE_GET_OP(AVG     , op_AVG     , 2, true , "AVERAGE TWO VALUES"                        ),
    MAKE_GET_OP(EQ      , op_EQ      , 2, true , "LOGIC: EQUAL"                              ),
    MAKE_GET_OP(NE      , op_NE      , 2, true , "LOGIC: NOT EQUAL"                          ),
    MAKE_GET_OP(LT      , op_LT      , 2, true , "LOGIC: LESS THAN"                          ),
    MAKE_GET_OP(GT      , op_GT      , 2, true , "LOGIC: GREATER THAN"                       ),
    MAKE_GET_OP(NZ      , op_NZ      , 1, true , "LOGIC: NOT ZERO"                           ),
    MAKE_GET_OP(EZ      , op_EZ      , 1, true , "LOGIC: EQUALS ZERO"                        ),
    MAKE_GET_OP(TR.TOG  , op_TR_TOG  , 1, false, "[A] TOGGLE TRIGGER A"                      ),
    MAKE_GET_OP(N       , op_N       , 1, true , "TABLE FOR NOTE VALUES"                     ),
    MAKE_GET_OP(S.ALL   , op_S_ALL   , 0, false, "S: EXECUTE ALL"                            ),
    MAKE_GET_OP(S.POP   , op_S_POP   , 0, false, "S: POP LAST"                               ),
    MAKE_GET_OP(S.CLR   , op_S_CLR   , 0, false, "S: FLUSH"                                  ),
    MAKE_GET_OP(DEL.CLR , op_DEL_CLR , 0, false, "DELAY: FLUSH"                              ),
    MAKE_GET_OP(M.RESET , op_M_RESET , 0, false, "METRO: RESET"                              ),
    MAKE_GET_OP(V       , op_V       , 1, true , "TO VOLT"                                   ),
    MAKE_GET_OP(VV      , op_VV      , 1, true , "TO VOLT WITH PRECISION"                    ),
    MAKE_GET_OP(P.INS   , op_P_INS   , 2, false, "PATTERN: INSERT"                           ),
    MAKE_GET_OP(P.RM    , op_P_RM    , 1, true , "PATTERN: REMOVE"                           ),
    MAKE_GET_OP(P.PUSH  , op_P_PUSH  , 1, false, "PATTERN: PUSH"                             ),
    MAKE_GET_OP(P.POP   , op_P_POP   , 0, true , "PATTERN: POP"                              ),
    MAKE_GET_OP(TR.PULSE, op_TR_PULSE, 1, false, "PULSE TRIGGER"                             ),
    MAKE_GET_OP(II      , op_II      , 2, false, "II"                                        ),
    MAKE_GET_OP(RSH     , op_RSH     , 2, true , "RIGHT SHIFT"                               ),
    MAKE_GET_OP(LSH     , op_LSH     , 2, true , "LEFT SHIFT"                                ),
    MAKE_GET_OP(S.L     , op_S_L     , 0, true , "STACK LENGTH"                              ),
    MAKE_GET_OP(CV.SET  , op_CV_SET  , 2, false, "CV SET"                                    ),
    MAKE_GET_OP(EXP     , op_EXP     , 1, true , "EXPONENTIATE"                              ),
    MAKE_GET_OP(ABS     , op_ABS     , 1, true , "ABSOLUTE VALUE"                            ),
    MAKE_GET_OP(AND     , op_AND     , 2, true , "LOGIC: AND"                                ),
    MAKE_GET_OP(OR      , op_OR      , 2, true , "LOGIC: OR"                                 ),
    MAKE_GET_OP(XOR     , op_XOR     , 2, true , "LOGIC: XOR"                                ),
    MAKE_GET_OP(JI      , op_JI      , 2, true , "JUST INTONE DIVISON"                       ),
    MAKE_GET_OP(SCRIPT  , op_SCRIPT  , 1, false, "CALL SCRIPT"                               ),
    MAKE_GET_OP(KILL    , op_KILL    , 0, false, "CLEAR DELAYS, STACK, SLEW"                 ),
    MAKE_GET_OP(MUTE    , op_MUTE    , 1, false, "MUTE INPUT"                                ),
    MAKE_GET_OP(UNMUTE  , op_UNMUTE  , 1, false, "UNMUTE INPUT"                              ),
    MAKE_GET_OP(SCALE   , op_SCALE   , 5, true , "SCALE NUMBER RANGES"                       ),
    MAKE_GET_OP(STATE   , op_STATE   , 1, true , "GET INPUT STATE"                           ),
    MAKE_GET_OP(ER      , op_ER      , 3, true , "EUCLIDEAN RHYTHMS"                         ),

    //              op       get             set        inputs output docs
    MAKE_GET_SET_OP(CV     , op_CV_get     , op_CV_set     , 1, true, "CV"                ),
    MAKE_GET_SET_OP(CV.OFF , op_CV_OFF_get , op_CV_OFF_set , 1, true, "CV.OFF"            ),
    MAKE_GET_SET_OP(CV.SLEW, op_CV_SLEW_get, op_CV_SLEW_set, 1, true, "CV.SLEW"           ),
    MAKE_GET_SET_OP(DRUNK  , op_DRUNK_get  , op_DRUNK_set  , 0, true, "DRUNK"             ),
    MAKE_GET_SET_OP(FLIP   , op_FLIP_get   , op_FLIP_set   , 0, true, "FLIP"              ),
    MAKE_GET_SET_OP(M      , op_M_get      , op_M_set      , 0, true, "M"                 ),
    MAKE_GET_SET_OP(M.ACT  , op_M_ACT_get  , op_M_ACT_set  , 0, true, "M.ACT"             ),
    MAKE_GET_SET_OP(O      , op_O_get      , op_O_set      , 0, true, "O"                 ),
    MAKE_GET_SET_OP(P      , op_P_get      , op_P_set      , 1, true, "PATTERN: GET/SET"  ),
    MAKE_GET_SET_OP(P.HERE , op_P_HERE_get , op_P_HERE_set , 0, true, "P.HERE"            ),
    MAKE_GET_SET_OP(P.END  , op_P_END_get  , op_P_END_set  , 0, true, "P.END"             ),
    MAKE_GET_SET_OP(P.I    , op_P_I_get    , op_P_I_set    , 0, true, "P.I"               ),
    MAKE_GET_SET_OP(P.L    , op_P_L_get    , op_P_L_set    , 0, true, "P.L"               ),
    MAKE_GET_SET_OP(P.N    , op_P_N_get    , op_P_N_set    , 0, true, "P.N"               ),
    MAKE_GET_SET_OP(P.NEXT , op_P_NEXT_get , op_P_NEXT_set , 0, true, "P.NEXT"            ),
    MAKE_GET_SET_OP(P.PREV , op_P_PREV_get , op_P_PREV_set , 0, true, "P.PREV"            ),
    MAKE_GET_SET_OP(P.START, op_P_START_get, op_P_START_set, 0, true, "P.START"           ),
    MAKE_GET_SET_OP(P.WRAP , op_P_WRAP_get , op_P_WRAP_set , 0, true, "P.WRAP"            ),
    MAKE_GET_SET_OP(PN     , op_PN_get     , op_PN_set     , 2, true, "PATTERN: GET/SET N"),
    MAKE_GET_SET_OP(Q      , op_Q_get      , op_Q_set      , 0, true, "Q"                 ),
    MAKE_GET_SET_OP(Q.AVG  , op_Q_AVG_get  , op_Q_AVG_set  , 0, true, "Q.AVG"             ),
    MAKE_GET_SET_OP(Q.N    , op_Q_N_get    , op_Q_N_set    , 0, true, "Q.N"               ),
    MAKE_GET_SET_OP(TR     , op_TR_get     , op_TR_set     , 1, true, "TR"                ),
    MAKE_GET_SET_OP(TR.POL , op_TR_POL_get , op_TR_POL_set , 1, true, "TR.POL"            ),
    MAKE_GET_SET_OP(TR.TIME, op_TR_TIME_get, op_TR_TIME_set, 1, true, "TR.TIME"           ),
    MAKE_GET_SET_OP(SCENE  , op_SCENE_get  , op_SCENE_set  , 0, true, "SCENE"             ),

    //               op           constant      doc
    MAKE_CONSTANT_OP(WW.PRESET  , WW_PRESET   , "WW.PRESET"  ),
    MAKE_CONSTANT_OP(WW.POS     , WW_POS      , "WW.POS"     ),
    MAKE_CONSTANT_OP(WW.SYNC    , WW_SYNC     , "WW.SYNC"    ),
    MAKE_CONSTANT_OP(WW.START   , WW_START    , "WW.START"   ),
    MAKE_CONSTANT_OP(WW.END     , WW_END      , "WW.END"     ),
    MAKE_CONSTANT_OP(WW.PMODE   , WW_PMODE    , "WW.PMODE"   ),
    MAKE_CONSTANT_OP(WW.PATTERN , WW_PATTERN  , "WW.PATTERN" ),
    MAKE_CONSTANT_OP(WW.QPATTERN, WW_QPATTERN , "WW.QPATTERN"),
    MAKE_CONSTANT_OP(WW.MUTE1   , WW_MUTE1    , "WW.MUTE1"   ),
    MAKE_CONSTANT_OP(WW.MUTE2   , WW_MUTE2    , "WW.MUTE2"   ),
    MAKE_CONSTANT_OP(WW.MUTE3   , WW_MUTE3    , "WW.MUTE3"   ),
    MAKE_CONSTANT_OP(WW.MUTE4   , WW_MUTE4    , "WW.MUTE4"   ),
    MAKE_CONSTANT_OP(WW.MUTEA   , WW_MUTEA    , "WW.MUTEA"   ),
    MAKE_CONSTANT_OP(WW.MUTEB   , WW_MUTEB    , "WW.MUTEB"   ),
    MAKE_CONSTANT_OP(MP.PRESET  , MP_PRESET   , "MP.PRESET"  ),
    MAKE_CONSTANT_OP(MP.RESET   , MP_RESET    , "MP.RESET"   ),
    MAKE_CONSTANT_OP(MP.SYNC    , MP_SYNC     , "MP.SYNC"    ),
    MAKE_CONSTANT_OP(MP.MUTE    , MP_MUTE     , "MP.MUTE"    ),
    MAKE_CONSTANT_OP(MP.UNMUTE  , MP_UNMUTE   , "MP.UNMUTE"  ),
    MAKE_CONSTANT_OP(MP.FREEZE  , MP_FREEZE   , "MP.FREEZE"  ),
    MAKE_CONSTANT_OP(MP.UNFREEZE, MP_UNFREEZE , "MP.UNFREEZE"),
    MAKE_CONSTANT_OP(MP.STOP    , MP_STOP     , "MP.STOP"    ),
    MAKE_CONSTANT_OP(ES.PRESET  , ES_PRESET   , "ES.PRESET"  ),
    MAKE_CONSTANT_OP(ES.MODE    , ES_MODE     , "ES.MODE"    ),
    MAKE_CONSTANT_OP(ES.CLOCK   , ES_CLOCK    , "ES.CLOCK"   ),
    MAKE_CONSTANT_OP(ES.RESET   , ES_RESET    , "ES.RESET"   ),
    MAKE_CONSTANT_OP(ES.PATTERN , ES_PATTERN  , "ES.PATTERN" ),
    MAKE_CONSTANT_OP(ES.TRANS   , ES_TRANS    , "ES.TRANS"   ),
    MAKE_CONSTANT_OP(ES.STOP    , ES_STOP     , "ES.STOP"    ),
    MAKE_CONSTANT_OP(ES.TRIPLE  , ES_TRIPLE   , "ES.TRIPLE"  ),
    MAKE_CONSTANT_OP(ES.MAGIC   , ES_MAGIC    , "ES.MAGIC"   ),
    MAKE_CONSTANT_OP(OR.TRK     , ORCA_TRACK  , "OR.TRK"     ),
    MAKE_CONSTANT_OP(OR.CLK     , ORCA_CLOCK  , "OR.CLK"     ),
    MAKE_CONSTANT_OP(OR.DIV     , ORCA_DIVISOR, "OR.DIV"     ),
    MAKE_CONSTANT_OP(OR.PHASE   , ORCA_PHASE  , "OR.PHASE"   ),
    MAKE_CONSTANT_OP(OR.RST     , ORCA_RESET  , "OR.RST"     ),
    MAKE_CONSTANT_OP(OR.WGT     , ORCA_WEIGHT , "OR.WGT"     ),
    MAKE_CONSTANT_OP(OR.MUTE    , ORCA_MUTE   , "OR.MUTE"    ),
    MAKE_CONSTANT_OP(OR.SCALE   , ORCA_SCALE  , "OR.SCALE"   ),
    MAKE_CONSTANT_OP(OR.BANK    , ORCA_BANK   , "OR.BANK"    ),
    MAKE_CONSTANT_OP(OR.PRESET  , ORCA_PRESET , "OR.PRESET"  ),
    MAKE_CONSTANT_OP(OR.RELOAD  , ORCA_RELOAD , "OR.RELOAD"  ),
    MAKE_CONSTANT_OP(OR.ROTS    , ORCA_ROTATES, "OR.ROTS"    ),
    MAKE_CONSTANT_OP(OR.ROTW    , ORCA_ROTATEW, "OR.ROTW"    ),
    MAKE_CONSTANT_OP(OR.GRST    , ORCA_GRESET , "OR.GRST"    ),
    MAKE_CONSTANT_OP(OR.CVA     , ORCA_CVA    , "OR.CVA"     ),
    MAKE_CONSTANT_OP(OR.CVB     , ORCA_CVB    , "OR.CVB"     ),
};
// clang-format on

static void op_ADD(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() + pop());
}
static void op_SUB(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() - pop());
}
static void op_MUL(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() * pop());
}
static void op_DIV(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() / pop());
}
// can be optimized:
static void op_MOD(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() % pop());
}
static void op_RAND(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                    exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    if (a == -1)
        push(0);
    else
        push(rand() % (a + 1));
}
static void op_RRAND(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a, b, min, max, range;
    a = pop();
    b = pop();
    if (a < b) {
        min = a;
        max = b;
    }
    else {
        min = b;
        max = a;
    }
    range = max - min + 1;
    if (range == 0)
        push(a);
    else
        push(rand() % range + min);
}
static void op_TOSS(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                    exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(rand() & 1);
}
static void op_MIN(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a, b;
    a = pop();
    b = pop();
    if (b > a)
        push(a);
    else
        push(b);
}
static void op_MAX(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a, b;
    a = pop();
    b = pop();
    if (a > b)
        push(a);
    else
        push(b);
}
static void op_LIM(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a, b, i;
    i = pop();
    a = pop();
    b = pop();
    if (i < a)
        push(a);
    else if (i > b)
        push(b);
    else
        push(i);
}
static void op_WRAP(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                    exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a, b, i, c;
    i = pop();
    a = pop();
    b = pop();
    if (a < b) {
        c = b - a + 1;
        while (i >= b) i -= c;
        while (i < a) i += c;
    }
    else {
        c = a - b + 1;
        while (i >= a) i -= c;
        while (i < b) i += c;
    }
    push(i);
}
static void op_QT(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                  exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    // this rounds negative numbers rather than quantize (choose closer)
    int16_t a, b, c, d, e;
    b = pop();
    a = pop();

    c = b / a;
    d = c * a;
    e = (c + 1) * a;

    if (abs(b - d) < abs(b - e))
        push(d);
    else
        push(e);
}
static void op_AVG(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push((pop() + pop()) >> 1);
}
static void op_EQ(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                  exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() == pop());
}
static void op_NE(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                  exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() != pop());
}
static void op_LT(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                  exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() < pop());
}
static void op_GT(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                  exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() > pop());
}
static void op_NZ(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                  exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() != 0);
}
static void op_EZ(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                  exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() == 0);
}
static void op_TR_TOG(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    // saturate and shift
    if (a < 1)
        a = 1;
    else if (a > 4)
        a = 4;
    a--;
    if (ss->variables.tr[a])
        ss->variables.tr[a] = 0;
    else
        ss->variables.tr[a] = 1;
    update_tr(a, ss->variables.tr[a]);
}
static void op_N(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                 exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();

    if (a < 0) {
        if (a < -127) a = -127;
        a = -a;
        push(-table_n[a]);
    }
    else {
        if (a > 127) a = 127;
        push(table_n[a]);
    }
}
static void op_S_ALL(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    for (int16_t i = 0; i < tele_stack_top; i++)
        process(&tele_stack[tele_stack_top - i - 1]);
    tele_stack_top = 0;
    (*update_s)(0);
}
static void op_S_POP(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    if (tele_stack_top) {
        tele_stack_top--;
        process(&tele_stack[tele_stack_top]);
        if (tele_stack_top == 0) (*update_s)(0);
    }
}
static void op_S_CLR(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    tele_stack_top = 0;
    (*update_s)(0);
}
static void op_DEL_CLR(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                       exec_state_t *NOTUSED(es),
                       command_state_t *NOTUSED(cs)) {
    clear_delays();
}
static void op_M_RESET(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es),
                       command_state_t *NOTUSED(cs)) {
    (*update_metro)(ss->variables.m, ss->variables.m_act, 1);
}
static void op_V(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                 exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    if (a > 10)
        a = 10;
    else if (a < -10)
        a = -10;

    if (a < 0) {
        a = -a;
        push(-table_v[a]);
    }
    else
        push(table_v[a]);
}
static void op_VV(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                  exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    uint8_t negative = 1;
    int16_t a = pop();
    if (a < 0) {
        negative = -1;
        a = -a;
    }
    if (a > 1000) a = 1000;

    push(negative * (table_v[a / 100] + table_vv[a % 100]));
}
static void op_P_get(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    int16_t a = pop();
    if (a < 0) {
        if (tele_patterns[pn].l == 0)
            a = 0;
        else if (a < -tele_patterns[pn].l)
            a = 0;
        else
            a = tele_patterns[pn].l + a;
    }
    if (a > 63) a = 63;

    push(tele_patterns[pn].v[a]);
}
static void op_P_set(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    int16_t a = pop();
    int16_t b = pop();
    if (a < 0) {
        if (tele_patterns[pn].l == 0)
            a = 0;
        else if (a < -tele_patterns[pn].l)
            a = 0;
        else
            a = tele_patterns[pn].l + a;
    }
    if (a > 63) a = 63;

    tele_patterns[pn].v[a] = b;
    (*update_pi)();
}
static void op_P_INS(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    int16_t a, b, i;
    a = pop();
    b = pop();

    if (a < 0) {
        if (tele_patterns[pn].l == 0)
            a = 0;
        else if (a < -tele_patterns[pn].l)
            a = 0;
        else
            a = tele_patterns[pn].l + a;
    }
    if (a > 63) a = 63;

    if (tele_patterns[pn].l >= a) {
        for (i = tele_patterns[pn].l; i > a; i--)
            tele_patterns[pn].v[i] = tele_patterns[pn].v[i - 1];
        if (tele_patterns[pn].l < 63) tele_patterns[pn].l++;
    }

    tele_patterns[pn].v[a] = b;
    (*update_pi)();
}
static void op_P_RM(const void *NOTUSED(data), scene_state_t *ss,
                    exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    int16_t a, i;
    a = pop();

    if (a < 0) {
        if (tele_patterns[pn].l == 0)
            a = 0;
        else if (a < -tele_patterns[pn].l)
            a = 0;
        else
            a = tele_patterns[pn].l + a;
    }
    else if (a > tele_patterns[pn].l)
        a = tele_patterns[pn].l;

    if (tele_patterns[pn].l > 0) {
        push(tele_patterns[pn].v[a]);
        for (i = a; i < tele_patterns[pn].l; i++)
            tele_patterns[pn].v[i] = tele_patterns[pn].v[i + 1];

        tele_patterns[pn].l--;
    }
    else
        push(0);
    (*update_pi)();
}
static void op_P_PUSH(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    int16_t a;
    a = pop();

    if (tele_patterns[pn].l < 64) {
        tele_patterns[pn].v[tele_patterns[pn].l] = a;
        tele_patterns[pn].l++;
        (*update_pi)();
    }
}
static void op_P_POP(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t pn = ss->variables.p_n;
    if (tele_patterns[pn].l > 0) {
        tele_patterns[pn].l--;
        push(tele_patterns[pn].v[tele_patterns[pn].l]);
        (*update_pi)();
    }
    else
        push(0);
}
static void op_PN_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    int16_t b = pop();

    if (a < 0)
        a = 0;
    else if (a > 3)
        a = 3;

    if (b < 0) {
        if (tele_patterns[a].l == 0)
            b = 0;
        else if (b < -tele_patterns[a].l)
            b = 0;
        else
            b = tele_patterns[a].l + b;
    }
    if (b > 63) b = 63;

    push(tele_patterns[a].v[b]);
}
static void op_PN_set(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    int16_t b = pop();
    int16_t c = pop();

    if (a < 0)
        a = 0;
    else if (a > 3)
        a = 3;

    if (b < 0) {
        if (tele_patterns[a].l == 0)
            b = 0;
        else if (b < -tele_patterns[a].l)
            b = 0;
        else
            b = tele_patterns[a].l + b;
    }
    if (b > 63) b = 63;

    tele_patterns[a].v[b] = c;
    (*update_pi)();
}
static void op_TR_PULSE(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es),
                        command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    // saturate and shift
    if (a < 1)
        a = 1;
    else if (a > 4)
        a = 4;
    a--;
    int16_t time = ss->variables.tr_time[a];  // pulse time
    if (time <= 0) return;                    // if time <= 0 don't do anything
    ss->variables.tr[a] = ss->variables.tr_pol[a];
    tr_pulse[a] = time;  // set time
    update_tr(a, ss->variables.tr[a]);
}
static void op_II(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                  exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    int16_t b = pop();
    update_ii(a, b);
}
static void op_RSH(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() >> pop());
}
static void op_LSH(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() << pop());
}
static void op_S_L(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(tele_stack_top);
}
static void op_CV_SET(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    int16_t b = pop();
    // saturate and shift
    if (a < 1)
        a = 1;
    else if (a > 4)
        a = 4;
    a--;
    if (b < 0)
        b = 0;
    else if (b > 16383)
        b = 16383;
    ss->variables.cv[a] = b;
    (*update_cv)(a, b, 0);
}
static void op_EXP(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    if (a > 16383)
        a = 16383;
    else if (a < -16383)
        a = -16383;

    a = a >> 6;

    if (a < 0) {
        a = -a;
        push(-table_exp[a]);
    }
    else
        push(table_exp[a]);
}
static void op_ABS(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();

    if (a < 0)
        push(-a);
    else
        push(a);
}
static void op_AND(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() & pop());
}
static void op_OR(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                  exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() | pop());
}
static void op_XOR(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                   exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    push(pop() ^ pop());
}
static void op_JI(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                  exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    uint32_t ji = (((pop() << 8) / pop()) * 1684) >> 8;
    while (ji > 1683) ji >>= 1;
    push(ji);
}
static void op_SCRIPT(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    uint16_t a = pop();
    if (a > 0 && a < 9) (*run_script)(a);
}
static void op_KILL(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                    exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    clear_delays();
    (*update_kill)();
}
static void op_MUTE(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                    exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a;
    a = pop();

    if (a > 0 && a < 9) { (*update_mute)(a - 1, 0); }
}
static void op_UNMUTE(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a;
    a = pop();

    if (a > 0 && a < 9) { (*update_mute)(a - 1, 1); }
}
static void op_SCALE(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a, b, x, y, i;
    a = pop();
    b = pop();
    x = pop();
    y = pop();
    i = pop();

    push((i - a) * (y - x) / (b - a) + x);
}
static void op_STATE(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                     exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t a = pop();
    a--;
    if (a < 0)
        a = 0;
    else if (a > 7)
        a = 7;

    (*update_input)(a);
    push(input_states[a]);
}

static void op_ER(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                  exec_state_t *NOTUSED(es), command_state_t *NOTUSED(cs)) {
    int16_t fill = pop();
    int16_t len = pop();
    int16_t step = pop();
    push(euclidean(fill, len, step));
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// PROCESS //////////////////////////////////////////////////////

error_t parse(char *cmd, tele_command_t *out) {
    char cmd_copy[32];
    strcpy(cmd_copy, cmd);
    const char *delim = " \n";
    const char *s = strtok(cmd_copy, delim);

    uint8_t n = 0;
    out->l = n;

    while (s) {
        // CHECK IF NUMBER
        if (isdigit(s[0]) || s[0] == '-') {
            out->data[n].t = NUMBER;
            out->data[n].v = strtol(s, NULL, 0);
        }
        else if (s[0] == ':')
            out->data[n].t = SEP;
        else {
            int16_t i = -1;

            if (i == -1) {
                // CHECK AGAINST OPS
                i = OPS;

                while (i--) {
                    if (!strcmp(s, tele_ops[i].name)) {
                        out->data[n].t = OP;
                        out->data[n].v = i;
                        break;
                    }
                }
            }

            if (i == -1) {
                // CHECK AGAINST MOD
                i = MODS;

                while (i--) {
                    if (!strcmp(s, tele_mods[i].name)) {
                        out->data[n].t = MOD;
                        out->data[n].v = i;
                        break;
                    }
                }
            }

            if (i == -1) {
                strcpy(error_detail, s);
                return E_PARSE;
            }
        }

        s = strtok(NULL, delim);

        n++;
        out->l = n;

        if (n == COMMAND_MAX_LENGTH) return E_LENGTH;
    }

    return E_OK;
}


/////////////////////////////////////////////////////////////////
// VALIDATE /////////////////////////////////////////////////////

error_t validate(tele_command_t *c) {
    int16_t stack_depth = 0;
    uint8_t idx = c->l;
    c->separator = -1;  // i.e. the index ':'

    while (idx--) {  // process words right to left
        tele_word_t word_type = c->data[idx].t;
        int16_t word_value = c->data[idx].v;
        // A first_cmd is either at the beginning of the command or immediately
        // after the SEP
        bool first_cmd = idx == 0 || c->data[idx - 1].t == SEP;

        if (word_type == NUMBER) { stack_depth++; }
        else if (word_type == OP) {
            tele_op_t op = tele_ops[word_value];

            // if we're not a first_cmd we need to return something
            if (!first_cmd && !op.returns) {
                strcpy(error_detail, op.name);
                return E_NOT_LEFT;
            }

            stack_depth -= op.params;

            if (stack_depth < 0) {
                strcpy(error_detail, op.name);
                return E_NEED_PARAMS;
            }

            stack_depth += op.returns ? 1 : 0;

            // if we are in the first_cmd position and there is a set fn
            // decrease the stack depth
            // TODO this is technically wrong. the only reason we get away with
            // it is that it's idx == 0, and the while loop is about to end.
            if (first_cmd && op.set != NULL) stack_depth--;
        }
        else if (word_type == MOD) {
            error_t mod_error = E_OK;

            if (idx != 0)
                mod_error = E_NO_MOD_HERE;
            else if (c->separator == -1)
                mod_error = E_NEED_SEP;
            else if (stack_depth < tele_mods[word_value].params)
                mod_error = E_NEED_PARAMS;
            else if (stack_depth > tele_mods[word_value].params)
                mod_error = E_EXTRA_PARAMS;

            if (mod_error != E_OK) {
                strcpy(error_detail, tele_mods[word_value].name);
                return mod_error;
            }

            stack_depth = 0;
        }
        else if (word_type == SEP) {
            if (c->separator != -1)
                return E_MANY_SEP;
            else if (idx == 0)
                return E_PLACE_SEP;

            c->separator = idx;
            if (stack_depth > 1)
                return E_EXTRA_PARAMS;
            else
                stack_depth = 0;
        }
    }

    if (stack_depth > 1)
        return E_EXTRA_PARAMS;
    else
        return E_OK;
}


/////////////////////////////////////////////////////////////////
// PROCESS //////////////////////////////////////////////////////

process_result_t process(tele_command_t *c) {
    top = 0;

    // if the command has a MOD, only process it
    // allow the MOD to deal with processing the remainder
    int16_t idx = c->separator == -1 ? c->l : c->separator;

    while (idx--) {  // process from right to left
        tele_word_t word_type = c->data[idx].t;
        int16_t word_value = c->data[idx].v;

        if (word_type == NUMBER) { push(word_value); }
        else if (word_type == OP) {
            tele_op_t op = tele_ops[word_value];

            // if we're in the first command position, and there is a set fn
            // pointer and we have enough params, then run set, else run get
            if (idx == 0 && op.set != NULL && top >= op.params + 1)
                op.set(op.data, &scene_state, &exec_state, &command_state);
            else
                op.get(op.data, &scene_state, &exec_state, &command_state);
        }
        else if (word_type == MOD) {
            // TODO mods should be called with the subcommand (at the moment the
            // mod creates the subcommand = lots of duplication)
            tele_mods[word_value].func(c);
        }
    }

    if (top) {
        process_result_t o = {.has_value = true, .value = pop() };
        return o;
    }
    else {
        process_result_t o = {.has_value = false, .value = 0 };
        return o;
    }
}


char *print_command(const tele_command_t *c) {
    int16_t n = 0;
    char number[8];
    char *p = pcmd;

    *p = 0;

    while (n < c->l) {
        switch (c->data[n].t) {
            case OP:
                strcpy(p, tele_ops[c->data[n].v].name);
                p += strlen(tele_ops[c->data[n].v].name) - 1;
                break;
            case NUMBER:
                itoa(c->data[n].v, number, 10);
                strcpy(p, number);
                p += strlen(number) - 1;
                break;
            case MOD:
                strcpy(p, tele_mods[c->data[n].v].name);
                p += strlen(tele_mods[c->data[n].v].name) - 1;
                break;
            case SEP: *p = ':'; break;
            default: break;
        }

        n++;
        p++;
        *p = ' ';
        p++;
    }
    p--;
    *p = 0;

    return pcmd;
}


void tele_set_in(int16_t value) {
    scene_state.variables.in = value;
}

void tele_set_param(int16_t value) {
    scene_state.variables.param = value;
}

void tele_set_scene(int16_t value) {
    scene_state.variables.scene = value;
}

void tele_tick(uint8_t i) {
    process_delays(i);

    // inc time
    if (scene_state.variables.time_act) scene_state.variables.time += i;
}

void tele_init() {
    u8 i;

    for (i = 0; i < 4; i++) {
        tele_patterns[i].i = 0;
        tele_patterns[i].l = 0;
        tele_patterns[i].wrap = 1;
        tele_patterns[i].start = 0;
        tele_patterns[i].end = 63;
    }

    for (i = 0; i < 8; i++) mutes[i] = 1;
}


const char *to_v(int16_t i) {
    static char n[3], v[7];
    int16_t a = 0, b = 0;

    if (i > table_v[8]) {
        i -= table_v[8];
        a += 8;
    }

    if (i > table_v[4]) {
        i -= table_v[4];
        a += 4;
    }

    if (i > table_v[2]) {
        i -= table_v[2];
        a += 2;
    }

    if (i > table_v[1]) {
        i -= table_v[1];
        a += 1;
    }

    if (i > table_vv[64]) {
        i -= table_vv[64];
        b += 64;
    }

    if (i > table_vv[32]) {
        i -= table_vv[32];
        b += 32;
    }

    if (i > table_vv[16]) {
        i -= table_vv[16];
        b += 16;
    }

    if (i > table_vv[8]) {
        i -= table_vv[8];
        b += 8;
    }

    if (i > table_vv[4]) {
        i -= table_vv[4];
        b += 4;
    }

    if (i > table_vv[2]) {
        i -= table_vv[2];
        b++;
    }

    if (i > table_vv[1]) {
        i -= table_vv[1];
        b++;
    }

    b++;

    itoa(a, n, 10);
    strcpy(v, n);
    strcat(v, ".");
    itoa(b, n, 10);
    strcat(v, n);
    strcat(v, "V");

    return v;
    // printf(" (VV %d %d)",a,b);
}
