#ifndef _PRESET_W_MODE_H_
#define _PRESET_W_MODE_H_

#include <stdbool.h>
#include <stdint.h>

void set_preset_w_mode(void);
void process_preset_w_keys(uint8_t key, uint8_t mod_key, bool is_held_key);
uint8_t screen_refresh_preset_w(void);

#endif
