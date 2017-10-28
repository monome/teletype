#ifndef _PATTERN_MODE_H_
#define _PATTERN_MODE_H_

#include <stdbool.h>
#include <stdint.h>

void set_pattern_mode(void);
void process_pattern_keys(uint8_t key, uint8_t mod_key, bool is_held_key);
void process_pattern_knob(uint16_t knob, uint8_t mod_key);
uint8_t screen_refresh_pattern(void);

#endif
