#ifndef __INC_MB1616DEV6_H
#define __INC_MB1616DEV6_H

#include "bsp_defines.h"

			

//at24cxx_t at24c256=
//{
//	{0},
//	{PC_14,PC_15},
//	0xA2,	//1 0 1 0 0 0 1 0
//};

#define CS_PIN_TMC429 		 		 PB_12
#define SELECT_TMC429()        GPIOB->BRR=BIT12
#define DESELECT_TMC429()      GPIOB->BSRR=BIT12
						
#define POSCMP1_PIN		PD_13
#define	INTOUT1_PIN		PD_14

#define	MX_SPI_Init			  MX_SPI2_Init					
#define	ReadWriteSPI			ReadWriteSPI2		

extern uint8_t homeSensorPin[3];

#endif
