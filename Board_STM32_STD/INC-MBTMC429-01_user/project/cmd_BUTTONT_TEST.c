#include <stdlib.h>
#include "main.h"
#include "bits.h"
#include <string.h>

#include "Commands.h"
#include "IO.h"
#include "UART.h"
#include "SysControl.h"

//
uint8_t  CommmandLine_Button(char *Commands)
{
	if(strcmp("is homed",Commands)==0)
  {
		if(MotorIsHomed) 	UART_SendStr("HOME<OK>\n");
		else 							UART_SendStr("HOME<NO>\n");
		return 1;
  }	
	if(strcmp("motor reset",Commands)==0)
  {
		UART_SendStr(">>");
		KEY_RESET_pressed=1;
		MotorHoming_preset(ProID);	
//		if(ProID==BUTTON_OFFLINE)	MotorHoming();
//		if(ProID==BUTTON_VER3)	MotorHoming_ver3();
//		if(ProID==BUTTON_ROAD)	MotorHoming_preset_Road();
		//UART_SendStr("<OK>\n");
		return 1;
  }
	if(strcmp("is start",Commands)==0)
  {
		if(KEY_START_pressed) UART_SendStr("START<OK>\n");
		else 									UART_SendStr("START<NO>\n");
		return 1;
  }	
	if(strcmp("is alarm",Commands)==0)
  {
		if(pressureAlarm) 	  UART_SendStr("alarm<1>\n");
		else 									UART_SendStr("alarm<0>\n");
		return 1;
  }	
	else if(strcmp("test over",Commands)==0)
  {
		KEY_START_pressed=0;
		OUTPUT1=0;
		UART_SendStr("<OK>\n");
		return 1;
  }
	else return 0;
}
