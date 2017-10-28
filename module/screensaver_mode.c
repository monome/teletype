#include "screensaver_mode.h"
#include "globals.h"
#include "region.h"

static bool blank;

void set_screensaver_mode() {
    for (int i = 0; i < 8; i++)
        region_fill(&line[i], 0);
    blank = false;
}

uint8_t screen_refresh_screensaver() {
    if (!blank) {
        blank = true;
        return 0xFF;
    }
    return 0;
} 
