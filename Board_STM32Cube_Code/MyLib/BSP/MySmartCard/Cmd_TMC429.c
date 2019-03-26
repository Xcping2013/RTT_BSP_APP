/**
  \file		 Cmd_TMC429.c
  \version 1.00

  \brief TMC429 Cmd access functions

  This file contains TMC429 Cmd access functions.
*/
#include "usart.h"
#include "tmc429.h"
#include "eeprom.h"


//读取电机位置
uint8_t MyCLI_MotorPos(char *Commands)
{
	if( strncmp(Commands,"position[",9)==0 &&Commands[10]==']'&& Commands[11]=='\0' ) 
	{		
		ActualMotor=Commands[9]-'0';	
		if(ActualMotor<N_O_MOTORS)
		{
			MotorConfig[ActualMotor].XACTUAL=Read429Int(IDX_XACTUAL|(ActualMotor<<5));
			printf("P[%d]<%d>\r\n",ActualMotor,MotorConfig[ActualMotor].XACTUAL);
			return CMD_GET_OK;
		}
		else return CMD_PAR_ERR;			
	}	
	if(strcmp(Commands,"position?")==0 ) 
	{
		MotorConfig[0].XACTUAL=Read429Int(IDX_XACTUAL|(0<<5));
		MotorConfig[1].XACTUAL=Read429Int(IDX_XACTUAL|(1<<5));
		MotorConfig[2].XACTUAL=Read429Int(IDX_XACTUAL|(2<<5));
		printf("P[0]<%d>,P[1]<%d>,P[2]<%d>\r\n",MotorConfig[0].XACTUAL,MotorConfig[1].XACTUAL,MotorConfig[2].XACTUAL);
		return CMD_GET_OK;
	}
	else return CMD_NEXT;	
}
//
uint8_t  MyCLI_MotorV(char *Commands)
{
	if(strncmp(Commands,"velocity[",9)==0&&Commands[10]==']'&& Commands[11]=='[' ) 
  {	
		char *p = NULL;
		char *s = &Commands[12];			
		ActualMotor=Commands[9]-'0';			
		MotorConfig[ActualMotor].VMax=strtol(s, &p, 10);	
		if(*(p)==']'&& *(p+1)=='\0' )
		{
			u8 len=p-s;		
			if(ActualMotor<N_O_MOTORS &&(0<len<=4))
			{
				Write429Short(IDX_VMAX|(ActualMotor<<5), MotorConfig[ActualMotor].VMax);
				return CMD_SET_OK;
			}
			else return CMD_PAR_ERR;
    }
		else return CMD_ERR;	
	}
	if(strncmp(Commands,"velocity[",9)==0&&Commands[10]==']'&& Commands[11]=='\0' ) 
  {
		ActualMotor=Commands[9]-'0';
		if(ActualMotor<N_O_MOTORS)
		{
			MotorConfig[ActualMotor].VACTUAL=Read429Short(IDX_VACTUAL|(ActualMotor<<5));
			printf("Vset[%d]<%d>,Vnow[%d]<%d>\r\n",ActualMotor,MotorConfig[ActualMotor].VMax,ActualMotor,MotorConfig[ActualMotor].VACTUAL);
			return CMD_GET_OK;
		}	
		else return CMD_PAR_ERR;		
	}
	if(strcmp(Commands,"velocity?")==0 ) 
  {
		u8 i;
		MotorConfig[0].VACTUAL=Read429Short(IDX_VACTUAL|(0<<5));
		MotorConfig[1].VACTUAL=Read429Short(IDX_VACTUAL|(1<<5));
		MotorConfig[2].VACTUAL=Read429Short(IDX_VACTUAL|(2<<5));
		for(i=0;i<3;i++)
		{
			printf("Vset[%d]<%d>,Vnow[%d]<%d>\r\n",i,MotorConfig[i].VMax,i,MotorConfig[i].VACTUAL);
		}
		return CMD_GET_OK;	
  }
	else return CMD_NEXT;	
}

