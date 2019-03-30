#include "bsp.h"
//#include "app_uart1_Commands.h"


static void 		MakeLowercase( char *str);
static void 		PrintCommandlist(void);

static uint8_t  CommmandLine_help(char *Commands);
static uint8_t  CommmandLine_rom(char *Commands);
static uint8_t  CommmandLine_sv(char *Commands);
static uint8_t  CommmandLine_hv(char *Commands);
static uint8_t  CommmandLine_fsn(char *Commands);
static uint8_t  CommmandLine_baud(char *Commands);
static uint8_t  CommmandLine_in(char *Commands);
static uint8_t  CommmandLine_out(char *Commands);
static uint8_t  CommmandLine_rgb(char *Commands);

static uint8_t  CommmandLine_speed(char *Commands);
static uint8_t  CommmandLine_acc(char *Commands);

static uint8_t  CommmandLine_rotate(char *Commands);
static uint8_t  CommmandLine_move(char *Commands);
static uint8_t  CommmandLine_pos(char *Commands);
static uint8_t  CommmandLine_limit(char *Commands);
static uint8_t  CommmandLine_reach(char *Commands);


//
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
/**				
**			控制板基本命令
**/	
	//else if(strcmp("cpu",		Commands)==0)				{		ResetCPU(TRUE);										return 1;		}		
	if		  (CommmandLine_help(Commands)==1)								return 1;
	
	else  if(CommmandLine_rom(Commands)==1)									return 1;	
	else	if(CommmandLine_sv(Commands)==1)									return 1;
	else	if(CommmandLine_hv(Commands)==1)									return 1;
	else	if(CommmandLine_fsn(Commands)==1)									return 1;	
	else	if(CommmandLine_baud(Commands)==1)						return 1;
	
	else	if(CommmandLine_in(Commands)==1)									return 1;
	else	if(CommmandLine_out(Commands)==1)									return 1;
	else	if(CommmandLine_rgb(Commands)==1)									return 1;
	
	else	if(CommmandLine_Button(Commands)==1)							return 1;
	else	if(CommmandLine_Lidopen(Commands)==1)							return 1;

	else	if(CommmandLine_speed(Commands)==1)						return 1;
	else	if(CommmandLine_acc(Commands)==1)									return 1;
	else	if(CommmandLine_rotate(Commands)==1)							return 1;		
	else	if(CommmandLine_move(Commands)==1)								return 1;
	else	if(CommmandLine_pos(Commands)==1)						return 1;
	
	else	if(CommmandLine_limit(Commands)==1)								return 1;
	else	if(CommmandLine_reach(Commands)==1)								return 1;	
	else	if(CommmandLine_RomPage(Commands)==1)							return 1;	
	
	else if(strcmp("type[button online]",					Commands)==0)	{		AT24CXX_WriteOneByte(16383,BUTTON_ONLINE);	UART_SendStr("OK\n");					return 1;		}		
	else if(strcmp("type[button offline]",	Commands)==0)				{		AT24CXX_WriteOneByte(16383,BUTTON_OFFLINE);	UART_SendStr("OK\n");					return 1;		}		
	else if(strcmp("type[lidopen]",					Commands)==0)				{		AT24CXX_WriteOneByte(16383,LIDOPEN);				UART_SendStr("OK\n");					return 1;		}	
	else if(strcmp("type[lcr]",							Commands)==0)				{		AT24CXX_WriteOneByte(16383,LCR);						UART_SendStr("OK\n");					return 1;		}		
	else if(strcmp("io test",								Commands)==0)				{		TestFlag=1;ProID=BASEIO;		UART_SendStr("OK\n");		return 1;		}		

	else
	{
		return 0;
	}
}
/**				List the command list and discription
**
**/
static void PrintCommandlist(void)
{
	//	UART_SendStr("BaudRate[115200]------------Set the baudrate to 115200\n");
	//	UART_SendStr("outputs[101010101010101]-----Set outputs status\n");
	UART_SendStr("\nswitch fixture for project:\n");
	UART_SendStr("type[button online]----------------set fixture for button\n");	
	UART_SendStr("type[lcr]-------------------set fixture for lcr\n");	
	UART_SendStr("type[button offline]--------set fixture for button offline\n");	
	UART_SendStr("type[lidopen]---------------set fixture for lidopen\n");	
	
	UART_SendStr("\ncommon commands:\n");	
	UART_SendStr("io test----------------------input control output for board debug\n");	
	UART_SendStr("INT[1~18]--------------------Get input status\n");
	UART_SendStr("OUT[1~14][1 or 0]------------Set output status\n");
	UART_SendStr("RGB[R|G|B| ]-----------------Set RGB LED\n");
	UART_SendStr("inputs-----------------------Get inputs status\n\n");

	UART_SendStr("fsn-------------------------Get fixture serial number\n");	
	UART_SendStr("FSN[20180105]---------------Set fixture serial number to 20180105\n");	
	UART_SendStr("ROM[1~100]-------------------Read one byte\n");
	UART_SendStr("ROM[1~100][2018]-------------Write one byte\n");
	UART_SendStr("SAVE INFO 1~200 YYYYY--------Write max 64 bytes to position\n");
	UART_SendStr("READ INFO 1~200--------------Read max 64 bytes from position\n\n");
	
	UART_SendStr("is left #0~2---REF[0~2]<1>----------check motor limit status\n");
	UART_SendStr("is right #0~2--REF[0~2]<1>----------check motor limit status\n");
	UART_SendStr("is limit-------LIMIT<L3R3L2R2L1R1>--check motor limit status\n");
	UART_SendStr("is reach #0~2--REACH[0~2]<1>--------check motor is reach the target position\n");
	UART_SendStr("is stop #0~2---STOP[0~2]<1>---------check motor is stop or not\n\n");
	
	UART_SendStr("0~2# velocity 0~2047----------------set the velocity to xx\n");
	UART_SendStr("0~2# acc 0~2047---------------------set the acc to n\n");	
	UART_SendStr("all# velocity 0~2047----------------set all velocity to xx\n");
	UART_SendStr("all# acc 0~2047---------------------set the acc to all\n\n");
	
	UART_SendStr("velocity #0~2-----------------------get the velocity\n");
	UART_SendStr("velocity #all-----------------------get velocitys\n");	
	UART_SendStr("acc #0~2----------------------------get the acc\n");
	UART_SendStr("acc #all----------------------------get the accs\n");
	UART_SendStr("position #0~2-----------------------get current position\n");
	UART_SendStr("position #all-----------------------get current positions\n\n");
		
	UART_SendStr("n# rotate +/-xx--<OK>--------------Rotate Right or left\n");
	UART_SendStr("n# move to +/-xx-<OK>--------------#n to ABS POS -/-xx\n");
	UART_SendStr("n# move +/-xx----<OK>--------------#n to REL POS +/-xx\n");
	UART_SendStr("n# stop---------STOP[n]<OK>--------#n stop\n");
	
	UART_SendStr("\nLidopen:\n");	
	UART_SendStr("button enable-----<OK>-------enable the button for next test\n");
	UART_SendStr("button disable----<OK>-------Disable the button\n");
	UART_SendStr("is button ok---<OK/NO>-------Get button status\n");
	UART_SendStr("motor go home---<OK>---------start homing\n");
	UART_SendStr("ready to test----------------door action ok\n");
	UART_SendStr("must send rom[1][300];rom[2][100];rom[3][100] once\n");
	
	UART_SendStr("\nButton:\n");	
	UART_SendStr("motor reset------<OK>--------start homing\n");
	UART_SendStr("test over--------<OK>---------------enable key-start for next test\n"); 
	UART_SendStr("is homed-------HOME<OK>-------------check motor homed status\n");		
	UART_SendStr("is start-------START<OK>------------check key-start pressed status\n\n");	
	
	UART_SendStr("Please send command as above end with \\r\\n\n");
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
//
static uint8_t  CommmandLine_rom(char *Commands)
{
	if( strncmp(Commands,"rom[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];	
		u8 i;
		Data_4Bytes DataSet;		
		u16 DataNum=0;
		DataNum=strtol(s, &p, 10);	
		//UART_Printf("%c;%c",*p,*(p+1));	OUTPUTH(1);
		if(*p==']'&& *(p+1)=='[' && ( DataNum>0 && DataNum<101) )
		{
			s=NULL;
			DataSet.f32=strtod(p+2, &s);			//OUTPUTL(1);
			if(*(s)==']'&& *(s+1)=='\0' )			
			{																	//OUTPUTH(2)
				for(i=0;i<4;i++)
				{
					AT24CXX_WriteOneByte(DataNum*4+100+i,DataSet.Byte[i]);
				}
				//AT24CXX_WriteLenByte(DataNum*4+100,(u32)DataSet.f32,4);
				UART_Printf("ROM[%d][%.3f]<OK>\n",(u16)DataNum,DataSet.f32); 
				return 1;
			}
		}	
	}
	if( strncmp(Commands,"rom[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];	
		u8 i;
		u16 DataNum=0;	
		Data_4Bytes	DataReads;	
		DataNum=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='\0'	&& (DataNum>0 && DataNum<101))
		{	
			for(i=0;i<4;i++)
			{
				DataReads.Byte[i]=AT24CXX_ReadOneByte(DataNum*4+100+i);
			}
			UART_Printf("ROM[%d]<%.3f>\n",(u16)DataNum,DataReads.f32);
			return 1;
		}
		UART_SendStr("<NA>\n");return 1;
	}		
	else return 0;
	
}
//
static uint8_t  CommmandLine_sv(char *Commands)
{
  if(strcmp("sv",		Commands)==0)			
	{		
		PrintSoftVersionFromEEPROM();			
		return 1;		
	}
	else if(strncmp("sv[",	Commands,3)==0)	
	{		
		char *p = NULL;
		char *s = &Commands[3];			
		u8 sv=(uint8_t)strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='\0')									
		{		
			WriteSWToEEPROM(sv);			
			return 1;		
		}
		UART_SendStr("<NA>\n");return 1;
	}	
	else return 0;
}
//
static uint8_t  CommmandLine_hv(char *Commands)
{
	if(strcmp("hv",		Commands)==0)			
	{		
		PrintHardVersionFromEEPROM();			
		return 1;	
	}
	else if(strncmp("hv[",	Commands,3)==0)	
	{		
		char *p = NULL;
		char *s = &Commands[3];			
		u8 hv=(uint8_t )strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='\0')									
		{		
			WriteHWToEEPROM(hv);		
			return 1;		
		}
		UART_SendStr("<NA>\n");return 1;
	}	
	else return 0;
}
//
static uint8_t  CommmandLine_fsn(char *Commands)
{
	if(strcmp("fsn",		Commands)==0)			
	{		
		PrintFSNFromEEPROM();			
		return 1;	
	}
	else if(strncmp("fsn[",	Commands,4)==0)	
	{	
		uint8_t i;
		for(i=0;i<11;i++)
		{
			if(Commands[i+4]==']') {Commands[i+4]='\0';	break ;}
		}
		if(	i>0	&& i<11	&& Commands[i+4]=='\0'&& Commands[i+5]=='\0')			
		{		
			WriteFSNToEEPROM((uint8_t*)&Commands[4]);		
			return 1;		
		}
		UART_SendStr("<NA>\n");return 1;
	}	
	else return 0;
}
//

