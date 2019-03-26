#include "IIC_SOFT.h"
#include "delay.h"

//产生IIC起始信号
void IIC_Start(void)
{
	IIC_SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	IIC_SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	IIC_SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(IIC_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	IIC_SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	IIC_SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	IIC_SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);  //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2);
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	IIC_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
				IIC_SCL=1;
		
        receive<<=1;
        if(IIC_READ_SDA)receive++;   
				delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}


//
void IIC_ReadByte(uint8_t DeviceAddr, uint8_t Addr_Data )
{	  	    																 
  IIC_Start();  
	IIC_Send_Byte(DeviceAddr & 0xFE);   //写器件地址 	   
	IIC_Wait_Ack();
	
  IIC_Send_Byte(Addr_Data);   				//发送寄存器地址
	IIC_Wait_Ack();	
	
	IIC_Start();  	 	   
	IIC_Send_Byte(DeviceAddr | 0x01);   //进入接收模式			   
	IIC_Wait_Ack();	
	
  Addr_Data=IIC_Read_Byte(0);	
  IIC_Stop();												 //产生一个停止条件	    
}
//
void IIC_ReadBytes(uint8_t DeviceAddr, char * Addr_Data, uint8_t len )
{
	u8 i=0;		  	    																 
  IIC_Start();  
	IIC_Send_Byte(DeviceAddr & 0xFE);   //写器件地址 	   
	IIC_Wait_Ack();
	
  IIC_Send_Byte(*Addr_Data);   				//发送寄存器地址
	IIC_Wait_Ack();	
	
	IIC_Start();  	 	   
	IIC_Send_Byte(DeviceAddr | 0x01);   //进入接收模式			   
	IIC_Wait_Ack();	
	
	for(i=0;i<len;i++)
	{
		*(Addr_Data+i)=IIC_Read_Byte(0);	
	}
  IIC_Stop();												 //产生一个停止条件	    
}
//
void IIC_WriteByte(uint8_t DeviceAddr, uint8_t RegAddr, uint8_t RegData )
{				   	  	    																 
  IIC_Start();  
	IIC_Send_Byte(DeviceAddr & 0xFE);   //写器件地址 	   
	IIC_Wait_Ack();
	
  IIC_Send_Byte(RegAddr);   					//发送寄存器地址
	IIC_Wait_Ack();	
 										  		   
	IIC_Send_Byte(RegData);     				//发送字节							   
	IIC_Wait_Ack(); 
	
  IIC_Stop();													//产生一个停止条件 
	HAL_Delay(10);
}
//
void IIC_WriteBytes(uint8_t DeviceAddr, uint8_t RegAddr, char *RegData , uint8_t len)
{	
  IIC_Start();  
	IIC_Send_Byte(DeviceAddr & 0xFE);   //写器件地址 	   
	IIC_Wait_Ack();
	
  IIC_Send_Byte(RegAddr);   					//发送寄存器地址
	IIC_Wait_Ack();	
 	
  while(len--)
	{
		IIC_Send_Byte(*(RegData++));     	//发送字节							   
		IIC_Wait_Ack(); 
	}
	
  IIC_Stop();													//产生一个停止条件 
	HAL_Delay(10);
}




















