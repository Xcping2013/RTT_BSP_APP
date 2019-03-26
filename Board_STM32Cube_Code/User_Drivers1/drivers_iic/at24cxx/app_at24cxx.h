#ifndef __APP_AT24CX_H
#define __APP_AT24CX_H

#include "bsp_at24cxx.h" 

void at24cxx_hw_init(void);
void save_data(uint8_t pageNum , char *data);
void read_data(uint8_t pageNum);

#endif
















