#include "bsp.h"

int bsp_Init(void)
{	
//	rt_hw_leds_init();	//outputs
//	
//	led_thread_init();	//sys indicate
//	timer_led_init();	//led_blink | led ≈‹¬Ìµ∆
	
//	bsp_InitKey();
//	key_thread_init();
	
	uarts_thread_init();
//	timer_KeyScan_init();
	
//	bsp_InitTimer();	
//	
//	bsp_InitUart();	
	return 0;
}

INIT_APP_EXPORT(bsp_Init);

FINSH_FUNCTION_EXPORT(input_read,read the input channel status);
FINSH_FUNCTION_EXPORT(input_reads,read all inputs status);

FINSH_FUNCTION_EXPORT_ALIAS(led_on_off,output_write,write 1 or 0 to output channel status);

FINSH_FUNCTION_EXPORT(led_blink,...);
FINSH_FUNCTION_EXPORT(led_running,...);

FINSH_FUNCTION_EXPORT(uart2_send,...);
