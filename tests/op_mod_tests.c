#include "op_mod_tests.h"

#include "greatest/greatest.h"

#include "ops/op.h"
#include "teletype.h"

// Check every op has a unique name
TEST unique_ops() {
    for (size_t i = 0; i < E_OP__LENGTH; i++) {
        for (size_t j = 0; j < E_OP__LENGTH; j++) {
            if (i == j) continue;
            ASSERTm(tele_ops[i]->name,
                    strcmp(tele_ops[i]->name, tele_ops[j]->name) != 0);
        }
    }
    PASS();
}

// Check every mod has a unique name
TEST unique_mods() {
    for (size_t i = 0; i < E_MOD__LENGTH; i++) {
        for (size_t j = 0; j < E_MOD__LENGTH; j++) {
            if (i == j) continue;
            ASSERTm(tele_mods[i]->name,
                    strcmp(tele_mods[i]->name, tele_mods[j]->name) != 0);
        }
    }
    PASS();
}

// Check every op manipulates the stack correctly
TEST op_stack_size() {
    for (size_t i = 0; i < E_OP__LENGTH; i++) {
        const tele_op_t *op = tele_ops[i];

        if (op->get != NULL) {
            scene_state_t ss;
            ss_init(&ss);
            exec_state_t es;
            es_init(&es);
            es_push(&es);
            es_variables(&es)->script_number = 0;
            command_state_t cs;
            cs_init(&cs);

            // add params to stack (plus an extra 2, to check that too many
            // values aren't removed, warning: note the maximum stack size in
            // state.h)
            const int16_t stack_extra = 2;
            for (int j = 0; j < op->params + stack_extra; j++) cs_push(&cs, 0);

            // execute get
            op->get(op->data, &ss, &es, &cs);

            // check that the stack has the correct number of items in it
            if (op->returns) {
                ASSERT_EQm(op->name, cs_stack_size(&cs), stack_extra + 1);
            }
            else {
                ASSERT_EQm(op->name, cs_stack_size(&cs), stack_extra);
            }
        }

        if (op->set != NULL) {
            scene_state_t ss;
            ss_init(&ss);
            exec_state_t es;
            es_init(&es);
            es_push(&es);
            es_variables(&es)->script_number = 0;
            command_state_t cs;
            cs_init(&cs);

            // add params to stack (plus an extra 2, to check that too many
            // values aren't removed, warning: note the maximum stack size in
            // state.h)
            // set functions require an extra value on the stack
            const int16_t stack_extra = 2;
            for (int j = 0; j < op->params + stack_extra + 1; j++)
                cs_push(&cs, 0);

            // execute get
            op->set(op->data, &ss, &es, &cs);

            // check that the stack has the correct number of items in it
            ASSERT_EQm(op->name, cs_stack_size(&cs), stack_extra);
        }
    }
    PASS();
}

// Check every mod manipulates the stack correctly
TEST mod_stack_size() {
    for (size_t i = 0; i < E_MOD__LENGTH; i++) {
        const tele_mod_t *mod = tele_mods[i];

        scene_state_t ss;
        ss_init(&ss);
        exec_state_t es;
        es_init(&es);
        es_push(&es);
        es_variables(&es)->script_number = 0;
        command_state_t cs;
        cs_init(&cs);

        // add params to stack (plus an extra 2, to check that too many
        // values aren't removed, warning: note the maximum stack size in
        // state.h)
        const int16_t stack_extra = 2;
        for (int j = 0; j < mod->params + stack_extra; j++) cs_push(&cs, 0);

        // execute func
        const tele_command_t sub_command = {.length = 1,
                                            .separator = 0,
                                            .data = { {.tag = OP,
                                                       .value = E_OP_A } } };
        mod->func(&ss, &es, &cs, &sub_command);

        // check that the stack has the correct number of items in it
        ASSERT_EQm(mod->name, cs_stack_size(&cs), stack_extra);
    }
    PASS();
}

SUITE(op_mod_suite) {
    RUN_TEST(unique_ops);
    RUN_TEST(unique_mods);
    RUN_TEST(op_stack_size);
    RUN_TEST(mod_stack_size);
}
