#ifndef  __MOTOR_SENSOR_PROCESS_H
#define  __MOTOR_SENSOR_PROCESS_H

#include "bsp_defines.h"

extern uint8_t	motorsResetInOrder;

int  MotorSensor_thread_init(void);

void motor_gohome(uint8_t actualMotor, int home_speed);

#endif



