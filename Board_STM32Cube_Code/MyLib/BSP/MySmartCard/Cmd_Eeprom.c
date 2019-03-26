/**
  \file		 Cmd_Eeprom.c
  \version 1.00

  \brief EEPROM Cmd access functions

  This file contains EEPROM Cmd access functions.
*/
#include "usart.h"
#include "eeprom.h"
#include "ctype.h"
#include "tmc429.h"

/*****************					EEPROM  	CMD   以下操作EEPROM空间都已分配定格，读写只涉及1页中的地址 安全*****************/
//1024~16383  17~256页		
uint8_t  MyCLI_RomStr(char *Commands)
{
	//AT24C128为256页前16页*64B---FSN+ROM+......
	if( strncmp(Commands,"page[",5)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[5];	
		u32 PageNum=0;
		u8 DataLen=strlen(Commands);
		PageNum=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='['&& Commands[DataLen-1]==']' )
		{
			char RxData[64];
			uint8_t i;
		 for(i=0;i<64;i++)
		 {
			 RxData[i]=*(p+2+i); 
			 if(RxData[i]==']') {RxData[i]='\0';	break ;} 
		 }
			if(	PageNum>0 && PageNum <=(256-16)	)			 
			{
				WriteEepromBlock(64*(15+PageNum),(u8 *)RxData,64);
				return CMD_SET_OK;
			}	
			else return CMD_PAR_ERR;
		}
	}
	if( strncmp(Commands,"page[",5)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[5];	
		u32 PageNum=0;	
		u8 DataRead[64];
		PageNum=strtol(s, &p, 10);		
		if(	*p==']')
		{
			if( PageNum>0 && PageNum <=(256-16)	)			
			{
				ReadEepromBlock(64*(15+PageNum),	DataRead,64);
				printf("page[%d]<%.64s>\r\n",(u16)PageNum,DataRead);
				return CMD_GET_OK;
			}
			else return CMD_PAR_ERR;
		}
	  else return CMD_ERR;
	}
	else return CMD_NEXT;
}
//384~1023 7~16页  384 --784--	960  ROM[101~340][@]  ROM[1~100][8888.88]
uint8_t  MyCLI_RomNum(char *Commands)
{
	//AT24C128 前7~17=10页640Byte=100个U32数据 +240个U8
	if( strncmp(Commands,"rom[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];	
		u32 RomNum=0;
		Data_4Bytes DataSave;	
		u8 DataLen=strlen(Commands);
		RomNum=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='['&& Commands[DataLen-1]==']' )
		{
			u8 RxTemp=*(p+2);
			//1~100	浮点型数据
			if(	RomNum>0 && RomNum <=100	)			 
			{
				DataSave.f32=strtod(p+2, &s);
				WriteEepromBlock((RomNum-1)*4+6*64,(u8*)DataSave.Byte,4);
				return CMD_SET_OK;
			}	
			//101~340	字符型数据
			else if(RomNum>0 && RomNum <=340	)			 
			{
				WriteEepromBlock((RomNum-1)+300+6*64,&RxTemp,1);
//				printf("ROM[%d]<%c>\r\n",(u16)RomNum,RxTemp);

				return CMD_SET_OK;
			}
			else return CMD_PAR_ERR;
		}
	}
	if( strncmp(Commands,"rom[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];	
		u32 RomNum=0;	
		Data_4Bytes	DataRead;
		RomNum=strtol(s, &p, 10);		
		if(*p==']'&& *(p+1)=='\0')
		{
			if( RomNum>0 && RomNum <=100	)			
			{
				ReadEepromBlock((RomNum-1)*4+6*64,	(u8*)DataRead.Byte ,4);
				printf("ROM[%d]<%.3f>\r\n",(u16)RomNum,DataRead.f32);
				return CMD_GET_OK;
			}
			else if(RomNum>0 && RomNum <=340	)			
			{
				u8 RxTemp;
				ReadEepromBlock((RomNum-1)+300+6*64,	&RxTemp ,1);
				if(isprint(RxTemp)==0) RxTemp=' ';
				printf("ROM[%d]<%c>\r\n",(u16)RomNum,RxTemp);
				return CMD_GET_OK;
			}
			else return CMD_PAR_ERR;
		}
	  else return CMD_ERR;
	}
	else return CMD_NEXT;
}
//前3页0~191 	1~3	保存电机配置等参数 	0   64    128
//后3页192~383 ·4~6 FSN+......				192  256   320
uint8_t  MyCLI_RomFsn(char *Commands)
{
	if(strcmp("fsn?",		Commands)==0)			
	{		
		//3页=64*3 每页写4个字符段 16个
		u8 ReadFsn[20];
		ReadEepromBlock(192,ReadFsn ,20);
		printf("FSN<%.16s>\r\n",ReadFsn);	
		return CMD_GET_OK;	
	}
	else if(strncmp("fsn[",	Commands,4)==0 && Commands[strlen(Commands)-1]==']')	
	{	
		uint8_t i;
		char FsnGet[21];
		for(i=0;i<21;i++)
		{
			FsnGet[i]=Commands[i+4];if(FsnGet[i]==']') {FsnGet[i]='\0';	break ;}
		}
		if(	i>0	&& i<21	)
		{
			WriteEepromBlock(192,(uint8_t*)&FsnGet,20);
			return CMD_SET_OK;	
		}
		else return CMD_PAR_ERR;
	}	
	else return CMD_NEXT;	
}
//
uint8_t  MyCLI_IAP_Flag(char *Commands)
{
	if(strcmp("run app2",	Commands)==0)
	{
		WriteEepromByte(256,'B'); 
		printf("runing the tmcl app...\r\n");	
		HAL_NVIC_SystemReset();
		return CMD_SET_OK;		
	}
	if(strcmp("run app1",	Commands)==0)
	{
		WriteEepromByte(256,'A');
		printf("runing the mycli app...\r\n");	
		HAL_NVIC_SystemReset();
		return CMD_SET_OK;		
	}
	if(strcmp("update app2",	Commands)==0)
	{
		WriteEepromByte(258,'b');
		printf("updating tmcl code...\r\n");	
		HAL_NVIC_SystemReset();
		return CMD_SET_OK;		
	}
	if(strcmp("update app1",	Commands)==0)
	{
		WriteEepromByte(258,'a');
		printf("updating mycli code...\r\n");	
		HAL_NVIC_SystemReset();
		return CMD_SET_OK;
	}
	return CMD_NEXT;
}
//SWITCH_PULSE
uint8_t  MyCLI_INV_SP(char *Commands)
{
	if(strcmp("dir invert!",	Commands)==0)
	{
		if(MotorConfig[0].FlagINVDir) MotorConfig[0].FlagINVDir=0; else MotorConfig[0].FlagINVDir=1;
		
		WriteEepromBlock((UINT) &MotorConfig[0].FlagINVDir-(UINT) &MotorConfig[0],
                               (UCHAR *) &MotorConfig[0].FlagINVDir, sizeof(MotorConfig[0].FlagINVDir));

		return CMD_SET_OK;
	}
	else if(strcmp("ref invert!",	Commands)==0)
	{
		if(MotorConfig[0].FlagINVRef) MotorConfig[0].FlagINVRef=0; else MotorConfig[0].FlagINVRef=1;
		WriteEepromBlock((UINT) &MotorConfig[0].FlagINVRef-(UINT) &MotorConfig[0],
                               (UCHAR *) &MotorConfig[0].FlagINVRef, sizeof(MotorConfig[0].FlagINVRef));

		return CMD_SET_OK;
	}
	else return CMD_NEXT;	
}
//
uint8_t  MyCLI_HomeMode(char *Commands)
{
	if(strncmp("home mode[",	Commands,10)==0	&& Commands[11]==']' && Commands[12]=='[' && Commands[14]==']')
	{
		u8 ActualValue =Commands[13]-'0';
		ActualMotor =Commands[10]-'0';
		if(ActualMotor<3 && 0<ActualValue<5)
		{
			MotorConfig[ActualMotor].HomeMode=ActualValue;
			WriteEepromBlock(ActualMotor*64 + (UINT) &MotorConfig[ActualMotor].HomeMode-(UINT) &MotorConfig[ActualMotor],	\
											(UCHAR *) &MotorConfig[ActualMotor].HomeMode, sizeof(MotorConfig[0].HomeMode));
			return CMD_SET_OK;
		}
		else return CMD_PAR_ERR;
	}
	else return CMD_NEXT;	
}
//
uint8_t  MyCLI_HomeV(char *Commands)
{
	if(strncmp("home v1[",	Commands,7)==0	&& Commands[8]==']' && Commands[9]=='[' && Commands[11]==']')
	{
		u16 ActualValue =Commands[10]-'0';
		ActualMotor =Commands[7]-'0';
		if(ActualMotor<3 )
		{
			MotorConfig[ActualMotor].HomeV1=ActualValue;
			WriteEepromBlock(ActualMotor*64 + (UINT) &MotorConfig[ActualMotor].HomeV1-(UINT) &MotorConfig[ActualMotor],	\
											(UCHAR *) &MotorConfig[ActualMotor].HomeV1, sizeof(MotorConfig[0].HomeV1));
			return CMD_SET_OK;
		}
		else return CMD_PAR_ERR;
	}
	else	if(strncmp("home v2[",	Commands,7)==0	&& Commands[8]==']' && Commands[9]=='[' && Commands[11]==']')
	{
		u16 ActualValue =Commands[10]-'0';
		ActualMotor =Commands[7]-'0';
		if(ActualMotor<3 )
		{
			MotorConfig[ActualMotor].HomeV2=ActualValue;
			WriteEepromBlock(ActualMotor*64 + (UINT) &MotorConfig[ActualMotor].HomeV2-(UINT) &MotorConfig[ActualMotor],	\
											(UCHAR *) &MotorConfig[ActualMotor].HomeV2, sizeof(MotorConfig[0].HomeV2));
			return CMD_SET_OK;
		}
		else return CMD_PAR_ERR;
	}
	else return CMD_NEXT;	
}
//
uint8_t  MyCLI_RomCodeUpdateFlag(char *Commands)
{
	if(strcmp("update code",		Commands)==0)
	{
		WriteEepromByte(258,'a');
		HAL_NVIC_SystemReset();//内部复位都是用内部MOS管拉低复位脚，MOS驱动能力很弱，在复位脚接了一个IMP809,就无法内部复位
////		//复位芯片需要串电阻再接Reset引脚  IMP811手册内部说明可串4.7K
//	
////需要关闭其他中断等配置 否则直接跳转后死机,为什么不能直接调用下面代码进入BootLoad,中断|os有关？
//		__set_PRIMASK(1);
//		HAL_UART_DeInit(&huart1);
//		JumpAddress = *(__IO uint32_t*) (0x08000000  + 4);
//    JumpToApplication = (pFunction) JumpAddress;
//    /* Initialize user application's Stack Pointer */
//    __set_MSP(*(__IO uint32_t*) 0x08000000);
//    JumpToApplication();
			
		return CMD_SET_OK;	
	}
	else return CMD_NEXT;	
}
//
uint8_t  MyCLI_ReadWtiteEEPROM(char *Commands)
{
	if( strncmp(Commands,"eeprom[",7)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[7];	
		u32 RomNum=0;
		u8 	DataLen=strlen(Commands);
		RomNum=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='['&& Commands[DataLen-1]==']' && RomNum <=16383)
		{
			WriteEepromByte(RomNum,*(p+2));
			return CMD_SET_OK;
		}
	}
	if( strncmp(Commands,"eeprom[",7)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[7];	
		u32 RomNum=0;	
		RomNum=strtol(s, &p, 10);		
		if(*p==']'&& *(p+1)=='\0')
		{
			if( RomNum < 16384)			
			{
				printf("EEPROM[%d]<%c>\r\n",(u16)RomNum,ReadEepromByte(RomNum));
				return CMD_GET_OK;
			}
			else return CMD_PAR_ERR;
		}
	  else return CMD_ERR;
	}
	else return CMD_NEXT;		
}
//




