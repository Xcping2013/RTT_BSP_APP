//#include "app_motor_home.h"
//#include "app_sys_control.h"

#include "bsp_include.h"	
#include "app_include.h"

homeInfo_t homeInfo;

/*********************************************************************************************************************/
static void setPositionAsOrigin(uint8_t axisNum);

/*********************************************************************************************************************/
/*
1.�ر����ԭ�㹦��, ���õ�ǰ���ԭ��OK
2.����Ŀ�Ž��п����¸����ԭ�㹦�ܲ�������ز���

*/
static void setPositionAsOrigin(u8 axisNum) 
{
	/* 
	Before overwriting X_ACTUAL choose velocity_mode or hold_mode.If X_ACTUAL is overwritten in ramp_mode or soft_mode the motor directly drives to X_TARGET
	*/
	//HardStop(axisNum);
	Stop_HardTimer();
	
	HardStop(axisNum);
	//StopMotorByRamp(axisNum);
	/*
	  To set target position X_TARGET and current position X_ACTUAL to an equivalent value (e.g. to set both to zero at a reference point)
  	the assigned stepper motor should be stopped first and the parameter V_MAX should be set to zero to hold the assigned stepper motor at rest 
	  before writing into the register X_TARGET and X_ACTUAL.	
	*/
	delay_ms(100);
	//��ת�˶�û��Ҫ������VMAX
	//Write429Short(IDX_VMAX|(axisNum<<5), 0);	
//	delay_ms(200);
	Set429RampMode(MOTOR_NUMBER(axisNum), RM_VELOCITY);
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
					if(axisNum==AXIS_X) //Y����������λOK
					{
						//Stop_HardTimer();
						motorsReset_InOrder=OK_2;
						buttonRESETpressed=FALSE;
						CMD_TRACE("motor auto reset ok\n");
					}
					if(axisNum==AXIS_Y) //Y����������λOK
					{
						homeInfo.GoHome[AXIS_Z]=TRUE;	
						TMC429_MotorRotate(AXIS_Z,-homeInfo.HomeSpeed[AXIS_Z]);	
						
						Start_HardTimer();	
					}
					if(axisNum==AXIS_Z) //Z�����Ϻ��¸�λOK
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
					if(axisNum==AXIS_Z) //Z�����Ϻ��¸�λOK
					{
						 homeInfo.GoHome[AXIS_X] =TRUE;
						 homeInfo.GoLimit[AXIS_X]=FALSE;
						 homeInfo.Homed[AXIS_X]  =FALSE;
						 homeInfo.HomeSpeed[AXIS_X]=g_tParam.speed_home[AXIS_X];

						 TMC429_MotorRotate(AXIS_X, g_tParam.speed_home[AXIS_X]);				//��ʱ����ת		L		Z���ȸ�λ		Z�����Ϻ���	
						 CMD_TRACE("motor[%d] is start go home by searching home sensor\n",AXIS_X);
						 Start_HardTimer();						
					}	
				break;
					
			case OQC_FLEX:
					if(axisNum==AXIS_Z) //Z�����Ϻ��¸�λOK
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
//���ٶ�Ϊ����λΪԭ��
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
		{																													  //REFR1  ˳ʱ�봥������λʱ��ת				
			{
				setPositionAsOrigin(axisNum);				
			}
		}
	}
}
//
/*
��̨������������   ԭ�����λ������������ V=H��δ����ʱV=L
road��FLEx  ԭ�㴥��ʽV=L����λδ����ʱV=L

BUG:  ԭ��һֱ������ʱ���ٴ�����λ������з����˶�
*/

void MotorHomingWithHomeSensor(uint8_t axisNum, int HomeSpeed)
{	
	if(homeInfo.GoHome[axisNum])
	{
		static uint8_t motorlimitedCNT[3]={0,0,0};
		//��ԭ��������ٶ�=0,���ж�Ϊ������λ�����������������
		//�����ԭ�������Ӧ���ֹͣ�������Ϊ��������ϻ�ԭ�㣬���ֹͣ�����������Ҹ���λ���ٷ�ֹͣ�������ַ����˶�
		//���Ե����ԭ���������Ӧ�����������������Ӧ��������ʱ�رջ�ԭ�㿪��
		if(Read429Short(IDX_VACTUAL|(axisNum<<5))==0)													
		{
			motorlimitedCNT[axisNum]++;
			if(motorlimitedCNT[axisNum]>=3)
			{
				u8 SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);
				if((SwitchStatus& (0x02<<axisNum*2)) ? 1:0)											 	//��������λ
				{		
						if(HomeSpeed>0)	TMC429_MotorRotate(axisNum,HomeSpeed/4);			//����ת
						else 						TMC429_MotorRotate(axisNum,-HomeSpeed/4);		
				}
				if((SwitchStatus& (0x01<<axisNum*2)) ? 1:0)												//��������λ
				{																													 		
						if(HomeSpeed>0)	TMC429_MotorRotate(axisNum,-HomeSpeed);				//��ת
						else 						TMC429_MotorRotate(axisNum,HomeSpeed);
				}
				motorlimitedCNT[axisNum]=0;
			}
		}
	  //������������д���ԭ��
		//road ��Z��ʹ������ԭ��	X��ʹ������ԭ��
    else if((g_tParam.Project_ID==BUTTON_ROAD && ((axisNum==0 && pinRead(homeSensorPin[axisNum])==IN_OFF) || \
			
																					 (axisNum!=0 && pinRead(homeSensorPin[axisNum])==IN_ON ))) ||	\
			 (g_tParam.Project_ID!=BUTTON_ROAD && pinRead(homeSensorPin[axisNum])==IN_ON ))		//����ԭ��
		
		{
			if((HomeSpeed>0 && (Read429Short(IDX_VACTUAL|(axisNum<<5)))<0 ) || (HomeSpeed<0 && (Read429Short(IDX_VACTUAL|(axisNum<<5)))>0 ))		 
			{				
				setPositionAsOrigin(axisNum);		
			}
		}
	}
}
//




