#include "process_tests.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // ssize_t

#include "greatest/greatest.h"

#include "teletype.h"
// runs multiple lines of commands and then asserts that the final answer is
// correct (allows contiuation of state)
TEST process_helper_state(scene_state_t* ss, size_t n, char* lines[],
                          int16_t answer) {
    process_result_t result = {.has_value = false, .value = 0 };
    exec_state_t es;
    es_init(&es);
    es_push(&es);
    es_variables(&es)->script_number = 1;
    for (size_t i = 0; i < n; i++) {
        tele_command_t cmd;
        char error_msg[TELE_ERROR_MSG_LENGTH];
        error_t error = parse(lines[i], &cmd, error_msg);
        if (error != E_OK) { FAIL(); }
        if (validate(&cmd, error_msg) != E_OK) { FAIL(); }
        result = process_command(ss, &es, &cmd);
    }

    ASSERT_EQ(result.has_value, true);

    // prep a message for the test
    ssize_t size = 0;
    for (size_t i = 0; i < n; i++) {
        size += strlen(lines[i]) + 3;  // 3 extra chars fo ' | '
    }
    char* message = calloc(size + 1, sizeof(char));
    for (size_t i = 0; i < n; i++) {
        strcat(message, lines[i]);
        if (i < n - 1) strcat(message, " | ");
    }

    ASSERT_EQm(message, result.value, answer);

    free(message);

    PASS();
}


// runs multiple lines of commands and then asserts that the final answer is
// correct
TEST process_helper(size_t n, char* lines[], int16_t answer) {
    scene_state_t ss;
    ss_init(&ss);

    CHECK_CALL(process_helper_state(&ss, n, lines, answer));

    PASS();
}

TEST test_numbers() {
    char* test1[1] = { "1" };
    CHECK_CALL(process_helper(1, test1, 1));

    char* test2[1] = { "2" };
    CHECK_CALL(process_helper(1, test2, 2));

    char* test3[1] = { "0" };
    CHECK_CALL(process_helper(1, test3, 0));

    char* test4[1] = { "-1" };
    CHECK_CALL(process_helper(1, test4, -1));

    PASS();
}

TEST test_ADD() {
    char* test1[1] = { "ADD 5 6" };
    CHECK_CALL(process_helper(1, test1, 11));

    char* test2[1] = { "ADD 2 -2" };
    CHECK_CALL(process_helper(1, test2, 0));

    PASS();
}

TEST test_IF() {
    char* test1[3] = { "X 0", "IF 1: X 1", "X" };
    CHECK_CALL(process_helper(3, test1, 1));

    char* test2[3] = { "X 0", "IF 0: X 1", "X" };
    CHECK_CALL(process_helper(3, test2, 0));

    char* test3[3] = { "PN 0 0 0", "IF 1: PN 0 0 1", "PN 0 0" };
    CHECK_CALL(process_helper(3, test3, 1));

    char* test4[3] = { "PN 0 0 0", "IF 0: PN 0 0 1", "PN 0 0" };
    CHECK_CALL(process_helper(3, test4, 0));

    char* test5[3] = { "X 0", "ELSE: X 1", "X" };
    CHECK_CALL(process_helper(3, test5, 0));

    char* test6[3] = { "X 0", "ELIF 1: X 1", "X" };
    CHECK_CALL(process_helper(3, test6, 0));

    char* test7[4] = { "X 0", "ELIF 1: X 1", "ELSE: X 1", "X" };
    CHECK_CALL(process_helper(4, test7, 0));

    char* test8[4] = { "X 0", "IF 0: X 2", "ELSE: X 1", "X" };
    CHECK_CALL(process_helper(4, test8, 1));

    char* test9[4] = { "X 0", "IF 0: X 2", "ELIF 1: X 1", "X" };
    CHECK_CALL(process_helper(4, test9, 1));

    char* test10[5] = { "X 0", "IF 1: X 1", "ELIF 1: X 2", "ELSE: X 3", "X" };
    CHECK_CALL(process_helper(5, test10, 1));

    char* test11[5] = { "X 0", "IF 0: X 1", "ELIF 1: X 2", "ELSE: X 3", "X" };
    CHECK_CALL(process_helper(5, test11, 2));

    char* test12[5] = { "X 0", "IF 0: X 1", "ELIF 0: X 2", "ELSE: X 3", "X" };
    CHECK_CALL(process_helper(5, test12, 3));

    PASS();
}

TEST test_FLIP() {
    char* test1[2] = { "FLIP 0", "FLIP" };
    CHECK_CALL(process_helper(2, test1, 0));

    char* test2[1] = { "FLIP" };
    CHECK_CALL(process_helper(1, test2, 0));

    char* test3[2] = { "FLIP", "FLIP" };
    CHECK_CALL(process_helper(2, test3, 1));

    char* test4[2] = { "FLIP 100", "FLIP" };
    CHECK_CALL(process_helper(2, test4, 1));

    PASS();
}

TEST test_L() {
    char* test1[3] = { "X 0", "L 1 10: X I", "X" };
    CHECK_CALL(process_helper(3, test1, 10));

    char* test2[3] = { "X 0", "L 1 -10: X I", "X" };
    CHECK_CALL(process_helper(3, test2, -10));

    char* test3[3] = { "X 0", "L 1 10: X ADD X I", "X" };
    CHECK_CALL(process_helper(3, test3, 55));

    PASS();
}

