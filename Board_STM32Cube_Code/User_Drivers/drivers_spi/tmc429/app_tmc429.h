#ifndef  __APP_TMC429_H
#define  __APP_TMC429_H

#include "bsp_tmc429.h"

#define MVP_ABS							'A'
#define	MVP_REL							'R'
#define DIR_R   						'R'
#define DIR_L   						'L'
#define DIR_NONE   					'N'
#define POSCMP1_PIN	    		PE_12 
#define INTOUT1_PIN   			PE_13 

//EEPROM_DATA_ADDR
#define ADDR_EN_PIN					0
#define ADDR_EN_VALUE				1
#define ADDR_ORG						2
#define ADDR_LIMIT					3
#define ADDR_TEST_MODE  		4

typedef struct 
{
	uint8_t Enable[3];
	uint8_t completed[3];
	uint8_t SearchOriginDirection[3];
	uint8_t SearchLimitDirection[3];	
	int HommingSpeed[3];
}home_t;

extern home_t Homming;

void rt_hw_tmc429_init(void);

void motor_disable(char* input,u8 value);
void motor_limitINV(void);
void motor_orgINV(void);
void motor_isRESET(UCHAR Motor);

void motor_rotate(UCHAR Motor, int speed);
void motor_stop(UCHAR Motor);
void motor_move(UCHAR Motor,char* Type, int Position);

void motor_set(UCHAR Motor,char* Type,long Value);
void motor_get(UCHAR Motor,char* Type);

void motor_goHOME(UCHAR Motor,char* Dir,int speed);
void motor_goLIMIT(UCHAR Motor,char* Dir,int speed);

int motor_thread_init(void);

#endif


