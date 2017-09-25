#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include "log.h"

static inline log_t *get_log() {
    static log_t log;
    return &log;
}

#define L get_log()

void log_init() {
    L->size = LOG_SIZE;
    L->buf = calloc(LOG_SIZE, sizeof(char));
    L->end = L->buf + LOG_SIZE;
}

void log_grow() {
    char *new = calloc(L->size * 2, sizeof(char));
    memcpy(new, L->buf, L->size);
    free(L->buf);
    L->buf = new;
    L->size *= 2;
    L->end = L->buf + L->size;
}

log_position_t *log_seek() {
    log_position_t *here = get_log()->buf;
    log_position_t *last = here;

    while (*here != 0) {
        last = here;
        here += *here + 1;
    }
    return last;
}


log_position_t *log_seek_next() {
    log_position_t *here = get_log()->buf;

    while (*here != 0)
        here += *here + 1;
    return here;
}

bool catting = false;

void lprintf(const char *format, ...) {
    log_t *log = L;
    va_list arg;
    char buf[LOG_LINE_SIZE];
    va_start(arg, format);
    size_t len = vsnprintf(buf, LOG_LINE_SIZE, format, arg);
    va_end(arg);
    log_position_t *here = log_seek_next();
    if (here + len + 2 > log->end)
        log_grow();
    here = log_seek_next();
    strcpy(here + 1, buf);
    *here = len + 1;
    catting = false;
}

void lcat(const char *str) {
    log_position_t *(*seek)(); // Function ponters: useful! 
    if (catting)
        seek = log_seek;
    else 
        seek = log_seek_next;
    log_position_t *here = seek();
    if (here + strlen(str) + 1 > L->end) {
        log_grow();
        here = seek();
    }

    char *new = strcat(here + 1, str);
    *here = strlen(new) + 1;
    catting = true;
}

void log_clear() {
    memset(L->buf, 0, L->size);
}

void log_print() {
    puts("\n"); // two newlines, actually.
    log_position_t *here = get_log()->buf;
    while (*here != 0) {
        puts(here + 1);
        here += *here + 1;
    }
}

