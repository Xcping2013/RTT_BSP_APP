/*
*********************************************************************************************************
*
*	模块名称 : BSP模块
*	文件名称 : bsp.c
*	版    本 : V1.1
*	说    明 : 这是硬件底层驱动程序模块的主文件。主要提供 bsp_Init()函数供主程序调用。主程序的每个c文件
*			可以在开头添加 #include "bsp.h" 来包含所有的外设驱动模块。
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2015-09-01 armfly  正式发布
*		V1.1    2015-10-01 armfly  整理注释，修改不规范的语句和描述
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
*********************************************************************************************************
*	函 数 名: bsp_Init
*	功能说明: 初始化硬件设备。上电只需要调用一次。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。
*			 全局变量。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Init(void)
{
	/*
		由于ST固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
		启动文件配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。

		系统时钟缺省配置为72MHz，如果需要更改，可以修改 system_stm32f10x.c 文件
	*/

	/* 中断优先级分组设置为4，即： 抢占优先级有0-15(0级别最高）级，无子优先级 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	bsp_InitUart(); 	/* 初始化串口 */
	bsp_InitLed(); 		/* 初始化LED指示灯端口 */
	bsp_InitKey();		/* 初始化按键 */

	bsp_InitRTC();		/* 初始化CPU内部RTC */
	
	bsp_InitExtSRAM();	/* 配置FSMC总线, 用于SRAM访问 */
	bsp_InitNorFlash();	/* 初始化FSMC NOR Flash */

	bsp_InitI2C();		/* 配置I2C总线 */
	
	bsp_InitSPIBus();	/* 配置SPI总线，必须在初始化串行Flash之前执行本函数 */
	bsp_InitSFlash();	/* 初始化串行Flash. 该函数会识别串行FLASH型号 */
	
	bsp_InitTimer();	/* 初始化系统滴答定时器 (此函数会开中断) */

	bsp_InitMPU6050();	/* 配置MPU6050三轴陀螺仪 */
	bsp_InitBH1750();	/* 配置BH1750光照度传感器 */
	
	bsp_InitPS2();		/* 初始化PS2键盘鼠标接口。注意此时并不启动PS2设备检测，还需要执行PS2_StartWork() */

	/* 下面的设备暂时不配置，等需要用的时候再配置和启动 */
	
#if 0	/* #if 0 是条件编译，0表示此段代码不参与编译，1表示参与编译 */
	PS2_StartWork();	/* 启动PS2设备检测 */
	IRD_StartWork();	/* 配置并启动红外接收模块 */
	
	bsp_InitHMC5883L();	/* 配置HMC5883L磁力计 */
	bsp_InitBMP180();	/* 配置BMP180气压传感器 */
	bsp_InitGPS();		/* 配置GPS模块 */	
#endif
	
}

/*
*********************************************************************************************************
*	函 数 名: bsp_RunPer10ms
*	功能说明: 该函数每隔10ms被Systick中断调用1次。详见 bsp_timer.c的定时中断服务程序。一些处理时间要求不严格的
*			任务可以放在此函数。比如：按键扫描、蜂鸣器鸣叫控制等。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_RunPer10ms(void)
{
	bsp_KeyScan();		/* 扫描按键 */
	
	BEEP_Pro();			/* 处理蜂鸣器定时器 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_RunPer1ms
*	功能说明: 该函数每隔1ms被Systick中断调用1次。详见 bsp_timer.c的定时中断服务程序。一些需要周期性处理的事务
*			 可以放在此函数。比如：触摸坐标扫描。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_RunPer1ms(void)
{
	TOUCH_Scan();		/* 触摸扫描（RA8875内置触摸和TSC2046触摸） */
	
	PS2_Timer();		/* PS/2键盘和鼠标定时处理 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_Idle
*	功能说明: CPU空闲时执行的函数。一般主程序在for和while循环程序体中调用本函数。不要在中断服务程序中调用。
*			 用户可以添加喂狗、设置CPU进入休眠模式的功能、也可以添加一些需要全局支持的功能，比如截屏。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
extern void SaveScreenToBmp(uint16_t _index);	/* 此函数在user\app\bmp_file.c --- 调用用了应用层的函数 */
void bsp_Idle(void)
{
	/* --- 喂狗 */

	/* --- 让CPU进入休眠，由Systick定时中断唤醒或者其他中断唤醒 */

	/* 例如 emWin 图形库，可以插入图形库需要的轮询函数 */
	//GUI_Exec();

	/* 例如 uIP 协议，可以插入uip轮询函数 */

	TOUCH_CapScan();	/* I2C接口电容触摸屏扫描 */
	
	if (bsp_GetKey2() == SYS_DOWN_K1K2)	/* K1 K2 组合键用于截屏 */
	{
		static uint16_t s_file_no = 0;

		SaveScreenToBmp(s_file_no++);	/* 截图保存为bmp文件，放在SD卡根目录 */
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
