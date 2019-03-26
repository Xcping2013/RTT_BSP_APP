#include "bsp.h"
//#include "app_uart1_console.h"
//int fputc(int c, FILE *f) 
//printf \r\n不显示为串口工具的功能问题

#if 1

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

uint8_t 	 USART_RX_BUF[USART_REC_LEN];  
uint16_t	 USART_RX_STA=0; 

#if defined(USING_STM32F0_STD)
	#include "stm32f0xx.h"
	#include "stm32f0xx_usart.h"
#elif	defined(USING_STM32F1_STD)
	#include "stm32f1xx.h"
	#include "stm32f1xx_usart.h"
#elif	defined(USING_STM32F0_HAL)
	#include "stm32f0xx_hal.h"
	#include "stm32f0xx_hal_usart.h"
#elif	defined(USING_STM32F1_HAL)
	#include "stm32f1xx_hal.h"
	#include "stm32f1xx_hal_usart.h"
#endif

PUTCHAR_PROTOTYPE
{
	#if defined(USING_STM32F0_STD)	|| defined(USING_STM32F1_STD) 
  USART_SendData(USART1,(uint8_t)  ch);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
	{}
	#else
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
	#endif
  return (ch);
}


void uart1_hw_init(int BaudRate)
{
	#if defined(USING_STM32F0_STD)
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;
					
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );
					
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_1);        
	/*
	*  USART1_TX -> PA9 , USART1_RX ->        PA10
	*/                                
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);        

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	USART_InitStructure.USART_BaudRate = BaudRate;						//设置串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//设置数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//设置停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//设置效验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//设置流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//设置工作模式
	USART_Init(USART1, &USART_InitStructure); //配置入结构体
	
	USART_ClearFlag(USART1, USART_FLAG_WU | USART_FLAG_CM  | USART_FLAG_EOB  | USART_FLAG_RTO	|
                          USART_FLAG_CTS  | USART_FLAG_LBD | USART_FLAG_TC |USART_FLAG_IDLE	|
                          USART_FLAG_ORE | USART_FLAG_NE   | USART_FLAG_FE | USART_FLAG_PE);

	USART_ITConfig(USART1,USART_IT_WU  	,DISABLE);
	USART_ITConfig(USART1,USART_IT_CM   ,DISABLE);
	USART_ITConfig(USART1,USART_IT_EOB  ,DISABLE);
	USART_ITConfig(USART1,USART_IT_RTO  ,DISABLE);
	USART_ITConfig(USART1,USART_IT_CTS  ,DISABLE);
	USART_ITConfig(USART1,USART_IT_LBD  ,DISABLE);
	
  USART_ITConfig(USART1,USART_IT_TXE  ,DISABLE);
  USART_ITConfig(USART1,USART_IT_TC 	,DISABLE);
  USART_ITConfig(USART1,USART_IT_RXNE	,ENABLE);
  USART_ITConfig(USART1,USART_IT_IDLE	,DISABLE);
  USART_ITConfig(USART1,USART_IT_PE 	,DISABLE);
  USART_ITConfig(USART1,USART_IT_ERR 	,DISABLE);

	USART_Cmd(USART1, ENABLE);//使能串口1
	
	#elif defined(USING_STM32F1_STD)
	
  USART_InitTypeDef  UART_InitStructure;
  NVIC_InitTypeDef 	 NVIC_InitStructure;
  GPIO_InitTypeDef 	 GPIO_InitStructure;

  USART_DeInit(USART1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  USART_StructInit(&UART_InitStructure);
  UART_InitStructure.USART_BaudRate=BaudRate;

  USART_Init(USART1,&UART_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


  USART_ClearFlag(USART1, USART_FLAG_CTS | USART_FLAG_LBD  | USART_FLAG_TXE  |
                          USART_FLAG_TC  | USART_FLAG_RXNE | USART_FLAG_IDLE |
                          USART_FLAG_ORE | USART_FLAG_NE   | USART_FLAG_FE |
                          USART_FLAG_PE);
  USART_ITConfig(USART1,USART_IT_PE  ,DISABLE);
  USART_ITConfig(USART1,USART_IT_TXE ,DISABLE);
  USART_ITConfig(USART1,USART_IT_TC  ,DISABLE);
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
  USART_ITConfig(USART1,USART_IT_IDLE,DISABLE);
  USART_ITConfig(USART1,USART_IT_LBD ,DISABLE);
  USART_ITConfig(USART1,USART_IT_CTS ,DISABLE);
  USART_ITConfig(USART1,USART_IT_ERR ,DISABLE);
  USART_Cmd(USART1, ENABLE);
	
	#elif defined(USING_STM32F0_HAL) || defined(USING_STM32F1_HAL)
	
	
	
	
	
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	#endif
}
		
//

#if defined(USING_STM32F0_STD) 		|| defined(USING_STM32F1_STD)
void USART1_IRQHandler(void)
{
	#if defined(USING_STM32F0_STD) 
	if(USART1->ISR & USART_FLAG_RXNE)
	#elif defined(USING_STM32F1_STD)
  if(USART1->SR & USART_FLAG_RXNE)
	#endif
  {
		u8 Res = USART_ReceiveData(USART1);									//读取接收到的数据	
		//USARTx->DR = (Res & (uint16_t)0x01FF);
		if((USART_RX_STA&0x8000)==0)												//接收未完成
		{
			if(USART_RX_STA&0x4000)															//接收到了0x0d
			{
				if(Res!=0x0a)	USART_RX_STA=0;											//接收错误,重新开始
				else 					USART_RX_STA|=0x8000;												//接收完成了 
			}
			else 																								//还没收到0X0D
			{	
				if(Res==0x0d)	USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		} 
	}
}
#else
void USART1_IRQHandler(void)
{
	if ((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_RXNE) != RESET))
	{
		if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)
		{
			#if defined(USING_STM32F0_HAL) 
			int ch = huart1.Instance->RDR & 0xff;
			#else
			int ch = huart1.Instance->DR & 0xff;
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
						if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}		
		}
		//printf("USART_RX_STA=%d\r\n" ,USART_RX_STA);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_RXNE);
  }

}
#endif

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

#endif


