#include "turtle.h"

#define min(X, Y) ((X) < (Y) ? (X) : (Y))
#define max(X, Y) ((X) > (Y) ? (X) : (Y))

void turtle_init(scene_turtle_t *st) {
    scene_turtle_t t = { 
            .fence = { .x1 = 0, .y1 = 0, .x2 = 3, .y2 = 63},
            .mode = TURTLE_BUMP,
            .heading = 180,
            .speed = 100,
            .stepped = false,
            .script_number = TEMP_SCRIPT 
            };
    memcpy(st, &t, sizeof(t));
    turtle_set_x(st, 0);
    turtle_set_y(st, 0);
    st->last = st->position;
}

typedef struct {
    QT x1, y1, x2, y2;
} Q_fence_t;

static inline Q_fence_t normalize_fence(turtle_fence_t in) {
    Q_fence_t out;

    // a Q fence
    // fence values are inclusive so 0 to 0 is L:1
    out.x1 = TO_Q(in.x1);
    out.x2 = TO_Q((in.x2 + 1));
    out.y1 = TO_Q(in.y1);
    out.y2 = TO_Q((in.y2 + 1));
    return out;
}

void turtle_check_step(scene_turtle_t *t) {
    turtle_position_t here;
    turtle_resolve_position(t, &t->position, &here);
    if (here.x != t->last.x || here.y != t->last.y) {
        t->last = here;
        t->stepped = true;
    }
}

void turtle_normalize_position(scene_turtle_t *t, turtle_position_t *tp, 
        turtle_mode_t mode) {
    Q_fence_t f = normalize_fence(t->fence);

    QT fxl = f.x2 - f.x1;
    QT fyl = f.y2 - f.y1;
  
    // stay put if we're fenced in
    if (fxl == Q_1 && fyl == Q_1)
        mode = TURTLE_BUMP;

    if (mode == TURTLE_WRAP) {
        if (fxl > Q_1 && tp->x < f.x1)
            tp->x = f.x2 + ((tp->x - f.x1) % fxl);
        else if (fxl > Q_1 && tp->x > f.x2)
            tp->x = f.x1 + ((tp->x - f.x1) % fxl);
        
        if (fyl > Q_1 && tp->y < f.y1)
            tp->y = f.y2 + ((tp->y - f.y1) % fyl);
        else if (fyl > Q_1 && tp->y > f.y2)
            tp->y = f.y1 + ((tp->y - f.y1) % fyl);
    }
    else if (mode == TURTLE_BOUNCE) {
        // pretty sure you can do this with a % but I couldn't get it right
        // what with the edge bounceback effectively changing the length
        // so here's a crappy while() loop wavefolder. --burnsauce / sliderule
        turtle_position_t last, here;
        turtle_resolve_position(t, &t->position, &last);
        while ( fxl > Q_1 // if we're penned in, don't move at all
                && (tp->x > f.x2 || tp->x < f.x1)) {
            if (tp->x > f.x2) {                                // right fence
                if (t->stepping)
                    turtle_set_heading(t, 360 - t->heading);
                //  right extent minus how far past we are
                tp->x = f.x2 - (tp->x - f.x2);
            }
            else if (tp->x < f.x1 ) {                           // left fence
                if (t->stepping)
                    turtle_set_heading(t, 360 - t->heading);
                //  left extent minus how far past we are
                tp->x = f.x1 + (f.x1 - tp->x);
            } 
            turtle_resolve_position(t, &t->position, &here);
            if (here.x == last.x)
                break;
            last = here;
        }
        while (fyl > Q_1 && (tp->y > f.y2 || tp->y < f.y1)) {
            if (tp->y >= f.y2) {                                // top fence
                if (t->stepping)
                    turtle_set_heading(t, 180 - t->heading);
                tp->y = f.y2 - (tp->y - f.y2);
            }
            else if (tp->y < f.y1) {                            // bottom fence
                if (t->stepping)
                    turtle_set_heading(t, 180 - t->heading);
                tp->y = f.y1 + (f.y1 - tp->y);
            } 
            turtle_resolve_position(t, &t->position, &here);
            if (here.y == last.y)
                break;
            last = here;
        }
        if (tp->x == f.x2)
            tp->x -= 1;
        if (tp->y == f.y2)
            tp->y -= 1;

    }
    // either mode is TURTLE_BUMP or something above is broken
    // both cases call for constraining the turtle to the fence
    tp->x = min(f.x2 - 1, max(f.x1, tp->x));
    tp->y = min(f.y2 - 1, max(f.y1, tp->y));
    turtle_check_step(t);
}

