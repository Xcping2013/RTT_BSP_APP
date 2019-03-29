//#include "app_motor_home.h"
//#include "app_sys_control.h"

#include "bsp_include.h"	
#include "app_include.h"

homeInfo_t homeInfo;
//////////////////////////////////////////////////////////
void setPositionAsOrigin(u8 axisNum) 
{
	/* 
	Before overwriting X_ACTUAL choose velocity_mode or hold_mode.If X_ACTUAL is overwritten in ramp_mode or soft_mode the motor directly drives to X_TARGET
	*/
	HardStop(axisNum);
	/*
	  To set target position X_TARGET and current position X_ACTUAL to an equivalent value (e.g. to set both to zero at a reference point)
  	the assigned stepper motor should be stopped first and the parameter V_MAX should be set to zero to hold the assigned stepper motor at rest 
	  before writing into the register X_TARGET and X_ACTUAL.	
	*/
	delay_ms(50);
	Write429Short(IDX_VMAX|(axisNum<<5), 0);	
	delay_ms(350);
	Write429Int(IDX_XTARGET|(axisNum<<5), 0);
	Write429Int(IDX_XACTUAL|(axisNum<<5), 0);
	
	homeInfo.Homed[axisNum]=TRUE;
	homeInfo.GoHome[axisNum]=FALSE;	
	homeInfo.GoLimit[axisNum]=FALSE;	
	CMD_TRACE("motor[%d] go home ok\n",axisNum);
	if(autoRESETmotor==TRUE)	
	{
		switch(g_tParam.Project_ID)
		{
			case BUTTON_ONLINE:
			case BUTTON_OFFLINE:
					if(axisNum==AXIS_X) //Y����������λOK
					{
						timer_stop();
						autoRESETmotor=OK_2;
						buttonRESETpressed=FALSE;
						CMD_TRACE("motor auto reset ok\n");
					}
					if(axisNum==AXIS_Y) //Y����������λOK
					{
						homeInfo.GoHome[AXIS_Z]=TRUE;	
						RotateLeft(AXIS_Z,homeInfo.HomeSpeed[AXIS_Z]);	
					}
					if(axisNum==AXIS_Z) //Z�����Ϻ��¸�λOK
					{
						homeInfo.GoHome[AXIS_X]=TRUE;	
						RotateRight(AXIS_X,homeInfo.HomeSpeed[AXIS_X]);	
					}			 
				break;
			case OQC_FLEX:
					if(axisNum==AXIS_Z) //Z�����Ϻ��¸�λOK
					{
						timer_stop();
						autoRESETmotor=OK_2;
						buttonRESETpressed=FALSE;
						setChOutput(2,0);	
						CMD_TRACE("motor auto reset ok\n");
					}			 
				break;
					
		}
	}
	else if(homeInfo.Homed[axisNum]==TRUE )
		timer_stop();
	
}
//
//if(buttonRESETpressed==TRUE)	MotorHomingWithHomeSensor(0,100);
void MotorHomingWithHomeSensor(uint8_t axisNum, int HomeSpeed)
{	
	if(homeInfo.GoHome[axisNum])
	{
		uint8_t SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);
		if( ((HomeSpeed>0 && (Read429Short(IDX_VACTUAL|(axisNum<<5)))<0 ) || \
				(HomeSpeed<0 && (Read429Short(IDX_VACTUAL|(axisNum<<5)))>0 )) && \
				 (pinRead(homeSensorPin[axisNum])==IN_ON ))
		{ setPositionAsOrigin(axisNum);		}																														  //ֻҪ�ɼ�����ԭ���ź�,�����������λ֮�������˶�,����Ӳ����λ�Զ�ֹͣ
		else 
		{
			uint8_t SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);
			if((SwitchStatus& (0x02<<axisNum*2)) ? 1:0)
			{		
				if(HomeSpeed>0) 
					RotateRight(axisNum,HomeSpeed);								 							  //˳ʱ�� ��ת R
				else RotateRight(axisNum,-HomeSpeed);
			}
			if((SwitchStatus& (0x01<<axisNum*2)) ? 1:0)
			{																													  //REFR1  ˳ʱ�봥������λʱ��ת
				if(HomeSpeed>0) 
				{
					RotateLeft(axisNum,HomeSpeed);
				}
				else RotateLeft(axisNum,-HomeSpeed);
			}
		}
	}
}
//���ٶ�Ϊ����λΪԭ��
void MotorHomingWithLimitSwitch(uint8_t axisNum, int HomeSpeed)
{	
	if(homeInfo.GoLimit[axisNum])
	{
		uint8_t SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);																								
		if(((SwitchStatus& (0x02<<axisNum*2)) ? 1:0) && (HomeSpeed<0)) 								
		{																													 
			//if(HomeSpeed>0) 
				//RotateRight(axisNum,HomeSpeed);								 							  //˳ʱ�� ��ת R
			//else 
			{
				setPositionAsOrigin(axisNum);							
			}
		}
		if(((SwitchStatus& (0x01<<axisNum*2)) ? 1:0)		&& (HomeSpeed>0)) 
		{																													  //REFR1  ˳ʱ�봥������λʱ��ת				
//			if(HomeSpeed>0) 
			{
				setPositionAsOrigin(axisNum);				
			}
//			else 
//			{
//				//RotateRight(axisNum,HomeSpeed);		
//			}	
		}
	}
}
//








