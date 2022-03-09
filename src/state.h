#ifndef _STATE_H_
#define _STATE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "command.h"
#include "every.h"
#include "random.h"
#include "scale.h"
#include "script.h"
#include "turtle.h"
#include "types.h"

#define STACK_SIZE 16
#define CV_COUNT 4
#define Q_LENGTH 64
#define TR_COUNT 4
#define TRIGGER_INPUTS 8
#define DELAY_SIZE 64
#define STACK_OP_SIZE 16
#define PATTERN_COUNT 4
#define PATTERN_LENGTH 64
#define SCRIPT_MAX_COMMANDS 6
#define EXEC_DEPTH 8
#define WHILE_DEPTH 10000
#define RAND_STATES_COUNT 5

#define GRID_GROUP_COUNT 64
#define GRID_MAX_DIMENSION 16
#define GRID_BUTTON_COUNT 256
#define GRID_FADER_COUNT 64
#define GRID_XYPAD_COUNT 8
#define LED_DIM -1
#define LED_BRI -2
#define LED_OFF -3
// H - horizontal, V - vertical
// C - coarse, F - fine
// H must be even, V must be odd
#define FADER_CH_BAR 0
#define FADER_CV_BAR 1
#define FADER_CH_DOT 2
#define FADER_CV_DOT 3
#define FADER_COARSE FADER_CV_DOT
#define FADER_FH_BAR 4
#define FADER_FV_BAR 5
#define FADER_FH_DOT 6
#define FADER_FV_DOT 7

#define MAX_MIDI_EVENTS 10

#define METRO_MIN_MS 25
#define METRO_MIN_UNSUPPORTED_MS 2

#define NB_NBX_SCALES 16


////////////////////////////////////////////////////////////////////////////////
// SCENE STATE /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// clang-format off
typedef struct {
    // Maintaining this order allows for efficient access to the group
    // WARNING: DO NOT CHANGE THE ORDER OF VARIABLES a THROUGH t
    int16_t a;
    int16_t x;
    int16_t b;
    int16_t y;
    int16_t c;
    int16_t z;
    int16_t d;
    int16_t t;
    // END WARNING SECTION
    int16_t j[TOTAL_SCRIPT_COUNT];
    int16_t k[TOTAL_SCRIPT_COUNT];
    int16_t cv[CV_COUNT];
    int16_t cv_off[CV_COUNT];
    int16_t cv_slew[CV_COUNT];
    int16_t drunk;
    int16_t drunk_max;
    int16_t drunk_min;
    int16_t drunk_wrap;
    int16_t flip;
    int16_t in;
    int16_t m;
    bool m_act;
    bool mutes[TRIGGER_INPUTS];  // TODO: replace with uint8_t bits
    int16_t o;
    int16_t o_inc;
    int16_t o_min;
    int16_t o_max;
    int16_t o_wrap;
    int16_t p_n;
    int16_t param;
    int16_t q[Q_LENGTH];
    int16_t q_n;
    int16_t q_grow;
    int16_t r_min;
    int16_t r_max;
    int16_t n_scale_bits[NB_NBX_SCALES];
    int16_t n_scale_root[NB_NBX_SCALES];
    int16_t scene;
    uint8_t script_pol[TRIGGER_INPUTS];
    int64_t time;
    uint8_t time_act;
    int16_t tr[TR_COUNT];
    int16_t tr_pol[TR_COUNT];
    int16_t tr_time[TR_COUNT];
    int16_t seed;
    scale_data_t in_range;
    scale_t in_scale;
    scale_data_t param_range;
    scale_t param_scale;
    scale_data_t fader_ranges[64];
    scale_t fader_scales[64];
    scale_data_t cv_ranges[4];
    scale_t cv_scales[4];
} scene_variables_t;
// clang-format on

typedef struct {
    int16_t idx;
    uint16_t len;
    uint16_t wrap;
    int16_t start;
    int16_t end;
    int16_t val[PATTERN_LENGTH];
} scene_pattern_t;

