#ifndef __CQT_BLOCK_H__
#define __CQT_BLOCK_H__


/*
#include "Board_LED.h"
#include "Board_Buttons.h"
#include "LPC17xx.h"
#include "PIN_LPC17xx.h"                // Keil::Device:PIN
#include "Open1768_LCD.h"
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "asciiLib.h"
#include "RTE_Device.h"
#include "RTE_Components.h"
#include "LCD_ILI9325.h"
#include "TP_Open1768.h"
*/
#include "math.h"
#include "stdint.h"
#include "fix32_t.h"

#define WSIZE 4096 //troche na sile magic number (patrz len w CQT)
#define CQT_RESULT_SIZE 32 //wielkosc dobrana eksperymentalnie
#define fs 16384.f //sampling frequency; satisfies shannon-nyquist theorem
#define fmin 32.f
#define fmax 8192.f
#define bins 4 //bins / octave

#ifndef M_PI
#define M_PI 3.1415926535
#endif

struct Block{
uint16_t block[3]; //holds 4 samples
};

uint16_t array_get(void * ref ,uint32_t pos);
void array_set(void * ref ,uint32_t pos, uint16_t val);
void sample_set(uint16_t pos, uint16_t var); //automatyczne odwrocenie bitow
uint16_t reverse(uint16_t input);

void FFT(struct fix32_t * result_real, struct fix32_t * result_img);
/*struct fix32_t*/ float * CQT();


#endif
