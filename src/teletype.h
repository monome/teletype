#ifndef _TELETYPE_H_
#define _TELETYPE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "command.h"
#include "state.h"

#define TELE_ERROR_MSG_LENGTH 16
// #define TELETYPE_PROFILE // un-comment this line to enable profiling

typedef enum {
    E_OK,
    E_PARSE,
    E_LENGTH,
    E_NEED_PARAMS,
    E_EXTRA_PARAMS,
    E_NO_MOD_HERE,
    E_MANY_PRE_SEP,
    E_NEED_PRE_SEP,
    E_PLACE_PRE_SEP,
    E_NO_SUB_SEP_IN_PRE,
    E_NOT_LEFT,
    E_NEED_SPACE_PRE_SEP,
    E_NEED_SPACE_SUB_SEP
} error_t;

typedef struct {
    bool has_value;
    int16_t value;
} process_result_t;


error_t parse(const char *cmd, tele_command_t *out,
              char error_msg[TELE_ERROR_MSG_LENGTH]);
error_t validate(const tele_command_t *c,
                 char error_msg[TELE_ERROR_MSG_LENGTH]);
process_result_t run_script(scene_state_t *ss, size_t script_no);
process_result_t run_script_with_exec_state(scene_state_t *ss, exec_state_t *es,
                                            size_t script_no);
process_result_t process_command(scene_state_t *ss, exec_state_t *es,
                                 const tele_command_t *cmd);

void tele_tick(scene_state_t *ss, uint8_t);

void clear_delays(scene_state_t *ss);

const char *tele_error(error_t);

#endif
