//#include "app_button.h"
//#include "app_sys_control.h"
#include "bsp_include.h"	
#include "app_include.h"

uint8_t buttonSTARTpressed=0;
uint8_t buttonRESETpressed=0;

//开始按键和复位按键由上位机控制，底层不封装任何特殊动作

//启动按键可以和启动灯、复位灯一起搭配  	//开始按键自锁 复位按键解锁	
void buttonSTART_process(uint8_t inCh, uint8_t outCh)
{
	if(buttonRESETpressed==FALSE)																											//电机在复位中，START无效
	{
		if( getChInput(inCh)==IN_ON && buttonSTARTpressed==FALSE )					
		{
			delay_ms(20);		
			if( getChInput(inCh)==IN_ON )											 																//开始按键
			{
				 setChOutput(outCh,1);	 									
				 while(getChInput(inCh)==IN_ON) {;} 																						//等待松开									 									 			
				 buttonSTARTpressed=TRUE;  
				 buttonRESETpressed=FALSE;			
				 CMD_TRACE("buttonSTART=1\n");
			}
		}
	}
}
/*
回原点速度从EEPROM读出
Bx35_button: Y轴先里后出 ---> Z轴先上后下 ---> X轴先左后右

B445_button: Z轴先上后下回原点后再往上运行一段位置放置传感器撞到Holder---> Y轴先里后出---> X轴先左后右

ROAD			 ：Z轴先上后下				
*/
void buttonRESET_process(uint8_t inCh, uint8_t outCh)
{
	if( getChInput(inCh)==IN_ON && buttonRESETpressed==FALSE )							
	{
		delay_ms(20);
		if( getChInput(inCh)==IN_ON )																				
		{		
			buttonRESETpressed=TRUE;	setChOutput(outCh,1);	 
			MotorAutoReset_preset();	
		}
	}
}

int buttonSTART(int argc, char **argv)
{
	if(argc==2)
	{
		if (!strcmp(argv[1], "status"))	CMD_TRACE("buttonSTART=%d\n",buttonSTARTpressed);
		if (!strcmp(argv[1], "enable")) 
		{
			buttonSTARTpressed=FALSE;
			CMD_TRACE("enable button START for next test\n");		
		}
	}
	return 0;
}
	