//
uint8_t  MyCLI_MotorAcc(char *Commands)
{
	//设置指定电机加速度
	if(strncmp(Commands,"acc[",4)==0&&Commands[5]==']'&& Commands[6]=='[' ) 
  {	
		char *p = NULL;
		char *s = &Commands[7];			
		ActualMotor=Commands[4]-'0';
		MotorConfig[ActualMotor].AMax=strtol(s, &p, 10);	
		if(*(p)==']'&& *(p+1)=='\0' )
		{			
			if(ActualMotor<N_O_MOTORS &&(0<(p-s)<=4))
			{
			  SetAMax(ActualMotor, MotorConfig[ActualMotor].AMax);
				return CMD_SET_OK;
			}
			else return CMD_PAR_ERR;
    }
		else return CMD_ERR;	
	}
	//读取指定电机加速度
	if(strncmp(Commands,"acc[",4)==0&&Commands[5]==']'&& Commands[6]=='\0' ) 
  {
		ActualMotor=Commands[4]-'0';
		if(ActualMotor<N_O_MOTORS)
		{
			MotorConfig[ActualMotor].AMax=Read429Short(IDX_AMAX|(ActualMotor<<5));
			printf("ACC[%d]<%d>\r\n",ActualMotor,MotorConfig[ActualMotor].AMax);
			return CMD_GET_OK;
		}	
		else return CMD_PAR_ERR;		
	}
	if(strcmp(Commands,"acc?")==0 ) 
  {
		u8 i;
		for(i=0;i<3;i++)
		{
			printf("ACC[%d]<%d>\r\n",i,MotorConfig[i].AMax);
		}
		return CMD_GET_OK;	
  }
	else return CMD_NEXT;	
}

