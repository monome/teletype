#include "ops/arc_ops.h"

#include "helpers.h"
#include "teletype.h"
#include "teletype_io.h"

#define SA ss->arc
const int pattern_lengths[3] = { 8 , 16 , 32 };

#define CLIP(var, min, max) \
    if (var < (min)) { \
      var = (min); \
    } else if (var > 254) { \
      var = 0; \
    } else if (var > (max)) { \
      var = (max); \
    }


// clang-format off

static void op_ARC_METRO_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_ARC_METRO_set  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_ARC_SYNC_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_ARC_SYNC_set  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_ARC_RST_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_ARC_LEN_set  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_ARC_LEN_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_ARC_PHASE_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_ARC_PHASE_set  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);




const tele_op_t op_ARC_METRO  = MAKE_GET_SET_OP(ARC.METRO, op_ARC_METRO_get, op_ARC_METRO_set, 0, true);
const tele_op_t op_ARC_SYNC   = MAKE_GET_SET_OP(ARC.SYNC, op_ARC_SYNC_get, op_ARC_SYNC_set, 0, true);
const tele_op_t op_ARC_RST    = MAKE_GET_OP(ARC.RST, op_ARC_RST_get, 0, true);
const tele_op_t op_ARC_LEN    = MAKE_GET_SET_OP(ARC.LEN, op_ARC_LEN_get, op_ARC_LEN_set, 1,  true);
const tele_op_t op_ARC_PHASE  = MAKE_GET_SET_OP(ARC.PHASE, op_ARC_PHASE_get, op_ARC_PHASE_set, 1, true);


// clang-format on

static void op_ARC_METRO_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
   int i = SA.metro;
    cs_push(cs, i);
}
static void op_ARC_METRO_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
  int i = cs_pop(cs);
  if(i==0)  SA.metro = false;
  if(i==1)  SA.metro = true;
}

static void op_ARC_SYNC_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
   int i = SA.sync;
    cs_push(cs, i);
}
static void op_ARC_SYNC_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
  int i = cs_pop(cs);
  if(i==0)  SA.sync = false;
  if(i==1)  SA.sync = true;
}


static void op_ARC_RST_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
   SA.reset = true;
}


static void op_ARC_LEN_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {

  int enc = cs_pop(cs);
  if(enc<0 || enc>3)return;

  cs_push(cs, pattern_lengths[SA.encoder[enc].pattern_index]);

}

static void op_ARC_LEN_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
  int enc = cs_pop(cs);
  int i = cs_pop(cs);

  if(enc<0 || enc>3)return;

  switch(i){
    case 8:
      SA.encoder[enc].pattern_index = 0;
      break;
    case 16:
      SA.encoder[enc].pattern_index = 1;
      break;
    case 32:
      SA.encoder[enc].pattern_index = 2;
      break;
    default:
      SA.encoder[enc].pattern_index = 0;
      break;
    }

}

static void op_ARC_PHASE_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
   int enc = cs_pop(cs);
   if(enc<0 || enc>3)return;

   cs_push(cs, SA.encoder[enc].phase_offset);

}

static void op_ARC_PHASE_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
  int enc = cs_pop(cs);
  int phase = cs_pop(cs);

  if(enc<0 || enc>3)return;

	CLIP( phase , 0 , pattern_lengths[SA.encoder[enc].pattern_index]-1);

  SA.encoder[enc].phase_offset = phase;

}
