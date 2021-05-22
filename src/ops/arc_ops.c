#include "ops/arc_ops.h"

#include "helpers.h"
#include "teletype.h"
#include "teletype_io.h"
//#include "arc.h"

#define SA ss->arc

// clang-format off

static void op_ARC_METRO_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_ARC_METRO_set  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);
static void op_ARC_TRIG_get  (const void *data, scene_state_t *ss, exec_state_t *es,  command_state_t *cs);

const tele_op_t op_ARC_METRO  = MAKE_GET_SET_OP(ARC.METRO, op_ARC_METRO_get, op_ARC_METRO_set, 0, true);
const tele_op_t op_ARC_TRIG  = MAKE_GET_OP(ARC.TRIG, op_ARC_METRO_get, 1, true);

// clang-format on

static void op_ARC_METRO_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
   s16 i = SA.metro;
    cs_push(cs, i);
}


static void op_ARC_METRO_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
  u8 i = cs_pop(cs);
  if(i==0)  SA.metro = false;
  if(i==1)  SA.metro = true;
}


static void op_ARC_TRIG_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
   u8 enc = cs_pop(cs);
   cs_push(cs, enc);

}
