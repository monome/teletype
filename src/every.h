#ifndef _EVERY_H
#define _EVERY_H

typedef struct {
    int16_t count;
    int16_t mod;
} every_count_t;
    

void every_tick(every_count_t*);
bool every_is_now(every_count_t*);
void every_set_count(every_count_t*, int16_t);
void every_set_mod(every_count_t*, int16_t);

#endif
