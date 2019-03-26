#include "mcu_gpio.h"
#include "mcu_delay.h"

#include "mcu_softi2c.h"

void SoftI2c_Init(IIC_GPIO pin);
void SoftI2c_start(IIC_GPIO pin);
void SoftI2c_stop(IIC_GPIO pin);
uint8_t SoftI2c_CheckDevice(IIC_GPIO pin, uint8_t _Address);
uint8_t SoftI2c_wait_ack(IIC_GPIO pin);
void SoftI2c_send_ack(IIC_GPIO pin);
void SoftI2c_send_no_ack(IIC_GPIO pin);
void SoftI2c_send_byte(IIC_GPIO pin,uint8_t byte);
uint8_t SoftI2c_read_byte(IIC_GPIO pin,uint8_t ack);
uint8_t 	SoftI2c_reads(IIC_GPIO pin, uint8_t DeviceType , uint8_t slave_address, uint16_t reg_address, uint8_t *data, uint16_t num_to_read);
uint8_t SoftI2c_writes(IIC_GPIO pin, uint8_t DeviceType, uint8_t slave_address, uint16_t reg_address, uint8_t *data, uint16_t num_to_write);

SoftI2c_DrvTypeDef	SoftI2c=
{
	SoftI2c_Init,
	SoftI2c_CheckDevice,
	SoftI2c_start,
	SoftI2c_stop,
	SoftI2c_wait_ack,
	SoftI2c_send_ack,
	SoftI2c_send_no_ack,
	SoftI2c_send_byte,
	SoftI2c_read_byte,
	SoftI2c_writes,
	SoftI2c_reads,	
};

