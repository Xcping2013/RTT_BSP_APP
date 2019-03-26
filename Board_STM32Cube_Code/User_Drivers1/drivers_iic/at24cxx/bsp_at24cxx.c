#include "bsp_at24cxx.h" 
#include "mcu_delay.h"

#define min(X,Y)  ( (X) > (Y) ? 	(Y) : (X) ) 

const device_t C24LC01   = {AT24C01, 0x007F, 0x08, 0x01,	1};
const device_t C24LC02   = {AT24C02, 0x00FF, 0x08, 0x01,	1};
const device_t C24LC04   = {AT24C04, 0x01FF, 0x10, 0x02,	2};
const device_t C24LC08   = {AT24C08, 0x03FF, 0x10, 0x04,	2};
const device_t C24LC16   = {AT24C16, 0x07FF, 0x10, 0x08,	2};
const device_t C24LC32   = {AT24C32, 0x0FFF, 0x20, 0x01,	2};
const device_t C24LC64   = {AT24C64, 0x1FFF, 0x20, 0x01,	2};
const device_t C24LC128  = {AT24C128,0x3FFF, 0x40, 0x01,	2};
const device_t C24LC256  = {AT24C256,0x7FFF, 0x40, 0x01,	2};
const device_t C24LC512  = {AT24C512,0xFFFF, 0x40, 0x01,	2};

//��ʼ��IIC�ӿ�
void AT24CXX_Init(at24cxx_t dev)
{
	SoftI2c.pin_init(dev.pins);
}
void AT24CXX_Reads(at24cxx_t dev, u16 address,u8 *data,	u16 length)
{
	if(address+length < (dev.eeprom.maxAddress + 2)	)
	{
		if (dev.eeprom.type < AT24C04)
		{
			SoftI2c.reads(dev.pins, 1, dev.devAddress, address ,data,length);
		}
		else if (dev.eeprom.type > AT24C02 && dev.eeprom.type < AT24C32)
		{
			SoftI2c.reads(dev.pins, 3, dev.devAddress, address ,data,length);
		}
		else 
		{
			SoftI2c.reads(dev.pins, 2, dev.devAddress, address ,data,length);
		}	
	}
}
/**
	ex:AT24C256_Writes(10,data,100):
	---1.	SizeLeft=100;				OffsetPage=10%64=10;	SizeNext=54 			write(10,&data[0],54);
	---2.	SizeLeft=100-54=46	OffsetPage=64%64=0;	 	SizeNext=46 			write(64,&data[54],46);
  ---3. SizeLeft==46-46=0		

 */
void AT24CXX_Writes(at24cxx_t dev, u16 address,u8 *data,	u16 length)
{	
	if(address+length < ( dev.eeprom.maxAddress + 2) )
	{
		s16 SizeLeft	 = length;						// bytes left to write
		s16 OffsetData = 0;									// current offset in data pointer
		s16 OffsetPage = 0;									// current offset in page
		s16 SizeNext 	 = 0;									// next n bytes to write	
		
		while(SizeLeft>0)									  //��������Ҫд
		{	
			//ҳƫ�Ƶ�ַ 	
			OffsetPage	 = address % dev.eeprom.pageSize;		
			SizeNext 		 = min(min(SizeLeft, dev.eeprom.pageSize), dev.eeprom.pageSize - OffsetPage);		
			//��ҳƫ�Ƶ�ַ��ʼд���´ο�д�ĳ���
			if (dev.eeprom.type < AT24C04)
			{
				SoftI2c.writes(dev.pins, 1, dev.devAddress, address,(data+OffsetData),SizeNext);
			}
			else if (dev.eeprom.type > AT24C02 && dev.eeprom.type < AT24C32)
			{
				SoftI2c.writes(dev.pins, 3, dev.devAddress, address,(data+OffsetData),SizeNext);
			}
			else 
			{
				SoftI2c.writes(dev.pins, 2, dev.devAddress, address,(data+OffsetData),SizeNext);
			}
			drv_mdelay(5);
			SizeLeft-=SizeNext;							//ʣ�µ��ֽ���
			OffsetData+=SizeNext;						//�´���Ҫд���DATAλ��
			address+=SizeNext;							//�´���Ҫд���ADDRλ��	
		}
	}
}
//��AT24CXXָ����ַ����һ������
u8 AT24CXX_ReadU8(at24cxx_t dev, u16 address)
{				  
	AT24CXX_Reads(dev,address,dev.data,1);
	return dev.data[0];
}
/**
 * Write byte
 */
