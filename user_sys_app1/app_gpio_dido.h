#ifndef __APP_GPIO_DIDO_H
#define __APP_GPIO_DIDO_H

#include "bsp_mcu_gpio.h"

#if defined(USING_INC_MB1616DEV6) 
//at24cxx_t at24c256=
//{
//	{0},
//	{PC_14,PC_15},
//	0xA2,	//1 0 1 0 0 0 1 0
//};
typedef struct 
{
	IIC_GPIO	pins;
	uint8_t devAddress;
	uint8_t in_data;	
	uint8_t out_data;	
	
}pca9539a_t;

extern pca9539a_t pca9539a_1;
extern pca9539a_t pca9539a_2;

#endif

void dido_hw_init(void);
int readinput(int argc, char **argv);
int output(int argc, char **argv);
int rgb(int argc, char **argv);
int beep(int argc, char **argv);

uint8_t getChInput(uint8_t channel);
void setChOutput(uint8_t channel, uint8_t setval);

#endif
