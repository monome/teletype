#include "ops/delay.h"

#include "helpers.h"
#include "teletype.h"
#include "teletype_io.h"

static bool delay_common_add(scene_state_t *ss, exec_state_t *es,
                             int16_t delay_time,
                             const tele_command_t *post_command);

static void mod_DEL_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command);

static void op_DEL_CLR_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);

static void mod_DEL_X_func(scene_state_t *ss, exec_state_t *es,
                           command_state_t *cs,
                           const tele_command_t *post_command);

static void mod_DEL_R_func(scene_state_t *ss, exec_state_t *es,
                           command_state_t *cs,
                           const tele_command_t *post_command);

static void mod_DEL_G_func(scene_state_t *ss, exec_state_t *es,
                           command_state_t *cs,
                           const tele_command_t *post_command);

static void mod_DEL_B_func(scene_state_t *ss, exec_state_t *es,
                           command_state_t *cs,
                           const tele_command_t *post_command);

const tele_mod_t mod_DEL = MAKE_MOD(DEL, mod_DEL_func, 1);
const tele_op_t op_DEL_CLR = MAKE_GET_OP(DEL.CLR, op_DEL_CLR_get, 0, false);
const tele_mod_t mod_DEL_X = MAKE_MOD(DEL.X, mod_DEL_X_func, 2);
const tele_mod_t mod_DEL_R = MAKE_MOD(DEL.R, mod_DEL_R_func, 2);
const tele_mod_t mod_DEL_G = MAKE_MOD(DEL.G, mod_DEL_G_func, 4);
const tele_mod_t mod_DEL_B = MAKE_MOD(DEL.B, mod_DEL_B_func, 2);

// common code to queue a delay shared between all delay ops
// NOTE it is the responsibility of the callee to call tele_has_delays
static bool delay_common_add(scene_state_t *ss, exec_state_t *es,
                             int16_t delay_time,
                             const tele_command_t *post_command) {
    int16_t i = 0;

    // 0 is the magic number for an empty slot.
    // Be careful not to set delay.time[i] to 0 before calling this function.
    while (ss->delay.time[i] != 0 && i != DELAY_SIZE) i++;

    if (delay_time < 1) delay_time = 1;

    if (i < DELAY_SIZE) {
        ss->delay.count++;
        ss->delay.time[i] = delay_time;
        ss->delay.origin_script[i] = es_variables(es)->script_number;
        ss->delay.origin_i[i] = es_variables(es)->i;
        copy_command(&ss->delay.commands[i], post_command);

        return true;
    }

    return false;
}

static void mod_DEL_func(scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs,
                         const tele_command_t *post_command) {
    int16_t delay_time = cs_pop(cs);

    delay_common_add(ss, es, delay_time, post_command);
    tele_has_delays(ss->delay.count > 0);
}

static void op_DEL_CLR_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es),
                           command_state_t *NOTUSED(cs)) {
    clear_delays(ss);
}

static void mod_DEL_X_func(scene_state_t *ss, exec_state_t *es,
                           command_state_t *cs,
                           const tele_command_t *post_command) {
    int16_t num_delays = cs_pop(cs);
    int16_t delay_time = cs_pop(cs);
    int16_t delay_time_next;

    if (delay_time < 1) delay_time = 1;

    // set first delay time to delay time
    delay_time_next = delay_time;

    while (num_delays > 0 &&
           delay_common_add(ss, es, delay_time_next, post_command)) {
        // increment delay time for next delay
        // normalise incremented value to stop negative wrap from increment
        delay_time_next += delay_time;
        delay_time_next = normalise_value(1, 32767, 1, delay_time_next);

        num_delays--;
    }

    tele_has_delays(ss->delay.count > 0);
}

static void mod_DEL_R_func(scene_state_t *ss, exec_state_t *es,
                           command_state_t *cs,
                           const tele_command_t *post_command) {
    int16_t num_delays = cs_pop(cs);
    int16_t delay_time = cs_pop(cs);
    int16_t delay_time_next;

    if (delay_time < 1) delay_time = 1;

    // set first delay time to 1ms to trigger immediately
    delay_time_next = 1;

    while (num_delays > 0 &&
           delay_common_add(ss, es, delay_time_next, post_command)) {
        // increment delay time for next delay
        // normalise incremented value to stop negative wrap from increment
        delay_time_next += delay_time;
        delay_time_next = normalise_value(1, 32767, 1, delay_time_next);

        num_delays--;
    }

    tele_has_delays(ss->delay.count > 0);
}

static void mod_DEL_G_func(scene_state_t *ss, exec_state_t *es,
                           command_state_t *cs,
                           const tele_command_t *post_command) {
    int16_t num_delays = cs_pop(cs);
    int16_t delay_time = cs_pop(cs);
    int16_t delay_mult_num = cs_pop(cs);
    int16_t delay_mult_denom = cs_pop(cs);
    int16_t delay_time_next;

    if (delay_time < 1) delay_time = 1;

    // set first delay time to 1ms to trigger immediately
    delay_time_next = 1;

    while (num_delays > 0 &&
           delay_common_add(ss, es, delay_time_next, post_command)) {
        // increment delay time for next delay
        // normalise incremented value to stop negative wrap from increment
        delay_time_next += delay_time;
        delay_time_next = normalise_value(1, 32767, 1, delay_time_next);
        delay_time = (delay_time * delay_mult_num) / delay_mult_denom;

        num_delays--;
    }

    tele_has_delays(ss->delay.count > 0);
}

static void mod_DEL_B_func(scene_state_t *ss, exec_state_t *es,
                           command_state_t *cs,
                           const tele_command_t *post_command) {

    int16_t base_time = cs_pop(cs);
	if (base_time < 1) base_time = 1;
	int16_t mask = cs_pop(cs);
	
    int16_t delay_time_next = 1;

	for (uint8_t i = 0; i <= 15; i++) {
		if ((mask >> i) & 1) {
			if (i == 0) {
				delay_time_next = 1;
			}
			else {
				delay_time_next = normalise_value(1, 32767, 0, i * base_time);
			}
			delay_common_add(ss, es, delay_time_next, post_command);
		}
	}

    tele_has_delays(ss->delay.count > 0);
}