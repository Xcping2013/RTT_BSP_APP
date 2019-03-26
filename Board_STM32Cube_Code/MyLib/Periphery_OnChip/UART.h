/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _UART_H
#define _UART_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
//#include "command.h"

#define UART_BUFFER_SIZE 		100											//----���������Ľ��ճ���
#define UART_TIMEOUT_VALUE 	20											//----���ڽ������ʱ��ֵ

#define TX_TIMEOUT          ((uint32_t)100)					//���ڷ��ͳ�ʱֵ
#define RX_TIMEOUT          HAL_MAX_DELAY						//���ڽ��ճ�ʱֵ��ÿ�ν���ֵ��Ϊ1ʱ��ѡ���ã�

HAL_StatusTypeDef Serial_PutByte	( UART_HandleTypeDef UartHandle , uint8_t param  );
void 							Serial_PutString(UART_HandleTypeDef UartHandle ,uint8_t *p_string );
void 							Serial_Printf		(UART_HandleTypeDef UartHandle , const char *fmt,...);

void UART_RxTimeOutCheck(void);
void UART_Process(UART_HandleTypeDef *huart);


#endif  /* __UART_H */
//////////////////////////////////////////
