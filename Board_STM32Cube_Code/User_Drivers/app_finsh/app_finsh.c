#include <rtthread.h>

#include "bsp.h"

int myAPP_hw_init(void)
{		
	led_thread_init();
		
	dido_hw_init();
	
	at24cxx_hw_init();
	
	if(at24cxx.readU8(at24c256, 16383)==0)
	{
		rt_kprintf("eeprom is inited ; Fixture Type=OS\r\n");
	}
	else if(at24cxx.readU8(at24c256, 16383)==2)
	{
		//B288 QT1
		pca9539a_2.out_data= pca9539a_2.out_data | (1 << (16-10));
		SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
		rt_kprintf("Fixture Type=HEAT\r\n");
	}
	else 
	{
		at24cxx.writeU8(at24c256, 16383, 0);	
		rt_kprintf("eeprom init...\r\n");
	}
	
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
	
	rt_kprintf("microcontroller init ok\r\n>>");
	
	return 0;

}

INIT_APP_EXPORT(myAPP_hw_init);

FINSH_FUNCTION_EXPORT(save_data,save_data to page1~255);
FINSH_FUNCTION_EXPORT(read_data,read_data from page1~255);

FINSH_FUNCTION_EXPORT(CMD_int,read_data from page1~255);
FINSH_FUNCTION_EXPORT(CMD_out,read_data from page1~255);

//FINSH_FUNCTION_EXPORT(motor_rotate,motor_rotate)
//FINSH_FUNCTION_EXPORT(motor_stop,motor_stop);
//FINSH_FUNCTION_EXPORT(motor_move,motor_move);
//FINSH_FUNCTION_EXPORT(motor_set,motor_set par);
//FINSH_FUNCTION_EXPORT(motor_get,motor_get par);

//FINSH_FUNCTION_EXPORT(motor_limitINV,motor_limitINV);
//FINSH_FUNCTION_EXPORT(motor_orgINV,motor_orgINV);
//FINSH_FUNCTION_EXPORT(motor_goHOME,motor_goHOME);
//FINSH_FUNCTION_EXPORT(motor_goLIMIT,motor_goLIMIT);
//FINSH_FUNCTION_EXPORT(motor_isRESET,motor_resetOK);

//FINSH_FUNCTION_EXPORT(motor_disable,motor enable control);

//FINSH_FUNCTION_EXPORT(add_test_mode,for test);
