#ifndef __MCU_DELAY_H
#define __MCU_DELAY_H 

extern void rt_hw_us_delay(rt_uint32_t us);

void drv_udelay(uint32_t us);
void drv_mdelay(uint16_t nms);

#endif
