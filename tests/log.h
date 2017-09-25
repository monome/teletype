#ifndef _LOG_H
#define _LOG_H

#define LOG_SIZE 2048
#define LOG_LINE_SIZE 255

typedef char log_position_t;

typedef struct {
    char *buf;
    char *end;
    size_t size;
} log_t;


void log_init(void);
void lprintf(const char *, ...);
void lcat(const char *);
void log_print(void);
void log_clear(void);

#endif
