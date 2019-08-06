/*******************************************************************************
Module:  TMC429.c

FEATURES AND BENEFITS

	Full Compatible Successor of the TMC428
	Controls up to three stepper motors
	3.3 V or 5 V operation with CMOS / TTL compatible IOs
	Interface for SPI motor drivers with data rates up to 1 Mbit/s
	Step/Direction interface
	Clock frequency: up to 32 MHz (can use CPU clock)
	Internal position counters 24 bit wide
	Microstep frequency up to 1 MHz
	Read-out option for all motion parameters
	Programmable 6 bit microstep table, up to 64 entries for a quarter sine wave period
	Ramp generators for autonomous positioning / speed control
	On-the-fly change of target motion parameters
	Power boost automatic acceleration dependent current control
	Low power operation: 1.25 mA at 4 MHz (typ.)
	Compact Size: ultra small 16 pin SSOP package, small 24 pin SOP package, and 32 pin QFN 5x5 mm package
	Directly controls TMC23x, TMC24x, TMC26x, and TMC389
	
	
*******************************************************************************/

/**
  \file TMC429.c
  \author Trinamic Motion Control GmbH & Co KG
  \version 1.00

  \brief TMC429 Stepper controller functions

  This file provides all functions needed for easy
  access to the TMC429 motion control IC.
*/
//USING_INC_MB1616DEV6	SPI2
//	#define CS_PIN_TMC429 		 		 PB_12					
//	#define POSCMP1_PIN						 PD_13
//	#define	INTOUT1_PIN						 PD_14

#include <rtthread.h>
#include <rtdevice.h>

#include "spi_smc_tmc429.h"
#include "app_eeprom_24xx.h"
/***************************************************************************************************/
//USING_INC_MBTMC429	SPI1					
#define POSCMP1_PIN						 	PE_12
#define	INTOUT1_PIN						 	PE_13	

#define PWM_DEV_NAME        		"pwm3"  
#define PWM_DEV_CHANNEL     		3   

#define TMC429_SPI_DEVICE_NAME     "spi10"
/***************************************************************************************************/
struct rt_device_pwm *pwm_dev;
struct rt_spi_device *spi_dev_tmc429;

static void TMC429_ParameterPresetToRam(void);

/***************************************************************************************************/
static void TMC429_ParameterPresetToRam(void)
{
  uint8_t i;
	for(i=0; i<N_O_MOTORS; i++) 					
	{																  
		MotorConfig[i].VMax 		= g_tParam.tmc429_VMax[i];   				
		MotorConfig[i].AMax 		=	g_tParam.tmc429_AMax[i];
		MotorConfig[i].PulseDiv	=	g_tParam.tmc429_PulseDiv[i];	
		MotorConfig[i].RampDiv	=	g_tParam.tmc429_RampDiv[i];	
		
		SpeedChangedFlag[i]			=	TRUE;
		homeInfo.Homed[i]				=	FALSE;
		homeInfo.GoHome[i]			=	FALSE;
		homeInfo.GoLimit[i]			=	FALSE;
		homeInfo.HomeSpeed[i]		=	g_tParam.speed_home[i];
	}
}
//
int rt_hw_spi_tmc429_init(void)
{
	pinMode(POSCMP1_PIN, PIN_MODE_INPUT_PULLUP);
  pinMode(INTOUT1_PIN, PIN_MODE_INPUT_PULLUP);	

	rt_uint32_t period, pulse, dir;

  period = 1000/12;   //��λΪ����
  pulse = period/2;  
	
	pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
	if (pwm_dev == RT_NULL)
	{
			rt_kprintf("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
			return RT_ERROR;
	}
  rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, pulse);

  rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
	
  __HAL_RCC_GPIOC_CLK_ENABLE();
		
	//���� SPI �豸���Ѿ�ע��õ�SPI������
  rt_hw_spi_device_attach("spi1", TMC429_SPI_DEVICE_NAME, GPIOC, GPIO_PIN_4);
	//���� SPI �豸
	spi_dev_tmc429 = (struct rt_spi_device *)rt_device_find(TMC429_SPI_DEVICE_NAME);
	
 if (!spi_dev_tmc429)
	{
			rt_kprintf("rt_hw_spi_tmc429_init failed! can't find %s device!\n", TMC429_SPI_DEVICE_NAME);
	}
	else 
	{
		//���� SPI �豸�� SPI ���ߺ���Ҫ���� SPI �豸�Ĵ��������
		struct rt_spi_configuration cfg;

		cfg.data_width = 8;
		cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;
		cfg.max_hz = 1 * 1000 *1000;                           /* 1M */

		rt_spi_configure(spi_dev_tmc429, &cfg);

		TMC429_ParameterPresetToRam();
		
		Init429();
		
		rt_kprintf("rt_hw_spi_tmc429_init ok\n");	
		
	}
   return RT_EOK;
}
//
/***************************************************************//**
   \fn Read429Status
   \brief Read TMC429 status byte

   \return TMC429 status byte

   This functions reads just the status byte of the TMC429 using
   a single byte SPI access which makes this a little bit faster.
********************************************************************/
UCHAR Read429Status(void)
{
	uint8_t sendbuf=0x01;
	uint8_t recvbuf=0;
	rt_spi_send_then_recv(spi_dev_tmc429,&sendbuf,1,&recvbuf,1);
  return recvbuf;
}

