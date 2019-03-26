#include "mcu_delay.h"

//rt_hw_us_delay�����ڹر��жϺ����
void drv_mdelay(uint16_t nms)
{
	drv_udelay(nms*1000);
}

void drv_udelay(uint32_t us)
{		
		//һ��ָ��4~6����������
    int i = (SystemCoreClock / 4000000 * us);
    while (i)
    {
        i--;
    }
}
