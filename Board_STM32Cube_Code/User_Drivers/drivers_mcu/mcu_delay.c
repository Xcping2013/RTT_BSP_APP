#include "mcu_delay.h"


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
//不能在关闭中断后调用
void delay_us(u32 nus)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;						//LOAD的值	    	 
	ticks=nus*72; 											//需要的节拍数	  		 
	tcnt=0;

	told=SysTick->VAL;        					//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;		//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;				//时间超过/等于要延迟的时间,则退出.
		}  
	}		
}