// Produce Q0 positions in dst
void turtle_resolve_position(scene_turtle_t *t, turtle_position_t *src,
                             turtle_position_t *dst) {
    dst->x = TO_I(src->x);
    dst->y = TO_I(src->y);
}

uint8_t turtle_get_x(scene_turtle_t *st) {
    turtle_position_t t;
    turtle_resolve_position(st, &st->position, &t);
    return t.x;
}

void turtle_set_x(scene_turtle_t *st, int16_t x) {
    st->position.x = TO_Q(x) + Q_05; // standing in the middle of the cell
    turtle_normalize_position(st, &st->position, TURTLE_BUMP);
}

uint8_t turtle_get_y(scene_turtle_t *st) {
    turtle_position_t t;
    turtle_resolve_position(st, &st->position, &t);
    return t.y;
}

void turtle_set_y(scene_turtle_t *st, int16_t y) {
    st->position.y = TO_Q(y) + Q_05;
    turtle_normalize_position(st, &st->position, TURTLE_BUMP);
}

void turtle_move(scene_turtle_t *st, int16_t x, int16_t y) {
    st->position.y += TO_Q(y);
    st->position.x += TO_Q(x);
    turtle_normalize_position(st, &st->position, st->mode);
}

/// http://www.coranac.com/2009/07/sines/
/// A sine approximation via a third-order approx.
/// @param x    Angle (with 2^15 units/circle)
/// @return     Sine value (Q12)
static inline int32_t sin(int32_t x)
{
    // S(x) = x * ( (3<<p) - (x*x>>r) ) >> s
    // n : Q-pos for quarter circle             13
    // A : Q-pos for output                     12
    // p : Q-pos for parentheses intermediate   15
    // r = 2n-p                                 11
    // s = A-1-p-n                              17

    int qN = 13, qA= 12, qP= 15, qR= 2*qN-qP, qS= qN+qP+1-qA;

    x= x<<(30-qN);          // shift to full s32 range (Q13->Q30)

    if( (x^(x<<1)) < 0)     // test for quadrant 1 or 2
        x= (1<<31) - x;

    x= x>>(30-qN);

    return x * ( (3<<qP) - (x*x>>qR) ) >> qS;
}

void turtle_step(scene_turtle_t *st) {
    // TODO watch out, it's a doozie ;)
    QT dx = 0, dy = 0;
    QT h1 = st->heading, h2 = st->heading;

    h1 = ((h1 % 360) << 15) / 360;
    h2 = (((h2 + 360 - 90) % 360) << 15) / 360;

    int32_t  dx_d_Q12 = (st->speed * sin(h1)) / 100;
    int32_t  dy_d_Q12 = (st->speed * sin(h2)) / 100; 
    
    
    if (dx_d_Q12 < 0)
        // delta x = round(v *sin(heading))
        dx = ((dx_d_Q12 >> (11 - Q_BITS)) - 1) >> 1;
    else
        dx = ((dx_d_Q12 >> (11 - Q_BITS)) + 1) >> 1;
    if (dy_d_Q12 < 0)
        dy = ((dy_d_Q12 >> (11 - Q_BITS)) - 1) >> 1;
    else
        dy = ((dy_d_Q12 >> (11 - Q_BITS)) + 1) >> 1;
   
    
    st->position.x += dx;
    st->position.y += dy;
    st->stepping = true;
    turtle_normalize_position(st, &st->position, st->mode);
    st->stepping = false;
}