//IIC���ŵ�·δ����������ֵ����Ҫ����ΪGPIO_Mode_Out_PP  ������ʱ����ΪGPIO_Mode_Out_OD
void SoftI2c_Init(IIC_GPIO pin)
{
		pinMode(pin.sda_pin,GPIO_MODE_OUTPUT_OD);	
    pinMode(pin.scl_pin,GPIO_MODE_OUTPUT_OD);	
		SoftI2c_stop(pin);
}
/*						����ʱ��						 */
//����IIC��ʼ�ź�
void SoftI2c_start(IIC_GPIO pin)
{
	pinMode(pin.sda_pin,GPIO_MODE_OUTPUT_OD);	
  pinSet(pin.sda_pin)  ;
	pinSet(pin.scl_pin)  ;
  drv_udelay(4);
  pinReset(pin.sda_pin)  ;								//START:when CLK is high,DATA change form high to low 
  drv_udelay(4);
  pinReset(pin.scl_pin)  ;								//ǯסI2C���ߣ�׼�����ͻ�������� 
	drv_udelay(4);
}
void SoftI2c_stop(IIC_GPIO pin)
{
    pinMode(pin.sda_pin,GPIO_MODE_OUTPUT_OD);//sda�����
    pinReset(pin.sda_pin)  ;
		drv_udelay(1);
    pinSet(pin.scl_pin)  ;								//STOP:when CLK is high DATA change form low to high
    drv_udelay(4);
    pinSet(pin.sda_pin)  ;								//����I2C���߽����ź�
    drv_udelay(4);
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t SoftI2c_wait_ack(IIC_GPIO pin)
{
	uint8_t ucErrTime=0;
	pinMode(pin.sda_pin,GPIO_MODE_INPUT);
	drv_udelay(1);	
	pinSet(pin.scl_pin)  ;
	drv_udelay(1);
	while(pinRead(pin.sda_pin))
	{
			ucErrTime++;
			if(ucErrTime>250)
			{
					//pinMode(SoftI2c.sda_pin,GPIO_Mode_Out_PP);
					SoftI2c_stop(pin);
					return 1;

			}

	}
	pinReset(pin.scl_pin)  ;
	drv_udelay(1);
	return 0;
}

//����ACKӦ��
void SoftI2c_send_ack(IIC_GPIO pin)
{
//	pinReset(pin.scl_pin)  ;							//L
	pinMode(pin.sda_pin,GPIO_Mode_Out_OD);
	pinReset(pin.sda_pin)  ;							//D
	drv_udelay(2);
	pinSet(pin.scl_pin)  ;								//H
	drv_udelay(2);
	pinReset(pin.scl_pin)  ;
	drv_udelay(2);
	
	pinSet(pin.sda_pin)  ;	
}
//����NACK		
void SoftI2c_send_no_ack(IIC_GPIO pin)
{
		//pinReset(pin.scl_pin)  ;
    pinMode(pin.sda_pin,GPIO_MODE_OUTPUT_OD);
    pinSet(pin.sda_pin)  ;
    drv_udelay(2);
    pinSet(pin.scl_pin)  ;
    drv_udelay(2);
    pinReset(pin.scl_pin)  ;
    drv_udelay(1);
}
/**
 * @brief ��������.
 *
 * @param[in] byte �������͵�����.
 *
 * @return ���ͽ��.����0��ʾ���ͳɹ�������1��ʾ����ʧ��.
 */
void SoftI2c_send_byte(IIC_GPIO pin,uint8_t byte)
{
    //uint8_t ret = 0;
    uint8_t t;
    pinMode(pin.sda_pin,GPIO_MODE_OUTPUT_OD);
		pinReset(pin.scl_pin)  ;									//����ʱ�ӿ�ʼ���ݴ���
    drv_udelay(1);
		for(t=0;t<8;t++)	
    {
        pinWrite(pin.sda_pin , (byte & 0x80)>>7);
				byte<<=1;
        drv_udelay(1);

        pinSet(pin.scl_pin)  ;
        drv_udelay(2);
        pinReset(pin.scl_pin)  ;
        drv_udelay(1);
    }
		pinSet(pin.sda_pin);										// �ͷ�����
//    ret = SoftI2c_wait_ack();
//    return ret;
}
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t SoftI2c_read_byte(IIC_GPIO pin,uint8_t ack)
{
    uint8_t i = 8;
    uint8_t byte = 0;
    pinMode(pin.sda_pin,GPIO_MODE_INPUT);
    for(i=0;i<8;i++ )
    {			
        pinSet(pin.scl_pin)  ;
				byte<<=1;	
				drv_udelay(2);
				if(pinRead(pin.sda_pin))	byte++;
				pinReset(pin.scl_pin)  ;
			  drv_udelay(2);
    }
		if (!ack)
				SoftI2c_send_no_ack(pin);//����nACK
		else
				SoftI2c_send_ack(pin); 	//����ACK  

		return byte;
}

uint8_t SoftI2c_CheckDevice(IIC_GPIO pin, uint8_t _Address)
{
	uint8_t ucAck;

	if (pinRead(pin.sda_pin) && pinRead(pin.scl_pin))
	{
		SoftI2c_start(pin);		/* ���������ź� */

		/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
		SoftI2c_send_byte(pin,_Address | I2C_WR);
		ucAck = SoftI2c_wait_ack(pin);	/* ����豸��ACKӦ�� */

		SoftI2c_stop(pin);			/* ����ֹͣ�ź� */

		return ucAck;
	}
	return 1;	/* I2C�����쳣 */
}
//
//��ַ1~2�ֽ��豸,��Ϊ������EEPROM,�豸REG�Ƚ���,һ�ֽ�
uint8_t SoftI2c_writes(IIC_GPIO pin, uint8_t DeviceType, uint8_t slave_address, uint16_t reg_address, uint8_t *data, uint16_t num_to_write)
{
  SoftI2c_start(pin);  
	switch(DeviceType)
	{
		case 0:	//�Ĵ���ֻ��һ���������ַָ��
						SoftI2c_send_byte(pin, slave_address & 0xFE);   //д������ַ 	  
						if(SoftI2c_wait_ack(pin)) return 1;
		case 1:	//һ�ֽڼĴ�����ַ			
						SoftI2c_send_byte(pin, slave_address & 0xFE);   //д������ַ 	  
						if(SoftI2c_wait_ack(pin)) return 1;
						SoftI2c_send_byte(pin , reg_address%256);  		 	//���͵͵�ַ
						if(SoftI2c_wait_ack(pin)) return 2;
			break;
		case 2:	//���ֽڼĴ�����ַ
						SoftI2c_send_byte(pin, slave_address & 0xFE);   //д������ַ 	  
						if(SoftI2c_wait_ack(pin)) return 1;
						SoftI2c_send_byte(pin, reg_address>>8);				//���͸ߵ�ַ
						if(SoftI2c_wait_ack(pin)) return 2;
						SoftI2c_send_byte(pin, reg_address%256);  		 //���͵͵�ַ
						if(SoftI2c_wait_ack(pin)) return 2;	 
			break;
		case 3:	//�Ĵ�����ַ��������ַ����
						SoftI2c_send_byte(pin, slave_address & 0xFE +((reg_address/256)<<1));   //����������ַ0XA0,д���� 
						if(SoftI2c_wait_ack(pin)) return 1;
						SoftI2c_send_byte(pin, reg_address%256);   			 //���͵͵�ַ
						if(SoftI2c_wait_ack(pin)) return 2;
			break;
		default:
			break;		
	}
		
  while(num_to_write--)
	{
		SoftI2c_send_byte(pin, *(data++));     		//�����ֽ�							   
		if(SoftI2c_wait_ack(pin)) return 3;
	}
  SoftI2c_stop(pin);													//����һ��ֹͣ���� 
	//drv_mdelay(10);
  return 0;
}


uint8_t 	SoftI2c_reads(IIC_GPIO pin, uint8_t DeviceType , uint8_t slave_address, uint16_t reg_address, uint8_t *data, uint16_t num_to_read)
{
	switch(DeviceType)
	{
		case 0:	//�Ĵ���ֻ��һ���������ַָ��
		case 1:	//һ�ֽڼĴ�����ַ			
				SoftI2c_start(pin);
				SoftI2c_send_byte(pin, slave_address & 0xFE); 		//д������ַ 	  
				if(SoftI2c_wait_ack(pin)) return 1;
				SoftI2c_send_byte(pin, reg_address %256);				//���͵͵�ַ
				if(SoftI2c_wait_ack(pin)) return 2;
			break;
		case 2:	//���ֽڼĴ�����ַ
					SoftI2c_start(pin);
					SoftI2c_send_byte(pin, slave_address & 0xFE);   //д������ַ 	  
					if(SoftI2c_wait_ack(pin)) return 1;
					SoftI2c_send_byte(pin, reg_address>>8);				 //���͸ߵ�ַ
					if(SoftI2c_wait_ack(pin)) return 2;
					SoftI2c_send_byte(pin, reg_address%256);  		 	//���͵͵�ַ
					if(SoftI2c_wait_ack(pin)) return 2;
			break;
		case 3:	//�Ĵ�����ַ��������ַ����
					SoftI2c_start(pin);
					SoftI2c_send_byte(pin, slave_address & 0xFE +((reg_address/256)<<1));   //����������ַ0XA0,д���� 
					if(SoftI2c_wait_ack(pin)) return 1;
					SoftI2c_send_byte(pin, reg_address%256);   			 //���͵͵�ַ
					if(SoftI2c_wait_ack(pin)) return 2;
			break;
		default:
			break;	
		
	}
	SoftI2c_start(pin);
	SoftI2c_send_byte(pin, slave_address | 0x01);					//�������ģʽ
	if(SoftI2c_wait_ack(pin)) return 3;

	while(num_to_read!=1)
	{
		num_to_read--;
		*data++ = SoftI2c_read_byte(pin, 1);
	}
	*data++ = SoftI2c_read_byte(pin, 0);
	SoftI2c_stop(pin);
	return 0;
}
//