TEST test_O() {
    scene_state_t ss;
    ss_init(&ss);

    char* test1[6] = {
        "O.MIN 0", "O.MAX 63", "O.INC 1", "O.WRAP 1", "O 0", "O"
    };
    CHECK_CALL(process_helper_state(&ss, 6, test1, 0));

    char* test2[1] = { "O" };
    CHECK_CALL(process_helper_state(&ss, 1, test2, 1));

    char* test3[2] = { "O 0", "O" };
    CHECK_CALL(process_helper_state(&ss, 2, test3, 0));

    char* test4[2] = { "O 63", "O" };
    CHECK_CALL(process_helper_state(&ss, 2, test4, 63));

    char* test5[1] = { "O" };
    CHECK_CALL(process_helper_state(&ss, 1, test5, 0));

    char* test6[4] = { "O 0", "O.INC -1", "O", "O" };
    CHECK_CALL(process_helper_state(&ss, 4, test6, 63));

    char* test7[4] = { "O 0", "O.WRAP 0", "O", "O" };
    CHECK_CALL(process_helper_state(&ss, 4, test7, 0));

    PASS();
}

TEST test_P() {
    char* test1[2] = { "P 0 1", "P 0" };
    CHECK_CALL(process_helper(2, test1, 1));

    char* test2[2] = { "P 0 2", "P 0" };
    CHECK_CALL(process_helper(2, test2, 2));

    PASS();
}

TEST test_PN() {
    char* test1[2] = { "PN 0 0 1", "PN 0 0" };
    CHECK_CALL(process_helper(2, test1, 1));

    char* test2[2] = { "PN 0 0 2", "PN 0 0" };
    CHECK_CALL(process_helper(2, test2, 2));

    char* test3[3] = { "P.N 0", "P 0 3", "PN 0 0" };
    CHECK_CALL(process_helper(3, test3, 3));

    char* test4[3] = { "P.N 0", "PN 0 0 4", "P 0" };
    CHECK_CALL(process_helper(3, test4, 4));

    PASS();
}

TEST test_Q() {
    scene_state_t ss;
    ss_init(&ss);

    char* test1[2] = { "Q.N 16", "Q.N" };
    CHECK_CALL(process_helper_state(&ss, 2, test1, 16));

    for (int i = 1; i <= 16; i++) {
        char buf1[20];
        char buf2[20];
        sprintf(buf1, "Q.N %d", i);
        sprintf(buf2, "Q %d", i);
        char* test2[3] = { buf1, buf2, "Q" };
        CHECK_CALL(process_helper_state(&ss, 3, test2, 1));
    }

    for (int i = 1; i <= 16; i++) {
        char buf1[20];
        sprintf(buf1, "Q.N %d", i);
        char* test3[2] = { buf1, "Q" };
        CHECK_CALL(process_helper_state(&ss, 2, test3, 17 - i));
    }

    // 1+2+3+4+5+6+7+8+9+10+11+12+13+14+15+16 = 136
    // 136 / 16 = 8.5
    char* test4[1] = { "Q.AVG" };
    CHECK_CALL(process_helper_state(&ss, 1, test4, 9));

    char* test5[2] = { "Q.AVG 5", "Q.AVG" };
    CHECK_CALL(process_helper_state(&ss, 2, test5, 5));

    for (int i = 1; i <= 16; i++) {
        char* test6[1] = { "Q" };
        CHECK_CALL(process_helper_state(&ss, 1, test6, 5));
    }

    PASS();
}

TEST test_X() {
    char* test1[2] = { "X 0", "X" };
    CHECK_CALL(process_helper(2, test1, 0));

    char* test2[2] = { "X 10", "X" };
    CHECK_CALL(process_helper(2, test2, 10));

    char* test3[2] = { "X 10", "ADD 5 X" };
    CHECK_CALL(process_helper(2, test3, 15));

    PASS();
}

TEST test_sub_commands() {
    char* test1[2] = { "X 10; Y 20; Z 30", "ADD X ADD Y Z" };
    CHECK_CALL(process_helper(2, test1, 60));

    char* test2[2] = { "IF 1: X 1; Y 2; Z 3", "ADD X ADD Y Z" };
    CHECK_CALL(process_helper(2, test2, 6));

    char* test3[3] = { "X 0; Y 0; Z 0", "IF 0: X 1; Y 2; Z 3",
                       "ADD X ADD Y Z" };
    CHECK_CALL(process_helper(3, test3, 0));

    PASS();
}

TEST test_blank_command() {
    scene_state_t ss;
    ss_init(&ss);
    exec_state_t es;
    es_init(&es);
    es_push(&es);
    es_variables(&es)->script_number = 1;
    tele_command_t cmd;
    char error_msg[TELE_ERROR_MSG_LENGTH];

    char* test = "";
    error_t error = parse(test, &cmd, error_msg);
    if (error != E_OK) { FAIL(); }
    if (validate(&cmd, error_msg) != E_OK) { FAIL(); }

    process_result_t result = process_command(&ss, &es, &cmd);

    ASSERT_EQ(result.has_value, false);
    ASSERT_EQ(result.value, 0);

    PASS();
}

SUITE(process_suite) {
    RUN_TEST(test_numbers);
    RUN_TEST(test_ADD);
    RUN_TEST(test_IF);
    RUN_TEST(test_FLIP);
    RUN_TEST(test_L);
    RUN_TEST(test_O);
    RUN_TEST(test_P);
    RUN_TEST(test_Q);
    RUN_TEST(test_PN);
    RUN_TEST(test_X);
    RUN_TEST(test_sub_commands);
    RUN_TEST(test_blank_command);
}