//
static uint8_t  CommmandLine_in(char *Commands)
{
	if( strcmp(Commands,"inputs")==0  ) 
	{
		u8 i; u8 readData[18];
		for(i=0; i<18; i++)
		{
			readData[i]=IO_Get(i+1)	? '1':'0';
		}
		UART_Printf("INPUTS<%.18s>\n",readData);
		return 1;
		
	}
	
	else if( strncmp(Commands,"int[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];			
		u8 channel=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='\0'	&& channel>0 && channel<=18)
		{	
			UART_Printf("INT[%d]<%ld>\n",channel,IO_Get(channel));
			return 1;
		}
		UART_SendStr("<NA>\n");return 1;	
	}
	else return 0;	
}
//
static uint8_t  CommmandLine_out(char *Commands)
{
	if( strncmp(Commands,"outputs[",8)==0  ) 
	{		
		if(strlen(Commands)==23 && Commands[22]	==']' && Commands[23]=='\0')
		{
			u8 i;
			for(i=1;i<15;i++)
			{
				IO_Set(i, Commands[7+i]-'0');
			}
			UART_Printf("<OK>\n");
			return 1;
		}
	}
	if( strncmp(Commands,"out[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];		
		u8 setval;		
		u8 channel=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='[' && channel>0 && channel<=14)	
		{
			s=NULL;
			setval=strtol(p+2, &s, 10);	
			if(*(s)==']'&& *(s+1)=='\0' )
			{
				IO_Set(channel, setval);
				UART_Printf("OUT[%d][%d]<OK>\n",channel,setval); 
				return 1;
			}			
		}	
		UART_SendStr("<NA>\n");return 1;
	}	
	else return 0;
}
//
static uint8_t  CommmandLine_rgb(char *Commands)
{
	if( strncmp(Commands,"rgb[",4)==0  ) 
	{			
		u8 led=Commands[4];
		if(Commands[5]==']'&& Commands[6]=='\0'	)
		{
			if(led=='r')
			{
				RGB_R_ON();UART_SendStr("RGB[R]<OK>\n");return 1;
			}
			else if(led=='g')
			{
				RGB_G_ON();UART_SendStr("RGB[G]<OK>\n");return 1;
			}
			else if(led=='b')
			{
				RGB_B_ON();UART_SendStr("RGB[B]<OK>\n");return 1;
			}
			else if(led==' ')
			{
				RGB_OFF();UART_SendStr("RGB[ ]<OK>\n");return 1;
			}
		}
		UART_SendStr("<NA>\n");return 1;
	}	
	else return 0;
}
//
static uint8_t  CommmandLine_speed(char *Commands)
{
	if(strncmp(Commands+1,"# velocity ",11)==0 ) 
  {
		ActualMotor=Commands[0]-'0';
		if(ActualMotor<N_O_MOTORS)
		{
			char *p = NULL;
			char *s = &Commands[12];
			u8 len;		
			MotorConfig[ActualMotor].VMax=strtol(s, &p, 10);	
			len=p-s;				
			if(0<len&&len<=4)			
      {
				Write429Short(IDX_VMAX|(ActualMotor<<5), MotorConfig[ActualMotor].VMax);
				if(ProID==BUTTON_ONLINE||ProID==BUTTON_OFFLINE)
				{
					if (MotorConfig[ActualMotor].VMax==70)
					{		SetAMax(ActualMotor, 25);MotorConfig[ActualMotor].AMax=25;}
					else if (MotorConfig[ActualMotor].VMax==100)
					{		SetAMax(ActualMotor, 40);MotorConfig[ActualMotor].AMax=40;}
					else if (MotorConfig[ActualMotor].VMax==1000)
					{		SetAMax(ActualMotor, 1600);MotorConfig[ActualMotor].AMax=1600;}
					else if (MotorConfig[ActualMotor].VMax<700)
					{		SetAMax(ActualMotor, 400);MotorConfig[ActualMotor].AMax=400;}
					else if (MotorConfig[ActualMotor].VMax<840)
					{		SetAMax(ActualMotor, 700);MotorConfig[ActualMotor].AMax=700;}
					else if (MotorConfig[ActualMotor].VMax<980)
					{		SetAMax(ActualMotor, 1000);MotorConfig[ActualMotor].AMax=1000;}
					else if (MotorConfig[ActualMotor].VMax<1180)
					{		SetAMax(ActualMotor, 1300);MotorConfig[ActualMotor].AMax=1300;}
					else if (MotorConfig[ActualMotor].VMax<2047)
					{		SetAMax(ActualMotor, 1800);MotorConfig[ActualMotor].AMax=1800;}
				}
				else if(ProID==LIDOPEN)
				{
					if (MotorConfig[ActualMotor].VMax<25)
					{		SetAMax(ActualMotor, MotorConfig[ActualMotor].VMax+5);	MotorConfig[ActualMotor].AMax=MotorConfig[ActualMotor].VMax+5;}			
					else if (MotorConfig[ActualMotor].VMax<100)
					{		SetAMax(ActualMotor, MotorConfig[ActualMotor].VMax+10);	MotorConfig[ActualMotor].AMax=MotorConfig[ActualMotor].VMax+10;}
					else if (MotorConfig[ActualMotor].VMax<150)
					{		SetAMax(ActualMotor, MotorConfig[ActualMotor].VMax+30);MotorConfig[ActualMotor].AMax=MotorConfig[ActualMotor].VMax+30;}
					else if (MotorConfig[ActualMotor].VMax<280)
					{		SetAMax(ActualMotor, MotorConfig[ActualMotor].VMax+60);MotorConfig[ActualMotor].AMax=MotorConfig[ActualMotor].VMax+60;}
					else if (MotorConfig[ActualMotor].VMax<420)
					{		SetAMax(ActualMotor, MotorConfig[ActualMotor].VMax+100);MotorConfig[ActualMotor].AMax=MotorConfig[ActualMotor].VMax+100;}
					else if (MotorConfig[ActualMotor].VMax<560)
					{		SetAMax(ActualMotor, MotorConfig[ActualMotor].VMax+150);MotorConfig[ActualMotor].AMax=MotorConfig[ActualMotor].VMax+150;}
					else if (MotorConfig[ActualMotor].VMax<700)
					{		SetAMax(ActualMotor, MotorConfig[ActualMotor].VMax+200);MotorConfig[ActualMotor].AMax=MotorConfig[ActualMotor].VMax+200;}
					else if (MotorConfig[ActualMotor].VMax<840)
					{		SetAMax(ActualMotor, MotorConfig[ActualMotor].VMax+250);MotorConfig[ActualMotor].AMax=MotorConfig[ActualMotor].VMax+250;}
					else if (MotorConfig[ActualMotor].VMax<980)
					{		SetAMax(ActualMotor, MotorConfig[ActualMotor].VMax+300);MotorConfig[ActualMotor].AMax=MotorConfig[ActualMotor].VMax+300;}
					else if (MotorConfig[ActualMotor].VMax<=1180)
					{		SetAMax(ActualMotor, MotorConfig[ActualMotor].VMax+350);MotorConfig[ActualMotor].AMax=MotorConfig[ActualMotor].VMax+350;}
					else if (MotorConfig[ActualMotor].VMax<2047)
					{		SetAMax(ActualMotor, MotorConfig[ActualMotor].VMax+600);MotorConfig[ActualMotor].AMax=MotorConfig[ActualMotor].VMax+600;}
					
					
				}
				//UART_Printf("Vset[%d#][%d]<OK>\r\n",ActualMotor,MotorConfig[ActualMotor].VMax);
				UART_Printf("Vset[%d-%d]<OK>\n",ActualMotor,MotorConfig[ActualMotor].VMax);

				return 1;
			}
			else return 0;
     }
		UART_SendStr("<NA>\n");return 1;
	}
	else if(strncmp(Commands,"all# velocity ",14)==0 ) 
  {
		char *p = NULL;
		char *s = &Commands[12];
		u8 len;
			
		MotorConfig[0].VMax=strtol(s, &p, 10);
		len=p-s;				
		if(0<len&&len<=4)			
    {
			MotorConfig[1].VMax=MotorConfig[0].VMax;
			MotorConfig[2].VMax=MotorConfig[0].VMax;
			Write429Short(IDX_VMAX|(0<<5), MotorConfig[0].VMax);
			Write429Short(IDX_VMAX|(1<<5), MotorConfig[1].VMax);
			Write429Short(IDX_VMAX|(2<<5), MotorConfig[2].VMax);			
			//UART_Printf("Vset[all#][%d]<OK>\r\n",MotorConfig[ActualMotor].VMax);
			UART_Printf("Vset[%d]<OK>\n",MotorConfig[0].VMax);
			return 1;
    }
		UART_SendStr("<NA>\n");return 1;
  }
/**   读取所有电机当前速度
**					
**/	
	else if(strcmp(Commands,"velocity #all")==0 ) 
	{
		MotorConfig[0].VACTUAL=Read429Short(IDX_VACTUAL|(0<<5));
		MotorConfig[1].VACTUAL=Read429Short(IDX_VACTUAL|(1<<5));
		MotorConfig[2].VACTUAL=Read429Short(IDX_VACTUAL|(2<<5));
		UART_Printf("Vset[0]<%d>,Vset[1]<%d>,Vset[2]<%d>,V[0]<%d>,V[1]<%d>,V[2]<%d>\n",MotorConfig[0].VMax,MotorConfig[1].VMax,MotorConfig[2].VMax,MotorConfig[0].VACTUAL,MotorConfig[1].VACTUAL,MotorConfig[2].VACTUAL);		
		return 1;
	}
/**   读取单个电机当前速度
**					
**/	
	else if(strncmp(Commands,"velocity #",10)==0 ) 
	{
		ActualMotor=Commands[10]-'0';
		if(ActualMotor<N_O_MOTORS)
		{
			MotorConfig[ActualMotor].VACTUAL=Read429Short(IDX_VACTUAL|(ActualMotor<<5));
			UART_Printf("Vset[%d]<%d>,V[%d]<%d>\n",ActualMotor,MotorConfig[ActualMotor].VMax,ActualMotor,MotorConfig[ActualMotor].VACTUAL);
			return 1;
		}	
		UART_SendStr("<NA>\n");return 1;
	}
	else return 0;	
}
//
static uint8_t  CommmandLine_acc(char *Commands)
{
	if(strncmp(Commands+1,"# acc ",6)==0 ) 
  {
		ActualMotor=Commands[0]-'0';
		if(ActualMotor<N_O_MOTORS)
		{
			char *p = NULL;
			char *s = &Commands[7];
			u8 len;
			MotorConfig[ActualMotor].AMax=strtol(s, &p, 10);	
			len=p-s;				
			if(0<len&&len<=4)				
      {
        SetAMax(ActualMotor, MotorConfig[ActualMotor].AMax);
				UART_Printf("A[%d-%d]<OK>\n",ActualMotor,MotorConfig[ActualMotor].AMax);				
				return 1;
      }
    }
		UART_SendStr("<NA>\n");return 1;
	}		
	else if(strncmp(Commands,"all# acc ",9)==0 ) 
  {
		char *p = NULL;
		char *s = &Commands[9];
		u8 len;
		MotorConfig[0].AMax=strtol(s, &p, 10);		
		len=p-s;				
		if(0<len&&len<=4)					
    {
			MotorConfig[1].AMax=MotorConfig[0].AMax;	
			MotorConfig[2].AMax=MotorConfig[0].AMax;	
      SetAMax(0, MotorConfig[0].AMax);
			SetAMax(1, MotorConfig[1].AMax);
			SetAMax(2, MotorConfig[2].AMax);
			UART_Printf("A[all-%d]<OK>\n",MotorConfig[0].AMax);			
			return 1;
    }
		UART_SendStr("<NA>\n");return 1;
  }
/**   读取所有电机当前加速度
**					
**/		
	else if(strcmp(Commands,"acc #all")==0 ) 
	{
		UART_Printf("A[0]<%d>,A[1]<%d>,A[2]<%d>\n",MotorConfig[0].AMax,MotorConfig[1].AMax,MotorConfig[2].AMax);
		return 1;
	}
	else return 0;
}
//
static uint8_t  CommmandLine_rotate(char *Commands)
{
	if(strncmp(Commands+1,"# rotate ",9)==0 ) 
  {
		if( Commands[10]=='+' || Commands[10]=='-' )
		{
			ActualMotor=Commands[0]-'0';
			if(ActualMotor<N_O_MOTORS)
			{
				char *p = NULL;
				char *s = &Commands[11];		
				u8 len;				
				MotorConfig[ActualMotor].VTARGET=strtol(s, &p, 10);		
				len=p-s;				
				if(0<len&&len<=4)					
				{
					SpeedChangedFlag[ActualMotor]=TRUE;
					Set429RampMode(ActualMotor, RM_VELOCITY);
					Write429Short(IDX_VMAX|(ActualMotor<<5), 2047);
					if( Commands[10]=='+' )
					{
						Write429Short(IDX_VTARGET|(ActualMotor<<5), MotorConfig[ActualMotor].VTARGET);
					}
					else Write429Short(IDX_VTARGET|(ActualMotor<<5), -MotorConfig[ActualMotor].VTARGET);

					UART_SendStr("<OK>\n");
					return 1;
				}
			}
		}
		UART_SendStr("<NA>\n");return 1;		
	}
	else return 0;
}
//
static uint8_t  CommmandLine_move(char *Commands)
{
/**   绝对运动
**					
**/		
	if(strncmp(Commands+1,"# move to ",10)==0 ) 
  {
		if( Commands[11]=='+' || Commands[11]=='-' )
		{
			ActualMotor=Commands[0]-'0';
			if(ActualMotor<N_O_MOTORS)
			{
				char *p = NULL;
				char *s = &Commands[12];	
				u8 len;
				MotorConfig[ActualMotor].XTARGET=(int)strtol(s, &p, 10);			
				len=p-s;				
				if(0<len&&len<=8)	
				{
					if(SpeedChangedFlag[ActualMotor])
					{
						Write429Short(IDX_VMAX|(ActualMotor<<5), MotorConfig[ActualMotor].VMax);
						SetAMax(ActualMotor, MotorConfig[ActualMotor].AMax);
						SpeedChangedFlag[ActualMotor]=FALSE;
					}
					Set429RampMode(ActualMotor, RM_VELOCITY);
					if( Commands[11]=='+' )
					{
						Write429Int(IDX_XTARGET|(ActualMotor<<5), MotorConfig[ActualMotor].XTARGET);
					}
					else Write429Int(IDX_XTARGET|(ActualMotor<<5), -MotorConfig[ActualMotor].XTARGET);
					
					Set429RampMode(ActualMotor, RM_RAMP);	
					UART_SendStr("<OK>\n");					
					return 1;
				}
			}
		}
		UART_SendStr("<NA>\n");return 1;
	}
/**   相对运动
**					
**/		
	else	if(strncmp(Commands+1,"# move ",7)==0 ) 
  {
		if( Commands[8]=='+' || Commands[8]=='-' )
		{
			ActualMotor=Commands[0]-'0';
			if(ActualMotor<N_O_MOTORS)
			{
				char *p = NULL;
				char *s = &Commands[9];	
				u8 len;
				MotorConfig[ActualMotor].XTARGET=(int)strtol(s, &p, 10);							
				len=p-s;				
				if(0<len&&len<=8)	
				{
					if(SpeedChangedFlag[ActualMotor])
					{
						Write429Short(IDX_VMAX|(ActualMotor<<5), MotorConfig[ActualMotor].VMax);
						SetAMax(ActualMotor, MotorConfig[ActualMotor].AMax);
						SpeedChangedFlag[ActualMotor]=FALSE;
					}
					Set429RampMode(ActualMotor, RM_VELOCITY);
					if( Commands[8]=='+' )
					{
						Write429Int(IDX_XTARGET|(ActualMotor<<5), Read429Int(IDX_XACTUAL|(ActualMotor<<5))+MotorConfig[ActualMotor].XTARGET);
					}
					else Write429Int(IDX_XTARGET|(ActualMotor<<5), Read429Int(IDX_XACTUAL|(ActualMotor<<5))-MotorConfig[ActualMotor].XTARGET);
					
					Set429RampMode(ActualMotor, RM_RAMP);	
					UART_SendStr("<OK>\r\n");	
					return 1;
				}
			}
		}
		UART_SendStr("<NA>\n");return 1;
	}
/**   停止运动
**					
**/	
	else 	if(strncmp(Commands+1,"# stop",6)==0 ) 
  {
		ActualMotor=Commands[0]-'0';
		if(ActualMotor<N_O_MOTORS)
		{
			Set429RampMode(ActualMotor, RM_VELOCITY);
			Write429Zero(IDX_VTARGET|(ActualMotor<<5));			
			UART_Printf("STOP[%d]<OK>\r\n",ActualMotor);			
			return 1;
		}
		UART_SendStr("<NA>\n");return 1;
	}
	else return 0;
}
//
static uint8_t  CommmandLine_pos(char *Commands)
{
	/**   读取位置
**					
**/	
	if(strcmp(Commands,"position #all")==0 ) 
	{
		MotorConfig[0].XACTUAL=Read429Int(IDX_XACTUAL|(0<<5));
		MotorConfig[1].XACTUAL=Read429Int(IDX_XACTUAL|(1<<5));
		MotorConfig[2].XACTUAL=Read429Int(IDX_XACTUAL|(2<<5));
		UART_Printf("P[0]<%d>,P[1]<%d>,P[2]<%d>\n",MotorConfig[0].XACTUAL,MotorConfig[1].XACTUAL,MotorConfig[2].XACTUAL);
		//UART_Printf("P[0]=%d,P[1]=%d,P[2]=%d\r\n",MotorConfig[0].XACTUAL,MotorConfig[1].XACTUAL,MotorConfig[2].XACTUAL);	
		return 1;
	}
	else 	if(strncmp(Commands,"position #",10)==0 ) 
	{
		ActualMotor=Commands[10]-'0';
		if(ActualMotor<N_O_MOTORS)
		{
			MotorConfig[ActualMotor].XACTUAL=Read429Int(IDX_XACTUAL|(ActualMotor<<5));
			UART_Printf("P[%d]<%d>\n",ActualMotor,MotorConfig[ActualMotor].XACTUAL);
			//UART_Printf("P[%d]=%d\r\n",ActualMotor,MotorConfig[ActualMotor].XACTUAL);
			return 1;
		}
		UART_SendStr("<NA>\n");return 1;
	}
	else return 0;
}
//
static uint8_t  CommmandLine_limit(char *Commands)
{
/**   读取指定轴左极限状态
**					
**/	
	if(strncmp(Commands,"is left #",9)==0 ) 
	{
		ActualMotor=Commands[9]-'0';
		if(ActualMotor<N_O_MOTORS)
		{
			MotorConfig[ActualMotor].REFL_FLAG=(Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<ActualMotor*2)) ? 1:0;
			UART_Printf("REFL[%d]<%d>\n",ActualMotor,MotorConfig[ActualMotor].REFL_FLAG);
			//UART_Printf("REFL[%d]=%d\r\n",ActualMotor,MotorConfig[ActualMotor].REFL_FLAG);
			return 1;
		}
		UART_SendStr("<NA>\n");return 1;
	}