void AT24CXX_WriteU8(at24cxx_t dev, u16 address,u8 data)
{			
	AT24CXX_Writes(dev, address, &data,1);
	drv_mdelay(5);
}
//
u16 AT24CXX_ReadU16(at24cxx_t dev, u16 address)
{			
	AT24CXX_Reads(dev, address, dev.data, 2);
	return *(u16*)&dev.data[0];
}
//
void AT24CXX_WriteU16(at24cxx_t dev, u16 address,u16 data)
{			
	AT24CXX_Writes(dev, address, (u8*)&data,2);
	drv_mdelay(5);
}
/**
 * Read long
 */
u32 AT24CXX_ReadU32(at24cxx_t dev, u16 address)
{			
	AT24CXX_Reads(dev, address, dev.data, 4);
	return *(u32*)&dev.data[0];
}
/**
 * Write long
 */
void AT24CXX_WriteU32(at24cxx_t dev, u16 address,u32 data)
{		
	AT24CXX_Writes(dev, address, (u8*)&data,4);	
	drv_mdelay(5);
}

/**
 * Read float
 */
f32 AT24CXX_ReadF32(at24cxx_t dev, u16 address)
{		
	AT24CXX_Reads(dev, address, dev.data, 4);
	return *(float*)&dev.data[0];
}
/**
 * Write float
 */
void AT24CXX_WriteF32(at24cxx_t dev, u16 address,f32 data)
{			
	AT24CXX_Writes(dev, address, (u8*)&data,4);	
	drv_mdelay(5);
}
//armfly
uint8_t AT24CXX_CheckOk(at24cxx_t dev)
{
	if (SoftI2c.check(dev.pins ,dev.devAddress ) == 0)
	{
		return 1;
	}
	else
	{
		/* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */
		SoftI2c.stop(dev.pins);
		return 0;
	}
}

