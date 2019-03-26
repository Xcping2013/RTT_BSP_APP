/********************************************************************************
 Author : CAC(Customer Applications Center) 

 Date :  June, 2013

 File name :  ADT7420.h

 Description :	Define some surnames for the each bit of the regisiters of ADT7420. 

 Hardware plateform : 	ADUc7026 DEMO-B
********************************************************************************/

#ifndef _ADT7420_H_
#define _ADT7420_H_

// add the header file here
/*
*/
unsigned char ReadFromADT7420ViaI2C(unsigned char );
void WriteToADT7420ViaI2C(unsigned char, unsigned char );
void WriteToADT7420Byte(unsigned char );

#endif //_ADT7420_H_

