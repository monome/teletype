#ifndef _ARC_H_
#define _ARC_H_

#include "monome.h"
#include "state.h"

#define SA ss->arc
#define MONOME_RING_MAX_ENCS 4
#define ARC_NUM_PRESETS 1

//extern void arc_set_control_mode(u8 control, u8 mode, scene_state_t *ss);
//extern void arc_metro_triggered(scene_state_t *ss);
extern void arc_refresh(scene_state_t *ss);
extern void arc_process_enc(scene_state_t *ss, u8 n, s8 delta, u8 emulated);
//extern void generate_scales(uint8_t n);
//extern void arc_draw_point(uint8_t n, uint16_t p);
//extern void arc_draw_point_dark(uint8_t n, uint16_t p);


static u16 bpm2msec(u8 a) {
    u16 ret;
    if (a < 2) a = 2;
    if (a > 1000) a = 1000;
    return ret = ((((uint32_t)(1 << 31)) / ((a << 20) / 60)) * 1000) >> 11;
}



#endif
