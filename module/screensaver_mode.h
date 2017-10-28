#ifndef SCREENSAVER_H
#define SCREENSAVER_H
#include <stdint.h>

#define SS_TIMEOUT 90 /* minutes */ * 60 * 100
//#define SS_TIMEOUT 1000 // 10 seconds
#define SS_DROP_KEYSTROKE 1 // drops the keystroke that caused wake-up

uint8_t screen_refresh_screensaver(void);
void set_screensaver_mode(void);

#endif
