#ifndef __FIX32_T__
#define __FIX32_T__


#include "stdint.h"
#include "math.h"


#ifndef M_PI
#define M_PI 3.1415926535
#endif



#ifndef __FIX32_PI
    #define __FIX32_PI float_fix32_t(M_PI)
#endif

#define __FIX32_PRECISION 8U

struct fix32_t{
int32_t value;
};


static inline struct fix32_t int32_t_fix32_t(int32_t var)
{
    uint32_t buffer = var * ((1U<<__FIX32_PRECISION));
    struct fix32_t result = { buffer}; 
    return result;
}

struct fix32_t float_fix32_t(float var);

float fix32_t_float(struct fix32_t v1);

static inline struct fix32_t fix32_t_mult(struct fix32_t v1, struct fix32_t v2)
{
    uint32_t temp = (v1.value * v2.value)>>__FIX32_PRECISION;
    struct fix32_t result = {temp};
    return result;
}

static inline struct fix32_t fix32_t_div(struct fix32_t v1, struct fix32_t v2)
{
    struct fix32_t result = {(v1.value<<__FIX32_PRECISION) / v2.value};
    return result;
}

static inline struct fix32_t fix32_t_add(struct fix32_t v1, struct fix32_t v2)
{
    struct fix32_t result = {v1.value + v2.value}; 
    return result;
}

static inline struct fix32_t fix32_t_sub(struct fix32_t v1, struct fix32_t v2)
{
    struct fix32_t result = {v1.value - v2.value}; 
    return result;
}


struct fix32_t fix32_t_sin(float x);

struct fix32_t fix32_t_cos(float x);

struct fix32_t fix32_t_sqrt(struct fix32_t v);




#endif