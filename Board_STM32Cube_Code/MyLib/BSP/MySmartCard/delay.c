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
//��ʱnms
//nms:Ҫ��ʱ��ms��
void delay_ms(u16 nms)
{	
	//delay_us((u32)(nms*1000));					//��ͨ��ʽ��ʱ  
	HAL_Delay(nms);
}
/*  ʹ�õδ�ʱ��ʱ���ȡ����ʱ
//void delay_us(u32 nus)
//{		
//	u32 ticks;
//	u32 told,tnow,tcnt=0;
//	u32 reload=SysTick->LOAD;							//LOAD��ֵ	    	 
//	ticks=nus*SystemCoreClock/1000; 			//��Ҫ�Ľ�����
//  //ticks=nus*SystemCoreClock/8000;			//SYSTICK_CLKSOURCE_HCLK_DIV8
//	tcnt=0;

//	told=SysTick->VAL;        						//�ս���ʱ�ļ�����ֵ
//	while(1)
//	{
//		tnow=SysTick->VAL;	
//		if(tnow!=told)
//		{	    
//			if(tnow<told)tcnt+=told-tnow;		//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
//			else tcnt+=reload-tnow+told;	    
//			told=tnow;
//			if(tcnt>=ticks)break;				//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
//		}  
//	};						    
//}
*/
