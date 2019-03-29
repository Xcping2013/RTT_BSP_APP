#include "app_serial.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>	//abs strtol

static void	PrintCommandlist(void);

uint8_t  ProcessCommand(char  *Commands)
{
	if	 (!strcmp("help",	Commands ))	{	PrintCommandlist();	 	return 1;	}
	
		
	else	if(CMD_RomPage(Commands)==1)							return 1;		
	
	else
	{
		printf("<NA>");	return 0;
	}
}
/**				List the command list and discription
**
**/
static void PrintCommandlist(void)
{
	printf("\r\n\r\n");
	printf("sv-------------------get software version\r\n");
	printf("hv-------------------get hardware version\r\n");
	printf("fsn------------------get fixture serial number\r\n");	
	printf("fsn[]----------------set fixture serial number\r\n");	
	printf("rom[]----------------read roms\r\n");
	printf("rom[][]--------------write roms\r\n");
	printf("\r\n");
	printf("save info xx yyyyy---write datas\r\n");
	printf("read info xx---------read datas\r\n");
	printf("\r\n");
	printf("int[]----------------get the input status\r\n");
	printf("out[][]--------------set the output status\r\n");
	printf("\r\n");
	printf("inputs\r\n");
	printf("outputs[16ch]--------set the outputs status\r\n");
	printf("io test--------------input trigger output\r\n");
	printf("\r\n");
	
	printf("type[os]\r\n");
	printf("type[bqt1]\r\n");
	printf("type[heat]\r\n");
	
	printf("Please send command as above end with \\r\\n\n");
}

void Task1_app(void)
{
	
}
void Task1NoneRTOS(void)
{
	uint32_t tickTask1start;	
	
	tickTask1start=HAL_GetTick();
	
	if(abs((int)(HAL_GetTick()-tickTask1start))>350)
	{
		Task1_app();
		tickTask1start=HAL_GetTick();
	}	
}

