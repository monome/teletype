#include "turtle_tests.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // ssize_t

#include "greatest/greatest.h"

#include "teletype.h"

char tbuf[20][140]; // let it go wide
int  tbuf_r = 0;
int32_t count = 0;

#define BUF_CR() (tbuf_r++)
#define BUF tbuf[tbuf_r]
#define BUF_NEWTEST() (count = 0)

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
    count++;
    process_result_t result = {.has_value = false, .value = 0 };
    exec_state_t es;
    memset(&es, 0, sizeof(es));
    es_init(&es);
    es_push(&es);
    es_variables(&es)->script_number = 1;

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
        error = validate(&cmd, error_msg);
        if (error != E_OK) {
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
    memset(&ss, 0, sizeof(ss));
    ss_init(&ss);

    CHECK_CALL(process_helper_state(&ss, n, lines, answer));

    PASS();
}

TEST test_turtle_fence_normal() {
    BUF_NEWTEST();
    char *test1[2] = { "@F 1 2 3 4", "@FX1" };
    CHECK_CALL(process_helper(2, test1, 1));
    char *test2[2] = { "@F 1 2 3 4", "@FY1" };
    CHECK_CALL(process_helper(2, test2, 2));
    char *test3[2] = { "@F 1 2 3 4", "@FX2" };
    CHECK_CALL(process_helper(2, test3, 3));
    char *test4[2] = { "@F 1 2 3 4", "@FY2" };
    CHECK_CALL(process_helper(2, test4, 4));
    PASS();
}
TEST test_turtle_fence_swapped() {
    BUF_NEWTEST();
    char *test1[2] = { "@F 3 4 1 2", "@FX1" };
    CHECK_CALL(process_helper(2, test1, 1));
    char *test2[2] = { "@F 3 4 1 2", "@FY1" };
    CHECK_CALL(process_helper(2, test2, 2));
    char *test3[2] = { "@F 3 4 1 2", "@FX2" };
    CHECK_CALL(process_helper(2, test3, 3));
    char *test4[2] = { "@F 3 4 1 2", "@FY2" };
    CHECK_CALL(process_helper(2, test4, 4));
    PASS();
}
TEST test_turtle_fence_oob() {
    BUF_NEWTEST();
    char *test1[2] = { "@F -1 -1 4 100", "@FX1" };
    CHECK_CALL(process_helper(2, test1, 0));
    char *test2[2] = { "@F -1 -1 4 100", "@FY1" };
    CHECK_CALL(process_helper(2, test2, 0));
    char *test3[2] = { "@F -1 -1 4 100", "@FX2" };
    CHECK_CALL(process_helper(2, test3, 3));
    char *test4[2] = { "@F -1 -1 4 100", "@FY2" };
    CHECK_CALL(process_helper(2, test4, 63));
    PASS();
}
TEST test_turtle_fence_individual() {
    BUF_NEWTEST();
    char *test1[2] = { "@FX1 1", "@FX1" };
    CHECK_CALL(process_helper(2, test1, 1));
    char *test2[2] = { "@FY1 1", "@FY1" };
    CHECK_CALL(process_helper(2, test2, 1));
    char *test3[2] = { "@FX2 1", "@FX2" };
    CHECK_CALL(process_helper(2, test3, 1));
    char *test4[2] = { "@FY2 1", "@FY2" };
    CHECK_CALL(process_helper(2, test4, 1));
    PASS();
}
TEST test_turtle_fence_ind_swapped() {
    BUF_NEWTEST();
    char *test1[3] = { "@FX1 1", "@FX2 0", "@FX1" };
    CHECK_CALL(process_helper(3, test1, 0));
    char *test2[3] = { "@FY1 1", "@FY2 0", "@FY1" };
    CHECK_CALL(process_helper(3, test2, 0));
    char *test3[3] = { "@FX1 1", "@FX2 0", "@FX2" };
    CHECK_CALL(process_helper(3, test3, 1));
    char *test4[3] = { "@FY1 1", "@FY2 0", "@FY2" };
    CHECK_CALL(process_helper(3, test4, 1));
    PASS();
}
TEST test_turtle_fence_ind_oob() {
    BUF_NEWTEST();
    char *test1[2] = { "@FX1 -1", "@FX1" };
    CHECK_CALL(process_helper(2, test1, 0));
    char *test2[2] = { "@FY1 -1", "@FY1" };
    CHECK_CALL(process_helper(2, test2, 0));
    char *test3[2] = { "@FX2 4", "@FX2" };
    CHECK_CALL(process_helper(2, test3, 3));
    char *test4[2] = { "@FY2 63", "@FY2" };
    CHECK_CALL(process_helper(2, test4, 63));
    // TODO more tests
    PASS();
}