//usteps=8; //200*8=1600	3R/S 	�����Y��2mm 1Ȧ�Ļ� V=70=30mm/min	 0.25R/S	3/0.25=12	839/12=70
TMotorConfig MotorConfig[N_O_MOTORS]=
{
				//12MHZ 																			
				//3R/S																			
  839,  //!< VMax					839,  											
  1000, //!< AMax					1000, 												
  5,    //!< Pulsediv			5,    													
  8,    //!< Rampdiv			8,    													
};

/***************************************************************//**
   \fn ReadWrite429(UCHAR *Read, UCHAR *Write)
   \brief 32 bit SPI communication with TMC429
   \param Read   four byte array holding the data read from the TMC428
   \param Write  four byte array holding the data to write to the TMC428

   This is the low-level function that does all SPI communication with
   the TMC429. It sends a 32 bit SPI telegramme to the TMC429 and
   receives the 32 bit answer telegramme from the TMC429.
   It also raises the motor current when there is write access to
   a register that could cause motor movement.
********************************************************************/
void ReadWrite429(UCHAR *Read, UCHAR *Write)
{
	struct rt_spi_message msg1;

//	uint8_t sendbuf[4];
//	uint8_t recvbuf[4];
//	&sendbuf=Write[0];
//	&recvbuf=Read[0];
	msg1.send_buf   = Write;
	msg1.recv_buf   = Read;
	msg1.length     = 4;
	msg1.cs_take    = 1;
	msg1.cs_release = 1;
	msg1.next       = RT_NULL;

	rt_spi_transfer_message(spi_dev_tmc429, &msg1);
}


/***************************************************************//**
   \fn Write429Bytes(UCHAR Address, UCHAR *Bytes)
   \brief TMC429 write access
   \param Address  TMC429 register address (see also TMC429.h)
   \param Bytes  Array holding three bytes to be written to the
                 TMC429 register.

   This function writes an array of  three bytes to a TMC429 register.
********************************************************************/
void Write429Bytes(UCHAR Address, UCHAR *Bytes)
{
  UCHAR Write[4], Read[4];

  Write[0]=Address;
  Write[1]=Bytes[0];
  Write[2]=Bytes[1];
  Write[3]=Bytes[2];
  ReadWrite429(Read, Write);
}


