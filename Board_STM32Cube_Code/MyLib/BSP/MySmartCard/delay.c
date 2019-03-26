#include "delay.h"

void delay_us(u32 nus)
{    
   u16 i=0;  
   while(nus--)
   {
      i=80;  
      while(i--) ;    
   }
}
//延时nms
//nms:要延时的ms数
void delay_ms(u16 nms)
{	
	//delay_us((u32)(nms*1000));					//普通方式延时  
	HAL_Delay(nms);
}
/*  使用滴答定时器时间截取法定时
//void delay_us(u32 nus)
//{		
//	u32 ticks;
//	u32 told,tnow,tcnt=0;
//	u32 reload=SysTick->LOAD;							//LOAD的值	    	 
//	ticks=nus*SystemCoreClock/1000; 			//需要的节拍数
//  //ticks=nus*SystemCoreClock/8000;			//SYSTICK_CLKSOURCE_HCLK_DIV8
//	tcnt=0;

//	told=SysTick->VAL;        						//刚进入时的计数器值
//	while(1)
//	{
//		tnow=SysTick->VAL;	
//		if(tnow!=told)
//		{	    
//			if(tnow<told)tcnt+=told-tnow;		//这里注意一下SYSTICK是一个递减的计数器就可以了.
//			else tcnt+=reload-tnow+told;	    
//			told=tnow;
//			if(tcnt>=ticks)break;				//时间超过/等于要延迟的时间,则退出.
//		}  
//	};						    
//}
*/
