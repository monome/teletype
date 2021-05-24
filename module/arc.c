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

#define CLIP_MAX_ROLL(var,max) if (var > max) var = 0;
#define CLIP_U16(var, min, max) \
  if (var < (min)) { \
    var = (min); \
  } else if (var > 65530) { \
    var = 0; \
  } else if (var > (max)) { \
    var = (max); \
  }
#define CLIP_U8(var, min, max) \
    if (var < (min)) { \
      var = (min); \
    } else if (var > 253) { \
      var = 0; \
    } else if (var > (max)) { \
      var = (max); \
    }


typedef struct {
    u8 on;
    scene_state_t *ss;
    softTimer_t timer;
} arc_timer_t;


typedef struct {
    u8 length;
    u8 shift;
} euclidean_lengths_t;

typedef enum {
    EUCL_CONF_LENGTH,
    EUCL_CONF_PHASE,
    EUCL_LIVE
} arc_config_mode_t ;

static arc_config_mode_t arc_config_mode = EUCL_LIVE;

// sensitiviy of arc clone , original arc must be larger
#define ARC_SENSITIVITY 2
#define ARC_SENSITIVITY_CONF 16
static u8 ring_sensitivity_shift;
static s16 delta_buffer = 0;

// if you change also take care of SHIFT_SENSITIVITY below
// first number is lenght, second is shift divider for 64 leds
// please keep sorted for efficient max_length detection!
const euclidean_lengths_t el[3] = {
  {8, 3},
  {16, 2},
  {32, 1}
};


void (*arc_refresh)(scene_state_t *ss);
void (*arc_process_enc)(scene_state_t *ss, u8 enc, s8 delta);
void (*arc_process_key)(scene_state_t *ss, u8 enc);
void (*arc_metro_triggered)(scene_state_t *ss);
void (*arc_script_triggered)(scene_state_t *ss, u8 script);



void arc_metro_triggered_eucl(scene_state_t *ss);
void arc_script_triggered_eucl(scene_state_t *ss, u8 enc);
void arc_process_enc_eucl(scene_state_t *ss, u8 enc);
void arc_process_enc_eucl_live(scene_state_t *ss, u8 enc, s8 delta);
void arc_process_key_eucl(scene_state_t *ss, u8 enc);
void arc_refresh_eucl(scene_state_t *ss);

void arc_process_enc_eucl_conf_length(scene_state_t *ss, u8 enc, s8 delta);
void arc_process_enc_eucl_conf_phase(scene_state_t *ss, u8 enc, s8 delta);

void arc_init_cycles(void);
void arc_reset(scene_state_t *ss);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void arc_init(scene_state_t *ss) {

  switch(SA.mode){
    case ARC_EUCLIDEAN:
    default:
      arc_metro_triggered = &arc_metro_triggered_eucl;
      arc_script_triggered = &arc_script_triggered_eucl;
      arc_process_enc = &arc_process_enc_eucl_live;
      arc_process_key = &arc_process_key_eucl;
      arc_refresh = &arc_refresh_eucl;
      break;
  }

}

void arc_reset(scene_state_t *ss){
  SA.encoder[0].cycle_step = 0;
  SA.encoder[1].cycle_step = 0;
  SA.encoder[2].cycle_step = 0;
  SA.encoder[3].cycle_step = 0;
  SA.reset = false;
}



void arc_metro_triggered_eucl(scene_state_t *ss) {
  if( SA.encoder[0].value<0
    ||SA.encoder[1].value<0
    ||SA.encoder[2].value<0
    ||SA.encoder[3].value<0)return;

  //identify enc with max length
  int max_enc = 0;
  for(u8 enc=0; enc< monome_encs();enc++){
     if (SA.encoder[enc].pattern_index>SA.encoder[max_enc].pattern_index)
       max_enc = enc;
  }

  if(SA.encoder[max_enc].cycle_step == 0){
    arc_reset(ss);
    }


  if(SA.reset) arc_reset(ss);

  for(u8 enc=0; enc< monome_encs();enc++){

  	SA.encoder[enc].cycle_step++;
  	CLIP_MAX_ROLL( SA.encoder[enc].cycle_step , el[SA.encoder[enc].pattern_index].length-1);

    ring_sensitivity_shift = el[SA.encoder[enc].pattern_index].shift+1;
  	u8 fill =SA.encoder[enc].value >> ring_sensitivity_shift;
  	s8 step =SA.encoder[enc].cycle_step - SA.encoder[enc].phase_offset;
  	u8 out = euclidean(fill, el[SA.encoder[enc].pattern_index].length,step);
	  if(out){
  		tele_tr(enc,1);
		   ss->tr_pulse_timer[enc]=ARC_TRIGGER_TIME;
    	}
  }

   SA.arc_dirty = 1;
}

