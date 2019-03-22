
#include "bsp_include.h"	
#include "app_include.h"

uint8_t LED_PIN=PB_2;
//MB1616DEV6
at24cxx_t at24c256=
{
	{0},
	{PC_14,PC_15},
	0xA2,	//1 0 1 0 0 0 1 0
};

int mb1616dev6_hw_init(void)
{		
	led_thread_init();
	dido_gpio_init();
	at24cxx_hw_init();
	
	rt_hw_tmc429_init();
	timer_motorSensorCheck_init();	
	motion_thread_init();

	uart_PressPos_thread_init();	
	
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
	
	rt_kprintf("fixture init ok\n");

  return 0;
}

INIT_APP_EXPORT(mb1616dev6_hw_init);

MSH_CMD_EXPORT(readinput, read the state of the input channel);
MSH_CMD_EXPORT(output, set the output on-off-toggle);
//MSH_CMD_EXPORT(rgb, set the rgb led);
//MSH_CMD_EXPORT(beep, set the beeper);
MSH_CMD_EXPORT(savedata, save data to page 1~200);
MSH_CMD_EXPORT(readdata, read data from page 1~200);
//MSH_CMD_EXPORT(FunctionTest, Check whether the control is normal);
MSH_CMD_EXPORT(reboot, cpu reboot);
MSH_CMD_EXPORT(motor, control motor motion by commands);
MSH_CMD_EXPORT(printdata, auto print position and press);
MSH_CMD_EXPORT(ParamSave, save parameter as set and load it after board restart);



