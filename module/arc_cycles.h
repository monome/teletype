#pragma once

#include "main.h"

typedef struct {
	uint16_t pos[4];
	int16_t speed[4];
	int8_t mult[4];
	uint8_t range[4];
	uint8_t mode;
	uint8_t shape;
	uint8_t friction;
	uint16_t force;
	uint8_t div[4];
} cycles_data_t;

extern cycles_data_t c;

void default_cycles(void);
void init_cycles(void);
void resume_cycles(void);
void clock_cycles(uint8_t phase);
void ii_cycles(uint8_t *d, uint8_t l);
void refresh_cycles(void);
void refresh_cycles_config(void);
void refresh_cycles_config_range(void);
void refresh_cycles_config_div(void);
void handler_CyclesEnc(s32 data);
void handler_CyclesRefresh(s32 data);
void handler_CyclesKey(s32 data);
void handler_CyclesTr(s32 data);
void handler_CyclesTrNormal(s32 data);
