#ifndef __SPI_H
#define __SPI_H
 	
#include "prototypes.h"

void SPI2_Init(void);			 //初始化SPI口
void SPI2_WriteByte(u8 TxData);//SPI总线读写一个字节
		 
#endif

