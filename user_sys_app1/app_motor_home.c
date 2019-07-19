//#include "app_motor_home.h"
//#include "app_sys_control.h"

#include "bsp_include.h"	
#include "app_include.h"

homeInfo_t homeInfo;

/*********************************************************************************************************************/
static void setPositionAsOrigin(uint8_t axisNum);

/*********************************************************************************************************************/
//在定时器调用的函数里面使用delay_ms？和调用定时器开关？
static void setPositionAsOrigin(u8 axisNum) 
{
	/* 
	Before overwriting X_ACTUAL choose velocity_mode or hold_mode.If X_ACTUAL is overwritten in ramp_mode or soft_mode the motor directly drives to X_TARGET
	*/
	//HardStop(axisNum);
	Stop_HardTimer();
	
	StopMotorByRamp(axisNum);
	/*
	  To set target position X_TARGET and current position X_ACTUAL to an equivalent value (e.g. to set both to zero at a reference point)
  	the assigned stepper motor should be stopped first and the parameter V_MAX should be set to zero to hold the assigned stepper motor at rest 
	  before writing into the register X_TARGET and X_ACTUAL.	
	*/
	delay_ms(50);
	Write429Short(IDX_VMAX|(axisNum<<5), 0);	
	delay_ms(200);
	Write429Int(IDX_XTARGET|(axisNum<<5), 0);
	Write429Int(IDX_XACTUAL|(axisNum<<5), 0);
	
	homeInfo.Homed[axisNum]=TRUE;
	homeInfo.GoHome[axisNum]=FALSE;	
	homeInfo.GoLimit[axisNum]=FALSE;	
	CMD_TRACE("motor[%d] go home ok\n",axisNum);
	if(motorsReset_InOrder==TRUE)	
	{
		switch(g_tParam.Project_ID)
		{
			case BUTTON_ONLINE:
			case BUTTON_OFFLINE:
					if(axisNum==AXIS_X) //Y轴先里后出复位OK
					{
						//Stop_HardTimer();
						motorsReset_InOrder=OK_2;
						buttonRESETpressed=FALSE;
						CMD_TRACE("motor auto reset ok\n");
					}
					if(axisNum==AXIS_Y) //Y轴先里后出复位OK
					{
						homeInfo.GoHome[AXIS_Z]=TRUE;	
						TMC429_MotorRotate(AXIS_Z,-homeInfo.HomeSpeed[AXIS_Z]);	
						
						Start_HardTimer();	
					}
					if(axisNum==AXIS_Z) //Z轴先上后下复位OK
					{
						homeInfo.GoHome[AXIS_X]=TRUE;	
						TMC429_MotorRotate(AXIS_X,homeInfo.HomeSpeed[AXIS_X]);
						
						Start_HardTimer();	
					}			 
				break;
			case BUTTON_ROAD:
					if(axisNum==AXIS_X) 
					{
						//Stop_HardTimer();
						motorsReset_InOrder=OK_2;
						buttonRESETpressed=FALSE;
						setChOutput(2,0);	
						CMD_TRACE("motor auto reset ok\n");
					}
					if(axisNum==AXIS_Z) //Z轴先上后下复位OK
					{
						 homeInfo.GoHome[AXIS_X] =TRUE;
						 homeInfo.GoLimit[AXIS_X]=FALSE;
						 homeInfo.Homed[AXIS_X]  =FALSE;
						 homeInfo.HomeSpeed[AXIS_X]=g_tParam.speed_home[AXIS_X];

						 TMC429_MotorRotate(AXIS_X, g_tParam.speed_home[AXIS_X]);				//逆时针旋转		L		Z轴先复位		Z轴先上后下	
						 CMD_TRACE("motor[%d] is start go home by searching home sensor\n",AXIS_X);
						 Start_HardTimer();						
					}	
				break;
					
			case OQC_FLEX:
					if(axisNum==AXIS_Z) //Z轴先上后下复位OK
					{
//						Stop_HardTimer();
						motorsReset_InOrder=OK_2;
						buttonRESETpressed=FALSE;
						setChOutput(2,0);	
						CMD_TRACE("motor auto reset ok\n");
					}							
				break;
			default:
				break;
					
		}
	}
	else if(homeInfo.Homed[axisNum]==TRUE )
		Stop_HardTimer();
	
}
//
//if(buttonRESETpressed==TRUE)	MotorHomingWithHomeSensor(0,100);
void MotorHomingWithHomeSensor1(uint8_t axisNum, int HomeSpeed)
{	
	if(homeInfo.GoHome[axisNum])
	{
		uint8_t SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);
		if(g_tParam.Project_ID==BUTTON_ROAD && axisNum==0)
		{
			if( ((HomeSpeed>0 && (Read429Short(IDX_VACTUAL|(0<<5)))<0 ) || \
				(HomeSpeed<0 && (Read429Short(IDX_VACTUAL|(0<<5)))>0 )) && \
				 (pinRead(homeSensorPin[axisNum])==IN_OFF ))
		     { setPositionAsOrigin(0);		}																   //只要采集不到原点信号,电机在两个限位之间来回运动,触发硬件限位自动停止	
		}
			
		if( ((HomeSpeed>0 && (Read429Short(IDX_VACTUAL|(axisNum<<5)))<0 ) || \
				(HomeSpeed<0 && (Read429Short(IDX_VACTUAL|(axisNum<<5)))>0 )) && \
				 (pinRead(homeSensorPin[axisNum])==IN_ON ))
		{ setPositionAsOrigin(axisNum);		}																   //只要采集不到原点信号,电机在两个限位之间来回运动,触发硬件限位自动停止
		else 
		{
			uint8_t SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);
			if((SwitchStatus& (0x02<<axisNum*2)) ? 1:0)
			{		
				TMC429_MotorRotate(axisNum,HomeSpeed);
			}
			if((SwitchStatus& (0x01<<axisNum*2)) ? 1:0)
			{																													  //REFR1  顺时针触发右限位时反转
				TMC429_MotorRotate(axisNum,-HomeSpeed);
			}
		}
	}
}
//正速度为正限位为原点
void MotorHomingWithLimitSwitch(uint8_t axisNum, int HomeSpeed)
{	
	if(homeInfo.GoLimit[axisNum])
	{
		uint8_t SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);																								
		if(((SwitchStatus& (0x02<<axisNum*2)) ? 1:0) && (HomeSpeed<0)) 								
		{																													 
			{
				setPositionAsOrigin(axisNum);							
			}
		}
		if(((SwitchStatus& (0x01<<axisNum*2)) ? 1:0)		&& (HomeSpeed>0)) 
		{																													  //REFR1  顺时针触发右限位时反转				
			{
				setPositionAsOrigin(axisNum);				
			}
		}
	}
}
//
/*
滑台传感器：内置   原点和限位传感器触发后 V=H，未触发时V=L
road和FLEx  原点触发式V=L，限位未触发时V=L

BUG:  原点一直触发的时候，再触发限位不会进行反向运动
*/

