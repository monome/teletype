#ifndef _ARC_H_
#define _ARC_H_

#include "monome.h"
#include "state.h"

#define SA ss->arc

#define ARC_EUCLIDEAN 0


extern void arc_init(scene_state_t *ss);
// callback function pointers
extern void (*arc_refresh)(scene_state_t *ss);
extern void (*arc_process_enc)(scene_state_t *ss, u8 enc, s8 delta);
extern void (*arc_process_key)(scene_state_t *ss, u8 enc);
extern void (*arc_metro_triggered)(scene_state_t *ss);
extern void (*arc_script_triggered)(scene_state_t *ss, u8 script);


#endif
