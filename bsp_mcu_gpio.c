//#include "bsp_include.h"	
//#include "app_include.h"

#include "bsp_mcu_gpio.h"

void pinMode(u8 pin, u8 mode)
{
	rt_pin_mode(pin,mode);
}
void pinSet(u8 pin)
{
  rt_pin_write(pin, PIN_HIGH);
}
void pinReset(u8 pin)
{
  rt_pin_write(pin, PIN_LOW);
}
void pinWrite(u8 pin,u8 val)
{
   if(val == 0)   pinReset(pin);
   else						pinSet(pin);
}
u8 pinRead(u8 pin)
{
    if(rt_pin_read(pin)) return 1;
    else 								 return  0;
}
void pinToggle(u8 pin)
{
	rt_pin_write(pin, 1 - rt_pin_read(pin));
}

uint8_t shift_in(rt_base_t data_pin, rt_base_t clock_pin, uint8_t bit_order)
{
    uint8_t value = 0;
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
       if (bit_order == LSB_FIRST)      value |= rt_pin_read(data_pin) << i;
       else									            value |= rt_pin_read(data_pin) << (7 - i);
       rt_pin_write(	clock_pin,	LOW		);	delay_us(1);
			 rt_pin_write(	clock_pin,	HIGH	);	delay_us(1);
    }
    return value;
}

void shift_out(rt_base_t data_pin, rt_base_t clock_pin, uint8_t bit_order, uint8_t val)
{
	int i;
	for (i = 0; i < 8; i++)
	{
			if (bit_order == LSB_FIRST)
					rt_pin_write(	data_pin,	(!!(val & (1 << i))));
			else
					rt_pin_write(	data_pin,(!!(val & (1 << (7 - i))))	);

			rt_pin_write(	clock_pin,	HIGH	);
			rt_pin_write(	clock_pin,	LOW		);
	}
}

