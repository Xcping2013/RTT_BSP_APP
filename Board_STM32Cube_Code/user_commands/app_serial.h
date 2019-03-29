
#ifndef __APP_SERIAL_H
#define __APP_SERIAL_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "usart.h"

#define USART_REC_LEN  		200  	
	  	
extern uint8_t  USART_RX_BUF[USART_REC_LEN]; 	
extern uint16_t USART_RX_STA;         				

void CommandCheckAndExe(void);
void uart_putstring(UART_HandleTypeDef dev, const uint8_t *s);

#ifdef __cplusplus
}
#endif
#endif 


