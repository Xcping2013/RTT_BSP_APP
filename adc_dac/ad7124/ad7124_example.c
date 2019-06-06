/***************************************************************************//**
*   @file    AD7124.c
*   @brief   AD7124 implementation file.
*   @devices AD7124-4, AD7124-8
*
********************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "ad7124.h"
#include "ad7124_regs.h"

/* Error codes */
#define INVALID_VAL -1 /* Invalid argument */
#define COMM_ERR    -2 /* Communication error on receive */
#define TIMEOUT     -3 /* A timeout has occured */

struct ad7124_dev my_ad7124;                    /* A new driver instance */
struct ad7124_dev *ad7124_handler = &my_ad7124; /* A driver handle to pass around */
enum ad7124_registers regNr;                       /* Variable to iterate through registers */
long timeout = 1000;                               /* Number of tries before a function times out */
long ret = 0;                                      /* Return value */
long sample;                                       /* Stores raw value read from the ADC */

struct ad7124_st_reg ad7124_regs1[AD7124_REG_NO] = {
	{0x00, 0,   1, 2}, /* AD7124_Status */
	{0x01, 0, 2, 1}, /* AD7124_ADC_Control */
	{0x02, 0, 3, 2}, /* AD7124_Data */
	{0x03, 0, 3, 1}, /* AD7124_IOCon1 */
	{0x04, 0, 2, 1}, /* AD7124_IOCon2 */
	{0x05, 0,   1, 2}, /* AD7124_ID */
	{0x06, 0, 3, 2}, /* AD7124_Error */
	{0x07, 0, 3, 1}, /* AD7124_Error_En */
	{0x08, 0,   1, 2}, /* AD7124_Mclk_Count */
	{0x09, 0, 2, 1}, /* AD7124_Channel_0 */
	{0x0A, 0, 2, 1}, /* AD7124_Channel_1 */
	{0x0B, 0, 2, 1}, /* AD7124_Channel_2 */
	{0x0C, 0, 2, 1}, /* AD7124_Channel_3 */
	{0x0D, 0, 2, 1}, /* AD7124_Channel_4 */
	{0x0E, 0, 2, 1}, /* AD7124_Channel_5 */
	{0x0F, 0, 2, 1}, /* AD7124_Channel_6 */
	{0x10, 0, 2, 1}, /* AD7124_Channel_7 */
	{0x11, 0, 2, 1}, /* AD7124_Channel_8 */
	{0x12, 0, 2, 1}, /* AD7124_Channel_9 */
	{0x13, 0, 2, 1}, /* AD7124_Channel_10 */
	{0x14, 0, 2, 1}, /* AD7124_Channel_11 */
	{0x15, 0, 2, 1}, /* AD7124_Channel_12 */
	{0x16, 0, 2, 1}, /* AD7124_Channel_13 */
	{0x17, 0, 2, 1}, /* AD7124_Channel_14 */
	{0x18, 0, 2, 1}, /* AD7124_Channel_15 */
	{0x19, 0, 2, 1}, /* AD7124_Config_0 */
	{0x1A, 0, 2, 1}, /* AD7124_Config_1 */
	{0x1B, 0, 2, 1}, /* AD7124_Config_2 */
	{0x1C, 0, 2, 1}, /* AD7124_Config_3 */
	{0x1D, 0, 2, 1}, /* AD7124_Config_4 */
	{0x1E, 0, 2, 1}, /* AD7124_Config_5 */
	{0x1F, 0, 2, 1}, /* AD7124_Config_6 */
	{0x20, 0, 2, 1}, /* AD7124_Config_7 */
	{0x21, 0, 3, 1}, /* AD7124_Filter_0 */
	{0x22, 0, 3, 1}, /* AD7124_Filter_1 */
	{0x23, 0, 3, 1}, /* AD7124_Filter_2 */
	{0x24, 0, 3, 1}, /* AD7124_Filter_3 */
	{0x25, 0, 3, 1}, /* AD7124_Filter_4 */
	{0x26, 0, 3, 1}, /* AD7124_Filter_5 */
	{0x27, 0, 3, 1}, /* AD7124_Filter_6 */
	{0x28, 0, 3, 1}, /* AD7124_Filter_7 */
	{0x29, 0, 3, 1}, /* AD7124_Offset_0 */
	{0x2A, 0, 3, 1}, /* AD7124_Offset_1 */
	{0x2B, 0, 3, 1}, /* AD7124_Offset_2 */
	{0x2C, 0, 3, 1}, /* AD7124_Offset_3 */
	{0x2D, 0, 3, 1}, /* AD7124_Offset_4 */
	{0x2E, 0, 3, 1}, /* AD7124_Offset_5 */
	{0x2F, 0, 3, 1}, /* AD7124_Offset_6 */
	{0x30, 0, 3, 1}, /* AD7124_Offset_7 */
	{0x31, 0, 3, 1}, /* AD7124_Gain_0 */
	{0x32, 0, 3, 1}, /* AD7124_Gain_1 */
	{0x33, 0, 3, 1}, /* AD7124_Gain_2 */
	{0x34, 0, 3, 1}, /* AD7124_Gain_3 */
	{0x35, 0, 3, 1}, /* AD7124_Gain_4 */
	{0x36, 0, 3, 1}, /* AD7124_Gain_5 */
	{0x37, 0, 3, 1}, /* AD7124_Gain_6 */
	{0x38, 0, 3, 1}, /* AD7124_Gain_7 */
};

