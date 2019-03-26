#ifndef __INC_MBTMC429__H
#define __INC_MBTMC429__H

#include "bsp_include.h"

//OSE 8MHZ  USE UART2	SPI1

#define INPUT1	PBin(5)
#define INPUT2	PBin(6)
#define INPUT3	PBin(7)
#define INPUT4	PBin(8)
#define INPUT5	PBin(9)

#define INPUT6	PEin(0)
#define INPUT7	PEin(1)
#define INPUT8	PEin(2)
#define INPUT9	PEin(3)
#define INPUT10	PEin(4)
#define INPUT11	PEin(5)
#define INPUT12	PEin(6)

#define OUTPUT1		PAout(15)
#define OUTPUT2		PCout(10)
#define OUTPUT3		PCout(11)
#define OUTPUT4		PCout(12)
#define OUTPUT5		PDout(0)
#define OUTPUT6		PDout(1)
#define OUTPUT7		PDout(2)
#define OUTPUT8		PDout(3)

#define RGB_R 	PDout(6)
#define RGB_G 	PDout(5)
#define RGB_B 	PDout(4)

//MBTMC429
//at24cxx_t at24c256=
//{
//	{0},
//	{PB_4,PB_3},	//SDA SCL
//	0xA0,
//};
//#define LED_PIN								 PD_7
#if defined(USING_INC_MBTMC429) 

	#define CS_PIN_TMC429 		 		 PC_4
	#define SELECT_TMC429()        GPIOC->BRR=BIT4
	#define DESELECT_TMC429()      GPIOC->BSRR=BIT4
												
	#define POSCMP1_PIN		91
	#define	INTOUT1_PIN		92	

	#define	MX_SPI_Init			  MX_SPI1_Init					
	#define	ReadWriteSPI			ReadWriteSPI1	

	extern uint8_t homeSensorPin[3];
#endif

extern uint8_t inputs_pin_num[12];
extern uint8_t outputs_pin_num[8];
extern uint8_t rgb_pin_num[3];

#endif
