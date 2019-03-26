/**
  \file 	TMC429.c
  \author Trinamic Motion Control GmbH & Co KG
  \version 1.00

  \brief TMC429 Stepper controller functions

  This file provides all functions needed for easy
  access to the TMC429 motion control IC.
*/

#include "SPI.h"
#include "TMC429.h"

UCHAR  SpeedChangedFlag[N_O_MOTORS];
UCHAR  ActualMotor=0;

TModuleConfig ModuleConfig=
{
  7,      //!<  bitrate (0=115200)
  1,      //!<  address
  2       //!<  reply address
};
TMotorConfig MotorConfig[N_O_MOTORS]=
{
 { 
	210,   //!< VMax
  30,    //!< AMax
  3,     //!< Pulsediv
  5,     //!< Rampdiv
	
	1,		 //FlagINVDir
	1,		 //FlagINVRef
	 
	0,	 	 //XTARGET 
	0,	 	 //XACTUAL	  
	0,	   //VTARGET 
	0,		 //VACTUAL 
	 
	210,	 //HomeV1
	210,	 //HomeV2

	1,		 //HomeMode
	1,		 //isStop
	1		   //isReach
 },
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
	SELECT_TMC429();
	HAL_SPI_TransmitReceive(&hspi1,Write,Read,4,1000);
	DESELECT_TMC429();
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
   \fn Read429Status
   \brief Read TMC429 status byte

   \return TMC429 status byte

   This functions reads just the status byte of the TMC429 using
   a single byte SPI access which makes this a little bit faster.
********************************************************************/
UCHAR Read429Status(void)
{
	uint8_t RxStatus;uint8_t TxStatus=0x01;
	SELECT_TMC429();
	HAL_SPI_TransmitReceive(&hspi1,&TxStatus,&RxStatus,1,1000);
	DESELECT_TMC429();
  return RxStatus;
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

  //return Read[Index+1];
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
	u16 IFCONF_invref=0x0000;
	u16 IFCONF_invdir=0x0000;
	//电机参数设置清零
  for(Motor=0; Motor<3; Motor++)
  {
    for(addr=0; addr<=IDX_XLATCHED; addr++)
      Write429Zero(addr|(Motor<<5));
  }
	//	        										低电平有效			 引脚不复用		输出S\D模式		REF使能R 			脉冲极限取反（电机方向）
	if( MotorConfig[0].FlagINVRef==1 ) IFCONF_invdir=0x0001;
	if( MotorConfig[0].FlagINVDir==1 ) IFCONF_invref=0x0010;
	
  Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_invref|IFCONF_INV_STEP|IFCONF_invdir);
  //mainly for SPI mode				主要设置CLK2_DIV	脉宽	 最小2us    >=500HZ 跟驱动器有关 
	Write429Datagram(IDX_SMGP, 0x00, 0x04, 0x02);//(1+X)us
	//PDIV-V RampDIV-ACC 
  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR0, 0x00, (MotorConfig[0].PulseDiv<<4)|(MotorConfig[0].RampDiv & 0x0f), 0x04);
  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR1, 0x00, (MotorConfig[1].PulseDiv<<4)|(MotorConfig[1].RampDiv & 0x0f), 0x04);
  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR2, 0x00, (MotorConfig[2].PulseDiv<<4)|(MotorConfig[2].RampDiv & 0x0f), 0x04);
  //X_LATCHED(X_ACTUAL)						 RO	电机限位后按参数停止	ramp_mode 
	Write429Datagram(IDX_REFCONF_RM|MOTOR0, 0x00, SOFT_REF, 0x00);
  Write429Datagram(IDX_REFCONF_RM|MOTOR1, 0x00, SOFT_REF, 0x00);
  Write429Datagram(IDX_REFCONF_RM|MOTOR2, 0x00, SOFT_REF, 0x00);
  //小于此值，电机停止
	Write429Short(IDX_VMIN|MOTOR0, 1);
  Write429Short(IDX_VMIN|MOTOR1, 1);
  Write429Short(IDX_VMIN|MOTOR2, 1);
	//VMAX AMAX 
  Write429Int(IDX_VMAX|MOTOR0, MotorConfig[0].VMax);
  SetAMax(0, MotorConfig[0].AMax);
  Write429Int(IDX_VMAX|MOTOR1, MotorConfig[1].VMax);
  SetAMax(1, MotorConfig[1].AMax);
  Write429Int(IDX_VMAX|MOTOR2, MotorConfig[2].VMax);
  SetAMax(2, MotorConfig[2].AMax);
}
















