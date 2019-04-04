
#include "bsp_include.h"	
#include "app_include.h"

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

#if defined(USING_INC_MBTMC429) 	//#define RT_USING_UART3

uint8_t LED_PIN=PD_7;

at24cxx_t at24c256=
{
	{0},
	{PB_4,PB_3},	//SDA SCL
	0xA0,
};

int mbtmc429_hw_init(void)
{		
	SysRunLed_thread_init();
	
	dido_hw_init();
	at24cxx_hw_init();
	
	tmc429_hw_init();
	
	MotorSensorCheck_timer_init();	
	MotorLimitCheck_thread_init();

	MX_USART3_UART_Init();
	//uart_stream_thread_init();	
	
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
	
	rt_kprintf("\nfirmware ver1.03 build at %s %s\n\n", __TIME__, __DATE__);

  return 0;
}

INIT_APP_EXPORT(mbtmc429_hw_init);

MSH_CMD_EXPORT(readinput, read the state of the input channel);
MSH_CMD_EXPORT(output, set the output on-off-toggle);//MSH_CMD_EXPORT(memcheck, check memory data);
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
//MSH_CMD_EXPORT(rgb, set the rgb led);
//MSH_CMD_EXPORT(beep, set the beeper);
MSH_CMD_EXPORT(savedata, save data to page 1~200);
MSH_CMD_EXPORT(readdata, read data from page 1~200);
//MSH_CMD_EXPORT(FunctionTest, Check whether the control is normal);
MSH_CMD_EXPORT(reboot, cpu reboot);
MSH_CMD_EXPORT(motor, control motor motion by commands);
MSH_CMD_EXPORT(printdata, auto print position and press);
MSH_CMD_EXPORT(ParamSave, save parameter as set and load it after board restart);

static void hw_in_out_debug(void)
{
	u8 ret_ok=1;
													if(INPUT1==1) 	{rt_kprintf("input1 or rgb_5v ............err\n");ret_ok=0;}
	RGB_R=1;RGB_G=0;RGB_B=0;delay_ms(10);
													if(INPUT2==1) 	{rt_kprintf("input2 or rgb_r .............err\n"); ret_ok=0;}
													if(INPUT3==0) 	{rt_kprintf("input3 or rgb_g .............err\n"); ret_ok=0;}
													if(INPUT4==0) 	{rt_kprintf("input4 or rgb_b .............err\n"); ret_ok=0;}
	RGB_R=0;RGB_G=1;RGB_B=0;delay_ms(10);										        
													if(INPUT2==0) 	{rt_kprintf("input2 or rgb_r .............err\n"); ret_ok=0;}
													if(INPUT3==1) 	{rt_kprintf("input3 or rgb_g .............err\n"); ret_ok=0;}
													if(INPUT4==0) 	{rt_kprintf("input4 or rgb_b .............err\n"); ret_ok=0;}
	RGB_R=0;RGB_G=0;RGB_B=1;delay_ms(10);												     
													if(INPUT2==0) 	{rt_kprintf("input2 or rgb_r .............err\n"); ret_ok=0;}
													if(INPUT3==0) 	{rt_kprintf("input3 or rgb_g .............err\n"); ret_ok=0;}
													if(INPUT4==1) 	{rt_kprintf("input4 or rgb_b .............err\n"); ret_ok=0;}	
														
	OUTPUT1=1;delay_ms(10);	if(INPUT5==1) 	{rt_kprintf("input5 or output1 ...........err\n");		ret_ok=0;}
	OUTPUT1=0;delay_ms(10);	if(INPUT5==0) 	{rt_kprintf("input5 or output1 ...........err\n");		ret_ok=0;}
	                                                                  
	OUTPUT2=1;delay_ms(10);	if(INPUT6==1) 	{rt_kprintf("input6 or output2 ...........err\n");   ret_ok=0;}
	OUTPUT2=0;delay_ms(10);	if(INPUT6==0) 	{rt_kprintf("input6 or output2 ...........err\n");   ret_ok=0;}
	                                                                   
	OUTPUT3=1;delay_ms(10);	if(INPUT7==1) 	{rt_kprintf("input7 or output3 ...........err\n");   ret_ok=0;}
	OUTPUT3=0;delay_ms(10);	if(INPUT7==0) 	{rt_kprintf("input7 or output3 ...........err\n");   ret_ok=0;}
	                                                                  
	OUTPUT4=1;delay_ms(10);	if(INPUT8==1) 	{rt_kprintf("input8 or output4 ...........err\n");   ret_ok=0;}
	OUTPUT4=0;delay_ms(10);	if(INPUT8==0) 	{rt_kprintf("input8 or output4 ...........err\n");   ret_ok=0;}
	          
	OUTPUT5=1;delay_ms(10);	if(INPUT9==1) 	{rt_kprintf("input9 or output5 ...........err\n");   ret_ok=0;}
	OUTPUT5=0;delay_ms(10);	if(INPUT9==0) 	{rt_kprintf("input9 or output5 ...........err\n");   ret_ok=0;}
            
	OUTPUT6=1;delay_ms(10);	if(INPUT10==1) 	{rt_kprintf("input10 or output6 ..........err\n");   ret_ok=0;}
	OUTPUT6=0;delay_ms(10);	if(INPUT10==0) 	{rt_kprintf("input10 or output6 ..........err\n");   ret_ok=0;}
	          
	OUTPUT7=1;delay_ms(10);	if(INPUT11==1) 	{rt_kprintf("input11 or output7 ..........err\n");   ret_ok=0;}
	OUTPUT7=0;delay_ms(10);	if(INPUT11==0) 	{rt_kprintf("input11 or output7 ..........err\n");   ret_ok=0;}
																									 
	OUTPUT8=1;delay_ms(10);	if(INPUT12==1) 	{rt_kprintf("input12 or output8 ..........err\n");   ret_ok=0;}   
	OUTPUT8=0;delay_ms(10);	if(INPUT12==0) 	{rt_kprintf("input12 or output8 ..........err\n");   ret_ok=0;}   
	if(ret_ok)	rt_kprintf("inputs and outputs test......ok\n");	
	
}

static void hw_eeprom_debug(void)
{
	u8 *save_info="this page data save for eeprom test";
	u8 read_info[64];
	u8 len=strlen(save_info);
					
	rt_kprintf("eeprom test..................");
	at24cxx.write(at24c256,100,save_info,len);
	delay_ms(10);
	at24cxx.read(at24c256 , 100, read_info, len);
	
  if(Buffercmp((uint8_t*)save_info,(uint8_t*)read_info,len))
  {
    rt_kprintf("err\n");
  }	
	else rt_kprintf("ok\n");
	
}

static void hw_motor_debug(void)
{
	rt_kprintf("tmc429 test..................");
	if(Read429SingleByte(IDX_REF_SWITCHES, 3)==0) 
		rt_kprintf("err");

	else 
	{		

		rt_kprintf("ok\n\n最后:检测限位采集电路");
		rt_kprintf("\n先发送motor get leftLimit 1 采集数据再把L1接GND后发送motor get leftLimit 1返回值有变化才ok\n");	

	}
}

void board_debug(void)
{
	rt_kprintf("\n");
	hw_in_out_debug();
	hw_eeprom_debug();
	hw_motor_debug();	
}

MSH_CMD_EXPORT(board_debug, list device in system);

#endif






