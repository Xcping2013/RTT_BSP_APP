#ifndef __BSP_MCU_SPI_H_
#define __BSP_MCU_SPI_H_

#include "bsp_include.h"

#define SPI_MODE0 0x02
#define SPI_MODE1 0x00
#define SPI_MODE2 0x03
#define SPI_MODE3 0x01

#define SPI_CLOCK_DIV2     SPI_BAUDRATEPRESCALER_2//SPI_BaudRatePrescaler_2
#define SPI_CLOCK_DIV4     SPI_BAUDRATEPRESCALER_4//SPI_BaudRatePrescaler_4
#define SPI_CLOCK_DIV8     SPI_BAUDRATEPRESCALER_8
#define SPI_CLOCK_DIV16    SPI_BAUDRATEPRESCALER_16
#define SPI_CLOCK_DIV32    SPI_BAUDRATEPRESCALER_32
#define SPI_CLOCK_DIV64    SPI_BAUDRATEPRESCALER_64
#define SPI_CLOCK_DIV128   SPI_BAUDRATEPRESCALER_128
#define SPI_CLOCK_DIV256   SPI_BAUDRATEPRESCALER_256

#define SPI_BITODER_MSB		SPI_FIRSTBIT_MSB//SPI_FirstBit_MSB
#define SPI_BITODER_LSB		SPI_FIRSTBIT_LSB//SPI_FirstBit_LSB

typedef struct
{
	uint8_t				cs_pin;
	//SPI_TypeDef* 	spix;
	uint8_t     	mode;
	uint16_t    	prescaler;
	uint16_t    	bit_order;
} SPI_CONFIG_TYPE;

typedef struct
{
	uint8_t				cs_pin;
	uint8_t				sck_pin;
	uint8_t				miso_pin;
	uint8_t				mosi_pin;
	uint8_t     	mode;
	uint16_t    	bit_order;
	
} SOFT_SPI_CONFIG_TYPE;

typedef struct 
{
	void (*init)(SPI_CONFIG_TYPE *);
	UCHAR (*readwrite)(SPI_CONFIG_TYPE * ,UCHAR , UCHAR );
}SPI_DrvTypeDef;

extern SPI_DrvTypeDef	spi;

typedef struct 
{
	void 		(*init)(SOFT_SPI_CONFIG_TYPE *);
	UCHAR 	(*readwrite)(SOFT_SPI_CONFIG_TYPE * ,UCHAR , UCHAR );
	
	uint8_t (*readByte)(SOFT_SPI_CONFIG_TYPE * );
	void 		(*readBytes)(SOFT_SPI_CONFIG_TYPE *, uint8_t *, uint16_t );
	void 		(*writeByte)(SOFT_SPI_CONFIG_TYPE *, uint8_t );
	void  	(*writeBytes)(SOFT_SPI_CONFIG_TYPE *, uint8_t *, uint16_t );
	
}SOFT_SPI_DrvTypeDef;

extern SOFT_SPI_DrvTypeDef	soft_spi;

void   SPI_init(SPI_CONFIG_TYPE *spi_config);
UCHAR  ReadWriteSPI(SPI_CONFIG_TYPE *spi_config,UCHAR Data, UCHAR LastTransfer);

void SoftSpi_init(SOFT_SPI_CONFIG_TYPE *spi_config);
uint8_t  SoftSpi_Readwrite(SOFT_SPI_CONFIG_TYPE *spi_config, uint8_t data, uint8_t LastTransfer);

uint8_t SoftSpi_readByte(SOFT_SPI_CONFIG_TYPE *spi_config );
void  SoftSpi_readBytes(SOFT_SPI_CONFIG_TYPE *spi_config, uint8_t *rcvdata, uint16_t data_length);
void  SoftSpi_writeByte(SOFT_SPI_CONFIG_TYPE *spi_config, uint8_t data);
void  SoftSpi_writeBytes(SOFT_SPI_CONFIG_TYPE *spi_config, uint8_t *data, uint16_t data_length);


#endif
