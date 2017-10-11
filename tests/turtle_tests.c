#include "turtle_tests.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // ssize_t

#include "greatest/greatest.h"

#include "log.h"
#include "teletype.h"

int32_t count = 0;

#define NEWTEST() count = 0;

static const char *error_message(error_t e) {
    static const char *em[] = { "E_OK",
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
                                "E_NEED_SPACE_SUB_SEP" };
    return em[e];
}

// runs multiple lines of commands and then asserts that the final answer is
// correct (allows contiuation of state)
TEST process_helper_state(scene_state_t *ss, size_t n, char *lines[],
                          int16_t answer) {
    count++;
    process_result_t result = { .has_value = false, .value = 0 };
    exec_state_t es;
    memset(&es, 0, sizeof(es));
    es_init(&es);
    es_push(&es);
    es_variables(&es)->script_number = 1;

    log_clear();
    lprintf("---- Test #%d ----", count);
    lcat("Command: ");
    // Format multi-line commands into one line
    for (size_t i = 0; i < n; i++) {
        lcat(lines[i]);
        if (i < n - 1) lcat(" | ");
    }

    for (size_t i = 0; i < n; i++) {
        tele_command_t cmd;
        char error_msg[TELE_ERROR_MSG_LENGTH];
        error_t error = parse(lines[i], &cmd, error_msg);
        if (error != E_OK) {
            lprintf("%s: %s", error_message(error), error_msg);
            log_print();
            FAILm("Parser failure.");
        }
        error = validate(&cmd, error_msg);
        if (error != E_OK) {
            lprintf("%s: %s", error_message(error), error_msg);
            log_print();
            FAILm("Validation failure");
        }
        result = process_command(ss, &es, &cmd);
    }

    if (result.has_value != true) {
        lprintf("Expected a value, found none.");
        log_print();
        FAIL();
    }
    if (result.value != answer) {
        lprintf("Value incorrect.  Expected %d, got %d", answer, result.value);
        log_print();
        FAIL();
    }

    PASS();
}

// runs multiple lines of commands and then asserts that the final answer is
// correct
TEST process_helper(size_t n, char *lines[], int16_t answer) {
    scene_state_t ss;
    memset(&ss, 0, sizeof(ss));
    ss_init(&ss);

    CHECK_CALL(process_helper_state(&ss, n, lines, answer));

    PASS();
}

TEST test_turtle_fence_normal() {
    NEWTEST();
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
    NEWTEST();
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
    NEWTEST();
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
    NEWTEST();
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
    NEWTEST();
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
    NEWTEST();
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
    NEWTEST();
    char *test1[3] = { "@WRAP 1", "@MOVE 0 -1", "@Y" };
    CHECK_CALL(process_helper(3, test1, 63));
    char *test1b[3] = { "@WRAP 1", "@MOVE 0 -1", "@X" };
    CHECK_CALL(process_helper(3, test1b, 0));
    char *test1c[3] = { "@WRAP 1", "@F 0 0 1 1; @MOVE 0 3", "@Y" };
    CHECK_CALL(process_helper(3, test1c, 1));
    char *test1d[3] = { "@WRAP 1", "@F 0 0 1 1; @MOVE 0 3", "@X" };
    CHECK_CALL(process_helper(3, test1d, 0));
    char *test1e[3] = { "@WRAP 1", "@STEP", "@X" };
    CHECK_CALL(process_helper(3, test1e, 0));
    char *test2[4] = { "@WRAP 1", "@FY2 1", "@MOVE 0 -1", "@Y" };
    CHECK_CALL(process_helper(4, test2, 1));
    char *test2b[4] = { "@WRAP 1", "@FY2 1", "@MOVE 0 -1", "@X" };
    CHECK_CALL(process_helper(4, test2b, 0));
    char *test3[3] = { "@WRAP 1", "@MOVE -1 0", "@X" };
    CHECK_CALL(process_helper(3, test3, 3));
    char *test4[4] = { "@WRAP 1", "@FX1 1", "@MOVE -1 0", "@X" };
    CHECK_CALL(process_helper(4, test4, 3));
    PASS();
}

