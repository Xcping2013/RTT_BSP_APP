#ifndef  __APP_MOTION_H
#define  __APP_MOTION_H

#include "bsp_tmc429.h"

void rt_hw_tmc429_init(void);

uint8_t TMCL_MotorRotate(void);
void RotateRight(uint8_t axisNum , int speed);
void RotateLeft(uint8_t axisNum , int speed);
void MotorStop(uint8_t axisNum);

int motion_thread_init(void);
int motor(int argc, char **argv);




#endif


