/*
*********************************************************************************************************
*	模块名称 : BSP模块
*	文件名称 : bsp.h
*	说    明 : 这是底层驱动模块所有的h文件的汇总文件。 应用程序只需 #include bsp.h 即可.
*
*********************************************************************************************************
*/
#ifndef __BSP_INCLUDE_H_
#define __BSP_INCLUDE_H_

/*
	stdlib.h : standard library标准库头文件,包含了C、C++语言的最常用的类型定义和系统函数，比如 malloc()、free()
	stdio.h : standard input & output 标准输入输出函数库。常用函数有 printf()、getc()、putc()
	string.h : 关于字符数组的函数定义的头文件库，常用函数有strlen、strcmp、strcpy	
	" " 和 < > 的区别:
	" " 一般表示源代码自带的h文件。编译时，编译器优先在工程配置指定的搜索路径列表中寻找头文件
	< > 一般表示编译系统自带的h文件。编译时，编译器优先在IDE自带的库文件夹寻找头文件。
*/
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include "stdarg.h"

#include "bsp_defines.h"

#if defined(USING_RTT)

	#include "mbtmc429_cube_usart.h"
	#include "mbtmc429_cube_main.h"
	#include "mbtmc429_cube_gpio.h"
	#include "mbtmc429_cube_tim.h"
	#include "mbtmc429_cube_spi.h"

#endif

#include "bsp_mcu_delay.h"
#include "bsp_mcu_gpio.h"
#include "bsp_mcu_softI2c.h"
#include "bsp_eeprom_24xx.h"


//#define USING_INC_MB1616DEV6	
#define USING_INC_MBTMC429	

#if defined(USING_INC_MB1616DEV6) 
  #include "inc_mb1616dev6.h"
	#include "bsp_tmc429.h"
#endif
#if defined(USING_INC_MBTMC429) 
  #include "inc_mbtmc429.h"
	#include "bsp_tmc429.h"
#endif

//#define CALENDAR_USED
//#define MCP3421_USED
//#define AT24CXX_USED
//#define W25QXX_USED
//#define L6480_USED
//#define LOGIC_USED
//#define ISL29125_USED
//#define DS18B20_USED
//#define EXIT_USED
//#define TM1638_USED

#ifdef MCP3421_USED
	#include "MCP3421.h"
#endif

#ifdef AT24CXX_USED
	#include "at24cxx.h"
	#include "mcu_softI2c.h"
#endif

#ifdef W25QXX_USED
	#include "mcu_spi.h"
	#include "w25qxx.h" 
#endif

#ifdef L6480_USED
	#include "l6480_config.h"
	#include "l6480.h"
#endif

#ifdef LOGIC_USED
	#include "74HC165.h"
	#include "74HC595.h"
#endif

#ifdef ISL29125_USED
	#include "isl29125.h"
#endif

#ifdef CALENDAR_USED
	#include "mcu_rtc.h"
	#include "Calendar.h"
#endif

#ifdef DS18B20_USED
	#include "ds18b20.h"
#endif

#ifdef EXIT_USED
	#include "mcu_exit.h"
#endif

#ifdef TM1638_USED
	#include "tm1638.h"
#endif







#endif

