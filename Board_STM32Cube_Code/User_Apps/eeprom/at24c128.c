#include <rtthread.h>
#include <rtdevice.h>
#include "fm24clxx.h"

#define EEPROM_RTT_DEBUG

#ifdef 	EEPROM_RTT_DEBUG
#define EEPROM_TRACE         rt_kprintf
#else
#define EEPROM_TRACE(...)    
#endif

fm24clxx_config at24c128_cfg=
{
	
	
};

void at24c128_hw_init(void)
{
	fm24clxx_register("at24c128","i2c1",0);	
}

void save_data(u8 pageNum , char *Commands)
{
	u8 len=strlen(Commands);
	if(len<65	&& pageNum>0)	
	{
		if(len!=64)	{Commands[len]='\0';len=len+1;}
		at24cxx.write(at24c256,pageNum*64,Commands,len);
		EEPROM_TRACE("<ok>");
	}
	else EEPROM_TRACE("<pageNum must be 1~255,commands len must be 1~64>");
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
