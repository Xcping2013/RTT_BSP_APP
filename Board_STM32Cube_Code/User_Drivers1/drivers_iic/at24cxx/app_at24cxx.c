#include "app_at24cxx.h" 

#define EEPROM_RTT_DEBUG
#ifdef 	EEPROM_RTT_DEBUG
#define EEPROM_TRACE         rt_kprintf
#else
#define EEPROM_TRACE				 printf    
#endif
/**********************************AT24CXX_USER_APP**********************************
*/
#include "stdio.h"	
#include "string.h"

at24cxx_t at24c256=
{
	{0},
	{&PB4,&PB3},
	0xA0,
};

void at24cxx_hw_init(void)
{
	at24c256.eeprom=C24LC256;
	at24cxx.init(at24c256);
	if (at24cxx.check(at24c256) == 0)
	{
		/* Ã»ÓÐ¼ì²âµ½EEPROM */
		EEPROM_TRACE("no eeprom found!!!\r\n");
	}
	else 
	{
		EEPROM_TRACE("eeprom at24c256 init...ok\r\n");
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

/*********************************   END  *********************************
*/