/***************************************************************//**
   \fn Write429Datagram(UCHAR Address, UCHAR HighByte, UCHAR MidByte, UCHAR LowByte)
   \brief TMC429 write access
   \param Address   TMC429 register address
   \param HighByte  MSB of the TMC429 register
   \param MidByte   mid byte of the TMC429 register
   \param LowByte   LSB of the TMC429 register

   This function write three bytes to a TMC429 register.
********************************************************************/
void Write429Datagram(UCHAR Address, UCHAR HighByte, UCHAR MidByte, UCHAR LowByte)
{
  UCHAR Write[4], Read[4];

  Write[0]=Address;
  Write[1]=HighByte;
  Write[2]=MidByte;
  Write[3]=LowByte;
  ReadWrite429(Read, Write);
}


/***************************************************************//**
   \fn Write429Zero(UCHAR Address)
   \brief Write zero to a TMC429 register
   \param Address  TMC429 register address

   This function sets a TMC429 register to zero. This can be useful
   e.g. to stop a motor quickly.
********************************************************************/
void Write429Zero(UCHAR Address)
{
  UCHAR Write[4], Read[4];

  Write[0]=Address;
  Write[1]=0;
  Write[2]=0;
  Write[3]=0;

  ReadWrite429(Read, Write);
}


/***************************************************************//**
   \fn Write429Short(UCHAR Address, int Value)
   \brief Write 16 bit value to a TMC429 register
   \param Address  TMC429 register address
   \param Value    Value to be written

   This function writes a 16 bit value to a TMC429 register.
********************************************************************/
void Write429Short(UCHAR Address, int Value)
{
  UCHAR Write[4], Read[4];

  Write[0]=Address;
  Write[1]=0;
  Write[2]=Value >> 8;
  Write[3]=Value & 0xff;

  ReadWrite429(Read, Write);
}


/***************************************************************//**
   \fn Write429Int(UCHAR Address, int Value)
   \brief Write 24 bit value to a TMC429 register
   \param Address  TMC429 register address
   \param Value    Value to be written

   This function writes a 24 bit value to a TMC429 register.
********************************************************************/
void Write429Int(UCHAR Address, int Value)
{
  UCHAR Write[4], Read[4];

  Write[0]=Address;
  Write[1]=Value >> 16;
  Write[2]=Value >> 8;
  Write[3]=Value & 0xff;

  ReadWrite429(Read, Write);
}


/***************************************************************//**
   \fn Read429Bytes(UCHAR Address, UCHAR *Bytes)
   \brief Read TMC429 register
   \param Address  TMC429 register address (see TMC429.h)
   \param Bytes  Pointer at array of three bytes

   \return TMC429 status byte

   This functions reads a TMC429 register and puts the result into
   an array of bytes. It also returns the TMC429 status bytes.
********************************************************************/
UCHAR Read429Bytes(UCHAR Address, UCHAR *Bytes)
{
  UCHAR Read[4], Write[4];

  Write[0]=Address|TMC429_READ;
  ReadWrite429(Read, Write);

  Bytes[0]=Read[1];
  Bytes[1]=Read[2];
  Bytes[2]=Read[3];

  return Read[0];
}


/***************************************************************//**
   \fn Read429SingleByte(UCHAR Address, UCHAR Index)
   \brief Read TMC429 register
   \param Address  TMC429 register address (see TMC429.h)
   \param Index  TMC429 register byte to be returned (0..3)

   \return TM429 register byte

   This functions reads a TMC429 register and returns the desired
   byte of that register.
********************************************************************/
UCHAR Read429SingleByte(UCHAR Address, UCHAR Index)
{
  UCHAR Read[4], Write[4];

  Write[0]=Address|TMC429_READ;
  ReadWrite429(Read, Write);

  //return Read[Index+1];  �ٷ��ṩ��BUG ������SPI���ͽ��մ�����Ʋ�һ��
	return Read[Index];
}


