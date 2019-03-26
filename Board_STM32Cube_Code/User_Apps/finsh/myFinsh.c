#include <rtthread.h>

#include "INC_MBTMC429.h"

#include "led.h"	
#include "gpio.h"	
#include "at24cxx.h"	
#include "tim.h"	
#include "motor_motion.h"
#include "board_debug.h"	

void myAPP_hw_init(void)
{		
	led_thread_init();
	
	dido_hw_init();
	
	at24cxx_hw_init();

//	if(1)
//	{	
//		motor_EN_PIN=4;motor_EN_VALUE=0;
//	}
//	else 
	{
		motor_EN_PIN=at24cxx.readU8(at24c256,ADDR_EN_PIN);
		motor_EN_VALUE=at24cxx.readU8(at24c256,ADDR_EN_VALUE);	
	}
	auto_test_mode=at24cxx.readU8(at24c256,ADDR_TEST_MODE);

	rt_hw_tmc429_init();
	
if(auto_test_mode)						
{	
	OUTPUT5=1;OUTPUT6=1;
	Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR|IFCONF_INV_REF);
}
	
	motor_thread_init();
	
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
	
	rt_kprintf("fixture init ok\r\n>>");

}

INIT_APP_EXPORT(myAPP_hw_init);

FINSH_FUNCTION_EXPORT(rgb_set,control rgb led r or g or b on);
FINSH_FUNCTION_EXPORT(input_read,read the input channel status);
FINSH_FUNCTION_EXPORT(input_reads,read all inputs status);
FINSH_FUNCTION_EXPORT(output_write,write 1 or 0 to output channel status);
FINSH_FUNCTION_EXPORT(board_debug,board test before assembly);

FINSH_FUNCTION_EXPORT(save_data,save_data to page1~255);
FINSH_FUNCTION_EXPORT(read_data,read_data from page1~255);

FINSH_FUNCTION_EXPORT(motor_rotate,motor_rotate)
FINSH_FUNCTION_EXPORT(motor_stop,motor_stop);
FINSH_FUNCTION_EXPORT(motor_move,motor_move);
FINSH_FUNCTION_EXPORT(motor_set,motor_set par);
FINSH_FUNCTION_EXPORT(motor_get,motor_get par);

FINSH_FUNCTION_EXPORT(motor_limitINV,motor_limitINV);
FINSH_FUNCTION_EXPORT(motor_orgINV,motor_orgINV);
FINSH_FUNCTION_EXPORT(motor_goHOME,motor_goHOME);
FINSH_FUNCTION_EXPORT(motor_goLIMIT,motor_goLIMIT);
FINSH_FUNCTION_EXPORT(motor_isRESET,motor_resetOK);

FINSH_FUNCTION_EXPORT(motor_enable,motor enable control);

FINSH_FUNCTION_EXPORT(add_test_mode,for test);

