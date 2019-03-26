/**
  \file 		Commands.c
  \author 	Xcping2016
  \version 	20161016:				Ver1.2
*/

#include "commands.h"
#include "usart.h"
#include "gpio.h"
#include "ctype.h"
#include "TMC429.h"
#include "SysControl.h"
#include "EEPROM.h"

static uint8_t 	MyCLI_Version(char *str);
static uint8_t  MyCLI_GetCmdList(char *str);
static uint8_t  MyCLI_SysReset(char *str);
static void 		MakeUpercase( char *str);
static void 		MakeLowercase( char *str);

/************************************************************
*************************************************************/
uint8_t  CommandCheckExe(char  *str)
{
	u8 i; char *Commands;u8 ReplyType;
	for(i = 0; str[i] != '\0'; i++) 
	{
		if(isgraph(str[i])) 									//printable character
		{
			Commands=&str[i];
			MakeLowercase(Commands);	
			ReplyType=MyCLI_GetCmdList(Commands);							if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_Version(Commands);								if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_SysReset(Commands);								if(ReplyType!=CMD_NEXT) return ReplyType;
				
			ReplyType=MyCLI_GetINT(Commands);									if(ReplyType!=CMD_NEXT) return ReplyType;		
			ReplyType=MyCLI_SetOUT(Commands);									if(ReplyType!=CMD_NEXT) return ReplyType;	
			ReplyType=MyCLI_SetLED(Commands);									if(ReplyType!=CMD_NEXT) return ReplyType;								
		
			ReplyType=MyCLI_RomStr(Commands);									if(ReplyType!=CMD_NEXT) return ReplyType;								
			ReplyType=MyCLI_RomNum(Commands);									if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_RomFsn(Commands);									if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_ReadWtiteEEPROM(Commands);				if(ReplyType!=CMD_NEXT) return ReplyType;
			
			ReplyType=MyCLI_IAP_Flag(Commands);								if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_INV_SP(Commands);									if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_HomeMode(Commands);								if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_HomeV(Commands);									if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_RomCodeUpdateFlag(Commands);			if(ReplyType!=CMD_NEXT) return ReplyType;

			ReplyType=MyCLI_MotorPos(Commands);								if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_MotorPulseDiv(Commands);					if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_MotorRampDiv(Commands);						if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_MotorV(Commands);									if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_MotorAcc(Commands);								if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_MotorRot(Commands);								if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_MotorMove(Commands);							if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_MotorStop(Commands);							if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_MotorLS(Commands);								if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_MotorStatus(Commands);						if(ReplyType!=CMD_NEXT) return ReplyType;
		
			ReplyType=MyCLI_MotorGoHome(Commands);						if(ReplyType!=CMD_NEXT) return ReplyType;
			ReplyType=MyCLI_StatusEN(Commands);								if(ReplyType!=CMD_NEXT) return ReplyType;		

			return CMD_ERR;
		}
	}
	return CMD_ERR;
}

