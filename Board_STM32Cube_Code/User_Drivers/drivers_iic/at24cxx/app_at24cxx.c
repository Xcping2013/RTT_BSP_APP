#include "bsp.h" 

#define EEPROM_RTT_DEBUG
#ifdef 	EEPROM_RTT_DEBUG
#define EEPROM_TRACE         rt_kprintf
#else
#define EEPROM_TRACE(...)	
#endif
/**********************************AT24CXX_USER_APP**********************************
*/
\

//1 0 1 0 A2 A1 A0 R/W

at24cxx_t at24c256=
{
	{0},
	{&PC14,&PC15},
	0xA2,	//1 0 1 0 0 0 1 0
};

void at24cxx_hw_init(void)
{
	at24c256.eeprom=C24LC256;
	at24cxx.init(at24c256);
	if (at24cxx.check(at24c256) == 0)
	{
		/* 没有检测到EEPROM */
		EEPROM_TRACE("no eeprom found!!!\n");
	}
	else 
	{
		EEPROM_TRACE("eeprom at24c256 init...ok\n");
	}
}

void save_data(u8 pageNum , char *data)
{
	u8 len=strlen(data);
	if(len<65	&& pageNum>0)	
	{
		if(len!=64)	{data[len]='\0';len=len+1;}
		at24cxx.write(at24c256,pageNum*64,(u8 *)data,len);
		EEPROM_TRACE("<ok>");
	}
	else EEPROM_TRACE("<pageNum must be 1~255,data len must be 1~64>");
}

void read_data(u8 pageNum)
{
	u8 read_info[64];
	u8 i;
	if(pageNum>0)	
	{
		at24cxx.read(at24c256 , pageNum*64, read_info, 64);
		for(i=0;i<64;i++)	
		{
			if(read_info[i]>127) { read_info[i]='\0';break;}
		}
		EEPROM_TRACE("<%.64s>",read_info);
	}
	else EEPROM_TRACE("<pageNum must be 1~255>");
}
//FSN---page0
uint8_t  CMD_fsn(char *Commands)
{
	if(strcmp("fsn",		Commands)==0)			
	{		
		uint8_t	i;
		uint8_t fsn[64];
		at24cxx.read(at24c256 , 0, fsn, 64);
		//at24lc04_reads(0);
		for(i=0;i<64;i++)	
		{
			if(fsn[i]>127) { fsn[i]='\0';break;}
		}
		printf("<%.64s>",fsn);
		return 1;	
	}
	if(	(Commands[0]=='s')	&&	(Commands[1]=='n')	&&	(Commands[2]==':')	)
	{
		at24cxx.write(at24c256, 0, &Commands[3], strlen(&Commands[3])+1);

		printf("<OK>");
		return 1;
	}

	else if(strncmp("fsn[",	Commands,4)==0)	
	{	
		uint8_t i;
		for(i=0;i<64;i++)
		{
			if(Commands[i+4]==']') {Commands[i+4]='\0';	break ;}
		}
		if(	i>0	&& i<64	&& Commands[i+4]=='\0'&& Commands[i+5]=='\0')			
		{		
			at24cxx.write(at24c256, 0, &Commands[4], strlen(&Commands[4])+1);

			printf("<OK>");
			return 1;		
		}
	}	
	else return 0;
}
//
uint8_t  CMD_rom(char *Commands)
{
	if( strncmp(Commands,"rom[",4)==0  ) 
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

				printf("<OK>");
				return 1;
			}
		}	
	}
	if( strncmp(Commands,"rom[",4)==0  ) 
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
	else return 0;
	
}
//
uint8_t  CMD_RomPage(char *Commands)
{
	if( strncmp(Commands,"save info ",10)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[10];	
		u32 PageNum=0;	
		PageNum=strtol(s, &p, 10);	

		if(	*p==' ' && PageNum>0 && PageNum <100	)			
		{
			at24cxx.write(at24c256,(PageNum+100)*64,p+1,strlen(p+1)+1);
			printf(" OK");

			return 1;
		}	
	}
	if( strncmp(Commands,"read info ",10)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[10];	
		u8 i;
		u32 PageNum=0;	
		u8 read_info[64];
		PageNum=strtol(s, &p, 10);	
		
		if(	*p=='\0' && PageNum>0 && PageNum <400	)			//前100页用于FSN+ROM 
		{
			at24cxx.read(at24c256 , (PageNum+100)*64, read_info, 64);
			for(i=0;i<64;i++)	
			{
				if(read_info[i]>127) { read_info[i]='\0';break;}
			}
			printf(" %.64s",read_info);

			return 1;
		}	
	}
	else return 0;
}
/******************			AT24CXXX_APP<---**************************
*/