TEST test_turtle_bounce() {
    NEWTEST();
    char *test1[3] = { "@BOUNCE 1", "@MOVE 0 -2", "@Y" };
    CHECK_CALL(process_helper(3, test1, 2));
    char *test2[3] = { "@BOUNCE 1", "@MOVE 0 64", "@Y" };
    CHECK_CALL(process_helper(3, test2, 62));
    char *test3[3] = { "@BOUNCE 1", "@MOVE -1 0", "@X" };
    CHECK_CALL(process_helper(3, test3, 1));
    char *test4[3] = { "@BOUNCE 1", "@MOVE -2 0", "@X" };
    CHECK_CALL(process_helper(3, test4, 2));
    char *test5[3] = { "@BOUNCE 1", "@MOVE 4 0", "@X" };
    CHECK_CALL(process_helper(3, test5, 2));
    char *test6[3] = { "@BOUNCE 1", "@MOVE 0 -1", "@Y" };
    CHECK_CALL(process_helper(3, test6, 1));
    char *test7[3] = { "@BOUNCE 1", "@MOVE 0 126", "@Y" };
    CHECK_CALL(process_helper(3, test7, 0));
    char *test8[3] = { "@BOUNCE 1", "@MOVE 0 130", "@Y" };
    CHECK_CALL(process_helper(3, test8, 4));
    char *test9[3] = { "@BOUNCE 1", "@MOVE 3 0", "@X" };
    CHECK_CALL(process_helper(3, test9, 3));
    char *test10[4] = { "@BOUNCE 1", "@F 0 0 1 1", "@STEP", "@Y" };
    CHECK_CALL(process_helper(4, test10, 1));
    char *test10b[4] = { "@BOUNCE 1", "@F 0 0 1 1", "@STEP", "@DIR" };
    CHECK_CALL(process_helper(4, test10b, 180));

    // The following tests reveal the charade that is the length between fences
#if 0 
    char *test10c[4] = { "@BOUNCE 1", "@F 0 0 1 1", "L 1 2: @STEP", "@DIR" };
    CHECK_CALL(process_helper(4, test10c, 0));
    char *test10d[4] = { "@BOUNCE 1", "@F 0 0 1 1", "L 1 3: @STEP", "@SPEED" };
    CHECK_CALL(process_helper(4, test10d, 100));
    char *test13[5] = {
        "@F 0 0 1 1",
        "@BOUNCE 1",
        "@SPEED 300",
        "@STEP",
        "@DIR"
    };
    CHECK_CALL(process_helper(5, test13, 0));
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
#endif

    char *test12[4] = { "@BOUNCE 1; @SPEED 141", "@DIR 135", "@STEP", "@X" };
    CHECK_CALL(process_helper(4, test12, 1));
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

TEST test_turtle_vars() {
    NEWTEST();
    char *test1[2] = { "@X 1", "@X" };
    CHECK_CALL(process_helper(2, test1, 1));
    PASS();
}

TEST test_turtle_step() {
    char *test1[2] = { "@STEP", "@Y" };
    CHECK_CALL(process_helper(2, test1, 1));
    char *test2[2] = { "@STEP", "@X" };
    CHECK_CALL(process_helper(2, test2, 0));
    PASS();
}

SUITE(turtle_suite) {
    log_init();
    RUN_TEST(test_turtle_fence_normal);
    RUN_TEST(test_turtle_fence_swapped);
    RUN_TEST(test_turtle_fence_oob);
    RUN_TEST(test_turtle_fence_individual);
    RUN_TEST(test_turtle_fence_ind_swapped);
    RUN_TEST(test_turtle_fence_ind_oob);
    RUN_TEST(test_turtle_wrap);
    RUN_TEST(test_turtle_bounce);
    RUN_TEST(test_turtle_vars);
    RUN_TEST(test_turtle_step);
}
