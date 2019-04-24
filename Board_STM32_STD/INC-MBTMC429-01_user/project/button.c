#include "main.h"
#include "Globals.h"

void RampInit_Button(void)
{
  uint8_t i;
	for(i=0; i<N_O_MOTORS; i++) 					//默认电机主要参数配置，主要为速度和加速度
	{																  
		MotorConfig[i].VMax=839;   					//  3R/S 	如果是Y轴2mm 1圈的话 V=70=30mm/min	 0.25R/S	3/0.25=12	839/12=70
		MotorConfig[i].AMax=1000;
		MotorConfig[i].PulseDiv=5;
		MotorConfig[i].RampDiv=8;	
		//MotorConfig[i].RampDiv=6;					//IAC ORIN
		
		MotorConfig[ActualMotor].usteps=8; //200*8=1600
		
		SpeedChangedFlag[i]=TRUE;
	}
}
//

