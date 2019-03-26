/**
  \file 		Commands.h
  \author 	Xcping2016
  \version 	1.00

  \brief 		Char Send&Receive command definitions

  This file contains all necessary definitions for the
  PC command interpreter.
*/
#ifndef __COMMANDS_H
#define	__COMMANDS_H

#include "stm32f1xx_hal.h"

void 		InitTMCL(void);
uint8_t CommandCheckExe(char  *str);
/*******************************************************/
extern uint8_t  MyCLI_GetINT(char *Commands);
extern uint8_t  MyCLI_SetOUT(char *Commands);
extern uint8_t  MyCLI_SetLED(char *Commands);
/*******************************************************/
extern uint8_t  MyCLI_RomStr(char *Commands);
extern uint8_t  MyCLI_RomNum(char *Commands);
extern uint8_t  MyCLI_RomFsn(char *Commands);
extern uint8_t  MyCLI_INV_SP(char *Commands);
extern uint8_t  MyCLI_RomCodeUpdateFlag(char *Commands);
extern uint8_t  MyCLI_EEPROMTest(char *Commands);
extern uint8_t  MyCLI_IAP_Flag(char *Commands);

extern uint8_t  MyCLI_ReadWtiteEEPROM(char *Commands);

extern uint8_t  MyCLI_HomeMode(char *Commands);
extern uint8_t	MyCLI_HomeV(char *Commands);
extern uint8_t 	MyCLI_MotorPos(char *Commands);
extern uint8_t 	MyCLI_MotorV(char *Commands);
extern uint8_t 	MyCLI_MotorAcc(char *Commands);
extern uint8_t 	MyCLI_MotorRot(char *Commands);
extern uint8_t 	MyCLI_MotorMove(char *Commands);
extern uint8_t 	MyCLI_MotorStop(char *Commands);
extern uint8_t 	MyCLI_MotorLS(char *Commands);
extern uint8_t 	MyCLI_MotorStatus(char *Commands);
extern uint8_t 	MyCLI_MotorRampDiv(char *Commands);
extern uint8_t 	MyCLI_MotorPulseDiv(char *Commands);



#endif

