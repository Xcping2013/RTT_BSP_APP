/********************************************************************************
 Author : CAC(Customer Applications Center) 

 Date :  June, 2013

 File name :  ADT7420Test.c

 Description :	Configure the regisiters of ADt7420, and read the convention data. 

 Hardware plateform : 	ADUc7026 DEMO-B
********************************************************************************/
#include "ADt7420.h"


int adt7420_app(void)
{
  unsigned char value1;
	value1 = ReadFromADT7420ViaI2C(0x06);
	while(1)
	{
		drv_udelay(5000);
	} 	   
}
