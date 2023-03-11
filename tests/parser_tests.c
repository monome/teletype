#include "parser_tests.h"

#include "greatest/greatest.h"

#include "ops/op.h"
#include "ops/op_enum.h"
#include "teletype.h"

#define CORPUS_COUNT 141
char corpus[CORPUS_COUNT][32] = { "CV 1 ADD N P.HERE Z",
                                  "CV 1 N P.HERE",
                                  "CV 1 N P.NEXT",
                                  "CV 1 N QT RAND 36 3",
                                  "CV 2 ADD CV 2 Z",
                                  "CV 2 N ADD Y PN P.N Z",
                                  "CV 2 N P.NEXT",
                                  "CV 2 N PN 1 P.HERE",
                                  "CV 2 N PN P.N ADD Z X",
                                  "CV 3 N ADD Y P.HERE",
                                  "CV 3 N P.NEXT",
                                  "CV 3 N PN 2 P.HERE",
                                  "CV 3 N WRAP X 0 48",
                                  "CV 3 V RAND 5",
                                  "CV 4 0",
                                  "CV 4 N PN 3 P.HERE",
                                  "CV 4 N WRAP 0 0 11",
                                  "CV 4 V 8",
                                  "CV 4 V RAND TOSS",
                                  "CV 4 VV P.NEXT",
                                  "CV.SET 3 V RAND 5",
                                  "CV.SET 4 0",
                                  "CV.SET 4 V RAND TOSS",
                                  "CV.SLEW 2 1000",
                                  "CV.SLEW 3 5",
                                  "CV.SLEW 3 500",
                                  "CV.SLEW 4 200",
                                  "CV.SLEW 4 T",
                                  "CV.SLEW 4 Y",
                                  "DEL ADD 40 Y: TR B 0",
                                  "DEL MUL T 4: TR.PULSE A",
                                  "DEL MUL T 7: TR.PULSE A",
                                  "DEL MUL T RRAND 1 5: TR.TOG A",
                                  "DEL RAND Y: TR.PULSE B",
                                  "DEL T: TR.PULSE C",
                                  "DEL T: TR.PULSE D",
                                  "DRUNK 12",
                                  "ELIF EQ 0 MOD X 3: P.N 0",
                                  "ELIF EQ X 4: P.N 2",
                                  "ELIF TOSS: P.PREV",
                                  "ELSE: P.N 1",
                                  "ELSE: P.NEXT",
                                  "IF EQ X 6: P.N 3",
                                  "IF TOSS: P.NEXT",
                                  "IF X: P.PREV",
                                  "ES.CLOCK 1",
                                  "ES.MODE Z",
                                  "ES.PATTERN X",
                                  "ES.PRESET X",
                                  "ES.RESET 1",
                                  "ES.STOP 1",
                                  "ES.TRANS X",
                                  "MP.PRESET X",
                                  "MP.RESET 1",
                                  "WW.MUTE1 0",
                                  "WW.MUTE1 1",
                                  "WW.MUTE2 0",
                                  "WW.MUTE2 1",
                                  "WW.MUTE3 0",
                                  "WW.MUTE3 1",
                                  "WW.MUTE4 0",
                                  "WW.MUTE4 1",
                                  "WW.PATTERN 0",
                                  "WW.POS X",
                                  "WW.PRESET X",
                                  "WW.QPATTERN X",
                                  "WW.SYNC X",
                                  "L 1 4: CV.SLEW 50",
                                  "L 1 4: CV.SLEW I 12",
                                  "L 1 4: TR I 0",
                                  "L A B: TR.TIME I 25",
                                  "L A B: TR.TIME I 40",
                                  "L A D: TR.PULSE I",
                                  "L A D: TR.TIME 25",
                                  "L A D: TR.TIME 30",
                                  "M 750",
                                  "M SUB 320 RSH PARAM 6",
                                  "P 0",
                                  "P 0 0",
                                  "P.END RRAND 1 7",
                                  "P.END WRAP SUB P.END 1 1 7",
                                  "P.I 0",
                                  "P.I ADD P.I RRAND -2 1",
                                  "P.I P.END",
                                  "P.I WRAP ADD P.I 1 0 14",
                                  "P.I WRAP ADD P.I 2 0 14",
                                  "P.I WRAP ADD P.I 3 0 14",
                                  "P.I WRAP SUB P.I 1 0 14",
                                  "P.N 0",
                                  "P.N 1",
                                  "P.N 2",
                                  "P.N 3",
                                  "P.N WRAP ADD P.N 1 0 3",
                                  "P.NEXT",
                                  "PN 0 0",
                                  "PN 0 0 0",
                                  "PROB 12: X ADD X RRAND -8 8",
                                  "PROB 20: TR.PULSE D",
                                  "PROB 25: X ADD X RRAND -4 4",
                                  "PROB 50: TR.PULSE A",
                                  "PROB 50: TR.PULSE C",
                                  "PROB 50: X ADD X RRAND -2 2",
                                  "PROB 6: X ADD X RRAND -16 16",
                                  "PROB 70: TR.PULSE 3",
                                  "PROB 80: TR.PULSE B",
                                  "S: CV 1 N PN 0 X",
                                  "S: CV 2 ADD N P.HERE V 1",
                                  "S: CV 2 CV 1",
                                  "S: TR.PULSE B",
                                  "S.ALL",
                                  "S.POP",
                                  "T 93",
                                  "T RRAND 10 10000",
                                  "TR A TOSS",
                                  "TR.PULSE A",
                                  "TR.PULSE A",
                                  "TR.PULSE B",
                                  "TR.PULSE C",
                                  "TR.PULSE D",
                                  "TR.TOG D",
                                  "X 0",
                                  "X ADD X RRAND -1 1",
                                  "X EZ X",
                                  "X RSH PARAM 10",
                                  "X; Y; Z",
                                  "X WRAP ADD X 1 0 6",
                                  "X WRAP ADD X 1 0 7",
                                  "X WRAP ADD X 3 0 6",
                                  "X WRAP SUB X 1 0 6",
                                  "X WRAP SUB X 2 0 6",
                                  "Y ADD 200 RSH PARAM 4",
                                  "Y PN 0 X",
                                  "Z ADD 2 MUL 4 TOSS",
                                  "Z EZ Z",
                                  "Z IN",
                                  "Z V RRAND -2 2" };

