
#include "bsp_include.h"	
#include "app_include.h"

int myAPP_hw_init(void)
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

INIT_APP_EXPORT(myAPP_hw_init);

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

//MSH_CMD_EXPORT(memcheck, check memory data);
//MSH_CMD_EXPORT(memtrace, dump memory trace information);
//MSH_CMD_EXPORT(version, show RT-Thread version information);
//MSH_CMD_EXPORT(list_thread, list thread);
//MSH_CMD_EXPORT(list_sem, list semaphore in system);
//MSH_CMD_EXPORT(list_event, list event in system);
//MSH_CMD_EXPORT(list_mutex, list mutex in system);
//MSH_CMD_EXPORT(list_mailbox, list mail box in system);
//MSH_CMD_EXPORT(list_msgqueue, list message queue in system);
//MSH_CMD_EXPORT(list_memheap, list memory heap in system);
//MSH_CMD_EXPORT(list_mempool, list memory pool in system);
//MSH_CMD_EXPORT(list_timer, list timer in system);
//MSH_CMD_EXPORT(list_device, list device in system);
