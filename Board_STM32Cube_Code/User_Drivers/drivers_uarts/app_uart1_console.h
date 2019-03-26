
#ifndef __APP_UART1_CONSOLE_H__
#define __APP_UART1_CONSOLE_H__

#include "stm32f1xx_hal.h"
#include "main.h"
#include "usart.h"

#define USART_REC_LEN	256

extern UART_HandleTypeDef huart1;

extern uint8_t 	 USART_RX_BUF[USART_REC_LEN];  

extern uint16_t	 USART_RX_STA; 

void uart1_hw_init(int BaudRate);

void CommandCheckAndExe(void);

#endif
