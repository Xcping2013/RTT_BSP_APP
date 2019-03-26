#ifndef __IIC_SOFT_H
#define __IIC_SOFT_H
#include "defines.h"
//////////////////////////////////////////////////////////////////////////////////	 
//IIC
#define IIC_SDA_IN()  							{GPIOC->CRL&=0XFF0FFFFF;GPIOC->CRL|=(u32)8<<20;}
#define IIC_SDA_OUT() 							{GPIOC->CRL&=0XFF0FFFFF;GPIOC->CRL|=(u32)3<<20;}
#define IIC_SCL    									PCout(4) 
#define IIC_SDA    									PCout(5) 
#define IIC_READ_SDA   							PCin(5)  

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 	 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 	 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 	 IIC_Read_One_Byte(u8 daddr,u8 addr);	  

void IIC_ReadByte(uint8_t DeviceAddr, uint8_t Addr_Data );
void IIC_ReadBytes(uint8_t DeviceAddr, char * Addr_Data, uint8_t len );
void IIC_WriteByte(uint8_t DeviceAddr, uint8_t RegAddr, uint8_t RegData );
void IIC_WriteBytes(uint8_t DeviceAddr, uint8_t RegAddr, char *RegData , uint8_t len);

#endif
















