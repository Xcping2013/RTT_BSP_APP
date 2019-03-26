/**
  \file 		TMC429.h
  \author 	Trinamic Motion Control GmbH & Co KG
  \version 	1.00

  \brief 		TMC429 library definitions

  This file contains all macro and function definitions of the TMC429
  library.
*/
#include "stm32f1xx_hal.h"

#define MOTOR0 0
#define MOTOR1 0x20
#define MOTOR2 0x40

#define IDX_XTARGET 0x00
#define IDX_XACTUAL 0x02
#define IDX_VMIN 0x04
#define IDX_VMAX 0x06
#define IDX_VTARGET 0x08
#define IDX_VACTUAL 0x0A
#define IDX_AMAX 0x0C
#define IDX_AACTUAL 0x0E
#define IDX_AGTAT_ALEAT 0x10
#define IDX_PMUL_PDIV 0x12
#define IDX_REFCONF_RM 0x14
#define IDX_IMASK_IFLAGS 0x16
#define IDX_PULSEDIV_RAMPDIV 0x18
#define IDX_DX_REFTOLERANCE 0x1A
#define IDX_XLATCHED 0x1C
#define IDX_USTEP_COUNT_429 0x1E

#define IDX_LOW_WORD 0x60
#define IDX_HIGH_WORD 0x62
#define IDX_COVER_POS_LEN 0x64
#define IDX_COVER_DATA 0x66

#define IDX_IF_CONFIG_429 0x68
#define IDX_POS_COMP_429 0x6A
#define IDX_POS_COMP_INT_429 0x6C
#define IDX_TYPE_VERSION_429 0x72

#define IDX_REF_SWITCHES 0x7c
#define IDX_SMGP 0x7e

#define TMC429_READ 0x01

#define RM_RAMP 0
#define RM_SOFT 1
#define RM_VELOCITY 2
#define RM_HOLD 3

#define NO_REF 0x03				//左右限位不停止电机
#define SOFT_REF 0x04			//电机限位后按参数停止
#define HARD_REF 0x00			//碰到限位立刻停止

#define NO_LIMIT 0
#define HARD_LIMIT 1
#define SOFT_LIMIT 2

#define REFSW_LEFT 0x02
#define REFSW_RIGHT 0x01

#define M0_POS_REACHED 0x01
#define M1_POS_REACHED 0x04
#define M2_POS_REACHED 0x10
#define TMC429_STATUS_CDGW 0x40
#define TMC429_STATUS_INT  0x80

#define IFLAG_POS_REACHED     0x01
#define IFLAG_REF_WRONG       0x02
#define IFLAG_REF_MISS        0x04
#define IFLAG_STOP            0x08
#define IFLAG_STOP_LEFT_LOW   0x10
#define IFLAG_STOP_RIGHT_LOW  0x20
#define IFLAG_STOP_LEFT_HIGH  0x40
#define IFLAG_STOP_RIGHT_HIGH 0x80

#define IFCONF_INV_REF        0x0001
#define IFCONF_SDO_INT        0x0002
#define IFCONF_STEP_HALF      0x0004
#define IFCONF_INV_STEP       0x0008
#define IFCONF_INV_DIR        0x0010
#define IFCONF_EN_SD          0x0020
#define IFCONF_POS_COMP_0     0x0000
#define IFCONF_POS_COMP_1     0x0040
#define IFCONF_POS_COMP_2     0x0080
#define IFCONF_POS_COMP_OFF   0x00C0
#define IFCONF_EN_REFR        0x0100

#define N_O_MOTORS 3                           //!< number of motors supported by this module
#define MOTOR_NUMBER(a) (a)                    //!< extract TMC429 motor number (for future extensions)


#define SPI_DEV_TMC429  0
#define SELECT_TMC429()        		GPIOA->BRR=BIT4
#define DESELECT_TMC429()         GPIOA->BSRR=BIT4  

#define SIZE_MOTOR_CONFIG		10

extern UCHAR  SpeedChangedFlag[N_O_MOTORS];
extern UCHAR  ActualMotor;

typedef struct																																		
{
															 //3Byte
  UCHAR SerialBitrate;         //!< RS485 baud rate (0..7, 0=9600bps)
  UCHAR SerialModuleAddress;   //!< RS485 TMCL module address
  UCHAR SerialHostAddress;     //!< RS485 TMCL reply address
} TModuleConfig;

//! Motor configuration data	42BYTE
typedef struct						
{
  UINT VMax;                   //!< maximum positioning velocity
  UINT AMax;                   //!< maximum acceleration
  UCHAR PulseDiv;              //!< pulse divisor (TMC429)
  UCHAR RampDiv;               //!< ramp divisor (TMC429)
	
	UCHAR FlagINVDir;
	UCHAR FlagINVRef;
	
	int XTARGET; 
	int XACTUAL;
	UINT VTARGET; 
	UINT VACTUAL; 
	
	UINT HomeV1;	
	UINT HomeV2;	
	
	UINT HomeMode;
	UCHAR isStop;
	UCHAR isReach;
	
} TMotorConfig;

typedef struct
{
  UCHAR Motor;       //!< motor/bank parameter
  union
  {
    long Int32;      //!< value parameter as 32 bit integer
    UCHAR Byte[4];   //!< value parameter as 4 bytes    Byte[3]为高字节
  } Value;           //!< value parameter
} TMCLCommand;





extern TMotorConfig MotorConfig[N_O_MOTORS];

void ReadWrite429(UCHAR *Read, UCHAR *Write);
void Write429Zero(UCHAR Address);
void Write429Bytes(UCHAR Address, UCHAR *Bytes);
void Write429Datagram(UCHAR Address, UCHAR HighByte, UCHAR MidByte, UCHAR LowByte);
void Write429Short(UCHAR Address, int Value);
void Write429Int(UCHAR Address, int Value);
UCHAR Read429Status(void);
UCHAR Read429Bytes(UCHAR Address, UCHAR *Bytes);
UCHAR Read429SingleByte(UCHAR Address, UCHAR Index);
int Read429Short(UCHAR Address);
int Read429Int(UCHAR Address);
void Set429RampMode(UCHAR Axis, UCHAR RampMode);
void Set429SwitchMode(UCHAR Axis, UCHAR SwitchMode);
UCHAR SetAMax(UCHAR Motor, UINT AMax);
void HardStop(UINT Motor);

void Init429(void);







