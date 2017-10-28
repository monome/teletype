#ifndef _LIVE_MODE_H_
#define _LIVE_MODE_H_

#include "stdbool.h"
#include "stdint.h"

void set_slew_icon(bool display);
void set_metro_icon(bool display);
void init_live_mode(void);
void set_live_mode(void);
void process_live_keys(uint8_t key, uint8_t mod_key, bool is_held_key);
uint8_t screen_refresh_live(void);

#endif
