#ifndef _TURTLE_H_
#define _TURTLE_H_
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// In this code, we use signed fixed-point maths.  This means that the int16_t
// is used to store a signed number with both an integer and a fraction part.
//
// Q_BITS represents how many are used for the fractional component.  Because
// we work with signed integers, 1 bit facilitates the sign component.
// Therefore, INT_BITS = 16 - Q_BITS - 1
//
// The main thing we do in fixed-point is keep a fractional index into the
// pattern data, so it will be our main reference point.
//
// 63 is 6 bits, so Q_BITS = 16 - 6 - 1 = 9
// The notation for this system is Q6.9

#define Q_BITS 9
#define Q_1 (1 << Q_BITS)                           // 1.0
#define Q_05 (1 << (Q_BITS - 1))                    // 0.5
#define Q_ROUND(X) ((((X >> (Q_BITS - 1)) + 1) >> 1) << Q_BITS)   // (int)(X + 0.5)
#define QT int32_t
#define TO_Q(X) (X << Q_BITS) 
#define TO_I(X) ((X >> Q_BITS) & 0xFFFF)

// Can't include state.h first, so put script_number_t here
// really should be in a different header file? 

typedef enum {
    TT_SCRIPT_1=0,
    TT_SCRIPT_2,
    TT_SCRIPT_3,
    TT_SCRIPT_4,
    TT_SCRIPT_5,
    TT_SCRIPT_6,
    TT_SCRIPT_7,
    TT_SCRIPT_8,
    METRO_SCRIPT,
    INIT_SCRIPT,
    TEMP_SCRIPT
} script_number_t;

typedef struct {
    int32_t x;         // higher resolution to permit fixed-point math
    int32_t y;
} turtle_position_t;

typedef struct {
    uint8_t x1;
    uint8_t y1;
    uint8_t x2;
    uint8_t y2;
} turtle_fence_t;

typedef enum {
    TURTLE_WRAP,
    TURTLE_BUMP,
    TURTLE_BOUNCE
} turtle_mode_t;

typedef struct {
    turtle_position_t   position;
    turtle_position_t   last;
    turtle_fence_t      fence;
    turtle_mode_t       mode;
    uint16_t            heading;
    int16_t             speed;
    script_number_t     script_number;
    bool                stepping;
    bool                stepped;
} scene_turtle_t;

/*
void     scene_set_turtle(scene_state_t*, scene_turtle_t*);
scene_turtle_t*
         scene_get_turtle(scene_state_t*);
*/
void     turtle_init(scene_turtle_t*);
void     turtle_normalize_position(scene_turtle_t*, turtle_position_t*,
                                   turtle_mode_t);
void     turtle_resolve_position(scene_turtle_t*, turtle_position_t*,
                                                  turtle_position_t*);
uint8_t  turtle_get_x(scene_turtle_t*);
void     turtle_set_x(scene_turtle_t*, int16_t);
uint8_t  turtle_get_y(scene_turtle_t*);
void     turtle_set_y(scene_turtle_t*, int16_t);
void     turtle_move(scene_turtle_t*, int16_t, int16_t);
void     turtle_step(scene_turtle_t*);
/*
void     turtle_set_home(scene_turtle_t*, int16_t, int16_t);
uint8_t  turtle_get_home_x(scene_turtle_t*);
uint8_t  turtle_get_home_y(scene_turtle_t*);
void     turtle_set_home_x(scene_turtle_t*, int16_t);
void     turtle_set_home_y(scene_turtle_t*, int16_t);
*/
turtle_fence_t *
         turtle_get_fence(scene_turtle_t*);
void     turtle_correct_fence(scene_turtle_t*);
void     turtle_set_fence(scene_turtle_t*, int16_t, int16_t, int16_t, int16_t);
turtle_mode_t
         turtle_get_mode(scene_turtle_t*);
void     turtle_set_mode(scene_turtle_t*, turtle_mode_t);
uint16_t turtle_get_heading(scene_turtle_t*);
void     turtle_set_heading(scene_turtle_t*, int16_t);
int16_t  turtle_get_speed(scene_turtle_t*);
void     turtle_set_speed(scene_turtle_t*, int16_t);
script_number_t
         turtle_get_script(scene_turtle_t*);
void     turtle_set_script(scene_turtle_t*, script_number_t);
void     turtle_check_step(scene_turtle_t*);

#endif
