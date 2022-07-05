#ifndef SCALE_H
#define SCALE_H

#include <stdint.h>

#define SCALE_T int16_t
#define _SCALE_T int32_t

typedef struct {
    _SCALE_T b;
    _SCALE_T m;
} scale_t;

typedef struct {
    SCALE_T p_min;
    SCALE_T p_max;
    SCALE_T i_min;
    SCALE_T i_max;
    SCALE_T f_min[64];
    SCALE_T f_max[64];
} cal_data_t;

extern cal_data_t blank_cal_data;

typedef struct {
    SCALE_T out_min;
    SCALE_T out_max;
} scale_data_t;

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

#define TO_Q15(x) ((x) << 15)
#define FROM_Q15(x) ((((x) >> 14) + 1) >> 1)

static inline scale_t scale_init(SCALE_T izero, SCALE_T imax, SCALE_T ozero,
                                 SCALE_T omax) {
    scale_t ret;
    if (izero == imax) imax = 1;
    // Impart 16 bits of precision
    ret.m = TO_Q15(omax - ozero) / (imax - izero);
    ret.b = ozero - FROM_Q15(ret.m * izero);
    return ret;
}

static inline SCALE_T scale_get(scale_t scale, SCALE_T x) {
    return FROM_Q15(scale.m * x) + scale.b;
}

#endif