void arc_script_triggered_eucl(scene_state_t *ss, u8 enc) {
  if(enc >= monome_encs())return;

  if(SA.sync){
      arc_metro_triggered_eucl(ss);
      return;
  }

  if(SA.reset) arc_reset(ss);

  if( SA.encoder[0].value<0
    ||SA.encoder[1].value<0
    ||SA.encoder[2].value<0
    ||SA.encoder[3].value<0)return;

  ring_sensitivity_shift = el[SA.encoder[enc].pattern_index].shift+1;
 	SA.encoder[enc].cycle_step++;
	CLIP_MAX_ROLL( SA.encoder[enc].cycle_step , el[SA.encoder[enc].pattern_index].length-1);
	u8 fill =SA.encoder[enc].value >> ring_sensitivity_shift;
	s8 step =SA.encoder[enc].cycle_step - SA.encoder[enc].phase_offset;
	u8 out = euclidean(fill, el[SA.encoder[enc].pattern_index].length,step);
	if(out){
  		tele_tr(enc,1);
		   ss->tr_pulse_timer[enc]=ARC_TRIGGER_TIME;
    	}
 SA.arc_dirty = 1;
}


void arc_process_enc_eucl(scene_state_t *ss, u8 enc) {
 ring_sensitivity_shift = el[SA.encoder[enc].pattern_index].shift+1;
  u8 phase_offset_ = SA.encoder[enc].phase_offset;
 for(u8 i=0;i<(64);i++){
		int eucl =euclidean(SA.encoder[enc].value >> ring_sensitivity_shift , el[SA.encoder[enc].pattern_index].length,(i >>el[SA.encoder[enc].pattern_index].shift) - phase_offset_);
 		SA.leds[enc][i] = eucl>0?ARC_BRIGHTNESS_ON:ARC_BRIGHTNESS_OFF;
 		SA.leds_layer2[enc][i] = eucl>0?ARC_BRIGHTNESS_DIM:ARC_BRIGHTNESS_OFF;
	   }
  SA.arc_dirty = 1;
}



void arc_process_enc_eucl_live(scene_state_t *ss, u8 enc, s8 delta) {
 delta_buffer += delta;

 if(delta_buffer>ARC_SENSITIVITY||delta_buffer<(-ARC_SENSITIVITY)){
  delta_buffer=0;
  SA.encoder[enc].value += (delta>0)?1:-1;

  CLIP_U16( SA.encoder[enc].value , 0 , 128);

  arc_process_enc_eucl(ss,enc);
 }
}



void arc_refresh_eucl(scene_state_t *ss) {

    for (u8 enc = 0; enc < monome_encs(); enc++) {
      for(u8 i=0;i<64;i++){
 	      if((i>>el[SA.encoder[enc].pattern_index].shift) != SA.encoder[enc].cycle_step){
              	monomeLedBuffer[i + (enc << 6)] = SA.leds[enc][i];
 	      }else{
              	monomeLedBuffer[i + (enc << 6)] = SA.leds_layer2[enc][i];
 	      }
      }
     	   monomeFrameDirty |= (1 << enc);
    }

    SA.arc_dirty = 0;
}


/////////////////////// EUCL CONFIG ///////////////////////////

void arc_process_key_eucl(scene_state_t *ss, u8 enc) {

switch(arc_config_mode){
  case EUCL_CONF_LENGTH:
  arc_config_mode = EUCL_CONF_PHASE;
  arc_process_enc = &arc_process_enc_eucl_conf_length;
  arc_refresh = &arc_refresh_eucl;
//  arc_refresh = &arc_refresh_eucl_conf_length;

    break;
  case EUCL_CONF_PHASE:
  arc_config_mode = EUCL_LIVE;
  arc_process_enc = &arc_process_enc_eucl_conf_phase;
  arc_refresh = &arc_refresh_eucl;
//  arc_refresh = &arc_refresh_eucl_conf_phase;
    break;
  case EUCL_LIVE:
  default:
    arc_config_mode = EUCL_CONF_LENGTH;
    arc_process_enc = &arc_process_enc_eucl_live;
    arc_refresh = &arc_refresh_eucl;
    break;
  }
delta_buffer=0;
SA.arc_dirty = 1;
}

void arc_process_enc_eucl_conf_length(scene_state_t *ss, u8 enc, s8 delta) {

 delta_buffer += delta;
 print_dbg("\r\nARC delta_buffer");
 print_dbg_hex (delta_buffer);

 if(delta_buffer>ARC_SENSITIVITY_CONF||(-delta_buffer)>ARC_SENSITIVITY_CONF){
  delta_buffer=0;
  switch(SA.encoder[enc].pattern_index){
    case 0:
      SA.encoder[enc].pattern_index = (delta>0)?1:0;
      break;
    case 1:
      SA.encoder[enc].pattern_index = (delta>0)?2:0;
      break;
    case 2:
      SA.encoder[enc].pattern_index = (delta>0)?2:1;
      break;
    default:
      SA.encoder[enc].pattern_index = 0;
      break;
    }

  }
     arc_process_enc_eucl(ss,enc);
}


void arc_process_enc_eucl_conf_phase(scene_state_t *ss, u8 enc, s8 delta) {
  delta_buffer += delta;

  if(delta_buffer>ARC_SENSITIVITY_CONF||delta_buffer<(-ARC_SENSITIVITY_CONF)){
    delta_buffer=0;
    SA.encoder[enc].phase_offset += (delta>0)?1:-1;

  	CLIP_U8( SA.encoder[enc].phase_offset , 0 , el[SA.encoder[enc].pattern_index].length-1);

  }
  arc_process_enc_eucl(ss,enc);
}
