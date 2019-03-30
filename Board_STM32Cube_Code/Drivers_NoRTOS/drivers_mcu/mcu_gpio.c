#include "bsp.h"

void pinMode(uint8_t pin, uint8_t mode)
{
	rt_pin_mode(pin,mode);
}
void pinSet(uint8_t pin)
{
  rt_pin_write(pin, PIN_HIGH);
}
void pinReset(uint8_t pin)
{
  rt_pin_write(pin, PIN_LOW);
}
void pinWrite(uint8_t pin,uint8_t val)
{
   if(val == 0)   pinReset(pin);
   else						pinSet(pin);
}
uint8_t pinRead(uint8_t pin)
{
    if(rt_pin_read(pin)) return  1;
    else 								 return  0;
}

