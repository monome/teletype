#include "ops/patterns.h"

#include "helpers.h"
#include "teletype.h"
#include "teletype_io.h"

////////////////////////////////////////////////////////////////////////////////
// Helpers /////////////////////////////////////////////////////////////////////

// limit pn to within 0 and 3 inclusive
static int16_t normalise_pn(const int16_t pn) {
    if (pn < 0)
        return 0;
    else if (pn >= PATTERN_COUNT)
        return PATTERN_COUNT - 1;
    else
        return pn;
}

// ensure that the pattern index is within bounds
// also adjust for negative indices (they index from the back)
static int16_t normalise_idx(scene_state_t *ss, const int16_t pn, int16_t idx) {
    const int16_t len = ss_get_pattern_len(ss, pn);
    if (idx < 0) {
        if (idx == len)
            idx = 0;
        else if (idx < -len)
            idx = 0;
        else
            idx = len + idx;
    }

    if (idx >= PATTERN_LENGTH) idx = PATTERN_LENGTH - 1;

    return idx;
}


////////////////////////////////////////////////////////////////////////////////
// P.N /////////////////////////////////////////////////////////////////////////

static void op_P_N_get(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, ss->variables.p_n);
}

static void op_P_N_set(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    ss->variables.p_n = normalise_pn(a);
}

const tele_op_t op_P_N = MAKE_GET_SET_OP(P.N, op_P_N_get, op_P_N_set, 0, true);


////////////////////////////////////////////////////////////////////////////////
// P and PN ////////////////////////////////////////////////////////////////////

// Get
static int16_t p_get(scene_state_t *ss, int16_t pn, int16_t idx) {
    pn = normalise_pn(pn);
    idx = normalise_idx(ss, pn, idx);
    return ss_get_pattern_val(ss, pn, idx);
}

static void op_P_get(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = ss->variables.p_n;
    int16_t a = cs_pop(cs);
    cs_push(cs, p_get(ss, pn, a));
}

static void op_PN_get(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = cs_pop(cs);
    int16_t a = cs_pop(cs);
    cs_push(cs, p_get(ss, pn, a));
}

// Set
static void p_set(scene_state_t *ss, int16_t pn, int16_t idx, int16_t val) {
    pn = normalise_pn(pn);
    idx = normalise_idx(ss, pn, idx);
    ss_set_pattern_val(ss, pn, idx, val);
}

static void op_P_set(const void *NOTUSED(data), scene_state_t *ss,
                     exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = ss->variables.p_n;
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    p_set(ss, pn, a, b);
    tele_pattern_updated();
}

static void op_PN_set(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = cs_pop(cs);
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    p_set(ss, pn, a, b);
    tele_pattern_updated();
}

// Make ops
const tele_op_t op_P = MAKE_GET_SET_OP(P, op_P_get, op_P_set, 1, true);
const tele_op_t op_PN = MAKE_GET_SET_OP(PN, op_PN_get, op_PN_set, 2, true);


////////////////////////////////////////////////////////////////////////////////
// P.L and PN.L ////////////////////////////////////////////////////////////////

// Get
static void op_P_L_get(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(ss->variables.p_n);
    cs_push(cs, ss_get_pattern_len(ss, pn));
}

static void op_PN_L_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(cs_pop(cs));
    cs_push(cs, ss_get_pattern_len(ss, pn));
}

// Set
static void p_l_set(scene_state_t *ss, int16_t pn, int16_t l) {
    pn = normalise_pn(pn);
    if (l < 0)
        ss_set_pattern_len(ss, pn, 0);
    else if (l > PATTERN_LENGTH)
        ss_set_pattern_len(ss, pn, PATTERN_LENGTH);
    else
        ss_set_pattern_len(ss, pn, l);
}

static void op_P_L_set(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = ss->variables.p_n;
    int16_t a = cs_pop(cs);
    p_l_set(ss, pn, a);
    tele_pattern_updated();
}

static void op_PN_L_set(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = cs_pop(cs);
    int16_t a = cs_pop(cs);
    p_l_set(ss, pn, a);
    tele_pattern_updated();
}

