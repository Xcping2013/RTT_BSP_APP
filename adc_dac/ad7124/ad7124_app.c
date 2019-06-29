/******************************************************************************/
/*		 配置
ready = (regs[AD7124_Status].value &
AD7124_STATUS_REG_RDY) != 0;			BUG1	

1.Channel_n:	
	AINP
	AINM 
	Setup=SetupN	
	Enable=TRUE
2.CONFIG_N: 
  PGA_N=X;	
	AIN_BUFP & AIN_BUFM =ENABLED ;	
	BIPOLAR=DISABLE 
	REF_SEL= Interanl Reference
3.FILTER_N
	Filter = Sinc4
	FS_0 = 384
4.ADC_Control
	MODE = Continuous Conversion
	POWER_MODE = FULL POWER
	REF_EN = Enabled
5.IO_CONTROL_1:
	IOUT1 Channel Enable = AINN
	IOUT1 Select = xxxuA
	
	校准:
1. 从寄存器树中选择ADC_Control寄存器。
2. 仅使能通道0。
3. 选择Low Power(低功耗)模式。
4. 执行内部满量程校准。
a. 单击ADC控制寄存器的Mode(模式)位域。
b. 选择内部满量程校准。
c. 选择寄存器树中的Gain0寄存器，检查校准是否已执行，并且系数是否改变。
5. 执行内部零电平校准。
a. 单击ADC控制寄存器的Mode(模式)位域。
b. 选择内部零电平校准。
c. 选择寄存器树中的Oset0寄存器，检查校准是否已执行，并且系数是否改变

			//111 0000	unipolar  internal reference PGA=1		
//			ad7124_regsW[AD7124_Filter_0].value=0x0070;
//			ad7124_regsW[AD7124_Offset_0].value=0x0070;				
//			ad7124_regsW[AD7124_Gain_0].value=0x0070;	
*/
/******************************************************************************/
#include <stdlib.h>
#include "ad7124.h"
#include "ad7124_regs.h"

#include "dmm.h"
#include "cal.h"
/* Error codes */
#define INVALID_VAL -1 			/* Invalid argument */
#define COMM_ERR    -2 			/* Communication error on receive */
#define TIMEOUT     -3 			/* A timeout has occured */

struct ad7124_dev my_ad7124;                       /* A new driver instance */
struct ad7124_dev *ad7124_handler = &my_ad7124; 	 /* A driver handle to pass around */
enum ad7124_registers regNr;                       /* Variable to iterate through registers */
long timeout = 1000;                               /* Number of tries before a function times out */
long ret = 0;                                      /* Return value */
long sample;                                       /* Stores raw value read from the ADC */

struct ad7124_st_reg ad7124_regsR[AD7124_REG_NO] = {
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

struct ad7124_st_reg ad7124_regsW[] =
{
	{0x00, 0x00,   1, 2}, /* AD7124_Status */
	{0x01, 0x0140, 2, 1}, /* AD7124_ADC_Control */	//1 0100 0000 REF_EN;POWER_MODE=zhong power  单次读模式
	{0x02, 0x0000, 3, 2}, /* AD7124_Data */
	{0x03, 0x0000, 3, 1}, /* AD7124_IOCon1 */
	{0x04, 0x0000, 2, 1}, /* AD7124_IOCon2 */
	{0x05, 0x02,   1, 2}, /* AD7124_ID */
	{0x06, 0x0000, 3, 2}, /* AD7124_Error */
	{0x07, 0x0044, 3, 1}, /* AD7124_Error_En */
	{0x08, 0x00,   1, 2}, /* AD7124_Mclk_Count */
//	{0x09, 0x8210, 2, 1}, /* AD7124_Channel_0 */ //Enable=1	10000 = temperature senso
	{0x09, 0x8064, 2, 1}, /* AD7124_Channel_0 */	//1000000001100100	 Enable=1	00011 = AIN3
	