/*
void turtle_set_home(scene_turtle_t *st, int16_t x, int16_t y) {
    if (x > 3)
        x = 3;
    else if (x < 0)
        x = 0;
    if (y > 63)
        y = 63;
    else if (y < 0)
        y = 0;
    st->home.x = TO_Q(x);
    st->home.y = TO_Q(y);
    turtle_normalize_position(st, &st->home, TURTLE_BUMP);
}

uint8_t turtle_get_home_x(scene_turtle_t *st) {
    turtle_position_t t;
    turtle_resolve_position(st, &st->home, &t);
    return t.x;
}

void turtle_set_home_x(scene_turtle_t *st, int16_t x) {
    if (x > 3)
        x = 3;
    else if (x < 0)
        x = 0;
    st->home.x = TO_Q(x);
    turtle_normalize_position(st, &st->home, TURTLE_BUMP);
}

uint8_t  turtle_get_home_y(scene_turtle_t *st) {
    turtle_position_t t;
    turtle_resolve_position(st, &st->home, &t);
    return t.y;
}

void turtle_set_home_y(scene_turtle_t *st, int16_t y) {
    if (y > 63)
        y = 63;
    else if (y < 0)
        y = 0;
    turtle_position_t h = { .x = st->home.x, .y = y << Q_BITS };
    st->home = h;
    turtle_normalize_position(st, &st->home, TURTLE_BUMP);
}
*/

inline void turtle_correct_fence(scene_turtle_t *st) {
    int16_t t;
    st->fence.x1 = min(3, max(0, st->fence.x1));
    st->fence.x2 = min(3, max(0, st->fence.x2));
    st->fence.y1 = min(63, max(0, st->fence.y1));
    st->fence.y2 = min(63, max(0, st->fence.y2));

    if (st->fence.x1 > st->fence.x2) {
        t = st->fence.x2;
        st->fence.x2 = st->fence.x1;
        st->fence.x1 = t;
    }
    if (st->fence.y1 > st->fence.y2) {
        t = st->fence.y2;
        st->fence.y2 = st->fence.y1;
        st->fence.y1 = t;
    }
    turtle_normalize_position(st, &st->position, TURTLE_BUMP);
}

void turtle_set_fence(scene_turtle_t *st, int16_t x1, int16_t y1, 
                                          int16_t x2, int16_t y2) {

    st->fence.x1 = min(3, max(0, x1));
    st->fence.x2 = min(3, max(0, x2));
    st->fence.y1 = min(63, max(0, y1));
    st->fence.y2 = min(63, max(0, y2));
    turtle_correct_fence(st);
}

turtle_mode_t turtle_get_mode(scene_turtle_t *st) {
    return st->mode;
}

void turtle_set_mode(scene_turtle_t *st, turtle_mode_t m) {
    if (m != TURTLE_WRAP && m != TURTLE_BUMP && m != TURTLE_BOUNCE)
        m = TURTLE_BUMP;
    st->mode = m;
    turtle_normalize_position(st, &st->position, m);
}

uint16_t turtle_get_heading(scene_turtle_t *st) {
    return st->heading;   
}

void turtle_set_heading(scene_turtle_t *st, int16_t h) {
    while(h < 0)
        h += 360;
    st->heading = h % 360;
}

int16_t turtle_get_speed(scene_turtle_t *st) {
    return st->speed;
}

void turtle_set_speed(scene_turtle_t *st, int16_t v) {
    st->speed = v;
}

void turtle_set_script(scene_turtle_t *st, script_number_t sn) {
    if (sn >= METRO_SCRIPT)
        st->script_number = TEMP_SCRIPT;
    else
        st->script_number = sn;
    st->stepped = false;
}

script_number_t turtle_get_script(scene_turtle_t *st) {
    return st->script_number;
}

#undef min
#undef max
