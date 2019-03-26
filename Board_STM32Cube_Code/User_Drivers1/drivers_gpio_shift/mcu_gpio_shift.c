#include "bsp.h"
//#include "mcu_gpio_shift.h"

#if defined(USING_RTT)

uint8_t shift_in(rt_base_t data_pin, rt_base_t clock_pin, uint8_t bit_order)
{
    uint8_t value = 0;
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
       if (bit_order == LSB_FIRST)      value |= rt_pin_read(data_pin) << i;
       else									            value |= rt_pin_read(data_pin) << (7 - i);
       rt_pin_write(	clock_pin,	LOW		);	rt_hw_us_delay(1);
	   rt_pin_write(	clock_pin,	HIGH	);	rt_hw_us_delay(1);
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
#endif