typedef struct {
    // TODO add a delay variables struct?
    tele_command_t commands[DELAY_SIZE];
    int16_t time[DELAY_SIZE];
    uint8_t origin_script[DELAY_SIZE];
    int16_t origin_i[DELAY_SIZE];
    uint8_t count;
} scene_delay_t;

typedef struct {
    tele_command_t commands[STACK_OP_SIZE];
    uint8_t top;
} scene_stack_op_t;

typedef struct {
    uint8_t l;
    tele_command_t c[SCRIPT_MAX_COMMANDS];
    every_count_t every[SCRIPT_MAX_COMMANDS];
    uint32_t last_time;
} scene_script_t;

typedef struct {
    u8 enabled;
    u8 group;
    u8 x, y;
    u8 w, h;
    s16 level;
    s8 script;
} grid_common_t;

typedef struct {
    u8 enabled;
    s8 script;
    s16 fader_min;
    s16 fader_max;
} grid_group_t;

typedef struct {
    grid_common_t common;
    u8 latch;
    u8 state;
} grid_button_t;

typedef struct {
    grid_common_t common;
    u8 type;
    u8 value;
    u8 slide;
    u8 slide_acc;
    u8 slide_end;
    u8 slide_delta;
    u8 slide_dir;
} grid_fader_t;

typedef struct {
    grid_common_t common;
    u8 value_x;
    u8 value_y;
} grid_xypad_t;

typedef struct {
    u8 grid_dirty;
    u8 scr_dirty;
    u8 clear_held;

    u8 rotate;
    u8 dim;

    u8 current_group;
    u8 latest_group;
    u8 latest_button;
    u8 latest_fader;

    s8 leds[GRID_MAX_DIMENSION][GRID_MAX_DIMENSION];
    grid_group_t group[GRID_GROUP_COUNT];

    grid_button_t button[GRID_BUTTON_COUNT];
    grid_fader_t fader[GRID_FADER_COUNT];
    grid_xypad_t xypad[GRID_XYPAD_COUNT];
} scene_grid_t;

typedef struct {
    int8_t on_script;
    int8_t off_script;
    int8_t cc_script;
    int8_t clk_script;
    int8_t start_script;
    int8_t stop_script;
    int8_t continue_script;

    uint8_t last_event_type;
    uint8_t last_channel;
    uint8_t last_note;
    uint8_t last_velocity;
    uint8_t last_controller;
    uint8_t last_cc;

    uint8_t on_count;
    uint8_t on_channel[MAX_MIDI_EVENTS];
    uint8_t note_on[MAX_MIDI_EVENTS];
    uint8_t note_vel[MAX_MIDI_EVENTS];
    uint8_t off_count;
    uint8_t note_off[MAX_MIDI_EVENTS];
    uint8_t off_channel[MAX_MIDI_EVENTS];
    uint8_t cc_count;
    uint8_t cc_channel[MAX_MIDI_EVENTS];
    uint8_t cn[MAX_MIDI_EVENTS];
    uint8_t cc[MAX_MIDI_EVENTS];
    uint8_t clock_div;
} scene_midi_t;

typedef struct {
    random_state_t rand;
    s16 seed;
} tele_rand_t;

typedef union {
    struct {
        tele_rand_t rand;
        tele_rand_t prob;
        tele_rand_t toss;
        tele_rand_t pattern;
        tele_rand_t drunk;
    } s;

    tele_rand_t a[RAND_STATES_COUNT];
} scene_rand_t;

typedef struct {
    bool initializing;
    scene_variables_t variables;
    scene_pattern_t patterns[PATTERN_COUNT];
    scene_delay_t delay;
    scene_stack_op_t stack_op;
    scene_script_t scripts[TOTAL_SCRIPT_COUNT];
    scene_turtle_t turtle;
    bool every_last;
    scene_grid_t grid;
    scene_rand_t rand_states;
    cal_data_t cal;
    int8_t i2c_op_address;
    scene_midi_t midi;
} scene_state_t;