struct ad7124_st_reg ad7124_regs0[] =
{
	{0x00, 0x00,   1, 2}, /* AD7124_Status */
	
	//REF_EN;POWER_MODE=full power  连续转换模式
	{0x01, 0x01C0, 2, 1}, /* AD7124_ADC_Control */
	
	{0x02, 0x0000, 3, 2}, /* AD7124_Data */
	{0x03, 0x0000, 3, 1}, /* AD7124_IOCon1 */
	{0x04, 0x0000, 2, 1}, /* AD7124_IOCon2 */
	{0x05, 0x02,   1, 2}, /* AD7124_ID */
	{0x06, 0x0000, 3, 2}, /* AD7124_Error */
	{0x07, 0x0044, 3, 1}, /* AD7124_Error_En */
	{0x08, 0x00,   1, 2}, /* AD7124_Mclk_Count */
	
	//Enable=1	10000 = temperature senso
//	{0x09, 0x8210, 2, 1}, /* AD7124_Channel_0 */
	
	//Enable=1	00011 = AIN3
	{0x09, 0x8064, 2, 1}, /* AD7124_Channel_0 */	
	
//	{0x0A, 0x0001, 2, 1}, /* AD7124_Channel_1 */
//	{0x0B, 0x0001, 2, 1}, /* AD7124_Channel_2 */
//	{0x0C, 0x0001, 2, 1}, /* AD7124_Channel_3 */
//	{0x0D, 0x0001, 2, 1}, /* AD7124_Channel_4 */
//	{0x0E, 0x0001, 2, 1}, /* AD7124_Channel_5 */
//	{0x0F, 0x0001, 2, 1}, /* AD7124_Channel_6 */
//	{0x10, 0x0001, 2, 1}, /* AD7124_Channel_7 */
//	{0x11, 0x0001, 2, 1}, /* AD7124_Channel_8 */
//	{0x12, 0x0001, 2, 1}, /* AD7124_Channel_9 */
//	{0x13, 0x0001, 2, 1}, /* AD7124_Channel_10 */
//	{0x14, 0x0001, 2, 1}, /* AD7124_Channel_11 */
//	{0x15, 0x0001, 2, 1}, /* AD7124_Channel_12 */
//	{0x16, 0x0001, 2, 1}, /* AD7124_Channel_13 */
//	{0x17, 0x0001, 2, 1}, /* AD7124_Channel_14 */
//	{0x18, 0x0001, 2, 1}, /* AD7124_Channel_15 */
	//bipolar  internal reference PGA=1
	{0x19, 0x0860, 2, 1}, /* AD7124_Config_0 */
	
//{0x19, 0x0070, 2, 1}, /* AD7124_Config_0 */
//	{0x1A, 0x0870, 2, 1}, /* AD7124_Config_1 */
//	{0x1B, 0x0870, 2, 1}, /* AD7124_Config_2 */
//	{0x1C, 0x0870, 2, 1}, /* AD7124_Config_3 */
//	{0x1D, 0x0870, 2, 1}, /* AD7124_Config_4 */
//	{0x1E, 0x0870, 2, 1}, /* AD7124_Config_5 */
//	{0x1F, 0x0870, 2, 1}, /* AD7124_Config_6 */
//	{0x20, 0x0870, 2, 1}, /* AD7124_Config_7 */
	{0x21, 0x060180, 3, 1}, /* AD7124_Filter_0 */
//	{0x22, 0x060180, 3, 1}, /* AD7124_Filter_1 */
//	{0x23, 0x060180, 3, 1}, /* AD7124_Filter_2 */
//	{0x24, 0x060180, 3, 1}, /* AD7124_Filter_3 */
//	{0x25, 0x060180, 3, 1}, /* AD7124_Filter_4 */
//	{0x26, 0x060180, 3, 1}, /* AD7124_Filter_5 */
//	{0x27, 0x060180, 3, 1}, /* AD7124_Filter_6 */
//	{0x28, 0x060180, 3, 1}, /* AD7124_Filter_7 */
	{0x29, 0x800000, 3, 1}, /* AD7124_Offset_0 */
//	{0x2A, 0x800000, 3, 1}, /* AD7124_Offset_1 */
//	{0x2B, 0x800000, 3, 1}, /* AD7124_Offset_2 */
//	{0x2C, 0x800000, 3, 1}, /* AD7124_Offset_3 */
//	{0x2D, 0x800000, 3, 1}, /* AD7124_Offset_4 */
//	{0x2E, 0x800000, 3, 1}, /* AD7124_Offset_5 */
//	{0x2F, 0x800000, 3, 1}, /* AD7124_Offset_6 */
//	{0x30, 0x800000, 3, 1}, /* AD7124_Offset_7 */
	
