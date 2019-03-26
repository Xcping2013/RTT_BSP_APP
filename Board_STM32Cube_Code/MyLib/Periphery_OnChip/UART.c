#include "UART.h"
#include "DataOperate.h"
#include "control.h"
#include "defines.h"
#include "stdarg.h"

extern UART_HandleTypeDef huart1;

//----���ڷ���---------ʹ��ϵͳ��printf			
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

//----���ڽ���
volatile uint16_t UART_TimeoutTimer=0;		 		//���ڽ��ճ�ʱ��ʱ
volatile uint8_t 	UART_TimeoutFlag;					  //���ճ�ʱ��־

UCHAR		 aRxBuffer[1];
UCHAR  	 UART_Buffer[UART_BUFFER_SIZE]={0};   //���ڽ��ջ��� 
uint8_t  UART_Count=0;											  //���ջ��������±�
uint8_t  UART_Flag=0;										 		  //���յ���Ч���ݱ�־
//----���ڽ��մ���
static uint8_t UARTTimeout_Process(void);			//���ڽ����������
static uint8_t ProcessCommand(char *str);					//ͨѶ�����

/**											����΢��printf����Ӧ�Ĵ���
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}
/**											�Զ��巢���ַ�
  * @brief  Transmit a byte to the HyperTerminal
  * @param  param The byte to be sent
  * @retval HAL_StatusTypeDef HAL_OK if OK
  */
HAL_StatusTypeDef Serial_PutByte( UART_HandleTypeDef UartHandle , uint8_t param  )
{
  /* May be timeouted... */
  if ( UartHandle.State == HAL_UART_STATE_TIMEOUT )
  {
    UartHandle.State = HAL_UART_STATE_READY;
  }
  return HAL_UART_Transmit(&UartHandle, &param, 1, TX_TIMEOUT);
	//return HAL_UART_Transmit_DMA(&UartHandle, &param, 1);
}
/**											�Զ��巢���ַ���
  * @brief  Print a string on the HyperTerminal
  * @param  p_string: The string to be printed
  * @retval None
  */
void Serial_PutString(UART_HandleTypeDef UartHandle ,uint8_t *p_string )
{
  uint16_t length = 0;

  while (p_string[length] != '\0')
  {
    length++;
  }
  HAL_UART_Transmit(&UartHandle, p_string, length, TX_TIMEOUT);
}
/**											�Զ����ʽ���
  */
void Serial_Printf(UART_HandleTypeDef UartHandle , const char *fmt,...)
{   
	va_list ap;	   							//This is support the format output 
	char string[200];
	va_start(ap,fmt);
	vsprintf(string,fmt,ap);	 // Store the string in the String buffer
	Serial_PutString( UartHandle , (uint8_t *)string);
	va_end(ap);	 
}
/**											����������õ���ʱ ���ڶ�ʱ����
  */
void UART_RxTimeOutCheck(void)
{
	if(UART_TimeoutTimer>0)
  {
    UART_TimeoutTimer--;
    if(UART_TimeoutTimer==0) UART_TimeoutFlag=1;
  }
}
/**											���������־����
  */
static uint8_t UARTTimeout_Process(void)
{
  if(UART_TimeoutFlag)
  {
    UART_TimeoutFlag=FALSE;
    return TRUE;
  }
  else return FALSE;
}
/**											�����Զ�����Ч�ַ�
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{								
	if(	aRxBuffer[0]=='\r') 						
	{
		UART_Buffer[UART_Count] ='\0';										
		UART_Flag=1;
		UART_Count=0;
	}
	else if(aRxBuffer[0]=='\n');			
	else if( UART_Count < UART_BUFFER_SIZE-1) UART_Buffer[UART_Count++]=aRxBuffer[0]; 

	UART_TimeoutTimer=UART_TIMEOUT_VALUE;								
	HAL_UART_Receive_IT( &huart1, aRxBuffer, 1 );
}
/**			���ڴ���			������Ч�жϣ�����������������
  */
void UART_Process(UART_HandleTypeDef *huart)
{
	if(UART_Flag==1)											 
	{
		//printf("-->>%s\r\n",(char *)UART_Buffer	);
		ProcessCommand((char *)UART_Buffer);
		//printf(">>\r\n"	);
		UART_Flag=0;
		UART_Count=0;
	}
	if(UARTTimeout_Process()) //��ʱ����		
	{ 
		UART_Flag=0;
		UART_Count=0;
	}	
	HAL_UART_Receive_IT( &huart1, aRxBuffer, 1 );
}
/**											ͨѶ�����
  */
static u8 ProcessCommand(char *str)
{
	MakeLowercase(str);	
	
	if(  strcmp(str,"reset")==0  )					{	NVIC_SystemReset();											return 1;			}		
										 	
	else if(  strcmp(str,"ver")==0  )				{	printf("Template\r\n");
																						printf("Hardware: Ver1.00\r\n");
																						printf("Software: Ver1.00\r\n");				return 1;			}	
																																																
	else if(  strcmp(str,"fsn")==0  )				{	printf("SN:Temp-0001\r\n");							return 1;			}			 	
																								
	else if(  strcmp(str,"help")==0  )			{	printf("help---------------command list\r\n");
																						printf("reset--------------Reset MPU\r\n");
																						printf("fsn----------------Get fixture SN number\r\n");
																						//printf("sn:xxx......-------Write the fixture SN number\r\n");	
																						printf("ver----------------print version\r\n");	
																						printf("in[x]--------------get input x status\r\n");
																						printf("out[x,y]-----------set output x to y\r\n"); 	
																																										return 1;			}		
	
	else if( strncmp(str,"in[",3)==0 && str[5]==']' ) 
	{
		u8 Pin=((str[3]-'0')*10)+((str[4])-'0');	
		if(	Pin	<=	INPUTn &&	Pin	>= 1	)
		{
			//printf("%s<%d>\r\n",str	,	GetInputState(Pin-1)  );	
			return 1;																																
		}																																									
	}	
	else if( strncmp(str,"out[",4)==0 && str[6]==',' && str[8]==']' ) 
	{
		u8 Pin=((str[4]-'0')*10)+((str[5])-'0');	
		if(	Pin	<=	OUTPUTn && Pin	>= 1	)
		{ 
			//if( (str[7]-'0')	!=0	)	SetOutput_On(Pin-1); else SetOutput_Off(Pin-1);
			printf("OUT[%d]<%c>\r\n",Pin,str[7] );																				
			return 1;					
		}																																										
	}	
	else if(str[0]=='\0' )																														return 1;	

	printf("Command '%s' not found.\r\n",str);		 																		return 0;
}
/**			
  */





	



