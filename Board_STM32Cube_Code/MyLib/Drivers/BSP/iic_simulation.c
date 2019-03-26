#include "iic_simulation.h"
#include "delay.h"

//SCL在操作后都置0	允许数据变化
void IIC_Init(void)
{
	//By CubeMX
}
//产生IIC起始信号		
void IIC_Start(void)
{  	
	IIC_SDA_OUT();
  
	IIC_SDA=1;			//START:when CLK is high,DATA change form high to low 
	IIC_SCL=1;			//SDA=1	SCL=1						
	delay_us(1);
	
	IIC_SDA=0;			//SDA=0
	delay_us(1);
	
	IIC_SCL=0;			//准备发送或接收数据 
}	    
//产生IIC停止信号
void IIC_Stop(void)
{
	IIC_SDA_OUT();	
	
	IIC_SDA=0;			//STOP:when CLK is high DATA change form low to high
	IIC_SCL=1;			//SDA=0	SCL=1	
	delay_us(1);

	IIC_SDA=1;			//SDA=1
	delay_us(1);						   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	IIC_SDA=1;		//IIC_SCL=0;
	IIC_SDA_IN(); delay_us(1);	//SDA设置为输入  	   
	
	IIC_SCL=1;		delay_us(1);	//稳定数据，前面的操作会使Device输出应答
	
	while(IIC_READ_SDA)				//读取数据
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;						//钳住I2C总线，准备发送或接收数据 必须 =0 SDA数据才会变化   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;					//L
	
	IIC_SDA_OUT();
	IIC_SDA=0;					//D
	delay_us(1);
	
	IIC_SCL=1;					//H
	delay_us(1);
	
	IIC_SCL=0;					//钳住I2C总线，准备发送或接收数据 必须 SCL=0 SDA数据才会变化
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;				//0
	
	IIC_SDA_OUT();
	IIC_SDA=1;				//
	
	delay_us(1);
	IIC_SCL=1;				//1
	
	delay_us(1);
	IIC_SCL=0;				 //钳住I2C总线，准备发送或接收数据 必须 SCL=0 SDA数据才会变化
}				 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{  
	//0-->1 Out	
	u8 t; 
	IIC_SDA_OUT(); 	  
	
	IIC_SCL=0;								//!> 0
	for(t=0;t<8;t++)				
	{              						
		IIC_SDA=(txd&0x80)>>7;	//!> 数据
		txd<<=1; 	  
		delay_us(1);  						//对于某些device这三个延时都是必须的
		
		IIC_SCL=1;							//!> 1			
		delay_us(1);
		
		IIC_SCL=0;							//!> 0					
		delay_us(1);
  }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	//!  1> 0 出来一个数据  第一个数据需先读走
	unsigned char i,receive=0;
	IIC_SDA=1; 		
	IIC_SDA_IN();										//SDA设置为输入
	//IIC_SCL=0;											//0	
	for(i=0;i<8;i++ )
	{		
		delay_us(1); 
		IIC_SCL=1;										//1
		receive<<=1;	
		if(IIC_READ_SDA)	receive++; 	//读数据
		delay_us(1);	
		IIC_SCL=0;										//0		
	}					 
	if (!ack)
			IIC_NAck();//发送nACK
	else
			IIC_Ack(); //发送ACK   
	return receive;
}


//
void IIC_DeviceReadBytes(uint8_t DeviceType, uint8_t DeviceAddr, u16 ReadAddr, uint8_t *RegData, uint8_t len )
{	  	    																 
  IIC_Start();  
	
	switch(DeviceType)
	{
		case 1:	//一字节寄存器地址			
						IIC_Send_Byte(DeviceAddr & 0xFE);   //写器件地址 	  
						IIC_Wait_Ack();
						IIC_Send_Byte(ReadAddr%256);  		  //发送低地址
						IIC_Wait_Ack(); 	 	
			break;
		case 2:	//两字节寄存器地址
						IIC_Send_Byte(DeviceAddr & 0xFE);   //写器件地址 	  
						IIC_Wait_Ack();
						IIC_Send_Byte(ReadAddr>>8);				 //发送高地址
						IIC_Wait_Ack();
						IIC_Send_Byte(ReadAddr%256);  		 //发送低地址
						IIC_Wait_Ack(); 
			break;
		case 3:	//寄存器地址与器件地址复用
						IIC_Send_Byte(DeviceAddr & 0xFE +((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 
						IIC_Wait_Ack();	
						IIC_Send_Byte(ReadAddr%256);   			 //发送低地址
						IIC_Wait_Ack(); 
			break;
		default:
			break;		
	}
	IIC_Start();  	 	   
	IIC_Send_Byte(DeviceAddr | 0x01);    //进入接收模式			   
	IIC_Wait_Ack();	
	
	while(len!=1)
	{
		len--;
		*(RegData++)=IIC_Read_Byte(1);	
	}
	*(RegData++)=IIC_Read_Byte(0);
  IIC_Stop();												 //产生一个停止条件	    
}
//
void IIC_DeviceWriteBytes(uint8_t DeviceType,uint8_t DeviceAddr, uint16_t WriteAddr, uint8_t  *RegData , uint8_t len)
{	
  IIC_Start();  
	switch(DeviceType)
	{
		case 1:	//一字节寄存器地址			
						IIC_Send_Byte(DeviceAddr & 0xFE);   //写器件地址 	  
						IIC_Wait_Ack();
						IIC_Send_Byte(WriteAddr%256);  		 //发送低地址
						IIC_Wait_Ack(); 	 	
			break;
		case 2:	//两字节寄存器地址
						IIC_Send_Byte(DeviceAddr & 0xFE);   //写器件地址 	  
						IIC_Wait_Ack();
						IIC_Send_Byte(WriteAddr>>8);				//发送高地址
						IIC_Wait_Ack();
						IIC_Send_Byte(WriteAddr%256);  		 //发送低地址
						IIC_Wait_Ack(); 	 
			break;
		case 3:	//寄存器地址与器件地址复用
						IIC_Send_Byte(DeviceAddr & 0xFE +((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 
						IIC_Wait_Ack();	
						IIC_Send_Byte(WriteAddr%256);   			 //发送低地址
						IIC_Wait_Ack(); 
			break;
		default:
			break;		
	}
		
  while(len--)
	{
		IIC_Send_Byte(*(RegData++));     	//发送字节							   
		IIC_Wait_Ack();		
	}
  IIC_Stop();													//产生一个停止条件 
	delay_ms(10);
}
//
