// Make ops
const tele_op_t op_P_L = MAKE_GET_SET_OP(P.L, op_P_L_get, op_P_L_set, 0, true);
const tele_op_t op_PN_L =
    MAKE_GET_SET_OP(PN.L, op_PN_L_get, op_PN_L_set, 1, true);


////////////////////////////////////////////////////////////////////////////////
// P.WRAP and PN.WRAP //////////////////////////////////////////////////////////

// Get
static void op_P_WRAP_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(ss->variables.p_n);
    cs_push(cs, ss_get_pattern_wrap(ss, pn));
}

static void op_PN_WRAP_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(cs_pop(cs));
    cs_push(cs, ss_get_pattern_wrap(ss, pn));
}

// Set
static void op_P_WRAP_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = ss->variables.p_n;
    int16_t a = cs_pop(cs);
    ss_set_pattern_wrap(ss, pn, a >= 1);
}

static void op_PN_WRAP_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(cs_pop(cs));
    int16_t a = cs_pop(cs);
    ss_set_pattern_wrap(ss, pn, a >= 1);
}

// Make ops
const tele_op_t op_P_WRAP =
    MAKE_GET_SET_OP(P.WRAP, op_P_WRAP_get, op_P_WRAP_set, 0, true);
const tele_op_t op_PN_WRAP =
    MAKE_GET_SET_OP(PN.WRAP, op_PN_WRAP_get, op_PN_WRAP_set, 1, true);


////////////////////////////////////////////////////////////////////////////////
// P.START and PN.START ////////////////////////////////////////////////////////

// Get
static void op_P_START_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(ss->variables.p_n);
    cs_push(cs, ss_get_pattern_start(ss, pn));
}

static void op_P_START_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(ss->variables.p_n);
    int16_t a = normalise_idx(ss, pn, cs_pop(cs));
    ss_set_pattern_start(ss, pn, a);
    tele_pattern_updated();
}

// Set
static void op_PN_START_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(cs_pop(cs));
    cs_push(cs, ss_get_pattern_start(ss, pn));
}

static void op_PN_START_set(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(cs_pop(cs));
    int16_t a = normalise_idx(ss, pn, cs_pop(cs));
    ss_set_pattern_start(ss, pn, a);
    tele_pattern_updated();
}

// Make ops
const tele_op_t op_P_START =
    MAKE_GET_SET_OP(P.START, op_P_START_get, op_P_START_set, 0, true);
const tele_op_t op_PN_START =
    MAKE_GET_SET_OP(PN.START, op_PN_START_get, op_PN_START_set, 1, true);


////////////////////////////////////////////////////////////////////////////////
// P.END and PN.END ////////////////////////////////////////////////////////////

// Get
static void op_P_END_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(ss->variables.p_n);
    cs_push(cs, ss_get_pattern_end(ss, pn));
}

static void op_PN_END_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(cs_pop(cs));
    cs_push(cs, ss_get_pattern_end(ss, pn));
}

// Set
static void op_P_END_set(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(ss->variables.p_n);
    int16_t a = normalise_idx(ss, pn, cs_pop(cs));
    ss_set_pattern_end(ss, pn, a);
    tele_pattern_updated();
}

static void op_PN_END_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(cs_pop(cs));
    int16_t a = normalise_idx(ss, pn, cs_pop(cs));
    ss_set_pattern_end(ss, pn, a);
    tele_pattern_updated();
}

// Make ops
const tele_op_t op_P_END =
    MAKE_GET_SET_OP(P.END, op_P_END_get, op_P_END_set, 0, true);
const tele_op_t op_PN_END =
    MAKE_GET_SET_OP(PN.END, op_PN_END_get, op_PN_END_set, 1, true);


////////////////////////////////////////////////////////////////////////////////
// P.I and PN.I ////////////////////////////////////////////////////////////////

// Get
static void op_P_I_get(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(ss->variables.p_n);
    cs_push(cs, ss_get_pattern_idx(ss, pn));
}

static void op_PN_I_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(cs_pop(cs));
    cs_push(cs, ss_get_pattern_idx(ss, pn));
}