/**   读取指定轴右极限状态
**					
**/		
	else if(strncmp(Commands,"is right #",10)==0 ) 
	{
		ActualMotor=Commands[10]-'0';
		if(ActualMotor<N_O_MOTORS)
		{
			MotorConfig[ActualMotor].REFR_FLAG=(Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x01<<ActualMotor*2)) ? 1:0;
			UART_Printf("REFR[%d]<%d>\n",ActualMotor,MotorConfig[ActualMotor].REFR_FLAG);
			//UART_Printf("REFR[%d]=%d\r\n",ActualMotor,MotorConfig[ActualMotor].REFR_FLAG);
			return 1;
		}
		UART_SendStr("<NA>\n");return 1;
	}
/**   读取所有轴极限开关状态
**					
**/	
	else if(strcmp(Commands,"is limit")==0 ) 
	{
		u8 limit_char[6];u8 i;
		u8 limit = Read429SingleByte(IDX_REF_SWITCHES, 3);
		for(i=0;i<6;i++)
		{
			limit_char[i]=(limit&(0x20>>i)) ? '1':'0';
		}
		UART_Printf("LIMIT<%.6s>\n",limit_char);
		//UART_Printf("limit=%6s\r\n",limit_char);		
		return 1;
	}
	else return 0;
}
//
static uint8_t  CommmandLine_reach(char *Commands)
{
	/**   判断指定轴是否运动到位
**					
**/	
	if(strncmp(Commands,"is reach #",10)==0 ) 
	{
		ActualMotor=Commands[10]-'0';
		if(ActualMotor<N_O_MOTORS)
		{
			MotorConfig[ActualMotor].POSITION_FLAG=(Read429Status() & (0x01<<ActualMotor*2)) ? 1:0;
			UART_Printf("REACH[%d]<%d>\n",ActualMotor,MotorConfig[ActualMotor].POSITION_FLAG);
			//UART_Printf("REACH[%d]=%d\r\n",ActualMotor,MotorConfig[ActualMotor].POSITION_FLAG);
			return 1;
		}
		UART_SendStr("<NA>\n");return 1;
	}
/**   判断指定轴是否停止
**					
**/	
	else 	if(strncmp(Commands,"is stop #",9)==0 ) 
	{
		u8 Swith=Read429SingleByte(IDX_REF_SWITCHES, 3);
		ActualMotor=Commands[9]-'0';
		if(ActualMotor<N_O_MOTORS)
		{
			if( Read429Short(IDX_VACTUAL|(ActualMotor<<5))==0 )  MotorConfig[ActualMotor].STOP_FLAG=1;
			else MotorConfig[ActualMotor].STOP_FLAG=0;
			
			if((Swith&0x03)==0x03) MotorConfig[0].STOP_FLAG=1;
			if((Swith&0x0c)==0x0c) MotorConfig[1].STOP_FLAG=1;
			if((Swith&0x30)==0x30) MotorConfig[2].STOP_FLAG=1;	

			UART_Printf("STOP[%d]<%d>\n",ActualMotor,MotorConfig[ActualMotor].STOP_FLAG);
			return 1;
		}
		UART_SendStr("<NA>\n");return 1;
	}							
	else return 0;
}
//
static uint8_t  CommmandLine_RomPage(char *Commands)
{
	if( strncmp(Commands,"save info ",10)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[10];	
		u32 PageNum=0;	
		PageNum=strtol(s, &p, 10);	

		if(	*p==' ' && PageNum>0 && PageNum <(256-10)	)			//前10页用于FSN+ROM 
		{
			IIC_DeviceWriteBytes(0xA0,64*(10+PageNum),(u8 *)p+1,64,2);
			UART_Printf("save info %d OK\n",PageNum); 
			return 1;
		}	
	  UART_SendStr("<NA>\n");return 1;
	}
	if( strncmp(Commands,"read info ",10)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[10];	
		u32 PageNum=0;	
		u8 DataRead[64];
		PageNum=strtol(s, &p, 10);	
		
		if(	*p=='\0' && PageNum>0 && PageNum <(256-10)	)			//前10页用于FSN+ROM 
		{
			IIC_DeviceReadBytes(0xA0,64*(10+PageNum),	DataRead,64,2);
			UART_Printf("read info %d %.64s\n",PageNum,DataRead);
			return 1;
		}	
	  UART_SendStr("<NA>\n");return 1;
	}
	else return 0;
}
//-----------------------------------------------------------common	end	
//-----------------------------------------------------------Project start	
static uint8_t  CommmandLine_Lidopen(char *Commands)
{
	if(strcmp("motor go home",Commands)==0)
  {
		HomeStep=1;MotorIsHomed=0;
		UART_SendStr("<OK>\n");
		return 1;
  }
	if(strcmp("button enable",Commands)==0)
	{
		KeySTART_EN=1;Test_Enable=0;PAout(3)=1;
		UART_SendStr("<OK>\n");
		return 1;
	}
	else if(strcmp("button disable",Commands)==0)
	{
		KeySTART_EN=0;Test_Enable=0;PAout(3)=1;
		UART_SendStr("<OK>\n");
		return 1;
	}
	else if(strcmp("is button ok",Commands)==0)
	{
		if(Test_Enable==1)
		{	
			UART_SendStr("BUTTON_ONLINE<OK>\n");
			return 1;
		}
		else 
		{	
			UART_SendStr("BUTTON_ONLINE<NO>\n");	
			return 1;
		}
	}
  else return 0; 
}
//
static uint8_t  CommmandLine_Button(char *Commands)
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
		MotorHoming();
		//UART_SendStr("<OK>\n");
		return 1;
  }
	if(strcmp("is start",Commands)==0)
  {
		if(KeySTART_ON) 	UART_SendStr("START<OK>\n");
		else 									UART_SendStr("START<NO>\n");
		return 1;
  }	
	else if(strcmp("test over",Commands)==0)
  {
		KeySTART_ON=0;
		OUTPUTL(1);
		UART_SendStr("<OK>\n");
		return 1;
  }
	else return 0;
}
//-----------------------------------------------------------Project end



