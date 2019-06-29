/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "iwdg.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "TM1638.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t  USART_RX_BUF[USART_REC_LEN]; 	
uint16_t USART_RX_STA=0; 

uint16_t display_num[10]={C7_0,C7_1,C7_2,C7_3,C7_4,C7_5,C7_6,C7_7,C7_8,C7_9};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
uint32_t tickLedstart;
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
//  MX_I2C1_Init();
//  MX_IWDG_Init();
  MX_USART1_UART_Init();
//  MX_SPI1_Init();
//  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
TM1638_init();
	
tickLedstart=HAL_GetTick();
		TM1638_writeData(display_num[9], SEG_BIT1);
		TM1638_writeData(display_num[1], SEG_BIT2);
		TM1638_writeData(display_num[0], SEG_BIT3);
		TM1638_writeData(display_num[2], SEG_BIT4);
printf("mcu init ok\r\n");
printf("if you see this string , the cable RX is OK\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		CommandCheckAndExe();
		if(abs((int)(HAL_GetTick()-tickLedstart))>800)
		{
			HAL_GPIO_TogglePin(SYS_LED_GPIO_Port,SYS_LED_Pin);
			tickLedstart=HAL_GetTick();
		if(cable_rx_ok==1)
		printf("if you see this string 线材通讯OK 调试板的LED需要亮\r\n");
//			printf("if you see this string , the cable RX is OK...线材接收OK...上位机发送字符串来验证Tx是否OK\r\n");
			//printf("\r\n");
		}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void CommandCheckAndExe(void)
{
	uint8_t len;
	if(USART_RX_STA&0x8000)									//接收到\r\n						
	{	
		len=USART_RX_STA&0x3fff;							//得到此次接收到的数据长度
		USART_RX_BUF[len]='\0';								//在末尾加入结束符. 
		printf((char*)USART_RX_BUF);
		printf("\n");
		if(USART_RX_BUF[0]=='\0')	
			printf("\n");
		else if(ProcessCommand((char*)USART_RX_BUF))	;
		else 
			printf("<NA>\n");								
		
		printf(">>");	
		USART_RX_STA=0;
	}	
}
uint8_t cable_rx_ok=0; 
uint8_t  ProcessCommand(char  *Commands)
{
	//MakeLowercase(Commands);	

//	if		  (CommmandLine_help(Commands)==1)								return 1;
//	
//	else if(strcmp("reset",		Commands)==0)								{		ResetCPU(TRUE);																														return 1;		}		

////board iO test
//	else if(strcmp("io test",							Commands)==0)	{		Board_debugFlag=1;ProID=PROJECT_NONE;									printf("OK\n");					return 1;		}		
////Project switch	
//	else if(strcmp("type[basic]",	Commands)==0)					{		AT24CXX_WriteOneByte(ADDR_PROJECT_ID,BASIC_APP);			printf("OK\n");					return 1;		}		
//	else if(strcmp("type[button online]",	Commands)==0)	{		AT24CXX_WriteOneByte(ADDR_PROJECT_ID,BUTTON_ONLINE);	printf("OK\n");					return 1;		}		
//	else if(strcmp("type[button offline]",Commands)==0)	{		AT24CXX_WriteOneByte(ADDR_PROJECT_ID,BUTTON_OFFLINE);	printf("OK\n");					return 1;		}		
//	else if(strcmp("type[button ver3]",		  Commands)==0)	{	AT24CXX_WriteOneByte(ADDR_PROJECT_ID,BUTTON_VER3);		printf("OK\n");					return 1;		}			
//	else if(strcmp("type[road]",		  Commands)==0)				{	AT24CXX_WriteOneByte(ADDR_PROJECT_ID,BUTTON_ROAD);		printf("OK\n");					return 1;		}			

//	else if(strcmp("type[lidopen]",				Commands)==0)	{		AT24CXX_WriteOneByte(ADDR_PROJECT_ID,LIDOPEN);				printf("OK\n");					return 1;		}		
//	else if(strcmp("type[jabil lcr]",		  Commands)==0)	{		AT24CXX_WriteOneByte(ADDR_PROJECT_ID,JABIL_LCR);			printf("OK\n");					return 1;		}			

	if(strcmp("display 6666 via uart",		  Commands)==0)	
	{			

		TM1638_writeData(C7_6, SEG_BIT1);
		TM1638_writeData(C7_6, SEG_BIT2);
		TM1638_writeData(C7_6, SEG_BIT3);
		TM1638_writeData(C7_6, SEG_BIT4);
		return 1;		
	}		
	if(strcmp("display 0000 via uart",		  Commands)==0)	
	{			

		TM1638_writeData(C7_0, SEG_BIT1);
		TM1638_writeData(C7_0, SEG_BIT2);
		TM1638_writeData(C7_0, SEG_BIT3);
		TM1638_writeData(C7_0, SEG_BIT4);
		return 1;		
	}	
	if(strlen(Commands)==4 )
	{
		TM1638_writeData(display_num[Commands[0]-'0'], SEG_BIT4);
		TM1638_writeData(display_num[Commands[1]-'0'], SEG_BIT3);
		TM1638_writeData(display_num[Commands[2]-'0'], SEG_BIT2);
		TM1638_writeData(display_num[Commands[3]-'0'], SEG_BIT1);
		return 1;
	}
	else if(strcmp("help",		Commands)==0)								
	{		
		printf("display 6666 via uart\r\n");		
		printf("display 0000 via uart\r\n");			
		return 1;		
	}		
	else if(strcmp("cable debug",		Commands)==0)								
	{		
		cable_rx_ok=1;
		//printf("if you see this string 线材通讯OK\r\n");
		return 1;		
	}	
	else
	{
		printf("if you see this string 线材通讯OK 调试板的LED需要亮\r\n");
		return 1;
		//return 0;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
