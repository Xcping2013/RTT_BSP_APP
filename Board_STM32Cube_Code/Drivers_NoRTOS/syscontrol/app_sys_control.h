
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_SYS_CONTROL_H
#define __APP_SYS_CONTROL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* USER CODE BEGIN Includes */
#include "bsp_defines.h"
/* USER CODE END Includes */

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
	 	
#define PARAM_ADDR		0			
#define PARAM_VER			0x00000102					

/* 全局参数 */
typedef struct
{
	uint32_t ParamVer;				//4/* 参数区版本控制（可用于程序升级时，决定是否对参数区进行升级） */
	uint32_t MasterBaudrate;				//4
	uint32_t SlaveBaudrate;
	
	uint8_t	 Project_ID;			//1
}
PARAM_T;

extern PARAM_T g_tParam;

enum ProjectId 
{
	COMMON, 
	BUTTON_ONLINE, 
	BUTTON_OFFLINE,	
	BUTTON_VER3,
	OQC_FLEX,
	LIDOPEN
};


void xstrcat(char *str1,char *str2);

void reboot(void);
	
void LoadParam(void);
void SaveParam(void);

//int ParamSave(int argc, char **argv);
//extern int motorPosition[3];
//extern uint8_t autoRESETmotor;
//extern uint8_t TimerOpened;
//void timer_start(void);
//void timer_stop(void);

#ifdef __cplusplus
}
#endif
#endif /*__APP_SYS_CONTROL_H */