extern void ss_init(scene_state_t *ss);
extern void ss_variables_init(scene_state_t *ss);
extern void ss_patterns_init(scene_state_t *ss);
extern void ss_pattern_init(scene_state_t *ss, size_t pattern_no);
extern void ss_grid_init(scene_state_t *ss);
extern void ss_grid_common_init(grid_common_t *gc);
extern void ss_rand_init(scene_state_t *ss);
extern void ss_midi_init(scene_state_t *ss);
extern void ss_cal_init(scene_state_t *ss);

extern void ss_set_in(scene_state_t *ss, int16_t value);
extern void ss_set_param(scene_state_t *ss, int16_t value);
extern void ss_set_scene(scene_state_t *ss, int16_t value);
extern uint8_t ss_get_script_pol(scene_state_t *ss, size_t idx);
extern void ss_set_script_pol(scene_state_t *ss, size_t idx, uint8_t pol);

extern bool ss_get_mute(scene_state_t *ss, uint8_t idx);
extern void ss_set_mute(scene_state_t *ss, uint8_t idx, bool value);

extern int16_t ss_get_pattern_idx(scene_state_t *ss, size_t pattern);
extern void ss_set_pattern_idx(scene_state_t *ss, size_t pattern, int16_t i);
extern int16_t ss_get_pattern_len(scene_state_t *ss, size_t pattern);
extern void ss_set_pattern_len(scene_state_t *ss, size_t pattern, int16_t l);
extern uint16_t ss_get_pattern_wrap(scene_state_t *ss, size_t pattern);
extern void ss_set_pattern_wrap(scene_state_t *ss, size_t pattern,
                                uint16_t wrap);
extern int16_t ss_get_pattern_start(scene_state_t *ss, size_t pattern);
extern void ss_set_pattern_start(scene_state_t *ss, size_t pattern,
                                 int16_t start);
extern int16_t ss_get_pattern_end(scene_state_t *ss, size_t pattern);
extern void ss_set_pattern_end(scene_state_t *ss, size_t pattern, int16_t end);
extern int16_t ss_get_pattern_val(scene_state_t *ss, size_t pattern,
                                  size_t idx);
extern void ss_set_pattern_val(scene_state_t *ss, size_t pattern, size_t idx,
                               int16_t val);
extern scene_pattern_t *ss_patterns_ptr(scene_state_t *ss);
extern size_t ss_patterns_size(void);

uint8_t ss_get_script_len(scene_state_t *ss, uint8_t idx);
const tele_command_t *ss_get_script_command(scene_state_t *ss,
                                            uint8_t script_idx, size_t c_idx);
void ss_copy_script_command(tele_command_t *dest, scene_state_t *ss,
                            uint8_t script_idx, size_t c_idx);
bool ss_get_script_comment(scene_state_t *ss, uint8_t script_idx, size_t c_idx);
void ss_set_script_comment(scene_state_t *ss, uint8_t script_idx, size_t c_idx,
                           uint8_t on);
void ss_toggle_script_comment(scene_state_t *ss, uint8_t script_idx,
                              size_t c_idx);
void ss_overwrite_script_command(scene_state_t *ss, uint8_t script_idx,
                                 size_t command_idx, const tele_command_t *cmd);
void ss_insert_script_command(scene_state_t *ss, uint8_t script_idx,
                              size_t command_idx, const tele_command_t *cmd);
void ss_delete_script_command(scene_state_t *ss, uint8_t script_idx,
                              size_t command_idx);
void ss_clear_script(scene_state_t *ss, size_t script_idx);

scene_script_t *ss_scripts_ptr(scene_state_t *ss);
size_t ss_scripts_size(uint8_t script_count);
int16_t ss_get_script_last(scene_state_t *ss, uint8_t idx);
void ss_update_script_last(scene_state_t *ss, uint8_t idx);
every_count_t *ss_get_every(scene_state_t *ss, uint8_t idx, uint8_t line);
void ss_sync_every(scene_state_t *ss, int16_t count);
bool every_is_now(scene_state_t *ss, every_count_t *e);
bool skip_is_now(scene_state_t *ss, every_count_t *e);
scene_turtle_t *ss_turtle_get(scene_state_t *);
void ss_turtle_set(scene_state_t *, scene_turtle_t *);
int16_t ss_turtle_get_val(scene_state_t *, scene_turtle_t *);
void ss_turtle_set_val(scene_state_t *, scene_turtle_t *, int16_t);

