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


typedef struct {
    u8 length;
    u8 shift;
} euclidean_lengths_t;

typedef enum {
    EUCL_CONF_LENGTH,
    EUCL_CONF_PHASE,
    EUCL_CONF_SYNC,
    EUCL_LIVE
} arc_config_mode_t ;

static arc_config_mode_t arc_config_mode = EUCL_LIVE;

// sensitiviy of arc clone (128+7), original arc must be larger
#define ARC_SENSITIVITY_EUCL 135
#define ARC_SENSITIVITY_CONF 32
static u8 ring_sensitivity_shift;
static s16 delta_buffer = 0;

// if you change also take care of SHIFT_SENSITIVITY below
// first number is lenght, second is shift divider for 64 leds
const euclidean_lengths_t el[3] = {
  {8, 3},
  {16, 2},
  {32, 1}
};

//static arc_state_t arc_state;
static arc_timer_t arc_timers[4];

const uint8_t friction_map[25] = { 0, 12, 20, 26, 30, 34, 39, 42, 45,
	48, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64};

const uint16_t div_map[4] = {0x2000,0x1000,0x0800,0x0400};

static uint16_t friction;
static int8_t add_force[4];
static bool cycle_dir[4];


void (*arc_refresh)(scene_state_t *ss);
void (*arc_process_enc)(scene_state_t *ss, u8 enc, s8 delta);
void (*arc_process_key)(scene_state_t *ss, u8 enc, s8 delta);
void (*arc_metro_triggered)(scene_state_t *ss);
void (*arc_script_triggered)(scene_state_t *ss, u8 script);



void arc_metro_triggered_eucl(scene_state_t *ss);
void arc_script_triggered_eucl(scene_state_t *ss, u8 enc);
void arc_process_enc_eucl(scene_state_t *ss, u8 enc, s8 delta);
void arc_process_key_eucl(scene_state_t *ss, u8 enc, s8 delta);
void arc_refresh_eucl(scene_state_t *ss);

void arc_process_enc_eucl_conf_length(scene_state_t *ss, u8 enc, s8 delta);
void arc_process_enc_eucl_conf_phase(scene_state_t *ss, u8 enc, s8 delta);
void arc_process_enc_eucl_conf_sync(scene_state_t *ss, u8 enc, s8 delta);
void arc_refresh_eucl_conf_length(scene_state_t *ss);
void arc_refresh_eucl_conf_phase(scene_state_t *ss);
void arc_refresh_eucl_conf_sync(scene_state_t *ss);

void arc_init_cycles(void);
void arc_metro_triggered_circles(scene_state_t *ss);
void arc_script_triggered_circles(scene_state_t *ss, u8 enc);
void arc_process_enc_circles(scene_state_t *ss, u8 enc, s8 delta);
void arc_process_key_circles(scene_state_t *ss, u8 enc, s8 delta);
void arc_refresh_circles(scene_state_t *ss);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void arc_init(scene_state_t *ss) {

  switch(SA.mode){
/*    case ARC_CIRCLES:
      arc_init_cycles();
      arc_metro_triggered = &arc_metro_triggered_circles;
      arc_script_triggered = &arc_script_triggered_circles;
      arc_process_enc = &arc_process_enc_circles;

      switch(SA.circles_mode) {
      	// key 1 UP
      	case case CIRCLES_MODE02:
      			friction = friction_map[24 - c.friction] + 192;
      		break;
      	case CIRCLES_MODE11:
      			SA.circles_mode = 2;
      			enc_count[0] = 0;
      			enc_count[1] = 0;
      			enc_count[2] = 0;
      			enc_count[3] = 0;
      			arc_refresh = &arc_refresh_cycles_config_range;
      			monomeFrameDirty++;
      		  break;
       	case CIRCLES_MODE12:
      			SA.circles_mode = 3;
      			enc_count[0] = 0;
      			enc_count[1] = 0;
      			enc_count[2] = 0;
      			enc_count[3] = 0;
      			arc_refresh = &arc_refresh_cycles_config_div;
      			monomeFrameDirty++;
      			break;
      		case CIRCLES_MODE13:
      			SA.circles_mode = 1;
      			enc_count[0] = 0;
      			enc_count[1] = 0;
      			enc_count[2] = 0;
      			enc_count[3] = 0;
      			arc_refresh = &arc_refresh_cycles_config;
      			monomeFrameDirty++;
      		case CIRCLES_MODE21:
      			c.pos[0] = 0;
      			c.pos[1] = 0;
      			c.pos[2] = 0;
      			c.pos[3] = 0;
      		  break;
      		case CIRCLES_MODE22:
      			SA.circles_mode = 0;
      			enc_count[0] = 0;
      			enc_count[1] = 0;
      			enc_count[2] = 0;
      			enc_count[3] = 0;
      			arc_refresh = &arc_refresh_cycles;
      			monomeFrameDirty++;
      			break;
      	   default:
      		  break;
      	}

      break;
*/
    case ARC_EUCLIDEAN:
    default:
      arc_metro_triggered = &arc_metro_triggered_eucl;
      arc_script_triggered = &arc_script_triggered_eucl;
      arc_process_enc = &arc_process_enc_eucl;
      arc_process_key = &arc_process_key_eucl;
      arc_refresh = &arc_refresh_eucl;
      break;
  }

}



