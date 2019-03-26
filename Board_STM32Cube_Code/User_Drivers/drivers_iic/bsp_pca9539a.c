#include "bsp.h"

//#include "bsp_pca9539a.h"

#define PCA9539A_DEBUG
#ifdef  PCA9539A_DEBUG
    #define PCA9539A_TRACE         printf
#else
    #define PCA9539A_TRACE(...)
#endif

static void PCA9539A_Init(pca9539a_t dev)
{
	SoftI2c.pin_init(dev.pins);
	
	if (SoftI2c.check(dev.pins,dev.devAddress) == 1)
	{
		PCA9539A_TRACE("no pca9539a found\r\n");
	}
	else
	{
		PCA9539A_TRACE("find iic device pca9539a device addr=%d\r\n",dev.devAddress);
		//Set po as input
		SoftI2c.writes(dev.pins,1,dev.devAddress,6,(char*)1,1);
		//set input Polarity
		SoftI2c.writes(dev.pins,1,dev.devAddress,4,(char*)0,1);
		//set p1 as output
		SoftI2c.writes(dev.pins,1,dev.devAddress,7,(char*)0,1);
	}
}