	{0x31, 0x500000, 3, 1}, /* AD7124_Gain_0 */
//	{0x32, 0x500000, 3, 1}, /* AD7124_Gain_1 */
//	{0x33, 0x500000, 3, 1}, /* AD7124_Gain_2 */
//	{0x34, 0x500000, 3, 1}, /* AD7124_Gain_3 */
//	{0x35, 0x500000, 3, 1}, /* AD7124_Gain_4 */
//	{0x36, 0x500000, 3, 1}, /* AD7124_Gain_5 */
//	{0x37, 0x500000, 3, 1}, /* AD7124_Gain_6 */
//	{0x38, 0x500000, 3, 1}, /* AD7124_Gain_7 */
};


int32_t ad7124_setup(struct ad7124_dev *device) 
{
	int32_t ret;
	enum ad7124_registers reg_nr;
	
	struct spi_init_param spi_param;
	
	if (!device)
		return INVALID_VAL;

	device->regs = ad7124_regs;
	device->regs = ad7124_regs0;
	device->spi_rdy_poll_cnt = 25000;
	
	/* Initialize the SPI communication. */
	
	ret = spi_init(&device->spi_desc, &spi_param);
	if (ret < 0)
		return ret;

	/*  Reset the device interface.*/
	ret = ad7124_reset(device);
	if (ret < 0)
		return ret;

	/* Update the device structure with power-on/reset settings */
	device->check_ready = 1;

	/* Initialize registers AD7124_ADC_Control through AD7124_Filter_7. */
	for(reg_nr = AD7124_Status; (reg_nr < 14) && !(ret < 0);//AD7124_Offset_0
	    reg_nr++) {
		if (device->regs[reg_nr].rw == AD7124_RW) {
			ret = ad7124_write_register(device, device->regs[reg_nr]);
			if (ret < 0)
				break;
		}

		/* Get CRC State and device SPI interface settings */
		if (reg_nr == AD7124_Error_En) {
			ad7124_update_crcsetting(device);
			ad7124_update_dev_spi_settings(device);
		}
	}
	return ret;
}