void MotorHomingWithHomeSensor(uint8_t axisNum, int HomeSpeed)
{	
	static u8 ror_L_lock=0;
	static u8 ror_R_lock=0;
	if(homeInfo.GoHome[axisNum])
	{
		if(Read429Short(IDX_VACTUAL|(axisNum<<5))==0)
		{
			u8 SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);
			if((SwitchStatus& (0x02<<axisNum*2)) ? 1:0)											  //触发左限位
			{		
				//delay_ms(100);//定时器里面使用delay?
				//if(ror_R_lock==0)
				{
					if(HomeSpeed>0)	TMC429_MotorRotate(axisNum,HomeSpeed);				  //向右转
					else 						TMC429_MotorRotate(axisNum,-HomeSpeed);
					ror_R_lock=1;
					ror_L_lock=0;
				}		
			}
			if((SwitchStatus& (0x01<<axisNum*2)) ? 1:0)												//触发右限位
			{																													 		
				//delay_ms(100);
				//if(ror_L_lock==0)
				{
					if(HomeSpeed>0)	TMC429_MotorRotate(axisNum,-HomeSpeed);					//左转
					else 						TMC429_MotorRotate(axisNum,HomeSpeed);
					ror_L_lock=1;
					ror_R_lock=0;
				}
			}
		}
		//road 的Z轴使用外置原点	X轴使用内置原点
    else if((g_tParam.Project_ID==BUTTON_ROAD && ((axisNum==0 && pinRead(homeSensorPin[axisNum])==IN_OFF) || \
			
																					 (axisNum!=0 && pinRead(homeSensorPin[axisNum])==IN_ON ))) ||	\
			 (g_tParam.Project_ID!=BUTTON_ROAD && pinRead(homeSensorPin[axisNum])==IN_ON ))		//外置原点
		
		{
			if((HomeSpeed>0 && (Read429Short(IDX_VACTUAL|(axisNum<<5)))<0 ) || (HomeSpeed<0 && (Read429Short(IDX_VACTUAL|(axisNum<<5)))>0 ))		 
			{				
				setPositionAsOrigin(axisNum);		
			}
		}

	}
}
//




