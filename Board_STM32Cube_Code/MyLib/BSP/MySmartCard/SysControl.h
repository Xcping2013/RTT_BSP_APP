/**
  \file SysControl.h
  \author Trinamic Motion Control GmbH & Co KG
  \version 1.00

  \brief Motor monitoring 

  This file contains the definitions of the functions from the SysControl.c
  module.
*/
#ifndef __SYSCONTROL_H
#define	__SYSCONTROL_H

#include "stm32f1xx_hal.h"

#define	REFL_SEARCH			1		//!> Left		Sensor
#define	REFR_SEARCH			2		//!> Right	Sensor
#define	REFML_SEARCH		3		//!> Middle	Sensor
#define	REFMR_SEARCH		4		//!> Middle	Sensor

void HomingProcess(u8 Which,u8 SeachMode);

void StartAction_Button(u8 Button);
void ResetAction_Button(u8 Button);

uint8_t MyCLI_MotorGoHome(char *str);
uint8_t MyCLI_StatusEN(char *str);

#endif
