#include "bsp.h"

static void 		PrintCommandlist(void);

static uint8_t  CMD_help(char *Commands);

static uint8_t  CMD_sw(char *Commands);
static uint8_t  CMD_hw(char *Commands);


uint8_t  ProcessCommand(char  *Commands)
{
	if			(strcmp("type[os]",	Commands )==0)							{at24cxx.writeU8(at24c256, 16383, 0);	 return 1;}
	else 	if(strcmp("type[bqt1]", Commands )==0)						{at24cxx.writeU8(at24c256, 16383, 1);	 return 1;}
	else 	if(strcmp("type[heat]", Commands )==0)						{at24cxx.writeU8(at24c256, 16383, 2);	 return 1;}
	
	else	if(CMD_help(Commands)==1)									return 1;
	
	else	if(CMD_sw(Commands)==1)										return 1;
	else	if(CMD_hw(Commands)==1)										return 1;
	else  if(CMD_rom(Commands)==1)									return 1;	
	else	if(CMD_fsn(Commands)==1)									return 1;	
	
	else	if(CMD_int(Commands)==1)									return 1;
	else	if(CMD_out(Commands)==1)									return 1;
		
	else	if(CMD_RomPage(Commands)==1)							return 1;		
	
//	else 	if(strcmp("io test", Commands )==0)								{IoTestFlag=1;	 return 1;}

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
//
static uint8_t  CMD_help(char *Commands)
{
	if(strcmp("help",	Commands)==0)				
	{		
		PrintCommandlist();								
		return 1;		
	}
	else return 0;
}
//
static uint8_t  CMD_sw(char *Commands)
{
  if(strcmp("sv",		Commands)==0)			
	{		
		printf("<B188-B235-01>");
		return 1;		
	}
	else return 0;
}
//
static uint8_t  CMD_hw(char *Commands)
{
	if(strcmp("hv",		Commands)==0)			
	{		
		printf("<INC-MB1616007-04>");
		return 1;	
	}
	else return 0;
}
//









