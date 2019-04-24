/**
  \file SysControl.h

  \version 1.00

  \brief Motor monitoring 

  This file contains the definitions of the functions from the SysControl.c
  module.
*/
#ifndef __SYSCONTROL_H
#define	__SYSCONTROL_H

#define AXIS_X	0
#define AXIS_Y	1
#define AXIS_Z	2

#define X_GoHome  					10
#define Y_GoHome  					11
#define Z_GoHome  					12
#define GoHome_OK 					13
#define NO_ACTION						0
#define SEARCH_L_LIMIT_V1 	20
#define MOVE_REF_RIGHT 			21
#define SEARCH_L_LIMIT_V2 	22
#define SET_L_LIMIT_HOME 		23

//#define GoHome_NONE 14

#define KEY_START_IN    			INPUT1
#define KEY_RESET_IN    			INPUT2
#define KEY_EMERGENCY_IN    	INPUT3

#define KEY_START_LED     		OUTPUT1
#define KEY_RESET_LED     		OUTPUT2

#define X_HomeSensor 					INPUT5
#define Y_HomeSensor 					INPUT6
#define Z_HomeSensor 					INPUT7

#define SERVO_ON_OUT 					OUTPUT6
//°´¼ü
extern u8 KEY_START_pressed;
extern u8 KEY_EMERGENCY_pressed;
extern u8 KEY_RESET_pressed;
extern u8 MotorIsHomed;
extern u16 Homing_speed[3];

void SetAmaxAuto(u8 ActualMotor,int speed);
void SetAmaxAuto_old(u8 ActualMotor,int speed);
void RotateRight(u8 axis, int velocity);
void RotateLeft(u8 axis, int velocity);

void MotorHoming_ver3(void);
void MotorHoming_preset( uint8_t ID);
void MotorHoming_preset_Road(void);
void CommandCheckAndExe(void);

void EXTIX_Init(void);

//-----------------------------------------------------------Lidopen
extern u8 Homing_step;

void MotorHoming_Lidopen(void);
//-----------------------------------------------------------Lidopen
void KeySTART_EXE(void);
void KeyRESET_EXE(void);

void MotorHoming_preset( uint8_t ID);
void MotorHoming_WithHomeSensor(void);
void MotorHoming_WithoutHomeSensor( uint8_t ID);

void KeyRESET_EXE_ONLINE(void);

void KeyRESET_EXE_IAC_LCR(void);
void MotorHoming_Button(void);
void MotorHoming_Button3(void);
void MotorHoming_Road(void);
void Cylinder_Reset_check(void);

void IO_DebugViaLedBlinking(void);


#endif
