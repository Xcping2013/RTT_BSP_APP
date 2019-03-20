#include "bsp_mcu_gpio.h"
#include "bsp_mcu_softI2c.h"

static void i2c_Delay(void);

void 		SoftI2c_Init(IIC_GPIO pin);
void 		SoftI2c_start(IIC_GPIO pin);
void 		SoftI2c_stop(IIC_GPIO pin);
uint8_t SoftI2c_CheckDevice(IIC_GPIO pin, uint8_t _Address);
uint8_t SoftI2c_wait_ack(IIC_GPIO pin);
void 		SoftI2c_send_ack(IIC_GPIO pin);
void 		SoftI2c_send_no_ack(IIC_GPIO pin);
void 		SoftI2c_send_byte(IIC_GPIO pin,uint8_t byte);
uint8_t SoftI2c_read_byte(IIC_GPIO pin,uint8_t ack);
uint8_t SoftI2c_reads(IIC_GPIO pin, uint8_t DeviceType , uint8_t slave_address, uint16_t reg_address, uint8_t *data, uint16_t num_to_read);
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
    pinMode(pin.sda_pin,GPIO_Mode_Out_OD);	
    pinMode(pin.scl_pin,GPIO_Mode_Out_OD);	
		SoftI2c_stop(pin);
}
/*						����ʱ��						 */
//����IIC��ʼ�ź�
void SoftI2c_start(IIC_GPIO pin)
{
	pinMode(pin.sda_pin,GPIO_Mode_Out_OD);	//sda�����

  pinSet(pin.sda_pin)  ;
	pinSet(pin.scl_pin)  ;
  delay_us(4);
  pinReset(pin.sda_pin)  ;								//START:when CLK is high,DATA change form high to low 
  delay_us(4);
  pinReset(pin.scl_pin)  ;								//ǯסI2C���ߣ�׼�����ͻ�������� 
	delay_us(4);
}
void SoftI2c_stop(IIC_GPIO pin)
{
    pinMode(pin.sda_pin,GPIO_Mode_Out_OD);//sda�����
    pinReset(pin.sda_pin)  ;
		delay_us(1);
    pinSet(pin.scl_pin)  ;								//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    pinSet(pin.sda_pin)  ;								//����I2C���߽����ź�
    delay_us(4);
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t SoftI2c_wait_ack(IIC_GPIO pin)
{
	uint8_t ucErrTime=0;
	pinMode(pin.sda_pin,GPIO_Mode_IPU);
	delay_us(1);	
	pinSet(pin.scl_pin)  ;
	delay_us(1);
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
	delay_us(1);
	return 0;
}

//����ACKӦ��
void SoftI2c_send_ack(IIC_GPIO pin)
{
	pinReset(pin.scl_pin)  ;							//L
	pinMode(pin.sda_pin,GPIO_Mode_Out_OD);
	pinReset(pin.sda_pin)  ;							//D
	delay_us(2);
	pinSet(pin.scl_pin)  ;								//H
	delay_us(2);
	pinReset(pin.scl_pin)  ;
	delay_us(2);
	pinSet(pin.sda_pin)  ;		/* CPU�ͷ�SDA���� */				
}
//����NACK		
void SoftI2c_send_no_ack(IIC_GPIO pin)
{
		pinReset(pin.scl_pin)  ;
    pinMode(pin.sda_pin,GPIO_Mode_Out_OD);
    pinSet(pin.sda_pin)  ;
    delay_us(2);
    pinSet(pin.scl_pin)  ;
    delay_us(2);
    pinReset(pin.scl_pin)  ;
    delay_us(1);
}

uint8_t SoftI2c_CheckDevice(IIC_GPIO pin, uint8_t _Address)
{
	uint8_t ucAck;

	if ( pinRead(pin.sda_pin)  &&  pinRead(pin.scl_pin))
	{
		SoftI2c_start(pin);		/* ���������ź� */

		/* �����豸��ַ+��д����bit��0 = w|I2C_WR�� 1 = r) bit7 �ȴ� */
		SoftI2c_send_byte(pin,_Address | 0 );
		ucAck = SoftI2c_wait_ack(pin);	/* ����豸��ACKӦ�� */

		SoftI2c_stop(pin);			/* ����ֹͣ�ź� */

		return ucAck;
	}
	return 1;	/* I2C�����쳣 */	
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
    u8 t;
    pinMode(pin.sda_pin,GPIO_Mode_Out_OD);
		pinReset(pin.scl_pin)  ;									//����ʱ�ӿ�ʼ���ݴ���
    delay_us(1);
		for(t=0;t<8;t++)	
    {
        pinWrite(pin.sda_pin , (byte & 0x80)>>7);
				byte<<=1;
        delay_us(1);

        pinSet(pin.scl_pin)  ;
        delay_us(2);
        pinReset(pin.scl_pin)  ;
        delay_us(1);
    }
		//pinSet(pin.sda_pin)  ;		/* CPU�ͷ�SDA���� */
//    ret = SoftI2c_wait_ack();
//    return ret;
}
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t SoftI2c_read_byte(IIC_GPIO pin,uint8_t ack)
{
    uint8_t i = 8;
    uint8_t byte = 0;
    pinMode(pin.sda_pin,GPIO_Mode_IPU);
    for(i=0;i<8;i++ )
    {
				delay_us(2);
        pinSet(pin.scl_pin)  ;
				byte<<=1;	
				if(pinRead(pin.sda_pin))	byte++;
        delay_us(2);
				pinReset(pin.scl_pin)  ;
    }
		if (!ack)
				SoftI2c_send_no_ack(pin);//����nACK
		else
				SoftI2c_send_ack(pin); 	//����ACK  

		return byte;
}
/*						����ʱ��						 */

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
	//delay_ms(10);
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
static void i2c_Delay(void)
{
	uint8_t i;

	/*��
		CPU��Ƶ72MHzʱ�����ڲ�Flash����, MDK���̲��Ż�����̨ʽʾ�����۲Ⲩ�Ρ�
		ѭ������Ϊ30ʱ��SCLƵ�� = 440KHz�� SCL�ߵ�ƽʱ��4us��SCL�͵�ƽʱ��8us

		��������ѡ��2.2Kŷʱ��SCL������ʱ��Լ0.5us�����ѡ4.7Kŷ����������Լ1us

		ʵ��Ӧ��ѡ��400KHz���ҵ����ʼ���
	*/
	for (i = 0; i < 8; i++);
}

//#define EEPROM_RTT_DEBUG

//#ifdef 	EEPROM_RTT_DEBUG
//#define EEPROM_TRACE         rt_kprintf
//#else
//#define EEPROM_TRACE(...)     
//#endif
///**********************************AT24CXX_USER_APP**********************************
//*/
//at24cxx_t at24c256=
//{
//	{0},
//	{PB_4,PB_3},	//SDA SCL
//	0xA0,
//};

//void at24cxx_hw_init(void)
//{
//	at24c256.eeprom=C24LC256;//C24LC256;
//	at24cxx.init(at24c256);
//}

//rt_err_t save_data(u8 pageNum , char *Commands)
//{
//	u8 len=strlen(Commands);
//	if(len<65	&& pageNum>0)	
//	{
//		if(len!=64)	{Commands[len]='\0';len=len+1;}
//		at24cxx.write(at24c256,pageNum*64,Commands,len);
//		EEPROM_TRACE("<ok>");
//	}
//	else EEPROM_TRACE("<pageNum must be 1~255,commands len must be 1~64>");
//}

//void read_data(u8 pageNum)
//{
//	u8 read_info[64];
//	u8 i;
//	if(pageNum>0)	
//	{
//		at24cxx.read(at24c256 , pageNum*64, read_info, 64);
//		for(i=0;i<64;i++)	
//		{
//			if(read_info[i]>127) { read_info[i]='\0';break;}
//		}
//		EEPROM_TRACE("<%.64s>",read_info);
//	}
//	else EEPROM_TRACE("<pageNum must be 1~255>");
//}

///*********************************   END  *********************************
//*/