uint8_t ee_ReadBytes(at24cxx_t dev,uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i;

	/* ���ô���EEPROM�漴��ȡָ�����У�������ȡ�����ֽ� */

	/* ��1��������I2C���������ź� */
	SoftI2c.start(dev.pins);

	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	SoftI2c.sendbyte(dev.pins,dev.devAddress | I2C_WR);	/* �˴���дָ�� */

	/* ��3��������ACK */
	if (SoftI2c.waitack(dev.pins) != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
	if (dev.eeprom.addrBytes == 1)
	{
		SoftI2c.sendbyte(dev.pins,(uint8_t)_usAddress);
		if (SoftI2c.waitack(dev.pins) != 0)
		{
			goto cmd_fail;	/* EEPROM������Ӧ�� */
		}
	}
	else
	{
		SoftI2c.sendbyte(dev.pins,(_usAddress >> 8));
		if (SoftI2c.waitack(dev.pins) != 0)
		{
			goto cmd_fail;	/* EEPROM������Ӧ�� */
		}

		SoftI2c.sendbyte(dev.pins,(uint8_t)_usAddress);
		if (SoftI2c.waitack(dev.pins) != 0)
		{
			goto cmd_fail;	/* EEPROM������Ӧ�� */
		}
	}

	/* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
	SoftI2c.start(dev.pins);

	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	SoftI2c.sendbyte(dev.pins,dev.devAddress | I2C_RD);	/* �˴��Ƕ�ָ�� */

	/* ��8��������ACK */
	if (SoftI2c.waitack(dev.pins) != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��9����ѭ����ȡ���� */
	for (i = 0; i < _usSize; i++)
	{
		_pReadBuf[i] = SoftI2c.readbyte(dev.pins,0);	/* ��1���ֽ� */

		/* ÿ����1���ֽں���Ҫ����Ack�� ���һ���ֽڲ���ҪAck����Nack */
		if (i != _usSize - 1)
		{
			SoftI2c.ack(dev.pins);	/* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */
		}
		else
		{
			SoftI2c.nack(dev.pins);	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
		}
	}
	/* ����I2C����ֹͣ�ź� */
	SoftI2c.stop(dev.pins);
	return 1;	/* ִ�гɹ� */

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	SoftI2c.stop(dev.pins);
	return 0;
}
//
uint8_t ee_WriteBytes(at24cxx_t dev, uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i,m;
	uint16_t usAddr;

	/*
		д����EEPROM�������������������ȡ�ܶ��ֽڣ�ÿ��д����ֻ����ͬһ��page��
		����24xx02��page size = 8
		�򵥵Ĵ�����Ϊ�����ֽ�д����ģʽ��ÿд1���ֽڣ������͵�ַ
		Ϊ���������д��Ч��: ����������page wirte������
	*/

	usAddr = _usAddress;
	for (i = 0; i < _usSize; i++)
	{
		/* �����͵�1���ֽڻ���ҳ���׵�ַʱ����Ҫ���·��������źź͵�ַ */
		if ((i == 0) || (usAddr & (dev.eeprom.pageSize - 1)) == 0)
		{
			/*���ڣ�������ֹͣ�źţ������ڲ�д������*/
			SoftI2c.stop(dev.pins);

			/* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С�� 10ms
				CLKƵ��Ϊ200KHzʱ����ѯ����Ϊ30������
			*/
			for (m = 0; m < 1000; m++)
			{
				/* ��1��������I2C���������ź� */
				SoftI2c.start(dev.pins);

				/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
				SoftI2c.sendbyte(dev.pins,dev.devAddress | I2C_WR);	/* �˴���дָ�� */

				/* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
				if (SoftI2c.waitack(dev.pins) == 0)
				{
					break;
				}
			}
			if (m  == 1000)
			{
				goto cmd_fail;	/* EEPROM����д��ʱ */
			}

			/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
			if (dev.eeprom.addrBytes == 1)
			{
				SoftI2c.sendbyte(dev.pins,(uint8_t)usAddr);
				if (SoftI2c.waitack(dev.pins) != 0)
				{
					goto cmd_fail;	/* EEPROM������Ӧ�� */
				}
			}
			else
			{
				SoftI2c.sendbyte(dev.pins,usAddr >> 8);
				if (SoftI2c.waitack(dev.pins) != 0)
				{
					goto cmd_fail;	/* EEPROM������Ӧ�� */
				}

				SoftI2c.sendbyte(dev.pins,usAddr);
				if (SoftI2c.waitack(dev.pins) != 0)
				{
					goto cmd_fail;	/* EEPROM������Ӧ�� */
				}
			}
		}

		/* ��6������ʼд������ */
		SoftI2c.sendbyte(dev.pins,_pWriteBuf[i]);

		/* ��7��������ACK */
		if (SoftI2c.waitack(dev.pins) != 0)
		{
			goto cmd_fail;	/* EEPROM������Ӧ�� */
		}

		usAddr++;	/* ��ַ��1 */
	}

	/* ����ִ�гɹ�������I2C����ֹͣ�ź� */
	SoftI2c.stop(dev.pins);
	return 1;

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	SoftI2c.stop(dev.pins);
	return 0;
}


//armfly

AT24CXX_DrvTypeDef at24cxx=
{
	AT24CXX_Init,
	AT24CXX_CheckOk,
	AT24CXX_Reads,
	AT24CXX_Writes,
	
	AT24CXX_ReadU8,
	AT24CXX_WriteU8,
	
	AT24CXX_ReadU16,
	AT24CXX_WriteU16,
	
	AT24CXX_ReadU32,
	AT24CXX_WriteU32,
	
	AT24CXX_ReadF32,
	AT24CXX_WriteF32,
	
	ee_ReadBytes,
	ee_WriteBytes,
};

/*********************************   END  *********************************
*/