static void MakeUpercase( char *str)
{
	while(*str!='\0')
	{
			if( ((*str)>='a')&&((*str)<='z') )
			{
					*str=*str-32;
			}
			str++;
	}	
}
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
static uint8_t MyCLI_Version(char *str)
{
	if(strcmp("version?",	str)==0)
	{
		printf("Board: HV<3.0>, SV<3.0>.\r\n");
		return CMD_GET_OK;
	}
	else return CMD_NEXT;
}
static uint8_t MyCLI_GetCmdList(char *str)
{
	if(strcmp("help?",	str)==0)				
	{	
		printf("==============Command List:==============\r\n");	
		printf("============Must End With\\r\\n\r\n\r\n");				
		
		printf("VERSION?-------------------Get board info\r\n");
		//printf("cpu reset!-------------------Reset cpu\r\n");
		printf("**************DI DO***************************\r\n");
		printf("INT[N]---------------------Get the input status\r\n");
		printf("OUT[N][Y]------------------Set the output status\r\n");
		printf("RGB[N]---------------------Set Led rgb on\r\n");
		printf("BLINK[N][Y]----------------Set led Blinking\r\n");
		printf("**************EEPROM********************\r\n");		
		printf("FSN?-----------------------Read FSN\r\n");	
		printf("FSN[Y]---------------------Write FSN\r\n");		
		printf("ROM[N]---------------------Read Datas\r\n");
		printf("ROM[N][Y]------------------Write Datas\r\n");
		printf("PAGE[N]--------------------Read Datas from page\r\n");
		printf("PAGE[N][Y]-----------------Write Datas to page\r\n");
		//printf("run app1-----------------\r\n");
		//printf("update app1--------------\r\n");
		printf("**************MOTOR ******************\r\n");	
		printf("PDIV[N][Y]-----------------Set motor v div\r\n");
		printf("RDIV[N][Y]-----------------Set motor acc div\r\n");
		printf("REF INVERT!----------------Set switch invert\r\n");
		printf("DIR INVERT!----------------Motor turnaround\r\n");
		
		printf("HOME MODE[N][Y]------------Set home mode\r\n");
		printf("HOME V1[N][Y]--------------Set home's v\r\n");
		printf("HOME V2[N][Y]--------------Set home's v\r\n");
		
		printf("POSITION[N]----------------Read motor position\r\n");
		printf("POSITION?------------------Read motors position\r\n");
		printf("VELOCITY[N][Y]-------------Set v to motor n\r\n");
		printf("VELOCITY[N]----------------Get v from motor n\r\n");
		printf("VELOCITY?------------------Get v from motors\r\n");
		printf("ACC[N][Y]------------------Set acc to motor n\r\n");
		printf("ACC[N]---------------------Get acc from motor n\r\n");
		printf("ACC?-----------------------Get acc from motors\r\n");
		printf("ROTATE[N][V]---------------set velocity and rotate motor\r\n");
		printf("STOP[N]--------------------Stop motor\r\n");
		printf("STOP!----------------------Stop motors\r\n");
		printf("MOVE ABS[N][Y]--------------N Move to Y\r\n");
		printf("MOVE REF[N][Y]--------------N Move + Y\r\n");
		printf("LIMIT?---------------------Check out switch status\r\n");
		printf("REACH?---------------------Is motor reach position\r\n");
		printf("STOP?----------------------Is motor stop\r\n");
		printf("**************MOTION ******************\r\n");	
		printf("HOME[N]--------------------Motor go home\r\n");
		printf("HOME!----------------------Motors go home\r\n");
		printf("HOME?----------------------Is motor homed\r\n");
		
//		printf("BUTTON ENABLE--------------Enable button process\r\n");
//		printf("BUTTON DISABLE-------------Disable button process\r\n");
//		printf("READY?---------------------Check out pre-process is done\r\n\r\n");									
		printf("\r\n============================================================\r\n");
		return CMD_GET_OK;
	}
	else 
	{
		return CMD_NEXT;
	}
}
//
static uint8_t MyCLI_SysReset(char *str)
{
  if(strcmp("cpu reset!",	str)==0)	
	{
	  //SCB->AIRCR = SCB_AIRCR_VECTKEY_Msk | (u32)0x04;
		NVIC_SystemReset();
		return CMD_SET_OK;
	}
  else if(strcmp("core reset!", str)==0)
	{
	  SCB->AIRCR = SCB_AIRCR_VECTKEY_Msk | (u32)0x01;
		return CMD_SET_OK;
	}
	else return CMD_NEXT;
}
//
void InitTMCL(void)
{
	uint8_t i;

	if (ReadEepromByte(191) != 'O')
	{	
		WriteEepromByte(191, 'O');
		WriteEepromBlock(0, 	(UCHAR *)&MotorConfig[0], sizeof(TMotorConfig));
		WriteEepromBlock(64,  (UCHAR *)&MotorConfig[0], sizeof(TMotorConfig));
		WriteEepromBlock(128, (UCHAR *)&MotorConfig[0], sizeof(TMotorConfig));	
	}
	for(i=0; i<N_O_MOTORS; i++) 	
	{		
		ReadEepromBlock(i*64, (UCHAR *)&MotorConfig[i], sizeof(TMotorConfig));
		SpeedChangedFlag[i]=TRUE;
	}
}
//



