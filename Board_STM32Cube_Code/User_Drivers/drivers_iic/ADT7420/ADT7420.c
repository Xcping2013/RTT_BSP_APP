/********************************************************************************
 Author : CAC(Customer Applications Center) 

 Date :  June, 2013

 File name :  ADT7420.c

 Description :	Several functions to Configure the regisiters of ADT7420. 

 Hardware plateform : 	ADUc7026 DEMO-B
********************************************************************************/

#include "mcu_gpio.h"
#include "mcu_delay.h"

#include "ADT7420.h"

#define SDA		PE0
#define SCL		PE1
/********************************************************************************
 Function: 	 ReadFromADT7420ViaI2C

 Parameter:  none

 Return value :	 unsigned int

 Description :	Configure the regisiters of ADT7420, and read the convention data. 
********************************************************************************/
unsigned char ReadFromADT7420ViaI2C(unsigned char Dads)
{
	unsigned char   i;
	unsigned char   i2c_Value;

	pinSet(SDA,1);	 
	drv_udelay(5);
	pinSet(SCL,1); 
  drv_udelay(5);  
	pinSet(SDA,0);  //开始条件

	WriteToADT7420Byte(0x92);
	WriteToADT7420Byte(Dads);  //输出芯片写地址和寄存器地址
	
	pinSet(SDA,1);	
  drv_udelay(5);	
	pinSet(SCL,1);
  drv_udelay(5);	
	pinSet(SDA,0);  //开始条件
	
	WriteToADT7420Byte(0x93);  //输出芯片读地址
	
	for(i=0;i<8;i++)  
	{  
   	pinSet(SCL,0); 
  	i2c_Value = (i2c_Value << 1);
		drv_udelay(2);
		if(ADuC7026InputBit(SDA) == 1)  
         	i2c_Value |= 0x01;  
    else  
         	i2c_Value &= ~0x01;
	
		pinSet(SCL,1);  
		drv_udelay(5);
	} 
	
	pinSet(SCL,0); 
	pinSet(SDA,0);
	drv_udelay(2);
  pinSet(SCL,1); 
  drv_udelay(5);
	
	pinSet(SDA,0);
	drv_udelay(5);
	pinSet(SCL,1);
	drv_udelay(5);
	pinSet(SDA,1);  //终止条件

	return  i2c_Value; 
}

/********************************************************************************
 Function: 	 WriteToADT7420ViaI2C

 Parameter:  unsigned char spi_mosiValue

 Return value :	 none

 Description :	Configure the regisiters of ADT7420, and read the convention data. 
********************************************************************************/
void WriteToADT7420ViaI2C(unsigned char Dads, unsigned char d)
{

	unsigned char k;
	unsigned char Value;
	Value = d;
	
	pinSet(SDA,1);	 
	drv_udelay(5);
	pinSet(SCL,1); 
  drv_udelay(5);  
	pinSet(SDA,0);  //开始条件

  WriteToADT7420Byte(0x92);	
	WriteToADT7420Byte(Dads);
	
	for(k=0;k<8;k++)  
	{  
   		pinSet(SCL,0); 

   		if((Value & 0x80)==0x80)  
     		pinSet(SDA,1);  
   		else  
     		pinSet(SDA,0); 
   		drv_udelay(2);

		pinSet(SCL,1);  
  		
   		Value = (Value << 1);  
		
		drv_udelay(5);
	}       //输出字节
	
	pinSet(SCL,0); 
	ADuC7026InputBit(SDA);
	drv_udelay(2);
  pinSet(SCL,1); 
  drv_udelay(5);
  
	pinSet(SDA,0);
	drv_udelay(5);
	pinSet(SCL,1);
	drv_udelay(5);
	pinSet(SDA,1);  //终止条件

}

	void WriteToADT7420Byte(unsigned char h)
	{
		unsigned char j;
		unsigned char i2c_Value;
		i2c_Value = h;
		
		for(j=0;j<8;j++)  
		{  
				pinSet(SCL,0); 

				if((i2c_Value & 0x80)==0x80)  
					pinSet(SDA,1);  
				else  
					pinSet(SDA,0); 
				drv_udelay(2);

			pinSet(SCL,1);  
				
				i2c_Value = (i2c_Value << 1);  
			
			drv_udelay(5);
		}       //输出字节
		
		pinSet(SCL,0); 
		ADuC7026InputBit(SDA);
		drv_udelay(2);
		pinSet(SCL,1); 
		drv_udelay(5);  //接收应答
		pinSet(SCL,0);
	}
