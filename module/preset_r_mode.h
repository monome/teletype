#ifndef _PRESET_R_MODE_H_
#define _PRESET_R_MODE_H_

#include <stdbool.h>
#include <stdint.h>

void set_preset_r_mode(uint16_t knob);
void process_preset_r_knob(uint16_t knob, uint8_t mod_key);
void process_preset_r_long_front(void);
void process_preset_r_keys(uint8_t key, uint8_t mod_key, bool is_held_key);
uint8_t screen_refresh_preset_r(void);

#endif
