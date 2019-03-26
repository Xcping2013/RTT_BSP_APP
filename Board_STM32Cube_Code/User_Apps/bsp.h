#ifndef _BSP_H_
#define _BSP_H

#define ENABLE_INT()	__set_PRIMASK(0)	
#define DISABLE_INT()	__set_PRIMASK(1)	

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <rthw.h>
#include <board.h>
#include <rtthread.h>
#include <drivers/pin.h>


#include "mcu_sys.h"
#include "mcu_delay.h"
#include "mcu_gpio.h"
#include "mcu_uart_fifo.h"


#include "inc_mbtmc429.h"

#include "bsp_led.h"
#include "bsp_key.h"
#include "app_led.h"
#include "app_uart.h"



#endif

