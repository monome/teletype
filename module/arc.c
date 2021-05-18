#include "arc.h"
#include "edit_mode.h"
#include "flash.h"
#include "font.h"
#include "globals.h"
#include "live_mode.h"
#include "pattern_mode.h"
#include "preset_r_mode.h"
#include "state.h"
#include "teletype.h"
#include "teletype_io.h"
#include "timers.h"
#include "util.h"
#include "print_funcs.h"
#include "euclidean/euclidean.h"

#define ARC_TRIGGER_TIME 60

#define ARC_BRIGHTNESS_ON 13
#define ARC_BRIGHTNESS_OFF 0
#define ARC_BRIGHTNESS_DIM 4
#define CLIP_MAX(var,max) if (var > max) var = max;
#define CLIP_MAX_ROLL(var,max) if (var > max) var = 0;
#define CLIP(var, min, max) \
  if (var < (min)) { \
    var = (min); \
  } else if (var > (max)) { \
    var = (max); \
  }



typedef struct {
    u8 on;
    scene_state_t *ss;
    softTimer_t timer;
} arc_timer_t;


static arc_timer_t arc_timers[4];


static void arc_metro_callback(void *o) {
    arc_timer_t *st = o;
    if(st->ss->arc.encoder[0].value<=0)return;

    for(u8 enc=1; enc<4;enc++){
 	st->ss->arc.encoder[enc].arc_cycle_step++;
	CLIP_MAX_ROLL( st->ss->arc.encoder[enc].arc_cycle_step , 15);
	int fill =st->ss->arc.encoder[enc].value >>2;
	int step =st->ss->arc.encoder[enc].arc_cycle_step;
	int out = euclidean(fill, 16,step);
	if(out){
  		tele_tr(enc,1);
		st->ss->tr_pulse_timer[enc]=ARC_TRIGGER_TIME;
    	} 
	//st->ss->arc.leds[enc][step] = ARC_BRIGHTNESS_DIM;
        //monomeLedBuffer[step + (enc << 6)] = ARC_BRIGHTNESS_DIM;
        //monomeFrameDirty |= (1 << enc);
    }

   st->ss->arc.arc_dirty = 1;
}

static void arc_metro_restart(scene_state_t *ss) {
    arc_timers[0].on = 1;
    arc_timers[0].ss = ss;
    timer_remove(&arc_timers[0].timer);
    timer_add(&arc_timers[0].timer,SA.arc_metro_ticks, &arc_metro_callback, (void *)&arc_timers[0]);
    SA.arc_dirty = 1;
}


//static void arc_fill_area(scene_state_t *ss,u8 enc, u8 start, u8 end, s8 level);



void arc_process_enc(scene_state_t *ss, u8 enc, s8 delta, u8 emulated) {
	SA.encoder[enc].value += delta;
	CLIP_MAX_ROLL( SA.encoder[enc].value , 63);
	if(enc==0){
		SA.arc_metro_ticks = bpm2msec(SA.encoder[0].value+70)>>2;
   	     	for (u8 i = 0; i < 64; i++) {
                  SA.leds[enc][i] = (SA.encoder[enc].value==i)?ARC_BRIGHTNESS_ON:ARC_BRIGHTNESS_OFF;
                  SA.leds_layer2[enc][i] = (SA.encoder[enc].value==i)?ARC_BRIGHTNESS_ON:ARC_BRIGHTNESS_OFF;
		}
    			
		arc_metro_restart(ss);
	}else{

	   for(u8 i=0;i<64;i++){
		int eucl =euclidean(SA.encoder[enc].value >>2, 16,i>>2);
 		SA.leds[enc][i] = eucl>0?ARC_BRIGHTNESS_ON:ARC_BRIGHTNESS_OFF;
 		SA.leds_layer2[enc][i] = eucl>0?ARC_BRIGHTNESS_DIM:ARC_BRIGHTNESS_OFF;
	   }
	}
	
	
	SA.arc_dirty = 1;

 /*print_dbg("\r\nenc:");
 print_dbg_short_hex(enc);
 print_dbg("delta:");
 print_dbg_short_hex(delta);
 print_dbg("value:");
 print_dbg_short_hex(SA.encoder[enc].value);
 print_dbg("tick:");
 print_dbg_short_hex(SA.arc_metro_ticks);
 */
 

}



void arc_refresh(scene_state_t *ss) {

    for (u8 enc = 0; enc < monome_encs(); enc++) {
            //arc_fill_area(i, 0, 63, 0);
	   for(u8 i=0;i<64;i++){
	      if(i>>2 != SA.encoder[enc].arc_cycle_step){
             	monomeLedBuffer[i + (enc << 6)] = SA.leds[enc][i];
	      }else{
             	monomeLedBuffer[i + (enc << 6)] = SA.leds_layer2[enc][i];
	      }
	   }	 
    	   monomeFrameDirty |= (1 << enc);
        }

    SA.arc_dirty = 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
static void arc_draw_point(uint8_t n, uint16_t p) {
        int c;

        // c = p / 16;
        c = p >> 4;

        monomeLedBuffer[(n * 64) + c] = 15;
        monomeLedBuffer[(n * 64) + ((c + 1) % 64)] = p % 16;
        monomeLedBuffer[(n * 64) + ((c + 63) % 64)] = 15 - (p % 16);
}

static void arc_draw_point_dark(uint8_t n, uint16_t p) {
        int c;

        c = p / 16;

        monomeLedBuffer[(n * 64) + c] = 7;
        monomeLedBuffer[(n * 64) + ((c + 1) % 64)] = (p % 16) >> 1;
        monomeLedBuffer[(n * 64) + ((c + 63) % 64)] = (15 - (p % 16)) >> 1;
}


static void generate_scales(uint8_t n) {
        levels_scales[n][0] = 0;
        for(uint8_t i1=0;i1<28;i1++) {
                levels_scales[n][i1+1] = levels_scales[n][i1] + SCALE_INT[l.scale[n]-1][i1 % 7];
        }
}


static void levels_dac_refresh(void) {
        for (uint8_t i = 0; i < 4; i++) {
                if (l.mode[i]) {
                        if (l.scale[i]) {
                                dac_set_value(i, tuning_table[i][ levels_scales[i][ l.note[i][play] ] + l.octave[i]*12 ]);
                        } else {
                                dac_set_value(i, tuning_table[i][ l.note[i][play] + l.octave[i]*12 ]);
                        }
                } else {
                        dac_set_value(i, (l.pattern[i][play] + l.offset[i]) << (2 + l.range[i]));
                }
        }
}
*/