// Set
static void p_i_set(scene_state_t *ss, int16_t pn, int16_t i) {
    pn = normalise_pn(pn);
    i = normalise_idx(ss, pn, i);
    int16_t len = ss_get_pattern_len(ss, pn);
    if (i < 0 || len == 0)
        ss_set_pattern_idx(ss, pn, 0);
    else if (i >= len)
        ss_set_pattern_idx(ss, pn, len - 1);
    else
        ss_set_pattern_idx(ss, pn, i);
}

static void op_P_I_set(const void *NOTUSED(data), scene_state_t *ss,
                       exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = ss->variables.p_n;
    int16_t a = cs_pop(cs);
    p_i_set(ss, pn, a);
    tele_pattern_updated();
}

static void op_PN_I_set(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = cs_pop(cs);
    int16_t a = cs_pop(cs);
    p_i_set(ss, pn, a);
    tele_pattern_updated();
}

// Make ops
const tele_op_t op_P_I = MAKE_GET_SET_OP(P.I, op_P_I_get, op_P_I_set, 0, true);
const tele_op_t op_PN_I =
    MAKE_GET_SET_OP(PN.I, op_PN_I_get, op_PN_I_set, 1, true);


////////////////////////////////////////////////////////////////////////////////
// P.HERE and PN.HERE //////////////////////////////////////////////////////////

// Get
static void op_P_HERE_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(ss->variables.p_n);
    cs_push(cs, ss_get_pattern_val(ss, pn, ss_get_pattern_idx(ss, pn)));
}

static void op_PN_HERE_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(cs_pop(cs));
    cs_push(cs, ss_get_pattern_val(ss, pn, ss_get_pattern_idx(ss, pn)));
}

// Set
static void op_P_HERE_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(ss->variables.p_n);
    int16_t a = cs_pop(cs);
    ss_set_pattern_val(ss, pn, ss_get_pattern_idx(ss, pn), a);
}

static void op_PN_HERE_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = normalise_pn(cs_pop(cs));
    int16_t a = cs_pop(cs);
    ss_set_pattern_val(ss, pn, ss_get_pattern_idx(ss, pn), a);
}

// Make ops
const tele_op_t op_P_HERE =
    MAKE_GET_SET_OP(P.HERE, op_P_HERE_get, op_P_HERE_set, 0, true);
const tele_op_t op_PN_HERE =
    MAKE_GET_SET_OP(PN.HERE, op_PN_HERE_get, op_PN_HERE_set, 1, true);


////////////////////////////////////////////////////////////////////////////////
// P.NEXT //////////////////////////////////////////////////////////////////////

// Increment I obeying START, END, WRAP and L
static void p_next_inc_i(scene_state_t *ss, int16_t pn) {
    pn = normalise_pn(pn);

    const int16_t len = ss_get_pattern_len(ss, pn);
    const int16_t start = ss_get_pattern_start(ss, pn);
    const int16_t end = ss_get_pattern_end(ss, pn);
    const uint16_t wrap = ss_get_pattern_wrap(ss, pn);

    int16_t idx = ss_get_pattern_idx(ss, pn);

    if ((idx == (len - 1)) || (idx == end)) {
        if (wrap) idx = start;
    }
    else
        idx++;

    if (idx > len || idx < 0 || idx >= PATTERN_LENGTH) idx = 0;

    ss_set_pattern_idx(ss, pn, idx);
}

// Get
static void op_P_NEXT_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    const int16_t pn = normalise_pn(ss->variables.p_n);
    p_next_inc_i(ss, pn);
    cs_push(cs, ss_get_pattern_val(ss, pn, ss_get_pattern_idx(ss, pn)));
    tele_pattern_updated();
}

static void op_PN_NEXT_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    const int16_t pn = normalise_pn(cs_pop(cs));
    p_next_inc_i(ss, pn);
    cs_push(cs, ss_get_pattern_val(ss, pn, ss_get_pattern_idx(ss, pn)));
    tele_pattern_updated();
}

// Set
static void op_P_NEXT_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    const int16_t pn = normalise_pn(ss->variables.p_n);
    int16_t a = cs_pop(cs);
    p_next_inc_i(ss, pn);
    ss_set_pattern_val(ss, pn, ss_get_pattern_idx(ss, pn), a);
    tele_pattern_updated();
}

