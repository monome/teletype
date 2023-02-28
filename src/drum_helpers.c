#include "drum_helpers.h"

#include <stdio.h>

#include "table.h"

static int wrap(int const k, int const lower_bound, int const upper_bound) {
    int range = upper_bound - lower_bound + 1;
    int kx = ((k - lower_bound) % range);
    if (kx < 0)
        return upper_bound + 1 + kx;
    else
        return lower_bound + kx;
}


static char get_byte(const char* a, int n) {
    return a[n >> 3];
}

static int get_bit(const char* a, int k) {
    char byte = get_byte(a, k);
    int bit_index = 7 - (k % 8);

    return (byte & (1 << bit_index)) != 0;
}

int tresillo(int bank, int pattern1, int pattern2, int len, int step) {
    if (bank < 0 || bank > 4) return 0;
    if (len < 8) return 0;
    if (step < 0) return 0;
    if (pattern1 >= drum_ops_pattern_len || pattern2 >= drum_ops_pattern_len)
        return 0;

    const char* table1;
    const char* table2;

    switch (bank) {
        case 0:
            table1 = table_t_r_e[pattern1];
            table2 = table_t_r_e[pattern2];
            break;
        case 1:
            table1 = table_dr_bd[pattern1];
            table2 = table_dr_bd[pattern2];
            break;
        case 2:
            table1 = table_dr_sd[pattern1];
            table2 = table_dr_sd[pattern2];
            break;
        case 3:
            table1 = table_dr_ch[pattern1];
            table2 = table_dr_ch[pattern2];
            break;
        case 4:
            table1 = table_dr_oh[pattern1];
            table2 = table_dr_oh[pattern2];
            break;
    }


    int multiplier = len / 8;

    int three = 3 * multiplier;
    int wrapped_step = wrap(step, 0, multiplier * 8 - 1);


    if (wrapped_step <= three - 1) { return get_bit(table1, wrapped_step); }
    else if (wrapped_step <= three * 2 - 1) {
        return get_bit(table1, wrapped_step - three);
    }

    return get_bit(table2, wrapped_step - (three * 2));
}

int drum(int bank, int pattern, int step) {
    if (bank < 0 || bank > 4) return 0;
    if (step < 0) return 0;
    if (pattern >= drum_ops_pattern_len) return 0;

    const char* table;

    switch (bank) {
        case 0: table = table_t_r_e[pattern]; break;
        case 1: table = table_dr_bd[pattern]; break;
        case 2: table = table_dr_sd[pattern]; break;
        case 3: table = table_dr_ch[pattern]; break;
        case 4: table = table_dr_oh[pattern]; break;
    }

    int wrapped_step = wrap(step, 0, 15);

    return get_bit(table, wrapped_step);
}

int velocity(int pattern, int step) {
    if (step < 0) return 0;
    if (pattern < 0 || pattern > 19) return 0;

    const uint16_t* table = table_vel_helper[pattern];
    int wrapped_step = wrap(step, 0, 15);
    return table[wrapped_step];
}