/*	文件功能描述

app_button: 							开始按键按下一次就发一次值, 按键手动松开解锁
						定时循环采集
													复位按键按下,电机开始按设置方式进行复位, 必须复位OK才能进行其他动作，复位先关闭其他动作开关，并使用定时器开始寻原点
													
app_gpio_dido:				    输入输出	原点信号	输入8压力报警中断 线程采集IN8

													speed>0 && IN8==0	motorstop

app_motion:								电机运动控制等命令的封装

app_motor_home:						电机回原点和回限位操作

app_serial:								Z轴未停止时，串口3可控制数据打印的开关 
                          目标位置大于当前位置时采集报警信号,立刻停止

inc_mbtmc429:				      inc_mbtmc429板卡资源和CMD的配置

inc_mb1616dev6:				    inc_mb1616dev6板卡资源和CMD的配置

app_sys_control:				  重启控制 EEPROM数存取控制 SPI操作定义 电机加速度配置 回原点定时器的配置 电机复位预处理

压力仪报警的时候 DOUT=0V,不报警的时候DOUT=22V或者浮空; 	限位传感器默认0V，触发后为24V或者浮空,二者需要转换	如果压力仪接在限位上，进行电平反相接入

*/

//
/*	v2.3 		20190708			RTT的初始应用版本	CMD+外部报警为中断只发一次	
*/
/*	v2.4 		20190709			命令回原点可反复触发，按键只触发一次，且按键回原点会受软件打断

---电机回原点不能被电机停止命令打断，即电机回原点中不响应运动命令
---电机复位的时候不能进行电机的各种运动操作，也不受压力报警影响
---软件回原点和复位回原点也存在互锁作用
---电机回原点动作出现故障无法完成，只能重启

重要的全局控制	运动前先判断是否是回原点操作,不是的话先关闭串口打印再控制动作
	closeSerial();	buttonRESETpressed	pressureAlarm	homeInfo.GoHome	homeInfo.HomeSpeed	
	
*/
/*	v2.5 		20190719			修改串口3压力处理为线程和串口设备处理方式

*/

/*  v2.6 		20190727   		程序跑久死机的修复	---> 串口中断错误的清除
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
/*  v2.7.2 	20190730 			压力报警	EXTI_CallBack_Fun	的优化   
					
INC-MBTMC429-RTT-272.hex  
更新：
 --->报警后屏蔽所有的正向运动命令,报警解除后可再次响应
 --->motor reset复位状态下屏蔽所有电机运行命令
 --->不要在回原点动作去采集电机位置和控制电机运动，这些信号会被回原点打断，返回值中会穿插一些异常数据
		( 要么不要再中断中调用SPI,要么在SPI中关相关中断，这样都会对系统操作造成影响)
     可以发送motor reset?来判断是否回原点OK	

  优化文件布局 统一并兼容  stm32f103ve-512-64 ： application --- application_v2 ---std drivers

*/
//  V273 		20190731  		保持双启按键自锁，由命令开关状态或者不进行自锁
/*  V274:		20190731			回原点动作采集限位延时后再动作，采集到原点信息采用立刻停止方式
									双启按键自行触发，不再命令使能
*/
/*  V275		20190801  


修复V274位置移动造成复位的加速度被覆盖问题;					TMC429_MotorRotate 需要再次设定加速度	setAMAX
项目号不再按参数保存版本PARAM_VER进行初始化					LoadParamFromEeprom

电机加速度和速度匹配正确，电机运动设置速度前最好保证硬件停止，运动时debug打印其他相关信息

VMAX-AMAX Maching Test	
set speed = set VMAX

LOG_D 后续增加此功能

//__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin); 不去除会一直报警，所以要先清除

extern volatile uint8_t pressureAlarm; 不被优化

//故障现象：电机往下运动超出0.7N命令停止，后发送返回命令，电机还是保持下压动作进行运动，报警功能失效
//motor stop动作未执行？

motor stop 和电机移动 命令 受复位动作屏蔽

motor moveto 还受压力报警影响，不往下走

不要使用定时器操作按键，定时器会影响打断SPI操作

rt_enter_critical();

*/


//  V276		20190801  		优化命令：		只靠外部中断采集报警信号	报警后不屏蔽向上运动的命令
//  V277		20190801  		增加动作：		在printdata中采集到报警信号就停止
//  V280:		20190801  		优化动作：		在printdata中采集到报警信号并且判断是向下运动就停止
/*  V281:		20190805  		增加动作：		开线程专门采集是否正转且报警	 完善项目号的判断 放置COMMON也初始化中断相关

压力报警的安全监控小结：不能只靠触发一次后就不进行判断和动作	即不能只靠步骤1来解决其他异常，需要增加3或者2

1.	IN8中断 回原点状态下触发报警,向上旋转找原点 否则电机停止 停止后屏蔽向下运动							H->L 触发一次,不管方向运动  复位L-H 不影响复位
		dido_hw_init	
		motor	moveto	
		MotorAutoReset_preset closeSerial
																									  moveto命令先进行目标位置判断	
 
2.	打印串口3压力值 判断是否为向下运动并触发报警    非复位状态下目标位置判断
		
		serial3_data_processing

3.  任务采集报警输入电平,正转触发，立刻停止					当前速度判断,reset速度为<0 不影响复位

    IN8_thread_init();

*/


/*  V282:		20190805  		优化动作：		采集到限位后回去找原点的速度改小 取寻限位的速度/2或者/4 这个原点更稳 更好采集
其他回原点和回限位等都不再重新编写操作,浪费时间做无用功
*/

//
//X后续优化X：回原点不使用定时器中断,开线程寻限位,开外部输入中断采集原点	采用RTT SPI | LOG	| BSP_STM32 验证后放入bsp applicaition内
//--------：中断里面和线程间都操作SPI会造成运动结果超出期望，产生故障，
//保留现有可控定时器中断寻原点，其他线程不进行电机操作或者采用线程回原点并进行临界操作保护
//MSH 是在 finsh基础上应用	考虑好再敲代码 浪费时间

/*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
*/
/*	中断 线程 优先级配置							数值越小的优先级越高，0 为最高优先级

#define IR_Priority_EXTI		4	 			irqmap->irqno	4
#define IR_Priority_USART1	5				USART1_IRQn		5
#define IR_Priority_TIM1		6				TIM1_UP_IRQn	6
#define IR_Priority_USART3	7				USART3_IRQn	  6

#define FINSH_THREAD_PRIORITY 			18
#define RT_TIMER_THREAD_PRIO 				4	

#define RT_MAIN_THREAD_PRIORITY 		10
#define UART3_THREAD_PRIORITY				19

rt_thread_create										程序中创建了线程数量 list_thread
rt_thread_init

*/

/*	rt-thread 应用中的问题

---	中断和线程优先级的正确配置，中断1>中断2>线程1>线程2

---	设备必须初始化后才能调用相关设备操作，不然hard fault

---	用户函数调用 INIT_APP_EXPORT会影响执行的顺序，尽量汇总在一个文件中调用

---	串口接收中断响应处理中需要清除其他错误标志

--- 不要在中断中操作SPI,除非中断中的操作较重要,允许打断其他SPI操作

	  多线程调用SPI操作是注意临界区的应用	rt_enter_critical();

*/
/*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
*/
/****************************************************************************************************************************/


