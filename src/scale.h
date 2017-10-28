#ifndef SCALE_H
#define SCALE_H

#define SCALE_T int16_t
#define _SCALE_T int32_t

typedef struct {
    _SCALE_T b;
    _SCALE_T m;
} scale_t;

/*
 * x1, y1 = izero, ozero
 * x2, y2 = imax, omax
 *
 * m = (omax - ozero) / (imax - izero)
 * b = ozero - m * izero
 *
 * y = mx + b
 *
 * Fixed-point implementation
 *
 * Q16.15 - signed integers with 15 digits of precision
 *
 * best available precision with no performance penalty
 *
 */

#define TO_Q16(x) ((x) << 16)
#define FROM_Q16(x) ((x) >> 16)

// WARNING!
// Not guarded against izero = imax. Will crash (/0).  Check before you call.
static inline scale_t scale_init(SCALE_T izero, SCALE_T imax, SCALE_T ozero,
                                 SCALE_T omax) {
    scale_t ret;
    // Impart 16 bits of precision
    ret.m = TO_Q16(omax - ozero) / (imax - izero);
    ret.b = ozero - FROM_Q16(ret.m * izero);
    return ret;
}

static inline SCALE_T scale_get(scale_t scale, SCALE_T x) {
    return FROM_Q16(scale.m * x) + scale.b;
}

#endif
