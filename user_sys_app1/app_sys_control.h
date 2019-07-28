
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_SYS_CONTROL_H
#define __APP_SYS_CONTROL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* USER CODE BEGIN Includes */
#include "bsp_defines.h"
/* USER CODE END Includes */
	 	 
#define ENABLE_DEBUG_CODE	0
	 
#define USING_IN8_EXIT	1
	 
/* USER CODE BEGIN Private defines */
#define	CMD_RESPONSE_EN	1
#ifdef	CMD_RESPONSE_EN
#define CMD_TRACE         	rt_kprintf
#else
#define CMD_TRACE(...)   
#endif
#define	DEBUG_EN	1
#ifdef	DEBUG_EN
#define DEBUG_TRACE         rt_kprintf
#else
#define DEBUG_TRACE(...)   
#endif
	 
#define SPI_DEV_TMC429 		 		 0                      			//!< SPI device number of TMC429
#define N_O_MOTORS 				 		 3                          			//!< number of motors supported by this module
	
#define PARAM_ADDR		0			
#define PARAM_VER			0x00000100				

/* 全局参数 */
typedef struct
{
	uint32_t ParamVer;				//4/* 参数区版本控制（可用于程序升级时，决定是否对参数区进行升级） */
	uint32_t MasterBaudrate;				//4
	uint32_t SlaveBaudrate;
	uint8_t	 Project_ID;			//1
	
	/*  电机配置参数 */
	UINT tmc429_VMax[N_O_MOTORS];  //12   		//!< maximum positioning velocity
	UINT tmc429_AMax[N_O_MOTORS];  //12  			//!< maximum acceleration
	UCHAR tmc429_PulseDiv[N_O_MOTORS]; //3    //!< pulse divisor (TMC429)
	UCHAR tmc429_RampDiv[N_O_MOTORS];  //3    //!< ramp divisor (TMC429)

	UINT speed_home[N_O_MOTORS];	 //12	
}
PARAM_T;

extern PARAM_T g_tParam;

#define AXIS_X	0
#define AXIS_Y	1
#define AXIS_Z	2
//Type codes of the MVP command
#define MVP_ABS   0            //!< absolute movement (with MVP command)
#define MVP_REL   1            //!< relative movement (with MVP command)
#define MVP_COORD 2            //!< coordinate movement (with MVO command)

//TMCL status codes
#define REPLY_OK 100                //!< command successfully executed
#define REPLY_CHKERR 1              //!< checksum error
#define REPLY_INVALID_CMD 2         //!< command not supported
#define REPLY_WRONG_TYPE 3          //!< wrong type code
#define REPLY_INVALID_VALUE 4       //!< wrong value
#define REPLY_EEPROM_LOCKED 5       //!< EEPROM is locked
#define REPLY_CMD_NOT_AVAILABLE 6   //!< command not available due to current state
#define REPLY_CMD_LOAD_ERROR 7      //!< error when storing command to EEPROM
#define REPLY_WRITE_PROTECTED 8     //!< EEPROM is write protected
#define REPLY_MAX_EXCEEDED 9        //!< maximum number of commands in EEPROM exceeded

/* USER CODE END Private defines */
//! Motor configuration data
typedef struct
{
  UINT VMax;                   //!< maximum positioning velocity
  UINT AMax;                   //!< maximum acceleration
  UCHAR PulseDiv;              //!< pulse divisor (TMC429)
  UCHAR RampDiv;               //!< ramp divisor (TMC429)
//  UCHAR IRun;                  //!< run current (0..255)
//  UCHAR IStandby;              //!< stand-by current(0..255)
//  UINT StallVMin;              //!< minimum speed for stallGuard
//  UINT FreewheelingDelay;      //!< freewheeling delay time (*10ms)
//  UINT SettingDelay;           //!< delay for switching to stand-by current (*10ms)
} TMotorConfig;

extern TMotorConfig MotorConfig[N_O_MOTORS];

enum ProjectId 
{
	COMMON, 
	BUTTON_ONLINE, 
	BUTTON_OFFLINE,	
	BUTTON_VER3,
	
	BUTTON_ROAD,
	OQC_FLEX,

	LIDOPEN,
	
	AOI_2AXIS
};

UCHAR ReadWriteSPI1(UCHAR DeviceNumber, UCHAR aTxBuffer, UCHAR LastTransfer);
UCHAR ReadWriteSPI2(UCHAR DeviceNumber, UCHAR aTxBuffer, UCHAR LastTransfer);

void xstrcat(char *str1,char *str2);
uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
void reboot(void);
	
void LoadParamFromEeprom(void);
void SaveParamToEeprom(void);

extern uint8_t pressureAlarm;

extern uint8_t homeSensorPin[3];
extern int motorPosition[3];
extern uint8_t motorsReset_InOrder;
extern volatile uint8_t HardTimer_StartStop;

int ParamSave(int argc, char **argv);

void LoadSettingViaProID(void);

void Start_HardTimer(void);
void Stop_HardTimer(void);

void MotorAutoReset_preset( void );
void MotorSensorCheck_timer_init(void);
void SetAmaxAutoByspeed(u8 axisNum,int speed);
void get_motor_position(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__APP_SYS_CONTROL_H */