static void op_PN_NEXT_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    const int16_t pn = normalise_pn(cs_pop(cs));
    int16_t a = cs_pop(cs);
    p_next_inc_i(ss, pn);
    ss_set_pattern_val(ss, pn, ss_get_pattern_idx(ss, pn), a);
    tele_pattern_updated();
}

// Make ops
const tele_op_t op_P_NEXT =
    MAKE_GET_SET_OP(P.NEXT, op_P_NEXT_get, op_P_NEXT_set, 0, true);
const tele_op_t op_PN_NEXT =
    MAKE_GET_SET_OP(PN.NEXT, op_PN_NEXT_get, op_PN_NEXT_set, 1, true);


////////////////////////////////////////////////////////////////////////////////
// P.PREV //////////////////////////////////////////////////////////////////////

// Increment I obeying START, END, WRAP and L
static void p_prev_dec_i(scene_state_t *ss, int16_t pn) {
    pn = normalise_pn(pn);

    const int16_t len = ss_get_pattern_len(ss, pn);
    const int16_t start = ss_get_pattern_start(ss, pn);
    const int16_t end = ss_get_pattern_end(ss, pn);
    const uint16_t wrap = ss_get_pattern_wrap(ss, pn);

    int16_t idx = ss_get_pattern_idx(ss, pn);

    if ((idx == 0) || (idx == start)) {
        if (wrap) {
            if (end < len)
                idx = end;
            else
                idx = len - 1;
        }
    }
    else
        idx--;

    ss_set_pattern_idx(ss, pn, idx);
}

// Get
static void op_P_PREV_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    const int16_t pn = normalise_pn(ss->variables.p_n);
    p_prev_dec_i(ss, pn);
    cs_push(cs, ss_get_pattern_val(ss, pn, ss_get_pattern_idx(ss, pn)));
    tele_pattern_updated();
}

static void op_PN_PREV_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    const int16_t pn = cs_pop(cs);
    p_prev_dec_i(ss, pn);
    cs_push(cs, ss_get_pattern_val(ss, pn, ss_get_pattern_idx(ss, pn)));
    tele_pattern_updated();
}

// Set
static void op_P_PREV_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    const int16_t pn = normalise_pn(ss->variables.p_n);
    const int16_t a = cs_pop(cs);
    p_prev_dec_i(ss, pn);
    ss_set_pattern_val(ss, pn, ss_get_pattern_idx(ss, pn), a);
    tele_pattern_updated();
}

static void op_PN_PREV_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    const int16_t pn = cs_pop(cs);
    const int16_t a = cs_pop(cs);
    p_prev_dec_i(ss, pn);
    ss_set_pattern_val(ss, pn, ss_get_pattern_idx(ss, pn), a);
    tele_pattern_updated();
}

// Make ops
const tele_op_t op_P_PREV =
    MAKE_GET_SET_OP(P.PREV, op_P_PREV_get, op_P_PREV_set, 0, true);
const tele_op_t op_PN_PREV =
    MAKE_GET_SET_OP(PN.PREV, op_PN_PREV_get, op_PN_PREV_set, 1, true);


////////////////////////////////////////////////////////////////////////////////
// P.INS ///////////////////////////////////////////////////////////////////////

// Get
static void p_ins_get(scene_state_t *ss, int16_t pn, int16_t idx, int16_t val) {
    pn = normalise_pn(pn);
    idx = normalise_idx(ss, pn, idx);
    const int16_t len = ss_get_pattern_len(ss, pn);

    if (len >= idx) {
        for (int16_t i = len; i > idx; i--) {
            int16_t v = ss_get_pattern_val(ss, pn, i - 1);
            ss_set_pattern_val(ss, pn, i, v);
        }
        if (len < PATTERN_LENGTH - 1) { ss_set_pattern_len(ss, pn, len + 1); }
    }

    ss_set_pattern_val(ss, pn, idx, val);
}

static void op_P_INS_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = ss->variables.p_n;
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    p_ins_get(ss, pn, a, b);

    tele_pattern_updated();
}

