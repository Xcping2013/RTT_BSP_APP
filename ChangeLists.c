/*	中断 线程 优先级配置

数值越小的优先级越高，0 为最高优先级

HAL_NVIC_SetPriority(irqmap->irqno, 4, 0);
HAL_NVIC_SetPriority(irqmap->irqno, 4, 0);
HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
HAL_NVIC_SetPriority(USART2_IRQn, 6, 0);
HAL_NVIC_SetPriority(USART3_IRQn, 6, 0);
HAL_NVIC_SetPriority(TIM1_UP_IRQn, 6, 0);

#define IR_Priority_EXTI		4	 
#define IR_Priority_USART1	5
#define IR_Priority_TIM1		6
#define IR_Priority_USART3	7

#define FINSH_THREAD_PRIORITY 			18
#define RT_TIMER_THREAD_PRIO 				4	

#define RT_MAIN_THREAD_PRIORITY 		10
#define UART3_THREAD_PRIORITY				19

#define MOTORLIMIT_THREAD_PRIORITY	25

rt_thread_create
rt_thread_init

*/


/*	 文件功能描述

app_button: 
													开始按键按下一次就发一次值，复位按键按下，必须复位OK才能进行其他动作
app_gpio_dido:						IO初始化+外部中断初始化; 定时采集按键状态	;	输入中断处理

app_motion:								电机运动控制+触发限位的处理	

app_motor_home:						电机回原点和回限位操作

app_serial:								Z轴未停止时，串口3可控制数据打印的开关

app_sys_control:				  重启控制 EEPROM数存取控制 SPI操作定义 电机加速度配置 回原点定时器的配置 电机复位预处理

inc_mbtmc429:				      inc_mbtmc429板卡资源和CMD的配置

inc_mb1616dev6:				    inc_mb1616dev6板卡资源和CMD的配置

*/

/*	v2.4 20190709		现场原点位置出现异常处理

---电机回原点不能被电机停止命令打断，即电机回原点中不响应运动命令
---电机复位的时候不能进行电机的各种运动操作，也不受压力报警影响
---软件回原点和复位回原点也存在互锁作用
---电机回原点动作出现故障无法完成，只能重启
		要么上位机要么下位机判断，搞的两个都要判断，真麻烦
//INIT_BOARD_EXPORT(rt_iwg_init);
设备必须初始化后才能调用相关设备操作，不然hard fault

重要的全局控制	运动前先判断是否是回原点操作,不是的话先关闭串口打印再控制动作
	closeSerial();	buttonRESETpressed	pressureAlarm	homeInfo.GoHome	homeInfo.HomeSpeed	
	
*/
/******************************************************************/
/*  v2.6 20190727  概率性死机的优化	---> 串口中断错误的清除
1.去除函数内调用delay_ms,特别是删除定时器内调用
2.更改部分变量类型static -->  volatile
3.更改中断优先级 串口3最低    
4.修改RT_TICK_PER_SECOND---1000和线程优先级			
5.增加MSH和串口缓存空间 
	#define RT_CONSOLEBUF_SIZE 128-->512		
	RT_SERIAL_RB_BUFSZ 64	---> 512
6.overhigh!!!  \r\n --->\n
7.HAL_SPI_TransmitReceive(&hspi2, &aTxBuffer, &aRxBuffer, 1,5000); 5000->1000

8.不是SPI冲突！！！

20190730：

rt_hw_serial_register(&serial3, "uart3",RT_DEVICE_FLAG_RDONLY | RT_DEVICE_FLAG_INT_RX,uart);

uart_isr UART_FLAG_ORE等的清除

		if (__HAL_UART_GET_FLAG(&(uart->huart), UART_FLAG_ORE) != RESET)
		{
				__HAL_UART_CLEAR_OREFLAG(&uart->huart);
		}
		if (__HAL_UART_GET_FLAG(&(uart->huart), UART_FLAG_NE) != RESET)
		{
				__HAL_UART_CLEAR_NEFLAG(&uart->huart);
		}
		if (__HAL_UART_GET_FLAG(&(uart->huart), UART_FLAG_FE) != RESET)
		{
				__HAL_UART_CLEAR_FEFLAG(&uart->huart);
		}
		if (__HAL_UART_GET_FLAG(&(uart->huart), UART_FLAG_PE) != RESET)
		{
				__HAL_UART_CLEAR_PEFLAG(&uart->huart);
		}
*/

//
/*  v2.7.2 20190730  压力报警	EXTI_CallBack_Fun	的优化   
					
INC-MBTMC429-RTT-272.hex  
更新：
 --->报警后屏蔽所有的正向运动命令,报警解除后可再次响应
 --->motor reset复位状态下屏蔽所有电机运行命令
 --->不要在回原点动作去采集电机位置和控制电机运动，这些信号会被回原点打断，返回值中会穿插一些异常数据
		( 要么不要再中断中调用SPI,要么在SPI中关相关中断，这样都会对系统操作造成影响)
     可以发送motor reset?来判断是否回原点OK	

  优化文件布局 统一并兼容  stm32f103ve-512-64 ： application --- application_v2 ---std drivers

*/
//

//
