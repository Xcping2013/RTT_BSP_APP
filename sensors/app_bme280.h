#ifndef __APP_BME280__H
#define __APP_BME280__H

#include "bsp_bme280.h"

#define BME280_CS_PIN		 		 PB_1
#define BME280_CS_PIN_0()    GPIOB->BRR=BIT1
#define BME280_CS_PIN_1()    GPIOB->BSRR=BIT1
		
extern struct bme280_dev bme280_dev1;	

int8_t Bme280InitViaSpi(struct bme280_dev *dev);
void msTaskNoneRTOS_bme280(uint16_t msTime);

//BME280_64BIT_ENABLE 	For 64 bit machine.

#endif
