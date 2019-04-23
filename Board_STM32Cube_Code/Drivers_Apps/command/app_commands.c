//#include "bsp_include.h"	
//#include "app_eeprom_24xx.h"
#include "app_uart.h"
#include "app_commands.h"

//
static void MakeLowercase( char *str);
static void	PrintCommandlist(void);
static uint8_t  ProcessCommand(char  *Commands);

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

void CommandCheckAndExe(void)
{
	u8 len;
	if(USART_RX_STA&0x8000)									//接收到\r\n						
	{		
		len=USART_RX_STA&0x3fff;							//得到此次接收到的数据长度
		USART_RX_BUF[len]='\0';								//在末尾加入结束符. 
		
		MakeLowercase((char*)USART_RX_BUF);
		printf((char*)USART_RX_BUF);
		
		if(USART_RX_BUF[0]=='\0')									;
		else ProcessCommand((char*)USART_RX_BUF)	;				
		printf("\r\n>>");	
		USART_RX_STA=0;
	}
}



static uint8_t  ProcessCommand(char  *Commands)
{
	if(!strcmp("help",	Commands ))	{	PrintCommandlist();	 					return 1;	}
	if(!strcmp("reboot",Commands ))	{	SCB_AIRCR=SCB_RESET_VALUE;	 	return 1;	}
#if 0
	if( !strncmp(Commands,"rom[",4) ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];	
		
		u8 DataSet;		
		u8 DataNum=0;
		
		DataNum=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='[' && ( DataNum>0 && DataNum<101 ))
		{
			s=NULL;
			DataSet=strtod(p+2, &s);			
			if(*(s)==']'&& *(s+1)=='\0' )			
			{																	
				*(s)='\0';
				at24cxx.write(at24c256,DataNum*64,p+2,strlen(p+2)+1);

				printf("<ok>");
				return 1;
			}
		}	
	}
	if( !strncmp(Commands,"rom[",4) ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];	
		u8 i;
		u8 rom[64];
		u16 DataNum=0;	
		DataNum=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='\0'	&& (DataNum>0 && DataNum<101))
		{	
			at24cxx.read(at24c256 , DataNum*64, rom, 64);
			for(i=0;i<64;i++)	
			{
				if(rom[i]>127) { rom[i]='\0';break;}
			}
			printf("<%.64s>",rom);
			return 1;
		}
	}	
	if( strncmp(Commands,"save info ",10)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[10];	
		u32 PageNum=0;	
		PageNum=strtol(s, &p, 10);	
		
		if(	*p==' ' && PageNum>0 && PageNum <100	)			
		{
							
			uint8_t len=strlen(p+1);	
			uint8_t dataTemp[65];
			if(0<len && len<65)
			{
				memcpy(dataTemp,p+1,len);	
				
				if(len!=64) dataTemp[len+1]='\0';

				at24cxx.write(at24c256,(PageNum+55)*64,dataTemp,len+1);
				printf(" <ok>");
			}
			

			return 1;
		}	
	}
	if( !strncmp(Commands,"read info ",10) ) 
	{	
		char *p = NULL;
		char *s = &Commands[10];	
		u8 i;
		u32 PageNum=0;	
		u8 read_info[64];
		memset(read_info,0,64);
		PageNum=strtol(s, &p, 10);	
		
		if(	*p=='\0' && PageNum>0 && PageNum <201)			//前100页用于FSN+ROM 
		{
			at24cxx.read(at24c256 , (PageNum+55)*64, read_info, 64);
			for(i=0;i<64;i++)	
			{
				if(read_info[i]>127) { read_info[i]='\0';break;}
			}
			printf(" %.64s",read_info);

			return 1;
		}	
	}
#endif	
	else
	{
		printf("<command not found>");	return 0;
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