/***************************************************************//**
   \fn Read429Short(UCHAR Address)
   \brief Read TMC429 register (12 bit)
   \param Address  TMC429 register address (see TMC429.h)

   \return TMC429 register value (sign extended)

   This functions reads a TMC429 12 bit register and sign-extends the
   register value to 32 bit.
********************************************************************/
int Read429Short(UCHAR Address)
{
  UCHAR Read[4], Write[4];
  int Result;

  Write[0]=Address|TMC429_READ;
  ReadWrite429(Read, Write);

  Result=(Read[2]<<8)|(Read[3]);
  if(Result & 0x00000800) Result|=0xfffff000;

  return Result;
}


/***************************************************************//**
   \fn Read429Int(UCHAR Address)
   \brief Read TMC429 register (24 bit)
   \param Address  TMC429 register address (see TMC429.h)

   \return TMC429 register value (sign extended)

   This functions reads a TMC429 24 bit register and sign-extends the
   register value to 32 bit.
********************************************************************/
int Read429Int(UCHAR Address)
{
  UCHAR Read[4], Write[4];
  int Result;

  Write[0]=Address|TMC429_READ;
  ReadWrite429(Read, Write);

  Result=(Read[1]<<16)|(Read[2]<<8)|(Read[3]);
  if(Result & 0x00800000) Result|=0xff000000;

  return Result;
}


/***************************************************************//**
   \fn Set429RampMode(UCHAR Axis, UCHAR RampMode)
   \brief Set the ramping mode of an axis
   \param  Axis  Motor number (0, 1 or 2)
   \param  RampMode  ramping mode (RM_RAMP/RM_SOFT/RM_VELOCITY/RM_HOLD)

   This functions changes the ramping mode of a motor in the TMC429.
   It is some TMC429 register bit twiddling.
********************************************************************/
void Set429RampMode(UCHAR Axis, UCHAR RampMode)
{
  UCHAR Write[4], Read[4];

  Write[0] = MOTOR_NUMBER(Axis)<<5|IDX_REFCONF_RM|TMC429_READ;
  ReadWrite429(Read, Write);

  Write[0] = MOTOR_NUMBER(Axis)<<5|IDX_REFCONF_RM;
  Write[1] = Read[1];
  Write[2] = Read[2];
  Write[3] = RampMode;
  ReadWrite429(Read, Write);
}


/***************************************************************//**
   \fn Set429SwitchMode(UCHAR Axis, UCHAR SwitchMode)
   \brief Set the end switch mode
   \param  Axis  Motor number (0, 1 or 2)
   \param  SwitchMode  end switch mode

   This functions changes the end switch mode of a motor in the TMC429.
   It is some TMC429 register bit twiddling.
********************************************************************/
void Set429SwitchMode(UCHAR Axis, UCHAR SwitchMode)
{
  UCHAR Write[4], Read[4];

  Write[0] = MOTOR_NUMBER(Axis)<<5|IDX_REFCONF_RM|TMC429_READ;
  ReadWrite429(Read, Write);

  Write[0] = MOTOR_NUMBER(Axis)<<5|IDX_REFCONF_RM;
  Write[1] = Read[1];
  Write[2] = SwitchMode;
  Write[3] = Read[3];
  ReadWrite429(Read, Write);
}


