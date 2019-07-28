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

//INIT_BOARD_EXPORT(rt_iwg_init);
设备必须初始化后才能调用相关设备操作，不然hard fault

重要的全局控制	运动前先判断是否是回原点操作,不是的话先关闭串口打印再控制动作
	closeSerial();	buttonRESETpressed	pressureAlarm	homeInfo.GoHome	homeInfo.HomeSpeed	
	
*/
/******************************************************************/
/*  v2.6 20190727  概率性死机的优化
1.去除函数内调用delay_ms,特别是删除定时器内调用
2.更改部分变量类型static -->  volatile
3.更改中断优先级 串口3最低    
4.修改RT_TICK_PER_SECOND---1000和线程优先级			
5.增加MSH和串口缓存空间 
	#define RT_CONSOLEBUF_SIZE 128-->512		
	RT_SERIAL_RB_BUFSZ 64	---> 512
6.overhigh!!!  \r\n --->\n
7.HAL_SPI_TransmitReceive(&hspi2, &aTxBuffer, &aRxBuffer, 1,5000); 5000->1000

8.SPI DMA

线程里面处理spi会和finsh中处理spi冲突 出现问题			spi之间的冲突
优化代码为V2版本，减少烧录时间
*/
