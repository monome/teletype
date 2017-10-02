#include <stdbool.h>
#include <stdint.h>
#include "every.h"

void every_tick(every_count_t *e) {
    (e->count)++;
    e->count %= e->mod;
}

void every_set_skip(every_count_t *e, bool skip) {
    e->skip = skip;
}

void every_set_count(every_count_t *e, int16_t count) {
    if (count < 0)
        count = 0;
    e->count = count;
}

void every_set_mod(every_count_t *e, int16_t mod) {
    if (mod < 0)
        mod = -mod;
    else if (mod == 0)
        mod = 1;        // lazy initialization
    e->mod = mod;
    e->count %= e->mod;
}
