#include "command.h"

#include <string.h>  // memcpy
#include "ops/op.h"
#include "util.h"

void copy_command(tele_command_t *dst, const tele_command_t *src) {
    // TODO does this need to use memcpy?
    memcpy(dst, src, sizeof(tele_command_t));
}

void copy_post_command(tele_command_t *dst, const tele_command_t *src) {
    dst->length = src->length - src->separator - 1;
    dst->separator = -1;
    memcpy(dst->data, &src->data[src->separator + 1],
           dst->length * sizeof(tele_data_t));
}

static void itoa_hex(uint16_t value, char *out) {
    static char num[] = "0123456789ABCDEF";

    out[0] = 'X';
    uint8_t v, index = 1, dont_ignore_zeros = 0;

    for (int8_t i = 3; i >= 0; i--) {
        v = (value >> (i << 2)) & 0xf;
        if (dont_ignore_zeros || v) {
            out[index++] = num[v];
            dont_ignore_zeros = 1;
        }
    }

    if (!dont_ignore_zeros) out[index++] = '0';
    out[index] = '\0';
}

static void itoa_bin(uint16_t value, char *out) {
    out[0] = 'B';
    uint8_t v, index = 1, dont_ignore_zeros = 0;

    for (int8_t i = 15; i >= 0; i--) {
        v = (value >> i) & 1;
        if (dont_ignore_zeros || v) {
            out[index++] = '0' + v;
            dont_ignore_zeros = 1;
        }
    }

    if (!dont_ignore_zeros) out[index++] = '0';
    out[index] = '\0';
}


static void itoa_rbin(uint16_t value, char *out) {
    out[0] = 'R';
    uint8_t v, index = 1;

    for (int8_t i = 0; i < 16; i++) {
        v = (value >> i) & 1;
        out[index++] = '0' + v;
    }

    index = 0; // catch this below if all 0s
    for (int8_t i = 16; i > 0; i--) {
        if (out[i] == '1') {
            index = i;
            break;
        }
    }
    if (index == 0){     // all 0s....
	index = 1;       // ..in idx1 ...
	out[index] = '0';// ..add a single 0 next to R
    }
    out[index + 1] = '\0'; //terminate
}

void print_command(const tele_command_t *cmd, char *out) {
    out[0] = 0;
    for (size_t i = 0; i < cmd->length; i++) {
        tele_word_t tag = cmd->data[i].tag;
        int16_t value = cmd->data[i].value;

        switch (tag) {
            case OP: strcat(out, tele_ops[value]->name); break;
            case NUMBER: {
                char number[8];
                itoa(value, number, 10);
                strcat(out, number);
                break;
            }
            case XNUMBER: {
                char number[6];
                itoa_hex(value, number);
                strcat(out, number);
                break;
            }
            case BNUMBER: {
                char number[18];
                itoa_bin(value, number);
                strcat(out, number);
                break;
            }
            case RNUMBER: {
                char number[18];
                itoa_rbin(value, number);
                strcat(out, number);
                break;
            }
            case MOD: strcat(out, tele_mods[value]->name); break;
            case PRE_SEP: strcat(out, ":"); break;
            case SUB_SEP: strcat(out, ";"); break;
        }

        // do we need to add a space?
        // first check if we're not at the end
        if (i < cmd->length - 1) {
            // otherwise, only add a space if the next tag is a not a seperator
            tele_word_t next_tag = cmd->data[i + 1].tag;
            if (next_tag != PRE_SEP && next_tag != SUB_SEP) {
                strcat(out, " ");
            }
        }
    }
}
