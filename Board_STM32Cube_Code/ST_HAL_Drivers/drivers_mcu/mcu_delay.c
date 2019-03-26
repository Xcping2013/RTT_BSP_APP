#include "bsp.h"

void drv_mdelay(uint16_t nms)
{
	drv_udelay(nms*1000);
}

void drv_udelay(uint32_t us)
{		
		//一条指令4~6个机器周期
    int i = (SystemCoreClock / 4000000 * us);
    while (i)
    {
        i--;
    }
}
