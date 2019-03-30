/*
*********************************************************************************************************
*	ģ������ : BSPģ��
*	�ļ����� : bsp.h
*	˵    �� : ���ǵײ�����ģ�����е�h�ļ��Ļ����ļ��� Ӧ�ó���ֻ�� #include bsp.h ����.
*
*********************************************************************************************************
*/
#ifndef __BSP_INCLUDE_H_
#define __BSP_INCLUDE_H_

/*
	stdlib.h : standard library��׼��ͷ�ļ�,������C��C++���Ե���õ����Ͷ����ϵͳ���������� malloc()��free()
	stdio.h : standard input & output ��׼������������⡣���ú����� printf()��getc()��putc()
	string.h : �����ַ�����ĺ��������ͷ�ļ��⣬���ú�����strlen��strcmp��strcpy	
	" " �� < > ������:
	" " һ���ʾԴ�����Դ���h�ļ�������ʱ�������������ڹ�������ָ��������·���б���Ѱ��ͷ�ļ�
	< > һ���ʾ����ϵͳ�Դ���h�ļ�������ʱ��������������IDE�Դ��Ŀ��ļ���Ѱ��ͷ�ļ���
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

