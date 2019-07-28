/**	
  \file 	  main.c
  \author 	Xcping2016	2016-5-10			2018-1-5
  \version 	1.00
  \brief   	Main file
*/
#include <stdlib.h>		//abs strtol
#include "main.h"			//mcu periphery bsp
#include "Globals.h"	//global variable ;fun 
#include "bits.h"

#include "SysTick.h"	
#include "UART.h"
#include "Timer.h"
#include "IO.h"

#include "SPI.h"
#include "TMC429.h"
#include "TMC262.h"

#include "Eeprom.h"
#include "Commands.h"
#include "SysControl.h"

#include "button.h"
#include "lidopen.h"
#include "JABIL_LCR.h"

//	NO74HC04	SYSCLK/1 =72MHZ
// CommmandLine_speed 自动设置加速度需要添加项目号

UCHAR ProID=LIDOPEN;
UCHAR Board_debugFlag=0;

int main(void)
{ 
  u32 	baud;
  U32_T BlinkDelay;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  EnableInterrupts();
  InitSysTick();

  InitPinAsGPIO();														//int	out	led
  AT24CXX_Init();															//iic gpio

  TIM3_PWM_Init(5,0);	 												//PWM=72/6=12M for clock output   
  TIM_SetCompare3(TIM3,2);										//TMC429_CLK=12MHZ p=50%
	
  if(DefaultSettingsToEEPROM())	baud=AT24CXX_ReadLenByte(ADDR_BT,4);else baud=115200;
  InitUART(baud);															//set uart1 with defined baudrate
	
	
	ProID=AT24CXX_ReadOneByte(ADDR_PROJECT_ID);	//get project id and set param for it
	
	switch(ProID)
	{
		case BUTTON_ONLINE:		RampInit_Button();	UART_SendStr(" Fixture Type=BUTTON ONLINE\n>>");	
				break ;
		case BUTTON_OFFLINE:	RampInit_Button();	UART_SendStr(" Fixture Type=BUTTON OFFLINE\n>>");
				break ;
		case BUTTON_VER3:			RampInit_Button();	UART_SendStr(" Fixture Type=BUTTON_VER3\n>>"); 
				break ;
		case BUTTON_ROAD:			RampInit_Button();	UART_SendStr(" Fixture Type=BUTTON ROAD\n>>");		
				
													InitIn8AsExti();			
				break ;
		
		case LIDOPEN:				  RampInit_Lidopen();	UART_SendStr(" Fixture Type=LIDOPEN\n>>");	
				break ;
		case JABIL_LCR:				SW_init_JABIL_LCR();UART_SendStr(" Fixture Type=JABIL LCR\n>>");	LED_BLUE_ON();	
				break ;
		case BASIC_APP:														UART_SendStr(" Fixture Type=IAC LCR\n>>");
				break ;
		default :						  AT24CXX_WriteOneByte(ADDR_PROJECT_ID, BUTTON_OFFLINE);			
												  RampInit_Button();	UART_SendStr(" Fixture Type=BUTTON OFFLINE\n>>");
				break ;
	}
	
	InitSPI();																	//init spi1 and cs1 pin
  Init429();																	//initial motor controller
	
	delay_ms(200);															//wait for all peripheral init ok
	
	UART_SendStr("Fixture Init Ok\n>>");
	UART_Printf("\nfirmware ver3.5 build at %s %s\n\n", __TIME__, __DATE__);
	
  BlinkDelay=GetSysTimer();
  task1Delay=GetSysTimer();
	
  for(;;)
  {		
		//board auto operate from hardware or respond cmd according to project
		if(ProID==BUTTON_ONLINE||ProID==BUTTON_OFFLINE ||ProID==BUTTON_VER3 || ProID==BUTTON_ROAD)
		{
			KeySTART_EXE();
			KeyRESET_EXE();	
			MotorHoming_WithHomeSensor();
		}
		if(ProID == LIDOPEN)
		{
		  if(KeyS_actEN_LidOpen)						//双启控制气缸进出和门开关
		  {
			  KeyS_act_LIDOPEN();
		  }
		  MotorHoming_Lidopen();						//电机回位			
		}
		/*
		PC send cmd to controller 
		*/
		CommandCheckAndExeViaUart1();

		if (ProID==BUTTON_ROAD) releaseAlarmTask();
		
  	if(abs((int)(GetSysTimer()-BlinkDelay))>350)
  	{
			/*
			running indicate
			*/
  		CODE_RUN_LED_TOGGLE();
  		BlinkDelay=GetSysTimer();
  	}
		//board self-test
		if(Board_debugFlag==1)
		{
			IO_DebugViaLedBlinking();	
		}
  }
}
