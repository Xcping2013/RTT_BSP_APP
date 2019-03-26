#ifndef __MCU_GPIO_H
#define __MCU_GPIO_H

#define GPIO_Mode_Out_PP					PIN_MODE_OUTPUT
#define GPIO_Mode_IN_FLOATING			PIN_MODE_INPUT
#define GPIO_Mode_IPU							PIN_MODE_INPUT_PULLUP
#define GPIO_Mode_IPD							PIN_MODE_INPUT_PULLDOWN
#define GPIO_Mode_Out_OD				  PIN_MODE_OUTPUT_OD

void pinMode(uint8_t pin, uint8_t mode);
void pinSet(uint8_t  pin);
void pinReset(uint8_t pin);
void pinWrite(uint8_t pin,uint8_t val);
uint8_t   pinRead(uint8_t pin);
//void pinToggle(uint8_t pin);

#endif


