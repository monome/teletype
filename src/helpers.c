#include "helpers.h"
#include <float.h>
#include <math.h>
#include <string.h>
#include "table.h"
#include "util.h"

int16_t normalise_value(int16_t min, int16_t max, int16_t wrap, int16_t value) {
    if (value >= min && value <= max) return value;

    if (wrap) {
        if (value < min)
            return max;
        else
            return min;
    }
    else {
        if (value < min)
            return min;
        else
            return max;
    }
}

const char *to_voltage(int16_t i) {
    static char n[3], v[7];
    int16_t a = 0, b = 0;

    if (i > table_v[8]) {
        i -= table_v[8];
        a += 8;
    }

    if (i > table_v[4]) {
        i -= table_v[4];
        a += 4;
    }

    if (i > table_v[2]) {
        i -= table_v[2];
        a += 2;
    }

    if (i > table_v[1]) {
        i -= table_v[1];
        a += 1;
    }

    if (i > table_vv[64]) {
        i -= table_vv[64];
        b += 64;
    }

    if (i > table_vv[32]) {
        i -= table_vv[32];
        b += 32;
    }

    if (i > table_vv[16]) {
        i -= table_vv[16];
        b += 16;
    }

    if (i > table_vv[8]) {
        i -= table_vv[8];
        b += 8;
    }

    if (i > table_vv[4]) {
        i -= table_vv[4];
        b += 4;
    }

    if (i > table_vv[2]) {
        i -= table_vv[2];
        b++;
    }

    if (i > table_vv[1]) {
        i -= table_vv[1];
        b++;
    }

    b++;

    itoa(a, n, 10);
    strcpy(v, n);
    strcat(v, ".");
    itoa(b, n, 10);
    strcat(v, n);
    strcat(v, "V");

    return v;
}

int16_t bit_reverse(int16_t unreversed, int8_t bits_to_reverse) {
    int16_t reversed = 0;
    for (int i = 0; i < bits_to_reverse; i++) {
        if ((unreversed & (1 << i)))
            reversed |= 1 << ((bits_to_reverse - 1) - i);
    }
    return reversed;
}

int16_t rev_bitstring_to_int(const char* token) {
    int8_t length = strlen(token);
    int16_t value = 0;
    for (int8_t i = 0; i < length; i++) {
        if (token[i] == '1') { value += 1 << i; }
    }
    return value;
}


double fs_pow(double x, double y) {
    double p = 0;

    if (0 > x && fs_fmod(y, 1) == 0) {
        if (fs_fmod(y, 2) == 0) { p = fs_exp(fs_log(-x) * y); }
        else {
            p = -fs_exp(fs_log(-x) * y);
        }
    }
    else {
        if (x != 0 || 0 >= y) { p = fs_exp(fs_log(x) * y); }
    }
    return p;
}

double fs_fmod(double x, double y) {
    double a, b;
    const double c = x;

    if (0 > c) { x = -x; }
    if (0 > y) { y = -y; }
    if (y != 0 && DBL_MAX >= y && DBL_MAX >= x) {
        while (x >= y) {
            a = x / 2;
            b = y;
            while (a >= b) { b *= 2; }
            x -= b;
        }
    }
    else {
        x = 0;
    }
    return 0 > c ? -x : x;
}

double fs_log(double x) {
    int n;
    double a, b, c, epsilon;
    static double A, B, C;
    static int initialized;

    if (x > 0 && DBL_MAX >= x) {
        if (!initialized) {
            initialized = 1;
            A = sqrt(2);
            B = A / 2;
            C = fs_log(A);
        }
        for (n = 0; x > A; x /= 2) { ++n; }
        while (B > x) {
            --n;
            x *= 2;
        }
        a = (x - 1) / (x + 1);
        x = C * n + a;
        c = a * a;
        n = 1;
        epsilon = DBL_EPSILON * x;
        if (0 > a) {
            if (epsilon > 0) { epsilon = -epsilon; }
            do {
                n += 2;
                a *= c;
                b = a / n;
                x += b;
            } while (epsilon > b);
        }
        else {
            if (0 > epsilon) { epsilon = -epsilon; }
            do {
                n += 2;
                a *= c;
                b = a / n;
                x += b;
            } while (b > epsilon);
        }
        x *= 2;
    }
    else {
        x = -DBL_MAX;
    }
    return x;
}


double fs_exp(double x) {
    unsigned n, square;
    double b, e;
    static double x_max, x_min, epsilon;
    static int initialized;

    if (!initialized) {
        initialized = 1;
        x_max = fs_log(DBL_MAX);
        x_min = fs_log(DBL_MIN);
        epsilon = DBL_EPSILON / 4;
    }
    if (x_max >= x && x >= x_min) {
        for (square = 0; x > 1; x /= 2) { ++square; }
        while (-1 > x) {
            ++square;
            x /= 2;
        }
        e = b = n = 1;
        do {
            b /= n++;
            b *= x;
            e += b;
            b /= n++;
            b *= x;
            e += b;
        } while (b > epsilon);
        while (square-- != 0) { e *= e; }
    }
    else {
        e = x > 0 ? DBL_MAX : 0;
    }
    return e;
}
