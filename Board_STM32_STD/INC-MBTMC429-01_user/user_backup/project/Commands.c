/**
  \file 		Commands.c
  \author 	Xcping2016
  \version 	1.00
*/

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "bits.h"
#include "main.h"
#include "Commands.h"
#include "Globals.h"
#include "UART.h"
#include "SysTick.h"
#include "TMC429.h"
#include "Eeprom.h"
#include "SysControl.h"

#include "IO.h"
#include "lidopen.h"
#include "JABIL_LCR.h"

static void 		MakeLowercase( char *str);
static void 		PrintCommandlist(void);
static uint8_t  CommmandLine_help(char *Commands);
void board_debug(void);

static void MakeLowercase( char *str)		
{
	while(*str!='\0')
	{
			if( ((*str)>='A')&&((*str)<='Z') )
			{
					*str=*str+32;
			}
			str++;
	}
}

//
uint8_t  ProcessCommand(char  *Commands)
{
	MakeLowercase(Commands);	

	if		  (CommmandLine_help(Commands)==1)								return 1;
	
	else	if(CommmandLine_in(Commands)==1)									return 1;
	else	if(CommmandLine_out(Commands)==1)									return 1;
	else	if(CommmandLine_rgb(Commands)==1)									return 1;
	
	else	if(CommmandLine_baud(Commands)==1)								return 1;
	else	if(CommmandLine_sv(Commands)==1)									return 1;
	else	if(CommmandLine_hv(Commands)==1)									return 1;
	else	if(CommmandLine_fsn(Commands)==1)									return 1;	
	else  if(CommmandLine_rom(Commands)==1)									return 1;	
	else	if(CommmandLine_RomPage(Commands)==1)							return 1;		

	else	if(CommmandLine_speed(Commands)==1)								return 1;
	else	if(CommmandLine_acc(Commands)==1)									return 1;
	else	if(CommmandLine_rotate(Commands)==1)							return 1;		
	else	if(CommmandLine_move(Commands)==1)								return 1;
	
	else	if(CommmandLine_pos(Commands)==1)									return 1;
	else	if(CommmandLine_limit(Commands)==1)								return 1;
	else	if(CommmandLine_reach(Commands)==1)								return 1;	
	
	else	if(CommmandLine_Button(Commands)==1)							return 1;
	else	if(CommmandLine_Lidopen(Commands)==1)							return 1;
	else	if(CommmandLine_IAC_LCR(Commands)==1)							return 1;
	else	if(CommmandLine_JABIL_LCR(Commands)==1)						return 1;	
//	else if(strcmp("reset",		Commands)==0)								{		ResetCPU(TRUE);																														return 1;		}		

//board iO test
	else if(strcmp("io test",							Commands)==0)	{		Board_debugFlag=1;ProID=PROJECT_NONE;									UART_SendStr("OK\n");					return 1;		}		
//Project switch	
	else if(strcmp("type[universal]",	Commands)==0)			{		AT24CXX_WriteOneByte(ADDR_PROJECT_ID,BASIC_APP);			UART_SendStr("OK\n");					return 1;		}		
	else if(strcmp("type[button online]",	Commands)==0)	{		AT24CXX_WriteOneByte(ADDR_PROJECT_ID,BUTTON_ONLINE);	UART_SendStr("OK\n");					return 1;		}		
	else if(strcmp("type[button offline]",Commands)==0)	{		AT24CXX_WriteOneByte(ADDR_PROJECT_ID,BUTTON_OFFLINE);	UART_SendStr("OK\n");					return 1;		}		
	else if(strcmp("type[button ver3]",		  Commands)==0)	{	AT24CXX_WriteOneByte(ADDR_PROJECT_ID,BUTTON_VER3);		UART_SendStr("OK\n");					return 1;		}			

	else if(strcmp("type[lidopen]",				Commands)==0)	{		AT24CXX_WriteOneByte(ADDR_PROJECT_ID,LIDOPEN);				UART_SendStr("OK\n");					return 1;		}	
	else if(strcmp("type[iac lcr]",				Commands)==0)	{		AT24CXX_WriteOneByte(ADDR_PROJECT_ID,IAC_LCR);				UART_SendStr("OK\n");					return 1;		}	
	else if(strcmp("type[jabil lcr]",		  Commands)==0)	{		AT24CXX_WriteOneByte(ADDR_PROJECT_ID,JABIL_LCR);			UART_SendStr("OK\n");					return 1;		}			
	else if(strcmp("type[button ver3]",		  Commands)==0)	{	AT24CXX_WriteOneByte(ADDR_PROJECT_ID,JABIL_LCR);			UART_SendStr("OK\n");					return 1;		}			

	else if(strcmp("board_debug()",		  Commands)==0)	{		board_debug();																																				return 1;		}			

	else
	{
		return 0;
	}
}
/**				List the command list and discription--->succinctly、Unified specification	简洁明了，规范统一 发送小写，返回大写
**
**/
static void PrintCommandlist(void)
{	
	
	UART_SendStr("\nFirmware：20180426\n");
	UART_SendStr("\nproject select:\n");
	UART_SendStr("type[button online]----------no cylinder\n");	
	UART_SendStr("type[button offline]---------cylinder\n");
	UART_SendStr("type[lidopen]----------------lidopen\n");	
	UART_SendStr("type[iac lcr]----------------iac lcr\n");	
	UART_SendStr("type[jabil lcr]--------------jabil lcr\n");	
	
	UART_SendStr("\ninputs and outputs:\n");	
	UART_SendStr("int[1~18]--------------------get input\n");
	UART_SendStr("out[1~14][1 or 0]------------set output\n");
	UART_SendStr("rgb[r|g|b| ]-----------------set rgb led\n");
	UART_SendStr("inputs-----------------------get inputs\n");
	//UART_SendStr("outputs[101010101010101]-----Set outputs status\n");
	UART_SendStr("io test----------------------board debug\n");	
	
	UART_SendStr("\nsave and read data:\n");	
	UART_SendStr("fsn--------------------------read sn\n");	
	UART_SendStr("fsn[20180324]----------------save sn max to 10 chars\n");	
	UART_SendStr("rom[1~100]-------------------read floating number\n");
	UART_SendStr("rom[1~100][2018]-------------save floating number\n");
	UART_SendStr("read info 1~200--------------read max 64 bytes from position\n");
	UART_SendStr("save info 1~200 YYYYY--------write max 64 bytes to position\n");
	//UART_SendStr("baudRate[115200]------------Set the baudrate to 115200\n");
	if( ProID==BUTTON_ONLINE || ProID==BUTTON_OFFLINE || ProID==LIDOPEN || ProID==BUTTON_VER3 )
	{
		UART_SendStr("\nread motion status:\n");	
		UART_SendStr("is left #0~2---REF[0~2]<1>----------read L limit\n");
		UART_SendStr("is right #0~2--REF[0~2]<1>----------read R limit\n");
		UART_SendStr("is limit-------LIMIT<L3R3L2R2L1R1>--read limit\n");
		UART_SendStr("is reach #0~2--REACH[0~2]<1>--------is targeted\n");
		UART_SendStr("is stop #0~2---STOP[0~2]<1>---------is stop\n");
		
		UART_SendStr("\nset motion parameters:\n");	
		UART_SendStr("0~2# velocity 0~2047----------------set the velocity to xx\n");
		UART_SendStr("all# velocity 0~2047----------------set all velocity to xx\n");
		UART_SendStr("0~2# acc 0~2047---------------------set the acc to n\n");	
		UART_SendStr("all# acc 0~2047---------------------set the acc to all\n");
		
		UART_SendStr("\nget motion parameters:\n");	
		UART_SendStr("velocity #0~2-----------------------get the velocity\n");
		UART_SendStr("velocity #all-----------------------get velocitys\n");	
		UART_SendStr("acc #0~2----------------------------get the acc\n");
		UART_SendStr("acc #all----------------------------get the accs\n");
		UART_SendStr("position #0~2-----------------------get current position\n");
		UART_SendStr("position #all-----------------------get current positions\n");
			
		UART_SendStr("\nmotor movement:\n");	
		UART_SendStr("n# rotate +/-xx--<OK>--------------Rotate Right or left\n");
		UART_SendStr("n# move to +/-xx-<OK>--------------#n to ABS POS -/-xx\n");
		UART_SendStr("n# move +/-xx----<OK>--------------#n to REL POS +/-xx\n");
		UART_SendStr("n# stop---------STOP[n]<OK>--------#n stop\n");
	}
	switch(ProID)
	{
		case LIDOPEN :
				{
					UART_SendStr("\nLidopen:\n");	
					UART_SendStr("button enable-----<OK>-------enable the button for next test\n");
					UART_SendStr("button disable----<OK>-------disable the button function\n");
					UART_SendStr("is button ok---<OK/NO>-------get button status\n");
					UART_SendStr("motor go home---<OK>---------start homing\n");
					UART_SendStr("is homed-------HOME<OK>------check motor homed status\n");		
					UART_SendStr("ready to test----------------door action ok\n");
				}
			break;
		case BUTTON_ONLINE:
		case BUTTON_OFFLINE:
		case BUTTON_VER3:
			
				{
					UART_SendStr("\nButton:\n");	
					UART_SendStr("motor reset------<OK>--------start homing\n");
					UART_SendStr("test over--------<OK>--------enable key-start for next test\n"); 
					UART_SendStr("is homed-------HOME<OK>------check motor homed status\n");		
					UART_SendStr("is start-------START<OK>-----check key-start pressed status\n\n");	
				}
			break;
		case JABIL_LCR:
				{
					UART_SendStr("\nJABIL LCR:\n");	
					UART_SendStr("measure coil1 to coil2-------connect LCR between coil and coil2\n");
					UART_SendStr("measure coil1 to gnd---------connect LCR between coil and gnd\n");
					UART_SendStr("measure coil2 to gnd---------connect LCR between coi2 and gnd\n\n");
					UART_SendStr("enable start key-----<OK>----enable start key action\n");
					UART_SendStr("disable start key----<OK>----disable the start key\n");
					UART_SendStr("ready to test?---<YES/NO>----start Key act status\n");
					UART_SendStr("ready to test----------------start action ok\n");
				}
			break ;
		default : break;
	}
	UART_SendStr("\nPlease send command as above end with \\r\\n\n");
}
//
//-----------------------------------------------------------common	start
static uint8_t  CommmandLine_help(char *Commands)
{
	if(strcmp("help",	Commands)==0)				
	{		
		PrintCommandlist();								
		return 1;		
	}
	else return 0;
}
//-------------------------------------------------------------
static void hw_in_out_debug(void)
{
	u8 ret_ok=1;
	
	OUTPUT1=0;	OUTPUT2=0;	OUTPUT3=0;	OUTPUT4=0;
	OUTPUT5=0;  OUTPUT6=0;	OUTPUT7=0;	OUTPUT8=0;
													if(INPUT1==1) 	{UART_SendStr("input1 or rgb_5v ............err\r\n");ret_ok=0;}
	RGB_R=1;RGB_G=0;RGB_B=0;delay_ms(10);
													if(INPUT2==1) 	{UART_SendStr("input2 or rgb_r .............err\r\n"); ret_ok=0;}
													if(INPUT3==0) 	{UART_SendStr("input3 or rgb_g .............err\r\n"); ret_ok=0;}
													if(INPUT4==0) 	{UART_SendStr("input4 or rgb_b .............err\r\n"); ret_ok=0;}
	RGB_R=0;RGB_G=1;RGB_B=0;delay_ms(10);										        
													if(INPUT2==0) 	{UART_SendStr("input2 or rgb_r .............err\r\n"); ret_ok=0;}
													if(INPUT3==1) 	{UART_SendStr("input3 or rgb_g .............err\r\n"); ret_ok=0;}
													if(INPUT4==0) 	{UART_SendStr("input4 or rgb_b .............err\r\n"); ret_ok=0;}
	RGB_R=0;RGB_G=0;RGB_B=1;delay_ms(10);												     
													if(INPUT2==0) 	{UART_SendStr("input2 or rgb_r .............err\r\n"); ret_ok=0;}
													if(INPUT3==0) 	{UART_SendStr("input3 or rgb_g .............err\r\n"); ret_ok=0;}
													if(INPUT4==1) 	{UART_SendStr("input4 or rgb_b .............err\r\n"); ret_ok=0;}	
														
	OUTPUT1=1;delay_ms(10);	if(INPUT5==1) 	{UART_SendStr("input5 or output1 ...........err\r\n");		ret_ok=0;}
	OUTPUT1=0;delay_ms(10);	if(INPUT5==0) 	{UART_SendStr("input5 or output1 ...........err\r\n");		ret_ok=0;}
	                                                                  
	OUTPUT2=1;delay_ms(10);	if(INPUT6==1) 	{UART_SendStr("input6 or output2 ...........err\r\n");   ret_ok=0;}
	OUTPUT2=0;delay_ms(10);	if(INPUT6==0) 	{UART_SendStr("input6 or output2 ...........err\r\n");   ret_ok=0;}
	                                                                   
	OUTPUT3=1;delay_ms(10);	if(INPUT7==1) 	{UART_SendStr("input7 or output3 ...........err\r\n");   ret_ok=0;}
	OUTPUT3=0;delay_ms(10);	if(INPUT7==0) 	{UART_SendStr("input7 or output3 ...........err\r\n");   ret_ok=0;}
	                                                                  
	OUTPUT4=1;delay_ms(10);	if(INPUT8==1) 	{UART_SendStr("input8 or output4 ...........err\r\n");   ret_ok=0;}
	OUTPUT4=0;delay_ms(10);	if(INPUT8==0) 	{UART_SendStr("input8 or output4 ...........err\r\n");   ret_ok=0;}
	          
	OUTPUT5=1;delay_ms(10);	if(INPUT9==1) 	{UART_SendStr("input9 or output5 ...........err\r\n");   ret_ok=0;}
	OUTPUT5=0;delay_ms(10);	if(INPUT9==0) 	{UART_SendStr("input9 or output5 ...........err\r\n");   ret_ok=0;}
            
	OUTPUT6=1;delay_ms(10);	if(INPUT10==1) 	{UART_SendStr("input10 or output6 ..........err\r\n");   ret_ok=0;}
	OUTPUT6=0;delay_ms(10);	if(INPUT10==0) 	{UART_SendStr("input10 or output6 ..........err\r\n");   ret_ok=0;}
	          
	OUTPUT7=1;delay_ms(10);	if(INPUT11==1) 	{UART_SendStr("input11 or output7 ..........err\r\n");   ret_ok=0;}
	OUTPUT7=0;delay_ms(10);	if(INPUT11==0) 	{UART_SendStr("input11 or output7 ..........err\r\n");   ret_ok=0;}
																									 
	OUTPUT8=1;delay_ms(10);	if(INPUT12==1) 	{UART_SendStr("input12 or output8 ..........err\r\n");   ret_ok=0;}   
	OUTPUT8=0;delay_ms(10);	if(INPUT12==0) 	{UART_SendStr("input12 or output8 ..........err\r\n");   ret_ok=0;}   
	if(ret_ok)	UART_SendStr("inputs and outputs test......ok\r\n");	
	
}

