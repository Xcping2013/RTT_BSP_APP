#ifndef __APP_EEPROM_24XX_H
#define __APP_EEPROM_24XX_H

#include "bsp_defines.h"

void at24cxx_hw_init(void);

	#if defined(USING_RTT) 
	int savedata(int argc, char **argv);
	int readdata(int argc, char **argv);

	#else 
//	void save_data(uint8_t pageNum , char *data);
//	void read_data(uint8_t pageNum);

//	uint8_t  CMD_fsn(char *Commands);
//	uint8_t  CMD_rom(char *Commands);
//	uint8_t  CMD_RomPage(char *Commands);

	#endif

#endif
















