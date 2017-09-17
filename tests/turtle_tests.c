#include "turtle_tests.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // ssize_t

#include "greatest/greatest.h"

#include "teletype.h"

char tbuf[20][140]; // let it go wide
int  tbuf_r = 0;

#define BUF_CR() (tbuf_r++)
#define BUF tbuf[tbuf_r]

static inline void clear_log() {
    for (;tbuf_r--;)
       tbuf[tbuf_r][0] = 0;
    tbuf_r = 0;
} 

static inline void print_log() {
    printf("\n");
    for (int i = 0; i <= tbuf_r; i++)
        printf("%s\n", tbuf[i]);
}

static const char *error_message(error_t e) {
    static const char *em[] = {
        "E_OK",
        "E_PARSE",
        "E_LENGTH",
        "E_NEED_PARAMS",
        "E_EXTRA_PARAMS",
        "E_NO_MOD_HERE",
        "E_MANY_PRE_SEP",
        "E_NEED_PRE_SEP",
        "E_PLACE_PRE_SEP",
        "E_NO_SUB_SEP_IN_PRE",
        "E_NOT_LEFT",
        "E_NEED_SPACE_PRE_SEP",
        "E_NEED_SPACE_SUB_SEP"
    };
    return em[e];
}
// runs multiple lines of commands and then asserts that the final answer is
// correct (allows contiuation of state)
TEST process_helper_state(scene_state_t* ss, size_t n, char* lines[],
                          int16_t answer) {
    static int32_t count=0;
    count++;
    process_result_t result = {.has_value = false, .value = 0 };
    exec_state_t es;
    es_init(&es);

    clear_log();
    sprintf(BUF, "---- Test #%d ---- Command: ", count);
    BUF_CR();
    // Format multi-line commands into one line
    ssize_t size = 0;
    for (size_t i = 0; i < n; i++) {
        size += strlen(lines[i]) + 3;  // 3 extra chars fo ' | '
    }
    for (size_t i = 0; i < n; i++) {
        strncat(BUF, lines[i], 140 - strlen(BUF));
        if (i < n - 1) strncat(BUF, " | ", 140 - strlen(BUF));
    }
    BUF_CR();

    for (size_t i = 0; i < n; i++) {
        tele_command_t cmd;
        char error_msg[TELE_ERROR_MSG_LENGTH];
        error_t error = parse(lines[i], &cmd, error_msg);
        if (error != E_OK) {
            strncat(BUF, error_message(error), 140 - strlen(BUF));
            strncat(BUF, ": ", 140 - strlen(BUF));
            strncat(BUF, error_msg, 140 - strlen(BUF));
            print_log();
            FAILm("Parser failure.");
        }
        if (validate(&cmd, error_msg) != E_OK) {
            strncat(BUF, error_message(error), 140 - strlen(BUF));
            strncat(BUF, ": ", 140 - strlen(BUF));
            strncat(BUF, error_msg, 140 - strlen(BUF));
            print_log();
            FAILm("Validation failure");
        }
        result = process_command(ss, &es, &cmd);
    }

    if (result.has_value != true) {
        snprintf(BUF, 140 - strlen(BUF),"Expected a value, found none.");
        print_log();
        FAIL();
    }
    if (result.value != answer) {
        snprintf(BUF, 140 - strlen(BUF),
                "Value incorrect.  Expected %d, got %d",
                answer, result.value);
        print_log();
        FAIL();
    }

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

TEST test_turtle_fence_normal() {
    // Proper fence usage
    char *test1[2] = { "@FENCE 1 2 3 4", "@FX1" };
    CHECK_CALL(process_helper(2, test1, 1));
    char *test2[2] = { "@FENCE 1 2 3 4", "@FY1" };
    CHECK_CALL(process_helper(2, test2, 2));
    char *test3[2] = { "@FENCE 1 2 3 4", "@FX2" };
    CHECK_CALL(process_helper(2, test3, 3));
    char *test4[2] = { "@FENCE 1 2 3 4", "@FY2" };
    CHECK_CALL(process_helper(2, test4, 4));
    PASS();
}
TEST test_turtle_fence_swapped() {
    char *test5[2] = { "@FENCE 3 4 1 2", "@FX1" };
    CHECK_CALL(process_helper(2, test5, 1));
    char *test6[2] = { "@FENCE 3 4 1 2", "@FY1" };
    CHECK_CALL(process_helper(2, test6, 2));
    char *test7[2] = { "@FENCE 3 4 1 2", "@FX2" };
    CHECK_CALL(process_helper(2, test7, 3));
    char *test8[2] = { "@FENCE 3 4 1 2", "@FY2" };
    CHECK_CALL(process_helper(2, test8, 4));
    PASS();
}
TEST test_turtle_fence_oob() {
    char *test9[2] = { "@FENCE -1 -1 4 100", "@FX1" };
    CHECK_CALL(process_helper(2, test9, 0));
    char *test10[2] = { "@FENCE -1 -1 4 100", "@FY1" };
    CHECK_CALL(process_helper(2, test10, 0));
    char *test11[2] = { "@FENCE -1 -1 4 100", "@FX2" };
    CHECK_CALL(process_helper(2, test11, 3));
    char *test12[2] = { "@FENCE -1 -1 4 100", "@FY2" };
    CHECK_CALL(process_helper(2, test12, 63));
    PASS();
}
TEST test_turtle_fence_individual() {
    char *test13[2] = { "@FX1 1", "@FX1" };
    CHECK_CALL(process_helper(2, test13, 1));
    char *test14[2] = { "@FY1 1", "@FY1" };
    CHECK_CALL(process_helper(2, test14, 1));
    char *test15[2] = { "@FX2 1", "@FX2" };
    CHECK_CALL(process_helper(2, test15, 1));
    char *test16[2] = { "@FY2 1", "@FY2" };
    CHECK_CALL(process_helper(2, test16, 1));
    PASS();
}
TEST test_turtle_fence_ind_swapped() {
    char *test17[3] = { "@FX1 1", "@FX2 0", "@FX1" };
    CHECK_CALL(process_helper(3, test17, 0));
    char *test18[3] = { "@FY1 1", "@FY2 0", "@FY1" };
    CHECK_CALL(process_helper(3, test18, 0));
    char *test19[3] = { "@FX1 1", "@FX2 0", "@FX2" };
    CHECK_CALL(process_helper(3, test19, 1));
    char *test20[3] = { "@FY1 1", "@FY2 0", "@FY2" };
    CHECK_CALL(process_helper(3, test20, 1));
    PASS();
}
TEST test_turtle_fence_ind_oob() {
    char *test21[2] = { "@FX1 -1", "@FX1" };
    CHECK_CALL(process_helper(2, test21, 0));
    char *test22[2] = { "@FY1 -1", "@FY1" };
    CHECK_CALL(process_helper(2, test22, 0));
    char *test23[2] = { "@FX2 4", "@FX2" };
    CHECK_CALL(process_helper(2, test23, 3));
    char *test24[2] = { "@FY2 63", "@FY2" };
    CHECK_CALL(process_helper(2, test24, 63));
    // TODO more tests
    PASS();
}

TEST test_turtle_wrap() {
    char *test1[3] = { "@WRAP", "@REV 1", "@Y" };
    CHECK_CALL(process_helper(3, test1, 63));
    char *test2[4] = { "@WRAP", "@FY2 1", "@REV 1", "@Y" };
    CHECK_CALL(process_helper(4, test2, 1));
    char *test3[4] = { "@WRAP", "@TURN -90", "@REV 1", "@X" };
    CHECK_CALL(process_helper(4, test3, 3));
    char *test4[5] = { "@WRAP", "@TURN -90", "@FX2 1", "@REV 1", "@X" };
    CHECK_CALL(process_helper(5, test4, 1));
    PASS();
}

SUITE(turtle_suite) {
    RUN_TEST(test_turtle_fence_normal);
    RUN_TEST(test_turtle_fence_swapped);
    RUN_TEST(test_turtle_fence_oob);
    RUN_TEST(test_turtle_fence_individual);
    RUN_TEST(test_turtle_fence_ind_swapped);
    RUN_TEST(test_turtle_fence_ind_oob);
    RUN_TEST(test_turtle_wrap);
}