static void op_PN_INS_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = cs_pop(cs);
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    p_ins_get(ss, pn, a, b);

    tele_pattern_updated();
}

// Make ops
const tele_op_t op_P_INS = MAKE_GET_OP(P.INS, op_P_INS_get, 2, false);
const tele_op_t op_PN_INS = MAKE_GET_OP(PN.INS, op_PN_INS_get, 3, false);


////////////////////////////////////////////////////////////////////////////////
// P.RM ////////////////////////////////////////////////////////////////////////

// Get
static int16_t p_rm_get(scene_state_t *ss, int16_t pn, int16_t idx) {
    pn = normalise_pn(pn);
    const int16_t len = ss_get_pattern_len(ss, pn);

    if (len > 0) {
        idx = normalise_idx(ss, pn, idx);
        int16_t ret = ss_get_pattern_val(ss, pn, idx);

        for (int16_t i = idx; i < len; i++) {
            int16_t v = ss_get_pattern_val(ss, pn, i + 1);
            ss_set_pattern_val(ss, pn, i, v);
        }

        ss_set_pattern_len(ss, pn, len - 1);

        return ret;
    }

    return 0;
}

static void op_P_RM_get(const void *NOTUSED(data), scene_state_t *ss,
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = ss->variables.p_n;
    int16_t a = cs_pop(cs);
    cs_push(cs, p_rm_get(ss, pn, a - 1));  // a is 1-indexed
    tele_pattern_updated();
}

static void op_PN_RM_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = cs_pop(cs);
    int16_t a = cs_pop(cs);
    cs_push(cs, p_rm_get(ss, pn, a));
    tele_pattern_updated();
}

// Make ops
const tele_op_t op_P_RM = MAKE_GET_OP(P.RM, op_P_RM_get, 1, true);
const tele_op_t op_PN_RM = MAKE_GET_OP(PN.RM, op_PN_RM_get, 2, true);


////////////////////////////////////////////////////////////////////////////////
// P.PUSH //////////////////////////////////////////////////////////////////////

// Get
static void p_push_get(scene_state_t *ss, int16_t pn, int16_t val) {
    pn = normalise_pn(pn);
    const int16_t len = ss_get_pattern_len(ss, pn);

    if (len < PATTERN_LENGTH) {
        ss_set_pattern_val(ss, pn, len, val);
        ss_set_pattern_len(ss, pn, len + 1);
    }
}

static void op_P_PUSH_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = ss->variables.p_n;
    int16_t a = cs_pop(cs);
    p_push_get(ss, pn, a);
    tele_pattern_updated();
}

static void op_PN_PUSH_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = cs_pop(cs);
    int16_t a = cs_pop(cs);
    p_push_get(ss, pn, a);
    tele_pattern_updated();
}

// Make ops
const tele_op_t op_P_PUSH = MAKE_GET_OP(P.PUSH, op_P_PUSH_get, 1, false);
const tele_op_t op_PN_PUSH = MAKE_GET_OP(PN.PUSH, op_PN_PUSH_get, 2, false);

////////////////////////////////////////////////////////////////////////////////
// P.POP ///////////////////////////////////////////////////////////////////////

// Get
static int16_t p_pop_get(scene_state_t *ss, int16_t pn) {
    pn = normalise_pn(pn);
    const int16_t len = ss_get_pattern_len(ss, pn);
    if (len > 0) {
        ss_set_pattern_len(ss, pn, len - 1);
        return ss_get_pattern_val(ss, pn, len - 1);
    }
    else
        return 0;
}

static void op_P_POP_get(const void *NOTUSED(data), scene_state_t *ss,
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    cs_push(cs, p_pop_get(ss, ss->variables.p_n));
    tele_pattern_updated();
}

static void op_PN_POP_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t pn = cs_pop(cs);
    cs_push(cs, p_pop_get(ss, pn));
    tele_pattern_updated();
}

// Make ops
const tele_op_t op_P_POP = MAKE_GET_OP(P.POP, op_P_POP_get, 0, true);
const tele_op_t op_PN_POP = MAKE_GET_OP(PN.POP, op_PN_POP_get, 1, true);
