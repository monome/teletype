#include <string.h> //memset
#include <stdlib.h> //abs

#include "print_funcs.h"
#include "gpio.h"

#include "dac.h"
#include "monome.h"
#include "music.h"
#include "init_common.h"

#include "main.h"
#include "arc.h"
#include "arc_cycles.h"

static int16_t enc_count[4];
cycles_data_t c;

static uint8_t mode;
static uint8_t mode_config;
static uint8_t play;
static uint8_t pattern_pos;
static uint8_t pattern_pos_play;
static int8_t pattern_read;
static int8_t pattern_write;
static uint8_t levels_scales[4][29];
static bool tr_state[4];
static uint16_t tr_time[4];
static int16_t tr_time_pw[4];
static void key_long_levels(uint8_t key);
static bool ext_clock;
static bool ext_reset;
static uint8_t arc_preset = 0;
uint8_t key_count_arc[2];





////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////




void resume_cycles() {
	uint8_t i1;

	mode = 0;

	for(i1=0;i1<4;i1++) {
		tele_cv_slew(i1, 2);
		tr_state[i1] = 0;
		cycle_dir[i1] = 1;
		add_force[i1] = 0;
	}

	key_count_arc[0] = 0;
	key_count_arc[1] = 0;

	friction = friction_map[24 - c.friction] + 192;

	ext_clock = !gpio_get_pin_value(B10);

	monomeFrameDirty++;
}







void handler_CyclesTr(s32 data) {
	// print_dbg("\r\n> cycles tr ");
	// print_dbg_ulong(data);

	switch(data) {
	case 0:
		friction = friction_map[24 - c.friction] + 192;
		break;
	case 1:
		friction = friction_map[(24 - c.friction) >> 1] + 192;
		break;
	case 2:
		if(ext_clock) {
			add_force[0] = 0;
			add_force[1] = 0;
			add_force[2] = 0;
			add_force[3] = 0;
		}
		break;
	case 3:
		if(ext_clock) {
			add_force[0] = (cycle_dir[0] * 2 - 1) * (1 << (c.force - 1));
			add_force[1] = (cycle_dir[1] * 2 - 1) * (1 << (c.force - 1));
			add_force[2] = (cycle_dir[2] * 2 - 1) * (1 << (c.force - 1));
			add_force[3] = (cycle_dir[3] * 2 - 1) * (1 << (c.force - 1));
		}
		else {
			c.pos[0] = 0;
			c.pos[1] = 0;
			c.pos[2] = 0;
			c.pos[3] = 0;
		}
		break;
	}
}

void handler_CyclesTrNormal(s32 data) {
	// print_dbg("\r\n> cycles tr normal ");
	// print_dbg_ulong(data);

	ext_clock = data;
}

static void key_long_cycles(uint8_t key) {
	// print_dbg("\r\nLONG PRESS >>>>>>> ");
	// print_dbg_ulong(key);

	if(key == 1) {
		mode = 1;
		enc_count[0] = 0;
		enc_count[1] = 0;
		enc_count[2] = 0;
		enc_count[3] = 0;
		arc_refresh_cycles = &refresh_cycles_config;
		monomeFrameDirty++;
	}
	else {
		friction = friction_map[(24 - c.friction) >> 1] + 192;
	}
}

void refresh_cycles(void) {
	uint8_t i1;
	memset(monomeLedBuffer,0,MONOME_MAX_LED_BYTES);

	if(c.mode) {
		for(i1=1;i1<4;i1++) {
			monomeLedBuffer[i1*64 + 30 + c.mult[i1]*4] = 3;
			monomeLedBuffer[i1*64 + 31 + c.mult[i1]*4] = 3;
			monomeLedBuffer[i1*64 + 32 + c.mult[i1]*4] = 3;
			monomeLedBuffer[i1*64 + 33 + c.mult[i1]*4] = 3;
		}
	}

	for(i1=0;i1<4;i1++)
		arc_draw_point(i1,c.pos[i1] >> 4);
}

void refresh_cycles_config(void) {
	uint8_t i1;
	memset(monomeLedBuffer,0,MONOME_MAX_LED_BYTES);

	if(c.mode) {
		monomeLedBuffer[36] = 7;
		monomeLedBuffer[39] = 7;
		monomeLedBuffer[42] = 7;
		monomeLedBuffer[45] = 7;
	}
	else {
		monomeLedBuffer[35] = 7;
		monomeLedBuffer[42] = 7;
		monomeLedBuffer[44] = 7;
		monomeLedBuffer[48] = 7;
	}

	if(c.shape) {
		for(i1=0;i1<16;i1++) {
			monomeLedBuffer[64 + i1*4 + 0] = i1;
			monomeLedBuffer[64 + i1*4 + 1] = i1;
			monomeLedBuffer[64 + i1*4 + 2] = i1;
			monomeLedBuffer[64 + i1*4 + 3] = i1;
		}
	}
	else {
		for(i1=0;i1<16;i1++) {
			monomeLedBuffer[64 + i1*2] = 15-i1;
			monomeLedBuffer[64 + i1*2 + 1] = 15-i1;
			monomeLedBuffer[64 + 32 + i1*2] = i1;
			monomeLedBuffer[64 + 32 + i1*2 + 1] = i1;
		}
	}

	for(i1=0;i1<8;i1++)
		monomeLedBuffer[128 + 40 + i1] = 3;

	monomeLedBuffer[128 + 38 + (c.force * 2)] = 15;
	monomeLedBuffer[128 + 39 + (c.force * 2)] = 15;

	monomeLedBuffer[192] = 3;
	memset(monomeLedBuffer + 232, 3, 24);
	monomeLedBuffer[192 + ((c.friction + 40) & 0x3f)] = 15;
	// uint16_t i = (c.friction * 3);
	// i = (i + 640) & 0x3ff;
	// arc_draw_point(3, i);
}

void refresh_cycles_config_range(void) {
	uint8_t i1, i2;
	memset(monomeLedBuffer,0,MONOME_MAX_LED_BYTES);

	for(i1=0;i1<4;i1++)
		for(i2=0;i2<c.range[i1];i2++)
			monomeLedBuffer[i1*64 + ((32 + i2) & 0x3f)] = 3;
}

void refresh_cycles_config_div(void) {
	uint8_t i1, i2;
	memset(monomeLedBuffer,0,MONOME_MAX_LED_BYTES);

	for(i1=0;i1<4;i1++) {
		for(i2=0; i2<(1<<c.div[i1]); i2++) {
			memset(i1*64 + monomeLedBuffer + i2*(64 >> c.div[i1]), 5, (64 >> (c.div[i1]+1)));
		}
	}
}
