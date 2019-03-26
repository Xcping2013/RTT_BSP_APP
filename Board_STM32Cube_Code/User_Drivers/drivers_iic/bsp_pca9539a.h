/*	Low-voltage 16-bit I2C-bus I/O port with interrupt and reset

1.VDD range of 1.65 V to 5.5 V
2.open-drain interrupt (INT)   
	indicate that an input state has changed
	if there is incoming data on its ports without having to communicate via the I2C-bus
3.The device outputs have 25 mA sink capabilities
4.Power-up with all channels configured as inputs

*/
/*
	Device address
	
	slave address
	1 1 1 0 1 A1 A0	R/W
		
		W:0xE8
		R:0xE9	
*/
/*	Registers

	ADDR (1Byte) + CMD(1Byte):

	Pointer register bits	B7 B6 B5 B4 B3 B2 B1 B0

	I0.7 I0.6 I0.5 I0.4 I0.3 I0.2 I0.1 I0.0
	O0.7 O0.6 O0.5 O0.4 O0.3 O0.2 O0.1 O0.0
	N0.7 N0.6 N0.5 N0.4 N0.3 N0.2 N0.1 N0.0

four register pairs

	0		read port0				
	1		read port1
	2		output port 0
	3		output port 1
	4		Polarity Inversion port 0
	5		Polarity Inversion port 1
	6		Configuration port 0		1(default) input or 0 output
	7		Configuration port 1
*/


#ifndef _BSP_PCA9539A_H
#define _BSP_PCA9539A_H

typedef struct 
{
	IIC_GPIO	pins;
	uint8_t devAddress;
	u8 in_data[8];	
	u8 out_data[8];	
	
}pca9539a_t;

typedef struct
{		
  void (*init)		(pca9539a_t);
	u8	 (*check)   (pca9539a_t);
	void (*read)		(pca9539a_t,  u16 , u8 *,	u16 );		
	void (*write)		(pca9539a_t,  u16 , u8 *,	u16 );	
	
	u8 	 (*readU8)	(pca9539a_t , u16 );
	void (*writeU8)	(pca9539a_t , u16 , u8 );
	
	u16	 (*readU16)	(pca9539a_t , u16 );
	void (*writeU16)(pca9539a_t , u16 , u16 );
	
	u32	 (*readU32)	(pca9539a_t , u16 );
	void (*writeU32)(pca9539a_t , u16 , u32 );

	f32  (*readF32)	(pca9539a_t , u16 );	
	void (*writeF32)(pca9539a_t , u16 , f32 );
	
	uint8_t (*ReadBytes)(pca9539a_t ,uint8_t *, uint16_t , uint16_t );
	uint8_t (*WriteBytes)(pca9539a_t , uint8_t *, uint16_t , uint16_t );
	

}PCA9539A_DrvTypeDef; 

extern PCA9539A_DrvTypeDef pca9539a;

extern at24cxx_t at24c256;

#endif
