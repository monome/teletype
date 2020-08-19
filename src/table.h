#ifndef _TABLE_H_
#define _TABLE_H_

#include "music.h"

// use the same note table (from libavr32) as ansible
#define table_n ET

extern const int table_v[11];
extern const int table_vv[100];
extern const int table_exp[256];
extern const int table_nr[32];
extern const int table_n_s[9][7];
extern const int table_n_c[13][4];
extern const int table_n_cs[9][7];

#endif