void arc_metro_triggered_eucl(scene_state_t *ss) {
    if(SA.encoder[0].value<=0)return;

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
	//st->ss->arc.leds[enc][step] = ARC_BRIGHTNESS_DIM;
        //monomeLedBuffer[step + (enc << 6)] = ARC_BRIGHTNESS_DIM;
        //monomeFrameDirty |= (1 << enc);
  }

   SA.arc_dirty = 1;
}

void arc_script_triggered_eucl(scene_state_t *ss, u8 enc) {
    if(enc >= monome_encs())return;
    if(SA.encoder[0].value<=0)return;

    if(SA.sync){
      arc_metro_triggered_eucl(ss);
      return;
    }

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


void arc_process_enc_eucl(scene_state_t *ss, u8 enc, s8 delta) {
	SA.encoder[enc].value += delta;
	CLIP_U16( SA.encoder[enc].value ,0, ARC_SENSITIVITY_EUCL);
  print_dbg("\r\nARC ring enc");
  print_dbg_hex(SA.encoder[enc].value);
 ring_sensitivity_shift = el[SA.encoder[enc].pattern_index].shift+1;
  u8 phase_offset_ = SA.encoder[enc].phase_offset;
 for(u8 i=0;i<(64);i++){
		int eucl =euclidean(SA.encoder[enc].value >> ring_sensitivity_shift , el[SA.encoder[enc].pattern_index].length,(i >>el[SA.encoder[enc].pattern_index].shift) - phase_offset_);
 		SA.leds[enc][i] = eucl>0?ARC_BRIGHTNESS_ON:ARC_BRIGHTNESS_OFF;
 		SA.leds_layer2[enc][i] = eucl>0?ARC_BRIGHTNESS_DIM:ARC_BRIGHTNESS_OFF;
	   }
  SA.arc_dirty = 1;
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

void arc_process_key_eucl(scene_state_t *ss, u8 enc, s8 delta) {
  print_dbg("\r\nARC ring key");
  print_dbg_hex (arc_config_mode);

switch(arc_config_mode){
  case EUCL_CONF_LENGTH:
  arc_config_mode = EUCL_CONF_PHASE;
  arc_process_enc = &arc_process_enc_eucl_conf_length;
  arc_refresh = &arc_refresh_eucl_conf_length;

    break;
  case EUCL_CONF_PHASE:
  arc_config_mode = EUCL_CONF_SYNC;
  arc_process_enc = &arc_process_enc_eucl_conf_phase;
  arc_refresh = &arc_refresh_eucl_conf_phase;
    break;
  case EUCL_CONF_SYNC:
  arc_config_mode = EUCL_LIVE;
  arc_process_enc = &arc_process_enc_eucl_conf_sync;
  arc_refresh = &arc_refresh_eucl_conf_sync;
    break;
  case EUCL_LIVE:
  default:
    arc_config_mode = EUCL_CONF_LENGTH;
    arc_process_enc = &arc_process_enc_eucl;
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
   SA.arc_dirty = 1;
}



void arc_refresh_eucl_conf_length(scene_state_t *ss) {

    for (u8 enc = 0; enc < monome_encs(); enc++) {

	   for(u8 i=0;i<64;i++){
	      if( i < el[SA.encoder[enc].pattern_index].length){
          monomeLedBuffer[i + (enc << 6)] = ARC_BRIGHTNESS_ON;
	      }else{
          monomeLedBuffer[i + (enc << 6)] = ARC_BRIGHTNESS_OFF;
        }
	   }
    	   monomeFrameDirty |= (1 << enc);
    }

    SA.arc_dirty = 0;
}


void arc_process_enc_eucl_conf_phase(scene_state_t *ss, u8 enc, s8 delta) {


  delta_buffer += delta;

  if(delta_buffer>ARC_SENSITIVITY_CONF||delta_buffer<(-ARC_SENSITIVITY_CONF)){
    delta_buffer=0;
    SA.encoder[enc].phase_offset += (delta>0)?1:-1;

    print_dbg("\r\nARC phase offset");
    print_dbg_hex (SA.encoder[enc].phase_offset);

  	CLIP_U8( SA.encoder[enc].phase_offset , 0 , el[SA.encoder[enc].pattern_index].length-1);

    print_dbg("\r\nARC phase offset clipped");
    print_dbg_hex (SA.encoder[enc].phase_offset);

  }

  SA.arc_dirty = 1;
}



void arc_refresh_eucl_conf_phase(scene_state_t *ss) {

    for (u8 enc = 0; enc < monome_encs(); enc++) {
     for(u8 i=0;i<64;i++){
       	monomeLedBuffer[i + (enc << 6)] = ARC_BRIGHTNESS_OFF;
     }

	     for(u8 i=0;i<64;i++){
          if((i>>el[SA.encoder[enc].pattern_index].shift)== SA.encoder[enc].phase_offset){
           monomeLedBuffer[i + (enc << 6)] = ARC_BRIGHTNESS_ON;
   	      }
        }


    	   monomeFrameDirty |= (1 << enc);
    }

    SA.arc_dirty = 0;
}

void arc_process_enc_eucl_conf_sync(scene_state_t *ss, u8 enc, s8 delta) {

   delta_buffer += delta;

   if(delta_buffer>ARC_SENSITIVITY_CONF||delta_buffer<(-ARC_SENSITIVITY_CONF)){
    delta_buffer=0;

    if(delta>0){
      SA.sync = 1;
      SA.encoder[3].cycle_step = 0;
      SA.encoder[2].cycle_step = 0;
      SA.encoder[1].cycle_step = 0;
      SA.encoder[0].cycle_step = 0;
    }else{
      SA.sync = 0;
    }
   }
   SA.arc_dirty = 1;
}


void arc_refresh_eucl_conf_sync(scene_state_t *ss) {

     if(SA.sync){
       memset(monomeLedBuffer,ARC_BRIGHTNESS_ON,MONOME_MAX_LED_BYTES);
     }else{
       memset(monomeLedBuffer,ARC_BRIGHTNESS_OFF,MONOME_MAX_LED_BYTES);
     }

	   monomeFrameDirty |= (1 << 1);
     monomeFrameDirty |= (1 << 2);
     monomeFrameDirty |= (1 << 3);
     monomeFrameDirty |= (1 << 4);


    SA.arc_dirty = 0;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*


void arc_init_cycles() {
	uint8_t i1;

	arc_preset = 0;
	c.mode = 0;
	c.shape = 0;
	c.friction = 0;
	c.force = 1;

	for(i1=0;i1<4;i1++) {
		c.pos[i1] = 0;
		c.speed[i1] = 0;
		c.mult[i1] = 0;
		c.range[i1] = 64;
		c.div[i1] = 0;
	}

}


void arc_timer_circles(scene_state_t *ss) {
	uint8_t i1;

	if(c.mode) {
		c.speed[0] += add_force[0];

		if(c.mult[1] > -1)
			c.speed[1] = c.speed[0] >> c.mult[1];
		else
			c.speed[1] = c.speed[0] << -c.mult[1];

		if(c.mult[2] > -1)
			c.speed[2] = c.speed[0] >> c.mult[2];
		else
			c.speed[2] = c.speed[0] << -c.mult[2];

		if(c.mult[3] > -1)
			c.speed[3] = c.speed[0] >> c.mult[3];
		else
			c.speed[3] = c.speed[0] << -c.mult[3];
	}

	for(i1=0;i1<4;i1++) {
		if(c.mode == 0)
			c.speed[i1] += add_force[i1];
		c.pos[i1] = (c.pos[i1] + c.speed[i1]) & 0x3fff;
		c.speed[i1] = (c.speed[i1] * (friction)) / 256;
		// c.speed[i1] = (c.speed[i1] * (friction)) >> 8;

		if(c.pos[i1] & div_map[c.div[i1]])
			set_tr(TR1 + i1);
		else
			clr_tr(TR1 + i1);

		if(c.shape)
			dac_set_value(i1, (c.pos[i1] * c.range[i1]) >> 6);
			// dac_set_value(i1, c.pos[i1] << 2);
		else
			dac_set_value(i1, (abs(0x2000 - c.pos[i1]) << 1) * c.range[i1] >> 6);
	}

	monomeFrameDirty++;
}



void arc_script_triggered_circles(scene_state_t *ss, u8 enc){
  arc_metro_triggered_circles(ss);
}

void arc_metro_triggered_circles(scene_state_t *ss) {
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



#define MAX_SPEED 2000

void arc_process_enc_cycles(scene_state_t *ss, u8 n, s8 delta) {
	int16_t i;
	int32_t s;


	switch(mode) {
	case 0:
		// sync
		if(c.mode) {
			if(n == 0) {
				s = c.speed[0] + (delta << c.force);

				if(s > MAX_SPEED)
					s = MAX_SPEED;
				if(s < -MAX_SPEED)
					s = -MAX_SPEED;
				c.speed[0] = s;

				cycle_dir[0] = delta > 0;
				cycle_dir[1] = delta > 0;
				cycle_dir[2] = delta > 0;
				cycle_dir[3] = delta > 0;

				if(c.mult[1] > -1)
					c.speed[1] = c.speed[0] >> c.mult[1];
				else
					c.speed[1] = c.speed[0] << -c.mult[1];

				if(c.mult[2] > -1)
					c.speed[2] = c.speed[0] >> c.mult[2];
				else
					c.speed[2] = c.speed[0] << -c.mult[2];

				if(c.mult[3] > -1)
					c.speed[3] = c.speed[0] >> c.mult[3];
				else
					c.speed[3] = c.speed[0] << -c.mult[3];

			}
			else {
				enc_count[n] += delta;
				i = enc_count[n] >> 4;
				enc_count[n] -= i << 4;

				if(i) {
					i += c.mult[n];
					if(i < -4)
						i = -4;
					else if(i > 1)
						i = 1;
					c.mult[n] = i;

					if(i > -1)
						c.speed[n] = c.speed[0] >> i;
					else
						c.speed[n] = c.speed[0] << -i;
				}
			}
		}
		// free
		else {
			s = c.speed[n] + (delta << c.force);

			if(s > MAX_SPEED)
				s = MAX_SPEED;
			if(s < -MAX_SPEED)
				s = -MAX_SPEED;
			c.speed[n] = s;

			cycle_dir[n] = delta > 0;

			// print_dbg("\r\n");
			// print_dbg_ulong(n);
			// print_dbg(" ");
			// print_dbg_ulong(s);
		}
		break;

	case 1:
		switch(n) {
		// mode
		case 0:
			if(delta > 0)
				c.mode = 1;
			else
				c.mode = 0;
			break;
		// shape
		case 1:
			if(delta > 0)
				c.shape = 1;
			else
				c.shape = 0;
			break;
		// force
		case 2:
			enc_count[n] += delta;
			i = enc_count[n] >> 4;
			enc_count[n] -= i << 4;

			i += c.force;
			if(i < 1)
				i = 1;
			else if(i > 4)
				i = 4;
			c.force = i;
			break;
		// friction
		case 3:
			enc_count[n] += delta;
			i = enc_count[n] >> 4;
			enc_count[n] -= i << 4;

			if(i) {
				i += c.friction;
				if(i < 0)
					i = 0;
				else if(i > 24)
					i = 24;
				c.friction = i;

				friction = friction_map[24 - c.friction] + 192;

				// print_dbg("\r\nfriction: ");
				// print_dbg_ulong(friction);
			}

			break;
		}
		monomeFrameDirty++;
		break;
	case 2:
		// range
		enc_count[n] += delta;
		i = enc_count[n] >> 4;
		enc_count[n] -= i << 4;

		if(i) {
			i += c.range[n];
			if(i < 1)
				i = 1;
			else if(i > 64)
				i = 64;
			c.range[n] = i;
		}
		break;
	case 3:
		// div
		enc_count[n] += delta;
		i = enc_count[n] >> 6;
		enc_count[n] -= i << 6;

		if(i) {
			i += c.div[n];
			if(i < 0)
				i = 0;
			else if(i > 3)
				i = 3;
			c.div[n] = i;
		}
		break;
	default: break;
	}
}



void refresh_cycles(scene_state_t *ss) {
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

void refresh_cycles_config(scene_state_t *ss) {
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

void refresh_cycles_config_range(scene_state_t *ss) {
	uint8_t i1, i2;
	memset(monomeLedBuffer,0,MONOME_MAX_LED_BYTES);

	for(i1=0;i1<4;i1++)
		for(i2=0;i2<c.range[i1];i2++)
			monomeLedBuffer[i1*64 + ((32 + i2) & 0x3f)] = 3;
}

void refresh_cycles_config_div(scene_state_t *ss) {
	uint8_t i1, i2;
	memset(monomeLedBuffer,0,MONOME_MAX_LED_BYTES);

	for(i1=0;i1<4;i1++) {
		for(i2=0; i2<(1<<c.div[i1]); i2++) {
			memset(i1*64 + monomeLedBuffer + i2*(64 >> c.div[i1]), 5, (64 >> (c.div[i1]+1)));
		}
	}
}
*/


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



///// OLD STUFF


//static arc_control_mode_t tt_mode = ARC_LIVE_V, tt_last_mode = ARC_LIVE_V;
//static u8 control_mode_on, tt_script, variable_edit, variable_changed;


/*
static void arc_timer_callback(void *o) {
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
*/
/*
static void arc_timer_restart(scene_state_t *ss) {
    arc_timers[0].on = 1;
    arc_timers[0].ss = ss;
    timer_remove(&arc_timers[0].timer);
    timer_add(&arc_timers[0].timer,SA.arc_metro_ticks, &arc_metro_callback, (void *)&arc_timers[0]);
    SA.arc_dirty = 1;
}


void arc_set_control_mode(u8 control, u8 mode, scene_state_t *ss) {
    if (mode == M_LIVE) {
  //      if (arc_mode == arc_mode_EDIT)
  //          tt_mode = ARC_LIVE_G;
  //      else if (arc_mode == arc_mode_FULL)
  //          tt_mode = ARC_LIVE_GF;
  //      else
            tt_mode = ARC_LIVE_V;
    }
    else if (mode == M_EDIT) {
        tt_mode = ARC_EDIT;
        tt_script = get_edit_script();
    }
    else if (mode == M_PATTERN) {
        tt_mode = ARC_TRACKER;
    }
    else if (mode == M_PRESET_W || mode == M_PRESET_R) {
        tt_mode = ARC_PRESET;
    }
    control_mode_on = control;
    SA.arc_dirty = 1;
}
*/
/*



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

}
*/
