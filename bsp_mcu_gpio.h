#ifndef __BSP_MCU_GPIO_H
#define __BSP_MCU_GPIO_H

#include "bsp_defines.h"

#define GPIO_Mode_Out_PP					PIN_MODE_OUTPUT
#define GPIO_Mode_IN_FLOATING			PIN_MODE_INPUT
#define GPIO_Mode_IPU							PIN_MODE_INPUT_PULLUP
#define GPIO_Mode_IPD							PIN_MODE_INPUT_PULLDOWN
#define GPIO_Mode_Out_OD				  PIN_MODE_OUTPUT_OD


void pinMode(u8 pin, u8 mode);
void pinSet(u8  pin);
void pinReset(u8 pin);
void pinWrite(u8 pin,u8 val);
u8   pinRead(u8 pin);
void pinToggle(u8 pin);

void      shift_out(rt_base_t data_pin, rt_base_t clock_pin, uint8_t bit_order, uint8_t val);
uint8_t   shift_in(rt_base_t data_pin, rt_base_t clock_pin, uint8_t bit_order);

#endif


