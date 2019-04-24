/**
  \file SysTick.c

  \version 1.00

  \brief 1ms system tick timer functions
*/

#include "main.h"

static volatile UINT SysTickTimer;
/***************************************************//**
  \fn SysTick_Handler(void)
  \brief System timer interrupt handler

  The system timer interrupt handler counts up the
  1ms counter.
*******************************************************/
void SysTick_Handler(void)
{
	SysTickTimer++;
}
/***************************************************//**
  \fn InitSysTick(void)
  \brief Initalize system timer

  Initalize the tick timer for generating an interrupt
  every 1ms.
*******************************************************/
void InitSysTick(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
	SysTick->LOAD=9000;
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   						//����SYSTICK�ж�
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   						//����SYSTICK    

}
/***************************************************//**
  \fn GetSysTimer(void)
  \brief Read the system timer
  \return System timer (1ms)

  This function returns the actual value of the 1ms
  system timer.
*******************************************************/
UINT GetSysTimer(void)
{
  return SysTickTimer;
}
//SYSTICK��һ���ݼ��ļ�����
void delay_us(uint32_t us)
{
    int i = (SystemCoreClock / 4000000 * us);
    while (i)
    {
        i--;
    }
}
void delay_us1(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;					//1ms��ʱ��Ҫ��LOADֵ 	T 
	ticks=nus*9; 											//9MHZ nus ��Ҫ�Ľ�����	   		 
	tcnt=0;

	told=SysTick->VAL;        			  //�ս���ʱ�ļ�����ֵ		T0
	while(1)
	{
		tnow=SysTick->VAL;							//����ʱ�̵�ʵʱ��ֵ	  T1
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//����T0-T1=��T1
			else tcnt+=reload-tnow+told;	//��T0=1	T1=9000	��T1=T0+T-T1   
			told=tnow;										//��T=��T1+��Tn
			if(tcnt>=ticks)break;				  //��Tʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};						    
}
void delay_ms(uint16_t nms)
{
	delay_us(nms*1000);
}
void delay_ms1(u16 nms)
{	
	delay_us((u32)(nms*1000));					//��ͨ��ʽ��ʱ  
}

