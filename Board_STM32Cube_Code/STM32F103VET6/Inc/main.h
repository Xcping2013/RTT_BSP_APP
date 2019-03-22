/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IN8_Pin GPIO_PIN_2
#define IN8_GPIO_Port GPIOE
#define IN9_Pin GPIO_PIN_3
#define IN9_GPIO_Port GPIOE
#define IN10_Pin GPIO_PIN_4
#define IN10_GPIO_Port GPIOE
#define IN11_Pin GPIO_PIN_5
#define IN11_GPIO_Port GPIOE
#define IN12_Pin GPIO_PIN_6
#define IN12_GPIO_Port GPIOE
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOC
#define TMC429_CLK1_Pin GPIO_PIN_0
#define TMC429_CLK1_GPIO_Port GPIOB
#define POSCMP1_Pin GPIO_PIN_12
#define POSCMP1_GPIO_Port GPIOE
#define INTOUT1_Pin GPIO_PIN_13
#define INTOUT1_GPIO_Port GPIOE
#define SPI2_CS_Pin GPIO_PIN_12
#define SPI2_CS_GPIO_Port GPIOB
#define OUT1_Pin GPIO_PIN_15
#define OUT1_GPIO_Port GPIOA
#define OUT2_Pin GPIO_PIN_10
#define OUT2_GPIO_Port GPIOC
#define OUT3_Pin GPIO_PIN_11
#define OUT3_GPIO_Port GPIOC
#define OUT4_Pin GPIO_PIN_12
#define OUT4_GPIO_Port GPIOC
#define OUT5_Pin GPIO_PIN_0
#define OUT5_GPIO_Port GPIOD
#define OUT6_Pin GPIO_PIN_1
#define OUT6_GPIO_Port GPIOD
#define OUT7_Pin GPIO_PIN_2
#define OUT7_GPIO_Port GPIOD
#define OUT8_Pin GPIO_PIN_3
#define OUT8_GPIO_Port GPIOD
#define RGB_B_Pin GPIO_PIN_4
#define RGB_B_GPIO_Port GPIOD
#define BEEP_Pin GPIO_PIN_5
#define BEEP_GPIO_Port GPIOD
#define LED_SYS_Pin GPIO_PIN_7
#define LED_SYS_GPIO_Port GPIOD
#define IIC_SDA_Pin GPIO_PIN_3
#define IIC_SDA_GPIO_Port GPIOB
#define IIC_SCL_Pin GPIO_PIN_4
#define IIC_SCL_GPIO_Port GPIOB
#define IN1_Pin GPIO_PIN_5
#define IN1_GPIO_Port GPIOB
#define IN2_Pin GPIO_PIN_6
#define IN2_GPIO_Port GPIOB
#define IN3_Pin GPIO_PIN_7
#define IN3_GPIO_Port GPIOB
#define IN4_Pin GPIO_PIN_8
#define IN4_GPIO_Port GPIOB
#define IN5_Pin GPIO_PIN_9
#define IN5_GPIO_Port GPIOB
#define IN6_Pin GPIO_PIN_0
#define IN6_GPIO_Port GPIOE
#define IN7_Pin GPIO_PIN_1
#define IN7_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

extern uint8_t buttonSTARTpressed;
extern uint8_t buttonRESETpressed;
extern uint8_t beep_flash_on;
void FlexButtonTest_APP(void);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