//
uint8_t MyCLI_MotorRot(char *Commands)
{
	if(strncmp(Commands,"rotate[",7)==0&&Commands[8]==']'&& Commands[9]=='[' ) 
  {
		char *p = NULL;
		char *s = &Commands[10];		
		ActualMotor=Commands[7]-'0';		
		MotorConfig[ActualMotor].VTARGET=strtol(s, &p, 10);	
		if(*(p)==']'&& *(p+1)=='\0' )
		{
			if(ActualMotor<N_O_MOTORS &&(0<(p-s)<=4))
			{
				SpeedChangedFlag[ActualMotor]=TRUE;
				Set429RampMode(ActualMotor, RM_VELOCITY);
				Write429Short(IDX_VMAX|(ActualMotor<<5), 2047);
				Write429Short(IDX_VTARGET|(ActualMotor<<5), MotorConfig[ActualMotor].VTARGET);			
				return CMD_SET_OK;
			}
			else return CMD_PAR_ERR;
		}
		else return CMD_ERR;
	}
	else return CMD_NEXT;	
}
//
uint8_t MyCLI_MotorStop(char *Commands)
{
	if(strncmp(Commands,"stop[",5)==0&&Commands[6]==']'&& Commands[7]=='\0' ) 
  {
		ActualMotor=Commands[5]-'0';
		if(ActualMotor<N_O_MOTORS)
		{
			Set429RampMode(ActualMotor, RM_VELOCITY);
			Write429Zero(IDX_VTARGET|(ActualMotor<<5));						
			return CMD_SET_OK;
		}	
		else return CMD_PAR_ERR;		
	}
	if(strcmp(Commands,"stop!")==0 ) 
  {		
		for(ActualMotor=0;ActualMotor<3;ActualMotor++)
		{
			Set429RampMode(ActualMotor, RM_VELOCITY);
			Write429Zero(IDX_VTARGET|(ActualMotor<<5));						
		}	
		return CMD_SET_OK;
	}
	else return CMD_NEXT;		
}
//
uint8_t MyCLI_MotorMove(char *Commands)
{
	//
	if((strncmp(Commands,"move abs[",9)==0)&&Commands[10]==']'&& Commands[11]=='[' ) 
  {
		char *p = NULL;
		char *s = &Commands[12];		
		ActualMotor=Commands[9]-'0';		
		MotorConfig[ActualMotor].XTARGET=(int)strtol(s, &p, 10);			
		if(*(p)==']'&& *(p+1)=='\0' )
		{
			if((ActualMotor<N_O_MOTORS) &&(0<(p-s)<=8))
			{
				if(SpeedChangedFlag[ActualMotor])
				{
					Write429Short(IDX_VMAX|(ActualMotor<<5), MotorConfig[ActualMotor].VMax);
					SetAMax(ActualMotor, MotorConfig[ActualMotor].AMax);
					SpeedChangedFlag[ActualMotor]=FALSE;
				}
				Write429Int(IDX_XTARGET|(ActualMotor<<5), MotorConfig[ActualMotor].XTARGET);
				Set429RampMode(ActualMotor, RM_RAMP);
				return CMD_SET_OK;
			}
			else return CMD_PAR_ERR;
		}
		else return CMD_ERR;
	}
	if((strncmp(Commands,"move ref[",9)==0)&&Commands[10]==']'&& Commands[11]=='[' ) 
  {
		char *p = NULL;
		char *s = &Commands[12];		
		ActualMotor=Commands[9]-'0';		
		MotorConfig[ActualMotor].XTARGET=(int)strtol(s, &p, 10);			
		if(*(p)==']'&& *(p+1)=='\0' )
		{
			if((ActualMotor<N_O_MOTORS) &&(0<(p-s)<=8))
			{
				if(SpeedChangedFlag[ActualMotor])
				{
					Write429Short(IDX_VMAX|(ActualMotor<<5), MotorConfig[ActualMotor].VMax);
					SetAMax(ActualMotor, MotorConfig[ActualMotor].AMax);
					SpeedChangedFlag[ActualMotor]=FALSE;
				}
				Write429Int(IDX_XTARGET|(ActualMotor<<5), Read429Int(IDX_XACTUAL|(ActualMotor<<5))+MotorConfig[ActualMotor].XTARGET);
				Set429RampMode(ActualMotor, RM_RAMP);
				return CMD_SET_OK;
			}
			else return CMD_PAR_ERR;
		}
		else return CMD_ERR;
	}
	else return CMD_NEXT;	
}
//
uint8_t MyCLI_MotorLS(char *Commands)
{
	if(strcmp(Commands,"limit?")==0 )
	{
		u8 limit_char[6];u8 i;
		u8 limit = Read429SingleByte(IDX_REF_SWITCHES, 3);
		for(i=0;i<6;i++)
		{
			limit_char[i]=(limit&(0x20>>i)) ? '1':'0';
		}
		printf("LIMIT<%.6s>\r\n",limit_char);	
		printf("REFL[0]<%c>,REFR[0]<%c>\r\n",limit_char[4],limit_char[5]);
		printf("REFL[1]<%c>,REFR[1]<%c>\r\n",limit_char[2],limit_char[3]);
		printf("REFL[2]<%c>,REFR[2]<%c>\r\n",limit_char[0],limit_char[1]);
		return CMD_GET_OK;
	}
	else return CMD_NEXT;
}
//
uint8_t MyCLI_MotorStatus(char *Commands)
{
	if(strcmp(Commands,"reach?")==0 ) 
	{
		for(ActualMotor=0;ActualMotor<N_O_MOTORS;ActualMotor++)
		{
			MotorConfig[ActualMotor].isReach=(Read429Status() & (0x01<<ActualMotor*2)) ? 1:0;
			printf("REACH[%d]<%d>\r\n",ActualMotor,MotorConfig[ActualMotor].isReach);
		}
		return CMD_GET_OK;
	}
	if(strcmp(Commands,"stop?")==0 ) 
	{
		u8 Swith=Read429SingleByte(IDX_REF_SWITCHES, 3);		
		if((Swith&0x03)==0x03 || Read429Short(IDX_VACTUAL|(0<<5))==0) MotorConfig[0].isStop=1;else MotorConfig[0].isStop=0;
		if((Swith&0x0c)==0x0c	|| Read429Short(IDX_VACTUAL|(1<<5))==0) MotorConfig[1].isStop=1;else MotorConfig[1].isStop=0;
		if((Swith&0x30)==0x30	|| Read429Short(IDX_VACTUAL|(2<<5))==0) MotorConfig[2].isStop=1;else MotorConfig[2].isStop=0;
		printf("STOP[0]<%d>\r\n",MotorConfig[0].isStop);
		printf("STOP[1]<%d>\r\n",MotorConfig[1].isStop);
		printf("STOP[2]<%d>\r\n",MotorConfig[2].isStop);
		return CMD_GET_OK;
	}							
	else return CMD_NEXT;
}
//
uint8_t MyCLI_MotorRampDiv(char *Commands)
{
	if(strncmp(Commands,"rdiv[",5)==0&&Commands[6]==']'&& Commands[7]=='['&& Commands[9]==']' && Commands[10]=='\0') 
  {	
		u8 ActualValue=Commands[8]-'0';
		ActualMotor=Commands[5]-'0';
		if(ActualMotor<N_O_MOTORS)
		{
			UCHAR Read[4], Write[4];
			Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(ActualMotor<<5);
			ReadWrite429(Read, Write);
			Write[1]=Read[1];
			Write[2]=(Read[2] & 0xf0) | (ActualValue & 0x0f);
			Write[3]=Read[3];
			Write[0]=IDX_PULSEDIV_RAMPDIV|(ActualMotor<<5);
			ReadWrite429(Read, Write);	
      MotorConfig[ActualMotor].RampDiv=ActualValue & 0x0f;
			
			WriteEepromBlock(ActualMotor*64+(UINT) &MotorConfig[ActualMotor].RampDiv-(UINT) &MotorConfig[ActualMotor],
                               (UCHAR *) &MotorConfig[ActualMotor].RampDiv, sizeof(MotorConfig[0].RampDiv));
			
			return CMD_SET_OK;
		}	
		else return CMD_PAR_ERR;		
	}
	if(strcmp(Commands,"rdiv?")==0 ) 
	{
		for(ActualMotor=0;ActualMotor<N_O_MOTORS;ActualMotor++)
		{
			MotorConfig[ActualMotor].RampDiv = Read429SingleByte(IDX_PULSEDIV_RAMPDIV|(ActualMotor<<5), 2) & 0x0f;
			printf("RampDiv[%d]<%d>\r\n",ActualMotor,MotorConfig[ActualMotor].RampDiv);
		}
		return CMD_GET_OK;
	}
	else return CMD_NEXT;	
}
//
uint8_t MyCLI_MotorPulseDiv(char *Commands)
{
	if(strncmp(Commands,"pdiv[",5)==0&&Commands[6]==']'&& Commands[7]=='['&& Commands[9]==']' && Commands[10]=='\0') 
  {
		u8 ActualValue=Commands[8]-'0';
		ActualMotor=Commands[5]-'0';
		if(ActualMotor<N_O_MOTORS)
		{
			UCHAR Read[4], Write[4];
			Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(ActualMotor<<5);
			ReadWrite429(Read, Write);
			Write[1]=Read[1];
			Write[2]=(Read[2] & 0x0f) | (ActualValue << 4);
			Write[3]=Read[3];
			Write[0]=IDX_PULSEDIV_RAMPDIV|(ActualMotor<<5);
			ReadWrite429(Read, Write);
			MotorConfig[ActualMotor].PulseDiv=ActualValue & 0x0f;
	
			WriteEepromBlock(ActualMotor*64+(UINT) &MotorConfig[ActualMotor].PulseDiv-(UINT) &MotorConfig[ActualMotor],
                               (UCHAR *) &MotorConfig[ActualMotor].PulseDiv, sizeof(MotorConfig[0].PulseDiv));
			return CMD_SET_OK;
		}	
		else return CMD_PAR_ERR;		
	}
	if(strcmp(Commands,"pdiv?")==0 ) 
	{
		for(ActualMotor=0;ActualMotor<N_O_MOTORS;ActualMotor++)
		{
			MotorConfig[ActualMotor].PulseDiv = Read429SingleByte(IDX_PULSEDIV_RAMPDIV|(ActualMotor<<5), 2) >> 4;
			printf("PulseDiv[%d]<%d>\r\n",ActualMotor,MotorConfig[ActualMotor].PulseDiv);
		}
		return CMD_GET_OK;
	}
	else return CMD_NEXT;	
}
//
