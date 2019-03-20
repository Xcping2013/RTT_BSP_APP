#ifndef __APP_EEPROM_24XX_H
#define __APP_EEPROM_24XX_H

#include "bsp_defines.h"

void at24cxx_hw_init(void);

int savedata(int argc, char **argv);
int readdata(int argc, char **argv);

#endif
