	{0x0A, 0x00c7, 2, 1}, /* AD7124_Channel_1 */
	{0x0B, 0x0022, 2, 1}, /* AD7124_Channel_2 */
	{0x0C, 0x0001, 2, 1}, /* AD7124_Channel_3 */
	{0x0D, 0x0001, 2, 1}, /* AD7124_Channel_4 */
	{0x0E, 0x0001, 2, 1}, /* AD7124_Channel_5 */
	{0x0F, 0x0001, 2, 1}, /* AD7124_Channel_6 */
	{0x10, 0x0001, 2, 1}, /* AD7124_Channel_7 */
	{0x11, 0x0001, 2, 1}, /* AD7124_Channel_8 */
	{0x12, 0x0001, 2, 1}, /* AD7124_Channel_9 */
	{0x13, 0x0001, 2, 1}, /* AD7124_Channel_10 */
	{0x14, 0x0001, 2, 1}, /* AD7124_Channel_11 */
	{0x15, 0x0001, 2, 1}, /* AD7124_Channel_12 */
	{0x16, 0x0001, 2, 1}, /* AD7124_Channel_13 */
	{0x17, 0x0001, 2, 1}, /* AD7124_Channel_14 */
	{0x18, 0x0001, 2, 1}, /* AD7124_Channel_15 */
	
	{0x19, 0x0070, 2, 1}, /* AD7124_Config_0 */	//111 0000 unipolar  internal reference PGA=1			BSP 
	{0x1A, 0x0077, 2, 1}, /* AD7124_Config_1 */	//111 0111	unipolar  internal reference PGA=128	BSP 0.38V
	{0x1B, 0x0075, 2, 1}, /* AD7124_Config_2 */	//111 0101	unipolar  internal reference PGA=32   BSP 1.05V
	{0x1C, 0x0870, 2, 1}, /* AD7124_Config_3 */
	{0x1D, 0x0870, 2, 1}, /* AD7124_Config_4 */
	{0x1E, 0x0870, 2, 1}, /* AD7124_Config_5 */
	{0x1F, 0x0870, 2, 1}, /* AD7124_Config_6 */
	{0x20, 0x0870, 2, 1}, /* AD7124_Config_7 */
	
	{0x21, 0x060180, 3, 1}, /* AD7124_Filter_0 */
	{0x22, 0x060180, 3, 1}, /* AD7124_Filter_1 */
	{0x23, 0x060180, 3, 1}, /* AD7124_Filter_2 */
	{0x24, 0x060180, 3, 1}, /* AD7124_Filter_3 */
	{0x25, 0x060180, 3, 1}, /* AD7124_Filter_4 */
	{0x26, 0x060180, 3, 1}, /* AD7124_Filter_5 */
	{0x27, 0x060180, 3, 1}, /* AD7124_Filter_6 */
	{0x28, 0x060180, 3, 1}, /* AD7124_Filter_7 */
	
