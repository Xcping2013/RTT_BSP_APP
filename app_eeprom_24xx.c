//#include "app_sys_control.h"
//#include "app_eeprom_24xx.h"

#include "bsp_include.h"	
#include "app_include.h"

//MB1616DEV6
at24cxx_t at24c256=
{
	{0},
	{PC_14,PC_15},
	0xA2,	//1 0 1 0 0 0 1 0
};

void at24cxx_hw_init(void)
{
	at24c256.eeprom=C24LC256;//C24LC256;
	at24cxx.init(at24c256);
	
	LoadParam();
}
//128=256*64  256=512*64byte 为了2芯片兼容 使用前256页就好
//数据存储	0~55其他用途 56~255 用户使用
int savedata(int argc, char **argv)
{
	if (argc !=3)
	{
			CMD_TRACE("Please input: savedata <page number> <data to save>\n");
			return -1;
	}	
	else 
	{
		int pageNum=atoi(argv[1]);
		uint8_t len=strlen(argv[2]);
		if(len<65	&& 0<pageNum &&  pageNum<201)	
		{
			if(len!=64)	{argv[2][len]='\0';len=len+1;}
			at24cxx.write(at24c256,(pageNum+55)*64,(u8 *)argv[2],len);
			CMD_TRACE("savedata[%d] ok\n",pageNum);
		}
		else CMD_TRACE("pageNum must be 1~200,data length must be 1~64\n");
	}
	return 0;
}
int readdata(int argc, char **argv)
{
	if (argc !=2)
	{
			CMD_TRACE("Please input: readdata <page number>\n");
			return -1;
	}	
	else 
	{
		u8 i;
		u8 read_info[64];
		int pageNum=atoi(argv[1]);

		if(0<pageNum &&  pageNum<201)	
		{
			at24cxx.read(at24c256 , (pageNum+55)*64, read_info, 64);
			for(i=0;i<64;i++)	
			{
				if(read_info[i]>127) { read_info[i]='\0';break;}
			}
			CMD_TRACE("readdata[%d]=%.64s\n",pageNum,read_info);
		}
		else CMD_TRACE("pageNum must be 1~200\n");
	}
	return 0;
}

