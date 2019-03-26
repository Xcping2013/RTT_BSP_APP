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

/*****************					EEPROM  	CMD   ���²���EEPROM�ռ䶼�ѷ��䶨�񣬶�дֻ�漰1ҳ�еĵ�ַ ��ȫ*****************/
//1024~16383  17~256ҳ		
uint8_t  MyCLI_RomStr(char *Commands)
{
	//AT24C128Ϊ256ҳǰ16ҳ*64B---FSN+ROM+......
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
//384~1023 7~16ҳ  384 --784--	960  ROM[101~340][@]  ROM[1~100][8888.88]
uint8_t  MyCLI_RomNum(char *Commands)
{
	//AT24C128 ǰ7~17=10ҳ640Byte=100��U32���� +240��U8
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
			//1~100	����������
			if(	RomNum>0 && RomNum <=100	)			 
			{
				DataSave.f32=strtod(p+2, &s);
				WriteEepromBlock((RomNum-1)*4+6*64,(u8*)DataSave.Byte,4);
				return CMD_SET_OK;
			}	
			//101~340	�ַ�������
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
//ǰ3ҳ0~191 	1~3	���������õȲ��� 	0   64    128
//��3ҳ192~383 ��4~6 FSN+......				192  256   320
uint8_t  MyCLI_RomFsn(char *Commands)
{
	if(strcmp("fsn?",		Commands)==0)			
	{		
		//3ҳ=64*3 ÿҳд4���ַ��� 16��
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
		HAL_NVIC_SystemReset();//�ڲ���λ�������ڲ�MOS�����͸�λ�ţ�MOS���������������ڸ�λ�Ž���һ��IMP809,���޷��ڲ���λ
////		//��λоƬ��Ҫ�������ٽ�Reset����  IMP811�ֲ��ڲ�˵���ɴ�4.7K
//	
////��Ҫ�ر������жϵ����� ����ֱ����ת������,Ϊʲô����ֱ�ӵ�������������BootLoad,�ж�|os�йأ�
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




