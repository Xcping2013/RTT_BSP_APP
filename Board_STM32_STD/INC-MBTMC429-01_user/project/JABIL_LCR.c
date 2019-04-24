#include <stdlib.h>
#include "main.h"
#include "IO.h"
#include "UART.h"
#include "systick.h"
#include "JABIL_LCR.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/*	GPIO��
		INPUT���������裬IN=0,INPUT=0,INT[N]=1
										 IN=1,INPUT=1,INT[N]=0
*/
/*	IO����Դ��
		IN1	��������1														          OUT1 ��������1		
		IN2	��������2														          OUT2 ��������2
		IN3	DUT����������������  ��λ										  OUT3 DUT���׽�ȥ ���ܿ�������
		IN4	DUT����������������  ʼλ										  OUT4 DUT���׵��� ���ܿ�������
			                                                
		IN5	���봫�������������� ʼλ		JABIL_LCR ȡ��	  OUT5 �������װ�ѹ ���ܿ�������
		IN6 ���봫�������������� ��λ		JABIL_LCR ȡ��	  OUT6 ������������ ���ܿ�������
		                                                  
		IN7 ���´��������������� ʼλ										  OUT7 ����������ѹ ���ܿ�������
		IN8 ���´��������������� ��λ										  OUT8 ������������ ���ܿ�������
*/
#define IN_ON					0
#define IN_OFF				1
#define OUT_ON				1
#define OUT_OFF				0
#define KEY_START1		INPUT1			
#define KEY_START2		INPUT2			

#define LED_START1		OUTPUT1
#define LED_START2		OUTPUT2

#define DutGoInside_LCR()		OUTPUT3=1;	OUTPUT4=0		
#define DutGoOutside_LCR()	OUTPUT3=0;	OUTPUT4=1		
#define CloseTheDoor_LCR()	OUTPUT7=1;	OUTPUT8=0		
#define OpenTheDoor_LCR()		OUTPUT7=0;	OUTPUT8=1		

#define STANDBY 				0
#define DUT_GO_INSIDE 	1
#define CLOSE_DOOR 			2
#define OPEN_DOOR 			3
#define DUT_GO_OUTSIDE 	4

#define	INSIDE					0
#define	OUTSIDE					1
#define	UNDEFINE				3
#define	UPSIDE					0
#define DOWNSIDE				1

uint8_t	OpStep_LCR=STANDBY;
uint8_t	KeyS_actOK_LCR=0;
uint8_t KeyS_actEN_LCR=0;
uint8_t KeyS_Delay=100;

static UCHAR DutPosition_JABIL_LCR(void);
static UCHAR DoorPosition_JABIL_LCR(void);

//
void SW_init_JABIL_LCR(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_12;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1|GPIO_Pin_12);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;									
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	U1_IN1_IN2=0;	U2_IN1_IN2=0;//Ĭ���л�״̬ΪCOIL1---COIL2
}
//
static UCHAR DutPosition_JABIL_LCR(void)
{
	if		 (INPUT3==IN_ON 	&& INPUT4==IN_OFF) return INSIDE;
	else if(INPUT3==IN_OFF 	&& INPUT4==IN_ON)  return OUTSIDE;	
	else {return UNDEFINE;}
}
static UCHAR DoorPosition_JABIL_LCR(void)
{
	if		 (INPUT7==IN_ON 	&& INPUT8==IN_OFF) return UPSIDE;
	else if(INPUT7==IN_OFF 	&& INPUT8==IN_ON)  return DOWNSIDE;
	else {return UNDEFINE;}	
}



