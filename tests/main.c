#include <stdint.h>

#include "greatest/greatest.h"

#include "teletype.h"
#include "teletype_io.h"

#include "match_token_tests.h"
#include "op_mod_tests.h"
#include "parser_tests.h"
#include "process_tests.h"
#include "turtle_tests.h"

uint32_t tele_get_ticks() {
    return 0;
}
void tele_metro_updated() {}
void tele_metro_reset() {}
void tele_tr(uint8_t i, int16_t v) {}
void tele_cv(uint8_t i, int16_t v, uint8_t s) {}
void tele_cv_slew(uint8_t i, int16_t v) {}
void tele_update_adc(uint8_t force) {}
void tele_has_delays(bool i) {}
void tele_has_stack(bool i) {}
void tele_cv_off(uint8_t i, int16_t v) {}
void tele_ii_tx(uint8_t addr, uint8_t *data, uint8_t l) {}
void tele_ii_rx(uint8_t addr, uint8_t *data, uint8_t l) {}
void tele_scene(uint8_t i, uint8_t init_grid, uint8_t init_pattern) {}
void tele_pattern_updated() {}
void tele_kill() {}
void tele_mute() {}
void tele_vars_updated() {}
void tele_profile_script(size_t s) {}
void tele_profile_delay(uint8_t d)  {}
bool tele_get_input_state(uint8_t n) {
    return false;
}
void device_flip() {}
void reset_midi_counter() {}

void tele_save_calibration() {}

void grid_key_press(uint8_t x, uint8_t y, uint8_t z) {}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(match_token_suite);
    RUN_SUITE(op_mod_suite);
    RUN_SUITE(parser_suite);
    RUN_SUITE(process_suite);
    RUN_SUITE(turtle_suite);

    GREATEST_MAIN_END();
}
