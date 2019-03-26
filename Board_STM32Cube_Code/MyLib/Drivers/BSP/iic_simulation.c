#include "iic_simulation.h"
#include "delay.h"

//SCL�ڲ�������0	�������ݱ仯
void IIC_Init(void)
{
	//By CubeMX
}
//����IIC��ʼ�ź�		
void IIC_Start(void)
{  	
	IIC_SDA_OUT();
  
	IIC_SDA=1;			//START:when CLK is high,DATA change form high to low 
	IIC_SCL=1;			//SDA=1	SCL=1						
	delay_us(1);
	
	IIC_SDA=0;			//SDA=0
	delay_us(1);
	
	IIC_SCL=0;			//׼�����ͻ�������� 
}	    
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	IIC_SDA_OUT();	
	
	IIC_SDA=0;			//STOP:when CLK is high DATA change form low to high
	IIC_SCL=1;			//SDA=0	SCL=1	
	delay_us(1);

	IIC_SDA=1;			//SDA=1
	delay_us(1);						   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	IIC_SDA=1;		//IIC_SCL=0;
	IIC_SDA_IN(); delay_us(1);	//SDA����Ϊ����  	   
	
	IIC_SCL=1;		delay_us(1);	//�ȶ����ݣ�ǰ��Ĳ�����ʹDevice���Ӧ��
	
	while(IIC_READ_SDA)				//��ȡ����
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;						//ǯסI2C���ߣ�׼�����ͻ�������� ���� =0 SDA���ݲŻ�仯   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL=0;					//L
	
	IIC_SDA_OUT();
	IIC_SDA=0;					//D
	delay_us(1);
	
	IIC_SCL=1;					//H
	delay_us(1);
	
	IIC_SCL=0;					//ǯסI2C���ߣ�׼�����ͻ�������� ���� SCL=0 SDA���ݲŻ�仯
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL=0;				//0
	
	IIC_SDA_OUT();
	IIC_SDA=1;				//
	
	delay_us(1);
	IIC_SCL=1;				//1
	
	delay_us(1);
	IIC_SCL=0;				 //ǯסI2C���ߣ�׼�����ͻ�������� ���� SCL=0 SDA���ݲŻ�仯
}				 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{  
	//0-->1 Out	
	u8 t; 
	IIC_SDA_OUT(); 	  
	
	IIC_SCL=0;								//!> 0
	for(t=0;t<8;t++)				
	{              						
		IIC_SDA=(txd&0x80)>>7;	//!> ����
		txd<<=1; 	  
		delay_us(1);  						//����ĳЩdevice��������ʱ���Ǳ����
		
		IIC_SCL=1;							//!> 1			
		delay_us(1);
		
		IIC_SCL=0;							//!> 0					
		delay_us(1);
  }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	//!  1> 0 ����һ������  ��һ���������ȶ���
	unsigned char i,receive=0;
	IIC_SDA=1; 		
	IIC_SDA_IN();										//SDA����Ϊ����
	//IIC_SCL=0;											//0	
	for(i=0;i<8;i++ )
	{		
		delay_us(1); 
		IIC_SCL=1;										//1
		receive<<=1;	
		if(IIC_READ_SDA)	receive++; 	//������
		delay_us(1);	
		IIC_SCL=0;										//0		
	}					 
	if (!ack)
			IIC_NAck();//����nACK
	else
			IIC_Ack(); //����ACK   
	return receive;
}


//
void IIC_DeviceReadBytes(uint8_t DeviceType, uint8_t DeviceAddr, u16 ReadAddr, uint8_t *RegData, uint8_t len )
{	  	    																 
  IIC_Start();  
	
	switch(DeviceType)
	{
		case 1:	//һ�ֽڼĴ�����ַ			
						IIC_Send_Byte(DeviceAddr & 0xFE);   //д������ַ 	  
						IIC_Wait_Ack();
						IIC_Send_Byte(ReadAddr%256);  		  //���͵͵�ַ
						IIC_Wait_Ack(); 	 	
			break;
		case 2:	//���ֽڼĴ�����ַ
						IIC_Send_Byte(DeviceAddr & 0xFE);   //д������ַ 	  
						IIC_Wait_Ack();
						IIC_Send_Byte(ReadAddr>>8);				 //���͸ߵ�ַ
						IIC_Wait_Ack();
						IIC_Send_Byte(ReadAddr%256);  		 //���͵͵�ַ
						IIC_Wait_Ack(); 
			break;
		case 3:	//�Ĵ�����ַ��������ַ����
						IIC_Send_Byte(DeviceAddr & 0xFE +((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 
						IIC_Wait_Ack();	
						IIC_Send_Byte(ReadAddr%256);   			 //���͵͵�ַ
						IIC_Wait_Ack(); 
			break;
		default:
			break;		
	}
	IIC_Start();  	 	   
	IIC_Send_Byte(DeviceAddr | 0x01);    //�������ģʽ			   
	IIC_Wait_Ack();	
	
	while(len!=1)
	{
		len--;
		*(RegData++)=IIC_Read_Byte(1);	
	}
	*(RegData++)=IIC_Read_Byte(0);
  IIC_Stop();												 //����һ��ֹͣ����	    
}
//
void IIC_DeviceWriteBytes(uint8_t DeviceType,uint8_t DeviceAddr, uint16_t WriteAddr, uint8_t  *RegData , uint8_t len)
{	
  IIC_Start();  
	switch(DeviceType)
	{
		case 1:	//һ�ֽڼĴ�����ַ			
						IIC_Send_Byte(DeviceAddr & 0xFE);   //д������ַ 	  
						IIC_Wait_Ack();
						IIC_Send_Byte(WriteAddr%256);  		 //���͵͵�ַ
						IIC_Wait_Ack(); 	 	
			break;
		case 2:	//���ֽڼĴ�����ַ
						IIC_Send_Byte(DeviceAddr & 0xFE);   //д������ַ 	  
						IIC_Wait_Ack();
						IIC_Send_Byte(WriteAddr>>8);				//���͸ߵ�ַ
						IIC_Wait_Ack();
						IIC_Send_Byte(WriteAddr%256);  		 //���͵͵�ַ
						IIC_Wait_Ack(); 	 
			break;
		case 3:	//�Ĵ�����ַ��������ַ����
						IIC_Send_Byte(DeviceAddr & 0xFE +((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
						IIC_Wait_Ack();	
						IIC_Send_Byte(WriteAddr%256);   			 //���͵͵�ַ
						IIC_Wait_Ack(); 
			break;
		default:
			break;		
	}
		
  while(len--)
	{
		IIC_Send_Byte(*(RegData++));     	//�����ֽ�							   
		IIC_Wait_Ack();		
	}
  IIC_Stop();													//����һ��ֹͣ���� 
	delay_ms(10);
}
//
























