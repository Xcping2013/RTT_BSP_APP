#ifndef _INC_MBTMC429_H_
#define _INC_MBTMC429_H_

#define LED_SYS_PIN			PD_7

#define	MCU_TX					PA_9
#define	MCU_RX					PA_10

#define	SWDIO						PA_13
#define	SWCLK						PA_14

#define	IIC_SDA					PB_3
#define	IIC_SCL					PB_4

#define	SPI1_CS					PB_4
#define	SPI1_SCK				PA_5
#define	SPI1_MISO				PA_6
#define	SPI1_MOSI				PA_7

#define	TMC429_CLK1			PB_0
#define	TMC429_CLK2			PB_1		//IN13

#define	POSCMP1					PE_12
#define	INTOUT1					PE_13
#define	INTOUT2					PE_14		//OUT13
#define	POSCMP2					PE_15		//OUT14

#define	SPI2_CS					PB_12		//LED2	H-ON
#define	SPI2_SCK				PB_13		//IN18
#define	SPI2_MISO				PB_14		//OUT9
#define	SPI2_MOSI				PB_15		//IN17

#define	TMC262_CS1			PD_8		//IN14
#define	TMC262_CS2			PD_9		//IN16
#define	TMC262_CS3			PD_10		//OUT10

#define	TMC262_EN1			PD_11		//OUT12
#define	TMC262_EN2			PD_12		//IN15
#define	TMC262_EN3			PD_13		//OUT11

#define INPUT_COUNT    	12	
#define OUTPUT_COUNT    8	

extern uint8_t	InputPin[INPUT_COUNT];
extern uint8_t	OutputPin[OUTPUT_COUNT];
extern uint8_t	OutRgbPin[3];

#endif

