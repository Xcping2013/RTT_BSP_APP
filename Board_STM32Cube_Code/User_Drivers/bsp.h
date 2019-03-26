#ifndef _BSP_H_
#define _BSP_H_

//#define ENABLE_INT()	__set_PRIMASK(0)	
//#define DISABLE_INT()	__set_PRIMASK(1)	

#define	INC_MB1616003
#define	INC_MB1616DEV6
#define	INC_MBTMC429

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"
//#include "usart.h"
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

/* mcu function initialization */

#include "mcu_sys.h"
#include "mcu_delay.h"
#include "mcu_gpio.h"
#include "mcu_tim_pwm.h"
//#include "bsp_led.h"
//#include "bsp_usart.h"

//#include "app_spi.h"
//#include "bsp_tmc429.h"
/* ic application */
//#include "app_tmc429.h"

#include "app_pca9539a.h"
#include "app_at24cxx.h" 

//#include "app_uart1_console.h"
#include "app_uart1_Commands.h"

int board_debug_thread_init(void);
void cpuReset(void);
	
#endif

