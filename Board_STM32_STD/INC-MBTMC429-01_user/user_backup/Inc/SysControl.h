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

#define X_GoHome  10
#define Y_GoHome  11
#define Z_GoHome  12
#define GoHome_OK 13

#define X_HomeSensor INPUT5
#define Y_HomeSensor INPUT6
#define Z_HomeSensor INPUT7

//°´¼ü
extern u8 KeySTART_ON;
extern u8 KeyEmergency_ON;
extern u8 KeyRESET_ON;
extern u8 MotorIsHomed;
extern u16 SpeedForHoming[3];

void SetAmaxAuto(u8 ActualMotor,int speed);
void RotateRight(u8 axis, int velocity);
void RotateLeft(u8 axis, int velocity);

void MotorHoming(void);
void MotorHoming_ver3(void);
void MotorHoming_preset( uint8_t ID);
void MotorHoming_preset_Road(void);
void CommandCheckAndExe(void);

void EXTIX_Init(void);

//-----------------------------------------------------------Lidopen
extern u8 HomeStep;

void MotorHoming_Lidopen(void);
//-----------------------------------------------------------Lidopen
u8 	KeyEmergency_EXE(void);
void KeySTART_EXE(void);
void KeyRESET_EXE(void);

void MotorHoming_preset( uint8_t ID);
void MotorHoming_WithHomeSensor( uint8_t ID);
void MotorHoming_WithoutHomeSensor( uint8_t ID);

void KeyRESET_EXE_ONLINE(void);
void KeyRESET_EXE_OFFLINE(void);
void KeyRESET_EXE_IAC_LCR(void);
void MotorHoming_Button(void);
void MotorHoming_Button3(void);
void MotorHoming_Road(void);
void Cylinder_Reset_check(void);

#endif