void ss_set_param_scale(scene_state_t *, int16_t, int16_t);
void ss_set_in_scale(scene_state_t *, int16_t, int16_t);
void ss_set_fader_scale(scene_state_t *ss, int16_t fader, int16_t min,
                        int16_t max);
void ss_set_cv_scale(scene_state_t *ss, int16_t n, int16_t min, int16_t max);
void ss_update_in_scale(scene_state_t *);
void ss_update_param_scale(scene_state_t *);
void ss_update_fader_scale(scene_state_t *ss, int16_t fader);
void ss_update_fader_scale_all(scene_state_t *ss);
void ss_update_cv_scale(scene_state_t *ss, int16_t n);
void ss_update_cv_scale_all(scene_state_t *ss);

int16_t ss_get_param(scene_state_t *);
int16_t ss_get_in(scene_state_t *);
int16_t ss_get_in_min(scene_state_t *);
int16_t ss_get_in_max(scene_state_t *);
void ss_set_in_min(scene_state_t *, int16_t);
void ss_set_in_max(scene_state_t *, int16_t);
void ss_reset_in_cal(scene_state_t *);
int16_t ss_get_param_min(scene_state_t *);
int16_t ss_get_param_max(scene_state_t *);
void ss_set_param_min(scene_state_t *, int16_t);
void ss_set_param_max(scene_state_t *, int16_t);
void ss_reset_param_cal(scene_state_t *);
int16_t ss_get_fader_min(scene_state_t *ss, int16_t fader);
int16_t ss_get_fader_max(scene_state_t *ss, int16_t fader);
void ss_set_fader_min(scene_state_t *ss, int16_t fader, int16_t min);
void ss_set_fader_max(scene_state_t *ss, int16_t fader, int16_t max);
void ss_reset_fader_cal(scene_state_t *ss, int16_t fader);

////////////////////////////////////////////////////////////////////////////////
// EXEC STATE //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    bool if_else_condition;
    int16_t i;
    bool while_continue;
    uint16_t while_depth;
    bool breaking;
    uint8_t script_number;
    uint8_t line_number;
    bool delayed;
    int16_t fparam1;
    int16_t fparam2;
    int16_t fresult;
    bool fresult_set;
} exec_vars_t;

typedef struct {
    exec_vars_t variables[EXEC_DEPTH];
    uint8_t exec_depth;
    bool overflow;
} exec_state_t;

extern void es_init(exec_state_t *es);
extern size_t es_depth(exec_state_t *es);
extern size_t es_push(exec_state_t *es);
extern size_t es_push_fparams(exec_state_t *es, int16_t param1, int16_t param2);
extern size_t es_pop(exec_state_t *es);
extern void es_set_script_number(exec_state_t *es, uint8_t script_number);
extern void es_set_line_number(exec_state_t *es, uint8_t line_number);
extern uint8_t es_get_line_number(exec_state_t *es);
extern exec_vars_t *es_variables(exec_state_t *es);

////////////////////////////////////////////////////////////////////////////////
// COMMAND STATE ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    int16_t values[STACK_SIZE];
    int16_t top;
} command_state_stack_t;

typedef struct {
    command_state_stack_t stack;
} command_state_t;

extern void cs_init(command_state_t *cs);
extern int16_t cs_stack_size(command_state_t *cs);

// by declaring the following static inline, each compilation unit (i.e. C
// file), gets its own copy of the function
static inline int16_t cs_pop(command_state_t *cs) {
    cs->stack.top--;
    return cs->stack.values[cs->stack.top];
}

static inline void cs_push(command_state_t *cs, int16_t data) {
    cs->stack.values[cs->stack.top] = data;
    cs->stack.top++;
}


#endif
