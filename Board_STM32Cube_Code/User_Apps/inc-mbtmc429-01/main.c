/*
*********************************************************************************************************
*
*	模块名称 : 主程序入口
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : 串口通信例子，和PC机超级终端软件进行交互
*	修改记录 :
*		版本号  日期       作者    说明
*		V1.0    2015-08-30 armfly  首发
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"				/* 底层硬件驱动 */

/* 定义例程名和例程发布日期 */
#define EXAMPLE_NAME	"V4-003_串口和PC机通信（串口中断、FIFO机制）"
#define EXAMPLE_DATE	"2015-08-30"
#define DEMO_VER		"1.0"

static void PrintfLogo(void);
static void PrintfHelp(void);

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t ucKeyCode;	
	int16_t count = 0;
	uint8_t fRefresh = 0;
	uint8_t read;
	
	/*
		ST固件库中的启动文件已经执行了 SystemInit() 函数，该函数在 system_stm32f4xx.c 文件，主要功能是
	配置CPU系统的时钟，内部Flash访问时序，配置FSMC用于外部SRAM
	*/
	bsp_Init();		/* 硬件初始化 */
	
	PrintfLogo();	/* 打印例程名称和版本等信息 */
	PrintfHelp();	/* 打印操作提示 */
	
	fRefresh = 1;
	/* 主程序大循环 */
	while (1)
	{
		bsp_Idle();		/* CPU空闲时执行的函数，在 bsp.c */
	
		/* 摇杆左右键（上下键）控制LED流动 */
		if (fRefresh)
		{
			fRefresh = 0;

			#if 0	/* 可以用 printf打印数据到串口 */
				printf("count = %6d \r", count);
			#else	/* 也可以用 sprintf先输出到一个buf，然后在发送到串口 */
			{
				char buf[64];
								
				sprintf(buf, "count = %6d \r", count);
				
				comSendBuf(COM1, (uint8_t *)buf, strlen(buf));
			}
			#endif
		}
		
		if (comGetChar(COM1, &read))
		{
			switch (read)
			{
				case '1':
					bsp_LedToggle(1);
					break;

				case '2':
					bsp_LedToggle(2);
					break;

				case '3':
					bsp_LedToggle(3);
					break;

				case '4':
					bsp_LedToggle(4);
					break;	
			}

		}
		
		/* 处理按键事件 */
		ucKeyCode = bsp_GetKey();
		if (ucKeyCode > 0)
		{
			/* 有键按下 */
			switch (ucKeyCode)
			{
				case JOY_DOWN_L:		/* 摇杆LEFT键按下, 控制LED显示流动 */
					count-=10;
					fRefresh = 1;
					break;		
				
				case JOY_DOWN_R:		/* 摇杆RIGHT键按下 */
					count+=10;		
					fRefresh = 1;
					break;

				case JOY_DOWN_U:		/* 摇杆UP键按下 */
					count++;		
					fRefresh = 1;
					break;				
				
				case JOY_DOWN_D:		/* 摇杆DOWN键按下 */
					count--;
					fRefresh = 1;
					break;

				case JOY_DOWN_OK:		/* 摇杆OK键按下 */
					break;		
				
				default:
					break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: PrintfHelp
*	功能说明: 打印操作提示
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfHelp(void)
{
	printf("操作提示:\r\n");
	printf("1. 摇杆控制count计数值\r\n");
	printf("     上键 = +1\r\n");
	printf("     下键 = -1\r\n");		
	printf("     左键 = -10\r\n");
	printf("     右键 = +10\r\n");
	printf("2. PC上输入数字1-4控制开发板上的LED指示灯\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: PrintfLogo
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	printf("*************************************************************\n\r");
	printf("* 例程名称   : %s\r\n", EXAMPLE_NAME);	/* 打印例程名称 */
	printf("* 例程版本   : %s\r\n", DEMO_VER);		/* 打印例程版本 */
	printf("* 发布日期   : %s\r\n", EXAMPLE_DATE);	/* 打印例程日期 */

	/* 打印ST固件库版本，宏定义在 stm32f4xx.h 文件 */
	printf("* 固件库版本 : %d.%d.%d\r\n", __STM32F10X_STDPERIPH_VERSION_MAIN,
			__STM32F10X_STDPERIPH_VERSION_SUB1,__STM32F10X_STDPERIPH_VERSION_SUB2);

	/* 打印 CMSIS 版本. 宏定义在 core_cm4.h 文件 */
	printf("* CMSIS版本  : %X.%02X\r\n", __CM3_CMSIS_VERSION_MAIN, __CM3_CMSIS_VERSION_SUB);

	printf("* \n\r");	/* 打印一行空格 */
	printf("* QQ    : 1295744630 \r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* Copyright www.armfly.com 安富莱电子\r\n");
	printf("*************************************************************\n\r");
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