	{0x29, 0x800148, 3, 1}, /* AD7124_Offset_0 */
	//{0x29, 0x800000, 3, 1}, /* AD7124_Offset_0 */
	{0x2A, 0x800000, 3, 1}, /* AD7124_Offset_1 */
	{0x2B, 0x800000, 3, 1}, /* AD7124_Offset_2 */
	{0x2C, 0x800000, 3, 1}, /* AD7124_Offset_3 */
	{0x2D, 0x800000, 3, 1}, /* AD7124_Offset_4 */
	{0x2E, 0x800000, 3, 1}, /* AD7124_Offset_5 */
	{0x2F, 0x800000, 3, 1}, /* AD7124_Offset_6 */
	{0x30, 0x800000, 3, 1}, /* AD7124_Offset_7 */
	
	
	{0x31, 0x555901, 3, 1}, /* AD7124_Gain_0 */
	//{0x31, 0x500000, 3, 1}, /* AD7124_Gain_0 */
	{0x32, 0x500000, 3, 1}, /* AD7124_Gain_1 */
	{0x33, 0x500000, 3, 1}, /* AD7124_Gain_2 */
	{0x34, 0x500000, 3, 1}, /* AD7124_Gain_3 */
	{0x35, 0x500000, 3, 1}, /* AD7124_Gain_4 */
	{0x36, 0x500000, 3, 1}, /* AD7124_Gain_5 */
	{0x37, 0x500000, 3, 1}, /* AD7124_Gain_6 */
	{0x38, 0x500000, 3, 1}, /* AD7124_Gain_7 */
};


int32_t ad7124_setup(struct ad7124_dev *device) 
{
	int32_t ret;
	enum ad7124_registers reg_nr;
	
	struct spi_init_param spi_param;
	
	if (!device)
		return INVALID_VAL;

	//device->regs = ad7124_regs;
	device->regs = ad7124_regsW;
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
	for(reg_nr = AD7124_Status; (reg_nr < AD7124_REG_NO) && !(ret < 0);//AD7124_Offset_0
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
		ret = ad7124_wait_for_conv_ready(ad7124_handler, timeout);

		sum += AD7124_ReadData(device);
	}
	
	return sum/10.0;
}
void ad7124_readData(void)
{
    ret = ad7124_wait_for_conv_ready(ad7124_handler, timeout);
    if (ret < 0)
    {
			AD7124_TRACE("ad7124 conv err,ret=%d\n",ret);
    }
    ret = ad7124_read_data(ad7124_handler, &sample);
		AD7124_TRACE("ad7124 read AD =%d\n",sample);
		double voltage;
		uint8_t voltageStr[10];
		voltage=AD7124_ReadAverData(ad7124_handler);
		//AD7124_TRACE("voltageD=%f uV\n",voltage);
		sprintf(voltageStr,"%lf",voltage);							
		AD7124_TRACE("ad7124 read voltage=%s uV\n",voltageStr);		//RTT prinf float
		if (ret < 0)
    {
			AD7124_TRACE("ad7124_read_data_err,ret=%d\n",ret);
    }	
}
void AD7124_ReadVoltage(void)
{
		ret = ad7124_wait_for_conv_ready(ad7124_handler, timeout);
		ret = ad7124_read_data(ad7124_handler, &sample);
		AD7124_TRACE("sampleD=%d\n",sample);
		double voltage;
		uint8_t voltageStr[10];
		voltage=AD7124_ReadAverData(ad7124_handler);
		sprintf(voltageStr,"%lf",voltage);		
		//sprintf(voltageStr,"%lf",voltage);							//RTT prinf float
		AD7124_TRACE("voltageStr=%s uV\n",voltageStr);
}

void ad7124_init(void)
{
    /* Initialize AD7124 device. */
		Cal_IdentifyRam();
		//Cal_FlashToRam();
    ret = ad7124_setup(&my_ad7124);
	  delay_ms(100);
    if (ret < 0)
    {
			AD7124_TRACE("AD7124 initialization failed,ret=%d\n",ret);
        /* AD7124 initialization failed, check the value of ret! */
    }
    else
    {
        /* AD7124 initialization OK */
    } 
    /* Read all registers */
    for (regNr = AD7124_Status; (regNr < AD7124_REG_NO) && !(ret < 0); regNr++)
    {
        ret = ad7124_read_register(ad7124_handler, &ad7124_regsR[regNr]);
			  AD7124_TRACE("ad7124_regs[%d]=0x%x\n",regNr,ad7124_regsR[regNr].value);
    }
    /* Read data from the ADC */
    ret = ad7124_wait_for_conv_ready(ad7124_handler, timeout);
    if (ret < 0)
    {
			AD7124_TRACE("ad7124 conv err,ret=%d\n",ret);
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
//
int ad7124(int argc, char **argv)
{
	int result = RT_ERROR;

	if (argc == 2 )
	{
		if (!strcmp(argv[1], "init"))
		{
			ad7124_init();
			return RT_EOK	; 				
		}
		else	if (!strcmp(argv[1], "readRegs"))
		{
			for (regNr = AD7124_Status; (regNr < AD7124_REG_NO) && !(ret < 0); regNr++)
			{
					ret = ad7124_read_register(ad7124_handler, &ad7124_regsR[regNr]);
					AD7124_TRACE("ad7124_regs[%d]=0x%x\n",regNr,ad7124_regsR[regNr].value);
			}
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "readData"))
		{
			ad7124_readData();
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "DMM_FUNC_DCV_100mV"))	
		{
			print_dmm_value(DMM_FUNC_DCV_100mV);
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "DMM_FUNC_DCV_1V"))	
		{
			print_dmm_value(DMM_FUNC_DCV_1V);
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "DMM_FUNC_DCV_10V"))	
		{
			print_dmm_value(DMM_FUNC_DCV_10V);
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "DMM_FUNC_DCV_25V"))	
		{
			print_dmm_value(DMM_FUNC_DCV_25V);
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "DMM_FUNC_DCI_100mA"))	
		{
			print_dmm_value(DMM_FUNC_DCI_100mA);
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "DMM_FUNC_DCI_1A"))	
		{
			print_dmm_value(DMM_FUNC_DCI_1A);
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "DMM_FUNC_OHM_10R_4W"))	
		{
			print_dmm_value(DMM_FUNC_OHM_10R_4W);
			return RT_EOK	; 
		}		
		else	if (!strcmp(argv[1], "DMM_FUNC_OHM_1K_4W"))	
		{
			print_dmm_value(DMM_FUNC_OHM_1K_4W);
			return RT_EOK	; 
		}	
		else	if (!strcmp(argv[1], "DMM_FUNC_OHM_10K_4W"))	
		{
			print_dmm_value(DMM_FUNC_OHM_10K_4W);
			return RT_EOK	; 
		}	
		else	if (!strcmp(argv[1], "SAVE_KC"))	
		{
			ad7124_getSetK();
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "dc_cal_ch0_2v5"))
		{				
			ad7124_regsW[AD7124_Channel_0].value=0x8064;	//AIN3、AIN4
			ad7124_regsW[AD7124_Channel_1].value=0x00c7;	//DISABLE
			ad7124_regsW[AD7124_Channel_2].value=0x0022;	//DISABLE	
			ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Channel_0]);
			ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Channel_1]);
			ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Channel_2]);
			//100000000	选择Low Power(低功耗)模式。
			ad7124_regsR[AD7124_ADC_CTRL_REG].value=0x0100;		
			ad7124_write_register(ad7124_handler,ad7124_regsR[AD7124_ADC_CTRL_REG]);
			delay_ms(10);
			//执行内部满量程校准
			ad7124_regsR[AD7124_ADC_CTRL_REG].value=0x0120;	
			ad7124_write_register(ad7124_handler,ad7124_regsR[AD7124_ADC_CTRL_REG]);
			AD7124_TRACE("ok\n");
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "dc_cal_ch0_0v0"))
		{		
			//执行内部零电平校准。			
			ad7124_regsR[AD7124_ADC_CTRL_REG].value=0x011C;		
			ad7124_write_register(ad7124_handler,ad7124_regsR[AD7124_ADC_CTRL_REG]);
			delay_ms(10);
			AD7124_TRACE("ok\n");
			
			//100000000	选择Low Power(低功耗)模式。
			ad7124_regsR[AD7124_ADC_CTRL_REG].value=0x0180;		
			ad7124_write_register(ad7124_handler,ad7124_regsR[AD7124_ADC_CTRL_REG]);
			delay_ms(10);
			
			return RT_EOK	; 
		}
		else
		{		
			result =RT_ERROR;
		}     
	}
	if (argc == 3 )
	{
		if (!strcmp(argv[1], "readReg"))
		{
			uint8_t regno=atoi(argv[2]);	
			ad7124_read_register(ad7124_handler,&ad7124_regsR[regno]);
			AD7124_TRACE("ad7124 read register[%d]=%d\n",regno,ad7124_regsR[regno].value);
			return RT_EOK	; 				
		}
		else
		{		
			result =RT_ERROR;
		}   
	}
	if (argc == 4 )
	{
		if (!strcmp(argv[1], "writeReg"))
		{
			uint8_t regno=atoi(argv[2]);
			ad7124_regsW[regno].value=atoi(argv[3]);
			ad7124_write_register(ad7124_handler,ad7124_regsW[regno]);
			AD7124_TRACE("ad7124_write_register[%d]=%d\n",regno,ad7124_regsW[regno].value);
			return RT_EOK	; 				
		}
		else
		{		
			result =RT_ERROR;
		}   
	}
	else if( result == RT_ERROR )
	{
		rt_kprintf("Usage: \n");
		rt_kprintf("ad7124 init                     \t-	setup ad7124\n");	
		rt_kprintf("ad7124 readRegs                 \t- read all register\n");			
		rt_kprintf("ad7124 readData                 \t-	read current data\n");	
		
		rt_kprintf("ad7124 readReg <regNum>         \t-	read the register\n");			
		rt_kprintf("ad7124 writeReg <regNum> <value>\t	-	write the register\n");	
		//1:10 or 1:1
		rt_kprintf("ad7124 dc_ch0_100mv             \t-	measure voltage within 100mv\n");		//0.1x16=1.6v
		rt_kprintf("ad7124 dc_ch0_1v                \t-	measure voltage within 1v\n");			//1x2=2v
		rt_kprintf("ad7124 dc_ch0_10v               \t-	measure voltage within 10v\n");			//10/10*2=2v
		rt_kprintf("ad7124 dc_ch0_25v               \t-	measure voltage within 25v\n");			//25/10*1=2.5V
		//多路选择+继电器
//		rt_kprintf("ad7124 dc_ch1_100uA             \t- measure current within 100uA\n");	//180ohm *0.0001 *128=2.304V
//		rt_kprintf("ad7124 dc_ch1_1mA               \t- measure current within 1mA\n");		//18ohm *0.001 *128=2.304V
//		rt_kprintf("ad7124 dc_ch1_10mA              \t- measure current within 10mA\n");	//1.8ohm *0.01 *128=2.304V
		rt_kprintf("ad7124 dc_ch1_100mA             \t- measure current within 100mA\n");	//0.15ohm *0.1 *128=1.92V
		rt_kprintf("ad7124 dc_ch1_1A             		\t- measure current within 1A\n");	//0.15ohm *1 *16=2.4V

		rt_kprintf("ad7124 dc_ch2_10ohm            \t- measure current within 100ohm\n"); //10ohm *0.001 *128=1.28V
		rt_kprintf("ad7124 dc_ch2_1kohm            \t- measure current within 1kohm\n");  //1000ohm *0.001 *2=2V
		rt_kprintf("ad7124 dc_ch2_10kohm            \t- measure current within 1kohm\n"); //10kohm *0.0001 *2=2V		
//		rt_kprintf("ad7124 dc_ch2_50kohm            \t- measure current within 1kohm\n"); //50kohm *0.00005 *1=2V
		

	}
	return result;
}


