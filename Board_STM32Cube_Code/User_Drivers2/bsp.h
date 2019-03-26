#ifndef _BSP_H_
#define _BSP_H_

#define ENABLE_INT()	__set_PRIMASK(0)	
#define DISABLE_INT()	__set_PRIMASK(1)	

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"
#include "usart.h"

#include "mcu_sys.h"
#include "mcu_delay.h"
#include "mcu_gpio.h"

#include "app_pca9539a.h"
#include "app_at24cxx.h" 

#include "app_uart1_console.h"
#include "app_uart1_Commands.h"

void bsp_Init(void);
	
#endif

