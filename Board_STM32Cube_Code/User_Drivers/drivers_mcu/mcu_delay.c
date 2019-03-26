#include "mcu_delay.h"


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
//�����ڹر��жϺ����
void delay_us(u32 nus)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;						//LOAD��ֵ	    	 
	ticks=nus*72; 											//��Ҫ�Ľ�����	  		 
	tcnt=0;

	told=SysTick->VAL;        					//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;		//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;				//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	}		
}