double AD7124_ReadData(struct ad7124_dev *device)
{
	struct ad7124_st_reg *regs;
	int32_t sum =0;
	double  ret =0.0;

	if(!device)
		return INVALID_VAL;

	regs = device->regs;


/* Read the value of the Data Register */
	ad7124_read_register(device, &regs[AD7124_Data]);

	/* Get the read result */
	sum = regs[AD7124_Data].value;

	//ret = (sum/8388608.0-1)*2500000.0 +10.0;
	ret = (sum*2500000.0)/16777216 ;
	
	return ret;
	
}
double AD7124_ReadAverData(struct ad7124_dev *device)
{
	double sum=0;
	int8_t i=0;
	
	for(i=0;i<10;i++)
	{
		sum += AD7124_ReadData(device);
	}
	
	return sum/10.0;
}

void AD7124(void)
{
    /* Initialize AD7124 device. */
		pinMode(PD_10, PIN_MODE_OUTPUT);
	  pinSet(PD_10);
    ret = ad7124_setup(&my_ad7124);
	  delay_ms(100);
    if (ret < 0)
    {
			AD7124_TRACE("ad7124_setup,ret=%d\n",ret);
        /* AD7124 initialization failed, check the value of ret! */
    }
    else
    {
        /* AD7124 initialization OK */
    } 
 
    /* Read all registers */
    for (regNr = AD7124_Status; (regNr < AD7124_REG_NO) && !(ret < 0); regNr++)
    {
        ret = ad7124_read_register(ad7124_handler, &ad7124_regs1[regNr]);
			  AD7124_TRACE("ad7124_regs[%d]=0x%x\n",regNr,ad7124_regs1[regNr].value);
    }
 
    /* Read data from the ADC */
    ret = ad7124_wait_for_conv_ready(ad7124_handler, timeout);
    if (ret < 0)
    {
			AD7124_TRACE("ad7124_conv_err,ret=%d\n",ret);
	/* Something went wrong, check the value of ret! */
    }
 
    ret = ad7124_read_data(ad7124_handler, &sample);
		AD7124_TRACE("sample=%d\n",sample);
    if (ret < 0)
    {
			AD7124_TRACE("ad7124_read_data_err,ret=%d\n",ret);
        /* Something went wrong, check the value of ret! */
    }
}
void readAD(void)
{
//			ready = (regs[AD7124_Status].value &
//			 AD7124_STATUS_REG_RDY) != 0;			BUG1			
		//ad7124_write_register(ad7124_handler,ad7124_regs1[regno]);
    ret = ad7124_wait_for_conv_ready(ad7124_handler, timeout);
    if (ret < 0)
    {
			AD7124_TRACE("ad7124_conv_err,ret=%d\n",ret);
    }
    ret = ad7124_read_data(ad7124_handler, &sample);
		AD7124_TRACE("sampleD=%d\n",sample);
		double voltage;
		uint8_t voltageStr[10];
		voltage=AD7124_ReadAverData(ad7124_handler);
		//AD7124_TRACE("voltageD=%f uV\n",voltage);
		sprintf(voltageStr,"%lf",voltage);							//RTT prinf float
		AD7124_TRACE("voltageStr=%s uV\n",voltageStr);
		if (ret < 0)
    {
			AD7124_TRACE("ad7124_read_data_err,ret=%d\n",ret);
    }	
}

