#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#ifndef FALSE
#define FALSE								0
#endif

#ifndef TRUE
#define TRUE								1
#endif

#ifndef IN_ON
#define IN_ON  							0
#endif

#ifndef IN_OFF
#define IN_OFF  						1
#endif

uint8_t buttonSTARTpressed=0;
uint8_t buttonRESETpressed=0;

//气缸下压 4 [1] 3 [0]	END=0	IN4下压触发	光栅触发是0
#define BUTTON_START 	  				HAL_GPIO_ReadPin(IN1_GPIO_Port,IN1_Pin) 						//INPUT1
#define BUTTON_RESET 						HAL_GPIO_ReadPin(IN2_GPIO_Port,IN2_Pin) 						//INPUT2
#define CYLINDER_START					HAL_GPIO_ReadPin(IN3_GPIO_Port,IN3_Pin) 						//INPUT3
#define CYLINDER_END	  				HAL_GPIO_ReadPin(IN4_GPIO_Port,IN4_Pin)							//INPUT4
#define SAFETY_LIGHT	  				HAL_GPIO_ReadPin(IN5_GPIO_Port,IN5_Pin)							//INPUT5
#define DUT_BUTTON  						HAL_GPIO_ReadPin(IN6_GPIO_Port,IN6_Pin)							//INPUT6
#define BUTTON_START_LED 	  																												//OUTPUT1
#define BUTTON_START_LED_ON			HAL_GPIO_WritePin(OUT1_GPIO_Port,OUT1_Pin, GPIO_PIN_SET);
#define BUTTON_START_LED_OFF		HAL_GPIO_WritePin(OUT1_GPIO_Port,OUT1_Pin, GPIO_PIN_RESET);
#define BUTTON_RESET_LED 	  																												//OUTPUT2
#define BUTTON_RESET_LED_ON  		HAL_GPIO_WritePin(OUT2_GPIO_Port,OUT2_Pin, GPIO_PIN_SET);
#define BUTTON_RESET_LED_OFF  	HAL_GPIO_WritePin(OUT2_GPIO_Port,OUT2_Pin, GPIO_PIN_RESET);
#define CYLINDER_START_OUT																													//OUTPUT3
#define CYLINDER_START_OUT_ON		HAL_GPIO_WritePin(OUT3_GPIO_Port,OUT3_Pin, GPIO_PIN_SET);
#define CYLINDER_START_OUT_OFF	HAL_GPIO_WritePin(OUT3_GPIO_Port,OUT3_Pin, GPIO_PIN_RESET);
#define CYLINDER_END_OUT																														//OUTPUT4
#define CYLINDER_END_OUT_ON			HAL_GPIO_WritePin(OUT4_GPIO_Port,OUT4_Pin, GPIO_PIN_SET);
#define CYLINDER_END_OUT_OFF		HAL_GPIO_WritePin(OUT4_GPIO_Port,OUT4_Pin, GPIO_PIN_RESET);
#define ALARM_LED_RED   																														//OUTPUT5
#define ALARM_LED_RED_ON				HAL_GPIO_WritePin(OUT5_GPIO_Port,OUT5_Pin, GPIO_PIN_SET);
#define ALARM_LED_RED_OFF				HAL_GPIO_WritePin(OUT5_GPIO_Port,OUT5_Pin, GPIO_PIN_RESET);
#define ALARM_LED_GREEN   																													//OUTPUT6
#define ALARM_LED_GREEN_ON			HAL_GPIO_WritePin(OUT6_GPIO_Port,OUT6_Pin, GPIO_PIN_SET);
#define ALARM_LED_GREEN_OFF			HAL_GPIO_WritePin(OUT6_GPIO_Port,OUT6_Pin, GPIO_PIN_RESET);
#define ALARM_SPEEKER		   																													//OUTPUT7
#define ALARM_SPEEKER_ON				HAL_GPIO_WritePin(OUT7_GPIO_Port,OUT7_Pin, GPIO_PIN_SET);
#define ALARM_SPEEKER_OFF				HAL_GPIO_WritePin(OUT7_GPIO_Port,OUT7_Pin, GPIO_PIN_RESET);

uint8_t beep_flash_on=0;
//IN3=1	IN4=0 
void cylinder_down(void)
{
	while(HAL_GPIO_ReadPin(IN5_GPIO_Port,IN5_Pin)==1)
	{
		HAL_Delay(500);		
	}
	HAL_Delay(200);
	HAL_GPIO_WritePin(OUT3_GPIO_Port,OUT3_Pin, GPIO_PIN_RESET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(OUT4_GPIO_Port,OUT4_Pin, GPIO_PIN_SET);
	HAL_Delay(200);
}
//IN3=0	IN4=1 
void cylinder_up(void)
{
	while(HAL_GPIO_ReadPin(IN5_GPIO_Port,IN5_Pin)==1)
	{
		HAL_Delay(500);			
	}
	HAL_Delay(200);	
	HAL_GPIO_WritePin(OUT3_GPIO_Port,OUT3_Pin, GPIO_PIN_SET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(OUT4_GPIO_Port,OUT4_Pin, GPIO_PIN_RESET);
	HAL_Delay(200);
}
//按下开始或者复位说明DUT按键已经放置OK
//气缸未按压DUT已经处于稳定状态
void FlexButtonTest_APP(void)
{
	if( BUTTON_START==IN_ON )					
	{
		HAL_Delay(20);		
		if( BUTTON_START==IN_ON )											 																
		{
			 BUTTON_START_LED_ON;	 	
			 if(DUT_BUTTON==0)
			 {
				  HAL_Delay(500);
				  BUTTON_START_LED_OFF;	 	
				 
					ALARM_LED_GREEN_OFF;
					ALARM_LED_RED_ON;	
				  beep_flash_on=1;
			 }
			 else 
			 {
				 while((CYLINDER_START==0 && CYLINDER_END==1)? 0:1){;}		//up   IN3=0;IN4=1
				 cylinder_down();	
				 while((CYLINDER_START==1 && CYLINDER_END==0)? 0:1){;}		//down IN3=1;IN4=0
				 HAL_Delay(1000);
				 if(DUT_BUTTON==0)
				 {
						ALARM_LED_GREEN_ON;
						ALARM_LED_RED_OFF;
						beep_flash_on=0;				
				}
				else 
				{
						ALARM_LED_GREEN_OFF;
						ALARM_LED_RED_ON;	
						beep_flash_on=1;						
				}
				cylinder_up();
				BUTTON_START_LED_OFF;
			}
		}
	}
	if( BUTTON_RESET==IN_ON )							
	{
		HAL_Delay(20);
		if( BUTTON_RESET==IN_ON )																				
		{		
				BUTTON_RESET_LED_ON;	
				cylinder_up();

				while((CYLINDER_START==0 && CYLINDER_END==1)? 0:1){;}	//up   IN3=0;IN4=1
			  HAL_Delay(200);
					
				BUTTON_RESET_LED_OFF;						
				ALARM_LED_GREEN_OFF;
				ALARM_LED_RED_OFF;
				beep_flash_on=0;
		}
	}	
}