static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}
static void hw_eeprom_debug(void)
{
	u8 *save_info="this page data save for eeprom test";
	u8 read_info[64];
	u8 len=strlen(save_info);
					
	UART_SendStr("eeprom test..................");
	IIC_DeviceWriteBytes(0xA0,100*64,save_info,len,2);
	delay_ms(10);
	IIC_DeviceReadBytes(0xA0,100*64,read_info,len,2);
	
  if(Buffercmp((uint8_t*)save_info,(uint8_t*)read_info,len))
  {
    UART_SendStr("err\r\n");
  }	
	else UART_SendStr("ok\r\n");
	
}

static void hw_motor_debug(void)
{
	UART_SendStr("tmc429 test..................");
	if(Read429SingleByte(IDX_REF_SWITCHES, 3)==0) 
		UART_SendStr("err");

	else 
	{
		UART_SendStr("ok\r\n最后:检测限位采集电路");
		UART_SendStr("\r\n先发送is limit采集数据再把L1接GND后发送is limit有变化才ok\r\n");	
	}
}

void board_debug(void)
{
	//UART_SendStr("\r\n");
	hw_in_out_debug();
	hw_eeprom_debug();
	if( ProID==BUTTON_ONLINE || ProID==BUTTON_OFFLINE || ProID==BUTTON_VER3 || ProID==LIDOPEN || BASIC_APP )
	{
		hw_motor_debug();
	}
}
//