void voltage(void)
{
		ad7124_regs1[AD7124_Channel_0].value=0x8064;
		ad7124_regs1[AD7124_Channel_1].value=0x00c7;
		ad7124_regs1[AD7124_Channel_2].value=0x0022;
		//Enable=1	00011 = AIN3
		ad7124_write_register(ad7124_handler,ad7124_regs1[AD7124_Channel_0]);
		ad7124_write_register(ad7124_handler,ad7124_regs1[AD7124_Channel_1]);
		ad7124_write_register(ad7124_handler,ad7124_regs1[AD7124_Channel_2]);

		ret = ad7124_wait_for_conv_ready(ad7124_handler, timeout);

		ret = ad7124_read_data(ad7124_handler, &sample);
		AD7124_TRACE("sampleD=%d\n",sample);
		double voltage;
		uint8_t voltageStr[10];
		voltage=AD7124_ReadAverData(ad7124_handler);
		//AD7124_TRACE("voltageD=%f uV\n",voltage);
		sprintf(voltageStr,"%lf",voltage);							//RTT prinf float
		AD7124_TRACE("voltageStr=%s uV\n",voltageStr);
}
void res(void)
{
	 ad7124_regs1[AD7124_IOCon1].value=0x0070;
	
		ad7124_regs1[AD7124_Channel_0].value=0x0064;
		ad7124_regs1[AD7124_Channel_1].value=0x00c7;		
		ad7124_regs1[AD7124_Channel_2].value=0x8022;
	
		ad7124_regs1[AD7124_Config_2].value=0x0877;														//PGA=128
	
		ad7124_write_register(ad7124_handler,ad7124_regs1[AD7124_Channel_0]);
		ad7124_write_register(ad7124_handler,ad7124_regs1[AD7124_Channel_1]);
		ad7124_write_register(ad7124_handler,ad7124_regs1[AD7124_Channel_2]);

		ad7124_write_register(ad7124_handler,ad7124_regs1[AD7124_Config_2]);
	
		ret = ad7124_wait_for_conv_ready(ad7124_handler, timeout);

		ret = ad7124_read_data(ad7124_handler, &sample);
		AD7124_TRACE("sampleD=%d\n",sample);
		double voltage;
		uint8_t voltageStr[10];
		voltage=AD7124_ReadAverData(ad7124_handler);
		//AD7124_TRACE("voltageD=%f uV\n",voltage);
		sprintf(voltageStr,"%lf",voltage);							//RTT prinf float
		AD7124_TRACE("voltageStr=%s uV\n",voltageStr);
}
void current(void)
{
		ad7124_regs1[AD7124_Channel_0].value=0x0064;
		ad7124_regs1[AD7124_Channel_1].value=0x80c7;													//ENABLE
		ad7124_regs1[AD7124_Channel_2].value=0x0022;
	
		ad7124_regs1[AD7124_Config_1].value=0x0877;														//PGA=128
	
		ad7124_write_register(ad7124_handler,ad7124_regs1[AD7124_Channel_0]);	//DISABLE
		ad7124_write_register(ad7124_handler,ad7124_regs1[AD7124_Channel_1]);
		ad7124_write_register(ad7124_handler,ad7124_regs1[AD7124_Channel_2]);
	
		ad7124_write_register(ad7124_handler,ad7124_regs1[AD7124_Config_1]);
		ret = ad7124_wait_for_conv_ready(ad7124_handler, timeout);

		ret = ad7124_read_data(ad7124_handler, &sample);
		AD7124_TRACE("sampleD=%d\n",sample);
		double voltage;
		uint8_t voltageStr[10];
		voltage=AD7124_ReadAverData(ad7124_handler);
		//AD7124_TRACE("voltageD=%f uV\n",voltage);
		sprintf(voltageStr,"%lf",voltage);							//RTT prinf float
		AD7124_TRACE("voltageStr=%s uV\n",voltageStr);
}
int readT(int argc, char **argv)
{
	if (argc == 2)
	{
		uint8_t regno=atoi(argv[1]);
		ad7124_read_register(ad7124_handler,&ad7124_regs[regno]);
		AD7124_TRACE("ad7124_read_register[%d]=%d......\n",regno,ad7124_regs[regno].value);
		return 0;
	}
	return 1;
}
int writeT(int argc, char **argv)
{
	if (argc == 3)
	{
		uint8_t regno=atoi(argv[1]);
		int32_t regval=atoi(argv[2]);
		ad7124_regs1[regno].value=regval;
		
		ad7124_write_register(ad7124_handler,ad7124_regs1[regno]);
		AD7124_TRACE("ad7124_write_register[%d]=%d......\n",regno,ad7124_regs1[regno].value);
		return 0;
	}
	return 1;
}
MSH_CMD_EXPORT(AD7124, AD7124);
MSH_CMD_EXPORT(readT, AD7124);
MSH_CMD_EXPORT(writeT, AD7124);
MSH_CMD_EXPORT(readAD, AD7124);

MSH_CMD_EXPORT(voltage, AD7124);
MSH_CMD_EXPORT(current, AD7124);
MSH_CMD_EXPORT(res, AD7124);
////
//ad7124_regs0 ad7124_regs1 ad7124_regs
/*

1	---	adc reset
2	---	select power mode
3	---	channel_0  pga	ref_select
4	---	DATA_STATUS
5	---	wait for rdy=0,read data
6 ---



*/




