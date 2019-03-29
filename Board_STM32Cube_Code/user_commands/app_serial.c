#include "app_serial.h"
#include "bsp_defines.h"
#include "bsp_mcu_delay.h"

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

uint8_t  USART_RX_BUF[USART_REC_LEN];     
uint16_t USART_RX_STA=0; 

#define RX_BUF_LEN 200;
static uint8_t TxBuffer[200]="[batman]";
static uint8_t RxBuffer[500];

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

static void mcu_Delay(u8 x)
{
	u16 i;
	for(i=0;i<x;i++)	__NOP();
}

void uart_putstring(UART_HandleTypeDef dev, const uint8_t *s)
{
	uint32_t timeout = 0;
  while(*s)
  {
			while (__HAL_UART_GET_FLAG(&dev, UART_FLAG_TXE) == RESET)
			{
				timeout++;
				if(timeout >1000) break;
			}
			dev.Instance->TDR = *s++;
  }
}

void uart_user_app(void)
{
	MX_USART1_UART_Init();
	_HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	
//	MX_USART2_UART_Init();
//	
//	memset(RxBuffer,0,RX_BUF_LEN);
//	HAL_UART_Receive_IT(&huart2, RxBuffer, RX_BUF_LEN);
//	
//	mcu_Delay(100);
//	HAL_UART_AbortReceive_IT(&huart2);	
//	memset(RxBuffer,0,RX_BUF_LEN);
//	while(HAL_UART_Receive_IT(&huart2, RxBuffer, RX_BUF_LEN) == HAL_OK);
//  while(HAL_UART_Transmit_IT(&huart2, TxBuffer, strlen((char*)TxBuffer) == HAL_OK);
//	//timeout
//	rcvCnt = huart2->RxXferSize - huart2->RxXferCount;
}

void USART1_IRQHandler(void)
{
	int ch;
	if ((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_RXNE) != RESET))
	{
		if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)
		{
			
#if defined(USING_STM32F0_HAL) 
			ch = huart1.Instance->RDR & 0xff;
#else
			ch = huart1.Instance->DR & 0xff;
#endif

			//huart1.Instance->TDR = ch;

			if((USART_RX_STA&0x8000)==0)
			{
				if(USART_RX_STA&0x4000)
				{
					if(ch!=0x0a)USART_RX_STA=0;
					else USART_RX_STA|=0x8000;
				}
				else
				{	
					if(ch==0x0d)USART_RX_STA|=0x4000;
					else
					{
						USART_RX_BUF[USART_RX_STA&0X3FFF]=ch;
						USART_RX_STA++;
						if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0; 
					}		 
				}
			}		
		}
    __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_RXNE);
  }
}
//
static void MakeLowercase( char *str)		
{
	while(*str!='\0')
	{
			if( ((*str)>='A')&&((*str)<='Z') )
			{
					*str=*str+32;
			}
			str++;
	}
}
//
void CommandCheckAndExe(void)
{
	u8 len;
	if(USART_RX_STA&0x8000)									//接收到\r\n						
	{		
		len=USART_RX_STA&0x3fff;							//得到此次接收到的数据长度
		USART_RX_BUF[len]='\0';								//在末尾加入结束符. 
		
		MakeLowercase((char*)USART_RX_BUF);
		printf((char*)USART_RX_BUF);
		
		if(USART_RX_BUF[0]=='\0')									;
		else ProcessCommand((char*)USART_RX_BUF)	;				
		printf("\r\n>>");	
		USART_RX_STA=0;
	}
}

//


