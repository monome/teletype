#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <stdbool.h>
#include <stdint.h>

#define COMMAND_MAX_LENGTH 16

typedef enum {
    NUMBER,
    XNUMBER,
    BNUMBER,
    RNUMBER,
    OP,
    MOD,
    PRE_SEP,
    SUB_SEP
} tele_word_t;

typedef struct {
    tele_word_t tag;
    int16_t value;
} tele_data_t;

typedef struct {
    uint8_t length;
    int8_t separator;
    tele_data_t data[COMMAND_MAX_LENGTH];
    bool comment;
} tele_command_t;

void copy_command(tele_command_t *dst, const tele_command_t *src);
void copy_post_command(tele_command_t *dst, const tele_command_t *src);
void print_command(const tele_command_t *c, char *out);

#endif
