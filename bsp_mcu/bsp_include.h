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

//#include "bsp_user_lib.h"
//#include "bsp_timer.h"
//#include "bsp_tim_pwm.h"
//#include "bsp_spi.h"
//#include "bsp_uart.h"
//#include "bsp_key.h"
//#include "bsp_led.h"
//#include "bsp_beep.h"
//#include "input_output.h"
//#include "bsp_i2c_gpio.h"
//#include "bsp_eeprom_24xx.h"
//#include "param_mbtmc429.h"
//#include "bsp_tmc429.h"
//#include "bsp_cpu_rtc.h"
//#include "bsp_cpu_flash.h"
//#include "bsp_msg.h"


//void bsp_Init(void);
//void bsp_Idle(void);

#endif

