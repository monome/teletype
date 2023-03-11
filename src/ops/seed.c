#include "ops/seed.h"
#include "helpers.h"
#include "random.h"

#define MAKE_SEED_OP(n, v)                                                    \
    {                                                                         \
        .name = #n, .get = op_peek_seed_i16, .set = op_poke_seed_i16,         \
        .params = 0, .returns = 1, .data = (void *)offsetof(scene_state_t, v) \
    }

#define MAKE_SEED_ALIAS_OP(n, v) MAKE_SEED_OP(n, v)

static void op_peek_seed_i16(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_poke_seed_i16(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_SEED_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_SEED_set(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);

// clang-format off
const tele_op_t op_SEED			= MAKE_GET_SET_OP(SEED, op_SEED_get, op_SEED_set, 0, true);
const tele_op_t op_RAND_SEED	= MAKE_SEED_OP(RAND.SEED, rand_states.s.rand);  
const tele_op_t op_SYM_RAND_SD	= MAKE_SEED_ALIAS_OP(RAND.SD, rand_states.s.rand);  
const tele_op_t op_SYM_R_SD		= MAKE_SEED_ALIAS_OP(R.SD, rand_states.s.rand);  
const tele_op_t op_TOSS_SEED	= MAKE_SEED_OP(TOSS.SEED, rand_states.s.toss);  
const tele_op_t op_SYM_TOSS_SD	= MAKE_SEED_ALIAS_OP(TOSS.SD, rand_states.s.toss);  
const tele_op_t op_PROB_SEED	= MAKE_SEED_OP(PROB.SEED, rand_states.s.prob);  
const tele_op_t op_SYM_PROB_SD	= MAKE_SEED_ALIAS_OP(PROB.SD, rand_states.s.prob);  
const tele_op_t op_DRUNK_SEED	= MAKE_SEED_OP(DRUNK.SEED, rand_states.s.drunk);  
const tele_op_t op_SYM_DRUNK_SD	= MAKE_SEED_ALIAS_OP(DRUNK.SD, rand_states.s.drunk);  
const tele_op_t op_P_SEED		= MAKE_SEED_OP(P.SEED, rand_states.s.pattern);  
const tele_op_t op_SYM_P_SD		= MAKE_SEED_ALIAS_OP(P.SD, rand_states.s.pattern);
// clang-format on

static void op_peek_seed_i16(const void *data, scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    char *base = (char *)ss;
    size_t offset = (size_t)data;
    tele_rand_t *ptr = (tele_rand_t *)(base + offset);
    cs_push(cs, ptr->seed);
}

static void op_poke_seed_i16(const void *data, scene_state_t *ss,
                             exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t s = cs_pop(cs);

    char *base = (char *)ss;
    size_t offset = (size_t)data;
    tele_rand_t *ptr = (tele_rand_t *)(base + offset);

    ptr->seed = s;
    random_seed(&ptr->rand, ptr->seed);
}

static void op_SEED_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->variables.seed);
}

static void op_SEED_set(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    uint16_t s = cs_pop(cs);

    for (u8 i = 0; i < RAND_STATES_COUNT; i++) {
        tele_rand_t *r = &ss->rand_states.a[i];
        r->seed = s;
        random_seed(&r->rand, r->seed);
    }

    ss->variables.seed = s;
}
