#ifndef __DELAY_H
#define __DELAY_H 

#define STD_35
//#define Cube_HAL
#if defined(STD_35)
	#include "stm32f10x.h"
#else
	#include "stm32f1xx_hal.h"
#endif

#include "stdint.h"

typedef uint8_t  	u8;
typedef uint16_t 	u16;
typedef uint32_t  u32;

typedef int8_t  	s8;
typedef int16_t 	s16;
typedef int32_t  	s32;

void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif
