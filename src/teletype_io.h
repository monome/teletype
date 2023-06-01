#ifndef _TELETYPE_IO_H_
#define _TELETYPE_IO_H_

#include <stdbool.h>
#include <stdint.h>

#define SUB_MODE_OFF 0
#define SUB_MODE_VARS 1
#define SUB_MODE_GRID 2
#define SUB_MODE_FULLGRID 3
#define SUB_MODE_DASH 4

// These functions are for interacting with the teletype hardware, each target
// must provide it's own implementation

// used for TIME and LAST
extern uint32_t tele_get_ticks(void);

// called when M or M.ACT are updated
extern void tele_metro_updated(void);

// called by M.RESET
extern void tele_metro_reset(void);

extern void tele_tr(uint8_t i, int16_t v);
extern void tele_tr_pulse(uint8_t i, int16_t time);
extern void tele_tr_pulse_clear(uint8_t i);
extern void tele_tr_pulse_time(uint8_t i, int16_t time);
extern void tele_cv(uint8_t i, int16_t v, uint8_t s);
extern void tele_cv_slew(uint8_t i, int16_t v);
extern uint16_t tele_get_cv(uint8_t i);
extern void tele_cv_cal(uint8_t n, int32_t b, int32_t m);

extern void tele_update_adc(uint8_t force);

// inform target if there are delays
extern void tele_has_delays(bool has_delays);

// inform target if the stack has entries
extern void tele_has_stack(bool has_stack);

extern void tele_cv_off(uint8_t i, int16_t v);
extern void tele_ii_tx(uint8_t addr, uint8_t *data, uint8_t l);
extern void tele_ii_rx(uint8_t addr, uint8_t *data, uint8_t l);
extern void tele_scene(uint8_t i, uint8_t init_grid, uint8_t init_pattern);

// called when a pattern is updated
extern void tele_pattern_updated(void);

extern void tele_vars_updated(void);

extern void tele_kill(void);
extern void tele_mute(void);
extern bool tele_get_input_state(uint8_t);

void tele_save_calibration(void);

#ifdef TELETYPE_PROFILE
void tele_profile_script(size_t);
void tele_profile_delay(uint8_t);
#endif

// emulate grid key press
extern void grid_key_press(uint8_t x, uint8_t y, uint8_t z);

// manage device config
extern void device_flip(void);

// live screen / dashboard
extern void set_live_submode(uint8_t submode);
extern void select_dash_screen(uint8_t screen);
extern void print_dashboard_value(uint8_t index, int16_t value);
extern int16_t get_dashboard_value(uint8_t index);

extern void reset_midi_counter(void);

#endif