/***************************************************************//**
   \fn SetAMax(UCHAR Motor, UINT AMax)
   \brief Set the maximum acceleration
   \param Motor  motor number (0, 1, 2)
   \param AMax: maximum acceleration (1..2047)

   This function sets the maximum acceleration and also calculates
   the PMUL and PDIV value according to all other parameters
   (please see the TMC429 data sheet for more info about PMUL and PDIV
   values).
********************************************************************/
UCHAR SetAMax(UCHAR Motor, UINT AMax)
{
  int pdiv, pmul, pm, pd;
  float p, p_reduced;
  int ramp_div;
  int pulse_div;
  UCHAR PulseRampDiv;
  UCHAR Data[3];

  AMax&=0x000007ff;
  Read429Bytes(IDX_PULSEDIV_RAMPDIV|MOTOR_NUMBER(Motor)<<5, Data);
  PulseRampDiv=Data[1];
  pulse_div=PulseRampDiv>>4;
  ramp_div=PulseRampDiv & 0x0f;

  pm=-1; pd=-1; // -1 indicates : no valid pair found

  if(ramp_div>=pulse_div)
    p = AMax / ( 128.0 * (1<<(ramp_div-pulse_div)));  //Exponent positive or 0
  else
    p = AMax / ( 128.0 / (1<<(pulse_div-ramp_div)));  //Exponent negative

  p_reduced = p*0.988;

  for (pdiv=0; pdiv<=13; pdiv++)
  {
    pmul = (int)(p_reduced * 8.0 * (1<<pdiv)) - 128;

    if ( (0 <= pmul) && (pmul <= 127) )
    {
      pm = pmul + 128;
      pd = pdiv;
    }
  }

  Data[0]=0;
  Data[1]=(UCHAR) pm;
  Data[2]=(UCHAR) pd;
  Write429Bytes((MOTOR_NUMBER(Motor)<<5)|IDX_PMUL_PDIV, Data);
  Write429Short((MOTOR_NUMBER(Motor)<<5)|IDX_AMAX, AMax);

  return 0;
}


/***************************************************************//**
   \fn HardStop(UINT Motor)
   \brief Stop a motor immediately
   \param Motor: motor number (0, 1, 2)

   This function stops a motor immediately (hard stop) by switching
   to velocity mode and then zeroing the V_TARGT and V_ACTUAL registers
   of that axis.
********************************************************************/
void HardStop(UINT Motor)
{
  Set429RampMode(MOTOR_NUMBER(Motor), RM_VELOCITY);
  Write429Zero((MOTOR_NUMBER(Motor)<<5)|IDX_VTARGET);
  Write429Zero((MOTOR_NUMBER(Motor)<<5)|IDX_VACTUAL);
}


/***************************************************************//**
   \fn Init429
   \brief TMC429 initialization

   This function does all necessary initializations of the TMC429.
********************************************************************/
void Init429(void)
{
  UINT addr;
  UCHAR Motor;

  for(Motor=0; Motor<3; Motor++)
  {
    for(addr=0; addr<=IDX_XLATCHED; addr++)
      Write429Zero(addr|(Motor<<5));
  }

	Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR);

	Write429Datagram(IDX_SMGP, 0x00, 0x04, 0x02);

  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR0, 0x00, (MotorConfig[0].PulseDiv<<4)|(MotorConfig[0].RampDiv & 0x0f), 0x04);
  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR1, 0x00, (MotorConfig[1].PulseDiv<<4)|(MotorConfig[1].RampDiv & 0x0f), 0x04);
  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR2, 0x00, (MotorConfig[2].PulseDiv<<4)|(MotorConfig[2].RampDiv & 0x0f), 0x04);
  Write429Datagram(IDX_REFCONF_RM|MOTOR0, 0x00, HARD_REF, 0x00);
  Write429Datagram(IDX_REFCONF_RM|MOTOR1, 0x00, HARD_REF, 0x00);
  Write429Datagram(IDX_REFCONF_RM|MOTOR2, 0x00, HARD_REF, 0x00);
  Write429Short(IDX_VMIN|MOTOR0, 1);
  Write429Short(IDX_VMIN|MOTOR1, 1);
  Write429Short(IDX_VMIN|MOTOR2, 1);

  Write429Int(IDX_VMAX|MOTOR0, MotorConfig[0].VMax);
  SetAMax(0, MotorConfig[0].AMax);
  Write429Int(IDX_VMAX|MOTOR1, MotorConfig[1].VMax);
  SetAMax(1, MotorConfig[1].AMax);
  Write429Int(IDX_VMAX|MOTOR2, MotorConfig[2].VMax);
  SetAMax(2, MotorConfig[2].AMax);
	
}
/********************************************************************/






