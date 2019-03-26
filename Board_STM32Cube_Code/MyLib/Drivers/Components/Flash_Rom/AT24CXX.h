#ifndef __AT24CX_H
#define __AT24CX_H

#include "iic_simulation.h"
//	     		Kbit		Byte				pageSize					MaxFreq	 		Write Cycle   data 	word 			address 
#define AT24C01			127					//16*8		2**7			400K	
#define AT24C02			255					//32*8		2**8   		1MHZ			8-bit 
#define AT24C04			511					//32*16							1MHZ		        											1010A2A1P0W/R
#define AT24C08			1023				//64*16																										1010A2P1P0W/R
#define AT24C16			2047				//128*16																									1010P2P1P0W/R
#define AT24C32			4095				//128*32																									1010A2A1A0W/R
#define AT24C64	    8191
#define AT24C128		16383				//256*64Byte				12MHZ			5ms Max/100W 		14-bit
#define AT24C256		32767  

#define DEVICE_TYPE 		2
#define EE_TYPE 				AT24C128
#define EEPROM_ADDR 		0xA0
#define EE_PAGE_SIZE 		64

//#define EE_Test 		

extern u8 dataTemp_EE[8];

void AT24CXX_Init(void);
					  
u8 	 AT24CXX_ReadU8(u16 ReadAddr);
void AT24CXX_WriteU8(u16 address,u8 data);

u16  AT24CXX_ReadU16(u16 address);
void AT24CXX_WriteU16(u16 address,u16 data);

u32  AT24CXX_ReadU32(u16 address);
void AT24CXX_WriteU32(u16 address,u32 data);

f32  AT24CXX_ReadF32(u16 address);
void AT24CXX_WriteF32(u16 address,f32 data);

void AT24CXX_Reads(u16 address,u8 *data,	u16 length);
void AT24CXX_Writes(u16 address,u8 *data,	u16 length);

void AT24CXX_FunctionTest(void);
	
#endif
