MSH_CMD_EXPORT(ad7124, ...);

/* 校准
int CAL_0(void)
{		
	//101010100
	ad7124_regsR[AD7124_ADC_CTRL_REG].value=0x0154;	
	ad7124_write_register(ad7124_handler,ad7124_regsR[AD7124_ADC_CTRL_REG]);
	delay_ms(10);
	AD7124_TRACE("ok\n");
//	ad7124_regsR[AD7124_ADC_CTRL_REG].value=0x0140;	
//	ad7124_write_register(ad7124_handler,ad7124_regsR[AD7124_ADC_CTRL_REG]);


	return 1;
}
int CAL_2V5(void)
{		
	//101011000
	ad7124_regsR[AD7124_ADC_CTRL_REG].value=0x0158;	
	ad7124_write_register(ad7124_handler,ad7124_regsR[AD7124_ADC_CTRL_REG]);
	delay_ms(10);
	AD7124_TRACE("ok\n");
//	ad7124_regsR[AD7124_ADC_CTRL_REG].value=0x0140;	
//	ad7124_write_register(ad7124_handler,ad7124_regsR[AD7124_ADC_CTRL_REG]);

	return 1;
}
MSH_CMD_EXPORT(CAL_0, ...);
MSH_CMD_EXPORT(CAL_2V5, ...);
*/