TEST test_turtle_wrap() {
    BUF_NEWTEST();
    char *test1[3] = { "@WRAP 1", "@MOVE 0 -1", "@Y" };
    CHECK_CALL(process_helper(3, test1, 63));
    char *test2[4] = { "@WRAP 1", "@FY2 1", "@MOVE 0 -1", "@Y" };
    CHECK_CALL(process_helper(4, test2, 1));
    char *test3[3] = { "@WRAP 1", "@MOVE -1 0", "@X" };
    CHECK_CALL(process_helper(3, test3, 3));
    char *test4[4] = { "@WRAP 1", "@FX1 1", "@MOVE -1 0", "@X" };
    CHECK_CALL(process_helper(4, test4, 3));
    PASS();
}

TEST test_turtle_bounce() {
    BUF_NEWTEST();
    // TODO check for DIR
    char *test1[3] = { "@BOUNCE 1", "@MOVE 0 -2", "@Y" };
    CHECK_CALL(process_helper(3, test1, 1));
    char *test2[3] = { "@BOUNCE 1", "@MOVE 0 63", "@Y" };
    CHECK_CALL(process_helper(3, test2, 63));
    char *test3[3] = { "@BOUNCE 1", "@MOVE -1 0", "@X" };
    CHECK_CALL(process_helper(3, test3, 0));
    char *test4[3] = { "@BOUNCE 1", "@MOVE -2 0", "@X" };
    CHECK_CALL(process_helper(3, test4, 1));
    char *test5[3] = { "@BOUNCE 1", "@MOVE 4 0", "@X" };
    CHECK_CALL(process_helper(3, test5, 3));
    char *test6[3] = { "@BOUNCE 1", "@MOVE 0 -1", "@Y" };
    CHECK_CALL(process_helper(3, test6, 0));
    char *test7[3] = { "@BOUNCE 1", "@MOVE 0 127", "@Y" };
    CHECK_CALL(process_helper(3, test7, 0));
    char *test8[3] = { "@BOUNCE 1", "@MOVE 0 130", "@Y" };
    CHECK_CALL(process_helper(3, test8, 2));
    char *test9[3] = { "@BOUNCE 1", "@MOVE 3 0", "@X" };
    CHECK_CALL(process_helper(3, test9, 3));
    char *test10[4] = { "@BOUNCE 1", "@F 0 0 1 1", "@STEP", "@Y" };
    CHECK_CALL(process_helper(4, test10, 1));
    char *test10b[4] = { "@BOUNCE 1", "@F 0 0 1 1", "@STEP", "@DIR" };
    CHECK_CALL(process_helper(4, test10b, 180));
    char *test10c[4] = { "@BOUNCE 1", "@F 0 0 1 1", "L 1 2: @STEP", "@DIR" };
    CHECK_CALL(process_helper(4, test10c, 0));
    char *test10d[4] = { "@BOUNCE 1", "@F 0 0 1 1", "L 1 3: @STEP", "@SPEED" };
    CHECK_CALL(process_helper(4, test10d, 100));
    char *test12[4] = { "@BOUNCE 1; @SPEED 141", "@DIR 135", "@STEP", "@X" };
    CHECK_CALL(process_helper(4, test12, 1));
    char *test13[5] = {
        "@F 0 0 1 1",
        "@BOUNCE 1",
        "@SPEED 300",
        "@STEP",
        "@DIR"
    };
    CHECK_CALL(process_helper(5, test13, 180));
    char *test14[6] = {
        "@F 0 0 1 1",
        "@BOUNCE 1",
        "@SPEED 400",
        "@DIR 135",
        "@STEP",
        "@DIR"
    };
    CHECK_CALL(process_helper(6, test14, 315));
    char *test15[6] = {
        "@F 0 0 1 1",
        "@BOUNCE 1",
        "@SPEED 141",
        "@DIR 135",
        "@STEP",
        "@Y"
    };
    CHECK_CALL(process_helper(6, test15, 1));
    PASS();
}

/*

TEST test_turtle_F() {
    char *testX[Y] = {
        "@",
        "@",
        "@",
        "@",
        "@",
        "@"
    };
    CHECK_CALL(process_helper(Y, testX, xpct));
    PASS();
} 

*/

TEST test_turtle_step() {
    char *test1[2] = {
        "@STEP",
        "@Y"
    };
    CHECK_CALL(process_helper(2, test1, 1));
    PASS();
} 
TEST test_turtle_vars() {
    BUF_NEWTEST();
    char *test1[2] = { "@X 1", "@X" };
    CHECK_CALL(process_helper(2, test1, 1));
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
    RUN_TEST(test_turtle_bounce);
    RUN_TEST(test_turtle_step);
    RUN_TEST(test_turtle_vars);
}