error_t parse_and_validate_helper(char* text) {
    tele_command_t cmd;
    char error_msg[TELE_ERROR_MSG_LENGTH];
    error_t result = parse(text, &cmd, error_msg);
    if (result != E_OK) { return result; }

    return validate(&cmd, error_msg);
}

TEST should_parse_and_validate() {
    for (size_t i = 0; i < CORPUS_COUNT; i++) {
        char* command = corpus[i];
        error_t result = parse_and_validate_helper(command);
        ASSERT_EQm(command, result, E_OK);
    }

    PASS();
}

TEST parser_test_sub_commands() {
    ASSERT_EQ(parse_and_validate_helper("X 1; Y 2"), E_OK);
    ASSERT_EQ(parse_and_validate_helper("X 1; Y 2 ;  ; "), E_OK);
    ASSERT_EQ(parse_and_validate_helper("IF 1: X 1; Y 2"), E_OK);
    ASSERT_EQ(parse_and_validate_helper("IF 1; Z 1: X 1"), E_NO_SUB_SEP_IN_PRE);
    ASSERT_EQ(parse_and_validate_helper("IF 1; Z 1: X 1 ; Y 2"),
              E_NO_SUB_SEP_IN_PRE);
    // possibly the following should return E_NO_SUB_SEP_IN_PRE one day
    ASSERT_EQ(parse_and_validate_helper("Z 1; IF 1: X 1"), E_PLACE_PRE_SEP);
    ASSERT_EQ(parse_and_validate_helper("X;Y"), E_NEED_SPACE_SUB_SEP);

    PASS();
}

// This test asserts that the parser always returns the correct op, it does this
// by starting with the op in question, extracting the name and running that
// through the parser. Then asserting that only 1 op is returned in
// tele_command_t and that it's value matches the op.
TEST parser_should_return_op() {
    for (size_t i = 0; i < E_OP__LENGTH; i++) {
        const tele_op_t* op = tele_ops[i];
        const char* text = op->name;
        tele_command_t cmd;
        char error_msg[TELE_ERROR_MSG_LENGTH];
        error_t result = parse(text, &cmd, error_msg);
        ASSERT_EQm(text, result, E_OK);
        ASSERT_EQm(text, cmd.length, 1);
        ASSERT_EQm(text, cmd.data[0].tag, OP);
        ASSERT_EQm(text, cmd.data[0].value, (int16_t)i);
    }
    PASS();
}

// As parser_should_return_op, but for mods.
TEST parser_should_return_mod() {
    for (size_t i = 0; i < E_MOD__LENGTH; i++) {
        const tele_mod_t* mod = tele_mods[i];
        const char* text = mod->name;
        tele_command_t cmd;
        char error_msg[TELE_ERROR_MSG_LENGTH];
        error_t result = parse(text, &cmd, error_msg);
        ASSERT_EQm(text, result, E_OK);
        ASSERT_EQm(text, cmd.length, 1);
        ASSERT_EQm(text, cmd.data[0].tag, MOD);
        ASSERT_EQm(text, cmd.data[0].value, (int16_t)i);
    }
    PASS();
}

// Check that print_command returns each of the corpus commands unchanged
TEST print_command_corpus_should_be_unchanged() {
    for (size_t i = 0; i < CORPUS_COUNT; i++) {
        char* text = corpus[i];
        tele_command_t cmd;
        char error_msg[TELE_ERROR_MSG_LENGTH];
        error_t result = parse(text, &cmd, error_msg);
        ASSERT_EQm(text, result, E_OK);
        char out[32];
        print_command(&cmd, out);
        char message[256];
        strcpy(message, text);
        strcat(message, " == ");
        strcat(message, out);
        ASSERT_EQm(message, strcmp(text, out), 0);
    }

    PASS();
}

SUITE(parser_suite) {
    RUN_TEST(should_parse_and_validate);
    RUN_TEST(parser_test_sub_commands);
    RUN_TEST(parser_should_return_op);
    RUN_TEST(parser_should_return_mod);
    RUN_TEST(print_command_corpus_should_be_unchanged);
}
