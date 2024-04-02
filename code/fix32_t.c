#include "fix32_t.h"



/**
 * @brief Konwersja fix32_t -> float
 * 
 * @param v1 fix32_t
 * @return float 
 */
float fix32_t_float(struct fix32_t v1)
{
    ;
    int32_t temp =    v1.value;
    float value = (float)temp / (1<<__FIX32_PRECISION);
    return value ;
}

/**
 * @brief Konwersja float -> fix32_t
 * 
 * @param var float
 * @return struct fix32_t 
 */
struct fix32_t float_fix32_t(float var)
{
    
uint32_t buffer = var * (float)((1U<<__FIX32_PRECISION));
struct fix32_t result = { buffer}; 
return result;
}



struct fix32_t fix32_t_sin(float x)
{
int16_t count = x / (2.*M_PI); //zaokraglane w kierunku zera
x -= 2*M_PI * (float)count; //sprowadzam do zakresu [-2pi, 2pi]
int16_t flag = 0;

if(x < 0)
{
    x += 2.*M_PI; //korzystam z okresowosci, by [0, 2pi]
}

if(x > M_PI) //tne na pol
{
    x -= M_PI;
    flag = !flag;
}


struct fix32_t arg = float_fix32_t(x);



return fix32_t_mult(fix32_t_div(


    fix32_t_mult(
        fix32_t_mult(
            int32_t_fix32_t(16), 
            arg
        ),   
        fix32_t_sub(__FIX32_PI, 
        arg)          
        )   
    ,
    fix32_t_sub(

        fix32_t_mult(int32_t_fix32_t(5),fix32_t_mult(__FIX32_PI,__FIX32_PI)),

        fix32_t_mult(int32_t_fix32_t(4),fix32_t_mult(arg, fix32_t_sub(__FIX32_PI, arg)))
    )    
), int32_t_fix32_t( flag ? -1 : 1 ));
//return 16.* x * (M_PI-x) / (5. * M_PI * M_PI - 4.*x *(M_PI - x)) * (flag ? -1. : 1.);
}


struct fix32_t fix32_t_cos(float x)
{
    return fix32_t_sin(x+M_PI/2.);
}



struct fix32_t fix32_t_sqrt(struct fix32_t v)
{
if(v.value < 0) v.value *= -1;

struct fix32_t prev = int32_t_fix32_t(1);
struct fix32_t new = int32_t_fix32_t(1);
do{
prev = new;

new = fix32_t_div(fix32_t_add(prev, fix32_t_div(v,prev)), int32_t_fix32_t(2)  );

}while(prev.value - new.value > 1);


return new;
}

