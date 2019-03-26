/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _UART_H
#define _UART_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
//#include "command.h"

#define UART_BUFFER_SIZE 		100											//----允许串口最大的接收长度
#define UART_TIMEOUT_VALUE 	20											//----串口接收溢出时间值

#define TX_TIMEOUT          ((uint32_t)100)					//串口发送超时值
#define RX_TIMEOUT          HAL_MAX_DELAY						//串口接收超时值（每次接收值不为1时刻选配置）

HAL_StatusTypeDef Serial_PutByte	( UART_HandleTypeDef UartHandle , uint8_t param  );
void 							Serial_PutString(UART_HandleTypeDef UartHandle ,uint8_t *p_string );
void 							Serial_Printf		(UART_HandleTypeDef UartHandle , const char *fmt,...);

void UART_RxTimeOutCheck(void);
void UART_Process(UART_HandleTypeDef *huart);


#endif  /* __UART_H */
//////////////////////////////////////////
