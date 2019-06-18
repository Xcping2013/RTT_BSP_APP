#include "bsp_mcu_delay.h"	

//rt_thread_delay
//rt_thread_sleep
//rt_hw_us_delay
//rt_thread_mdelay

//��Ҫ�߾��ȵ�ʱ��ʹ��Ӳ����ʱ��
//�ж��ﲻ������rt_thread_delay
#ifndef	USING_RTT
#define RT_TICK_PER_SECOND 1000
#define rt_uint32_t	uint32_t
#endif

void drv_mdelay(uint16_t nms)
{
	drv_udelay(nms*1000);
}

void drv_udelay(uint32_t us)
{
    int i = (SystemCoreClock / 4000000 * us);
    while (i)
    {
        i--;
    }
}
//�����ڹر��жϺ����
void delay_us(int us)
{
	rt_uint32_t delta;
	/* �����ʱ������tick�� */
	us = us * (SysTick->LOAD/(1000000/RT_TICK_PER_SECOND));
	/* ��õ�ǰʱ�� */
	delta = SysTick->VAL;
	/* ѭ����õ�ǰʱ�䣬ֱ���ﵽָ����ʱ����˳�ѭ�� */
	while (delta - SysTick->VAL< us);
}
void delay_ms(u16 nms)
{
	//rt_thread_delay(nms/10);
	drv_udelay(nms*1000);
}


