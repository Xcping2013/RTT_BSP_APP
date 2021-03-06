/*			控制卡功能说明

1. 命令交互采集输入控制输出：
2. 命令交互EEPROM存取
3. 电机操作

项目系统控制：

road: 
	只要电机处于向下运动（目标位置大于当前位置 | 当前速度>0 ）采集到报警信号,电机里面停止
	
	Z轴电机回原 > 电机停止 > 报警信号	> 运动命令


*/
/*	压力报警的安全监控

1.	IN8中断  电机停止 停止后屏蔽向下运动							H->L 触发一次,不管方向运动  复位L-H 不影响

																									  moveto命令先进行目标位置判断	
 
2.	打印串口3压力值 判断是否为向下运动并触发报警    非复位状态下目标位置判断

3.  任务采集报警输入电平,正转触发，立刻停止					当前速度判断,reset速度<0 不影响



*/
/*			线程
serial_thread_entry				
main_thread_entry
finsh_thread_entry
board_debug_thread_entry

led_thread_entry
motion_thread_entry
finsh_thread_entry
code_test_thread_entry
*/

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

/*
复位内移除定时器内的delay，调用delay时先停止定时器
优化回原点动作，回原点的时候，不响应stop命令

移除其他不恰当的delay；
在外部中断和电机停止命令中先 关闭压力采集串口

*/
#if defined(USING_INC_MBTMC429) 	//#define RT_USING_UART3

uint8_t LED_PIN=PD_7;

at24cxx_t at24c256=
{
	{0},
	{PB_4,PB_3},	//SDA SCL
	0xA0,
};
//HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
//HAL_NVIC_SetPriority(USART3_IRQn, 6, 0);
int mbtmc429_hw_init(void)		//thread: led; uart1; uart3
{		
	SysRunLed_thread_init();
	
	at24cxx_hw_init();
	
	MotorSensorCheck_timer_init();	
	
	tmc429_hw_init();
	//MotorLimitCheck_thread_init();
#if defined(RT_USING_UART3)
#else
	MX_USART3_UART_Init();
#endif
	uart_stream_thread_init();	

	dido_hw_init();					//add in8 as exit  防止程序未进入系统却开始响应 复位按键命令 从而死机
	
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
	
	//rt_show_version
	//rt_kprintf("\nfirmware ver2.5 build at %s %s\n\n", __TIME__, __DATE__);

  return 0;
}
/*
ver1.04---中断采集过压报警信号;立即停止电机;循环发送报警字符串
*/
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
MSH_CMD_EXPORT_ALIAS(printdata_disable,stop, stop print position and press);
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






