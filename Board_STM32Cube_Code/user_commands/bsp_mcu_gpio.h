#ifndef __BSP_MCU_GPIO_H
#define __BSP_MCU_GPIO_H

#include "bsp_defines.h"


	#ifndef	USING_RTT

		#define PIN_LOW                 0x00
		#define PIN_HIGH                0x01

		#define PIN_MODE_OUTPUT         0x00
		#define PIN_MODE_INPUT          0x01
		#define PIN_MODE_INPUT_PULLUP   0x02
		#define PIN_MODE_INPUT_PULLDOWN 0x03
		#define PIN_MODE_OUTPUT_OD      0x04

	#endif

void pinMode(uint8_t pin, uint8_t mode);
void pinSet(uint8_t  pin);
void pinReset(uint8_t pin);
void pinWrite(uint8_t pin,uint8_t val);
uint8_t   pinRead(uint8_t pin);
void pinToggle(uint8_t pin);

void      shift_out(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order, uint8_t val);
uint8_t   shift_in(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order);

#endif


