#ifndef __MCU_DELAY_H
#define __MCU_DELAY_H 

#include "mcu_sys.h"

void delay_us(uint32_t us);
void drv_udelay(uint32_t us);
void drv_mdelay(uint16_t nms);

#endif
