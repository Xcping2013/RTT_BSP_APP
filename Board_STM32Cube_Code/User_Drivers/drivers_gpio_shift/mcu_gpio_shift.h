#ifndef __MCU_GPIO_SHIFT_H
#define __MCU_GPIO_SHIFT_H	

#include <rtthread.h>

void        shift_out(rt_base_t data_pin, rt_base_t clock_pin, uint8_t bit_order, uint8_t val);
uint8_t     shift_in(rt_base_t data_pin, rt_base_t clock_pin, uint8_t bit_order);

#endif

