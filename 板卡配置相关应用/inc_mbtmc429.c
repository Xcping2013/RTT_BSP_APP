
#include "bsp_include.h"	
#include "app_include.h"

uint8_t LED_PIN=PB_2;

at24cxx_t at24c256=
{
	{0},
	{PB_4,PB_3},	//SDA SCL
	0xA0,
};

int mbtmc429_hw_init(void)
{		
	led_thread_init();
	dido_gpio_init();
	at24cxx_hw_init();
	
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
	
	rt_kprintf("fixture init ok\n");

  return 0;
}

INIT_APP_EXPORT(mbtmc429_hw_init);

MSH_CMD_EXPORT(readinput, read the state of the input channel);
MSH_CMD_EXPORT(output, set the output on-off-toggle);
MSH_CMD_EXPORT(rgb, set the rgb led);
MSH_CMD_EXPORT(beep, set the beeper);
MSH_CMD_EXPORT(savedata, save data to page 1~200);
MSH_CMD_EXPORT(readdata, read data from page 1~200);
MSH_CMD_EXPORT(reboot, cpu reboot);






