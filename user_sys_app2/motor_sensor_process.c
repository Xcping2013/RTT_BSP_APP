/***********************************************************************/
#include "app_sys_control.h"
/***********************************************************************/
homeInfo_t homeInfo;

uint8_t motorsResetInOrder=FALSE;

/*********************************************************************************************************************/
static void setPositionAsOrigin(uint8_t axisNum);

/*********************************************************************************************************************/
static void setPositionAsOrigin(u8 axisNum) 
{
	stop_timer_5ms();
	/* 
	Before overwriting X_ACTUAL choose velocity_mode or hold_mode.If X_ACTUAL is overwritten in ramp_mode or soft_mode the motor directly drives to X_TARGET
	*/	
	HardStop(axisNum);
	rt_thread_delay(100);
	Write429Int(IDX_XTARGET|(axisNum<<5), 0);
	Write429Int(IDX_XACTUAL|(axisNum<<5), 0);
	
	homeInfo.Homed[axisNum]=TRUE;
	homeInfo.GoHome[axisNum]=FALSE;	
	homeInfo.GoLimit[axisNum]=FALSE;	
	
	rt_kprintf("motor[%d] go home ok\n",axisNum);
	
	if(motorsResetInOrder==TRUE)	
	{
		switch(g_tParam.Project_ID)
		{
			case BUTTON_ONLINE:
			case BUTTON_OFFLINE:
					if(axisNum==AXIS_X) //YÖáÏÈÀïºó³ö¸´Î»OK
					{
						//stop_timer_5ms();
						motorsResetInOrder=OK_2;
						buttonRESETpressed=FALSE;
						rt_kprintf("motor auto reset ok\n");
					}
					if(axisNum==AXIS_Y) //YÖáÏÈÀïºó³ö¸´Î»OK
					{
						homeInfo.GoHome[AXIS_Z]=TRUE;	
						TMC429_MotorRotate(AXIS_Z,-homeInfo.HomeSpeed[AXIS_Z]);	
						
						start_timer_5ms();	
					}
					if(axisNum==AXIS_Z) //ZÖáÏÈÉÏºóÏÂ¸´Î»OK
					{
						homeInfo.GoHome[AXIS_X]=TRUE;	
						TMC429_MotorRotate(AXIS_X,homeInfo.HomeSpeed[AXIS_X]);
						
						start_timer_5ms();	
					}			 
				break;
			case BUTTON_ROAD:
					if(axisNum==AXIS_X) 
					{
						//stop_timer_5ms();
						motorsResetInOrder=OK_2;
						buttonRESETpressed=FALSE;
						setChOutput(2,0);	
						rt_kprintf("motor auto reset ok\n");
					}
					if(axisNum==AXIS_Z) //ZÖáÏÈÉÏºóÏÂ¸´Î»OK
					{
						 homeInfo.GoHome[AXIS_X] =TRUE;
						 homeInfo.GoLimit[AXIS_X]=FALSE;
						 homeInfo.Homed[AXIS_X]  =FALSE;
						 homeInfo.HomeSpeed[AXIS_X]=g_tParam.speed_home[AXIS_X];

						 TMC429_MotorRotate(AXIS_X, g_tParam.speed_home[AXIS_X]);				//ÄæÊ±ÕëĞı×ª		L		ZÖáÏÈ¸´Î»		ZÖáÏÈÉÏºóÏÂ	
						 rt_kprintf("motor[%d] is start go home by searching home sensor\n",AXIS_X);
						 start_timer_5ms();						
					}	
				break;
					
			case OQC_FLEX:
					if(axisNum==AXIS_Z) //ZÖáÏÈÉÏºóÏÂ¸´Î»OK
					{
//						stop_timer_5ms();
						motorsResetInOrder=OK_2;
						buttonRESETpressed=FALSE;
						setChOutput(2,0);	
						rt_kprintf("motor auto reset ok\n");
					}							
				break;
			default:
				break;
					
		}
	}
	else if(homeInfo.Homed[axisNum]==TRUE )
		stop_timer_5ms();
	
}

void MotorHomingWithLimitSwitch(uint8_t axisNum, int HomeSpeed)
{	
	//ÕıËÙ¶ÈÎªÕıÏŞÎ»ÎªÔ­µã
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
		{																													  //REFR1  Ë³Ê±Õë´¥·¢ÓÒÏŞÎ»Ê±·´×ª				
			{
				setPositionAsOrigin(axisNum);				
			}
		}
	}
}
/*
»¬Ì¨´«¸ĞÆ÷£ºÄÚÖÃ   Ô­µãºÍÏŞÎ»´«¸ĞÆ÷´¥·¢ºó V=H£¬Î´´¥·¢Ê±V=L
roadºÍFLEx  Ô­µã´¥·¢Ê½V=L£¬ÏŞÎ»Î´´¥·¢Ê±V=L

BUG:  Ô­µãÒ»Ö±´¥·¢µÄÊ±ºò£¬ÔÙ´¥·¢ÏŞÎ»²»»á½øĞĞ·´ÏòÔË¶¯
*/

void MotorHomingWithHomeSensor(uint8_t axisNum, int HomeSpeed)
{	
	if(homeInfo.GoHome[axisNum])
	{
		static uint8_t motorGetLimitCNT[3];
		//»ØÔ­µã¹ı³ÌÖĞËÙ¶È=0,Ôò´¥·¢Ô­µã£¬µç»ú»áÍù·½ÏòÔËĞĞ
		//Èç¹û»ØÔ­µã¹ı³ÌÏìÓ¦µç»úÍ£Ö¹ÃüÁî£¬Ôò¶¯×÷Îª£ºµç»úÏòÉÏ»ØÔ­µã£¬µç»úÍ£Ö¹ÃüÁî£¬µç»úÏòÏÂÕÒ¸ºÏŞÎ»£¬ÔÙ·¢Í£Ö¹ÃüÁî£¬µç»úÓÖ·´ÏòÔË¶¯
		//ËùÒÔµç»ú»ØÔ­µãÃüÁîºó²»ÏìÓ¦ÆäËû¶¯×÷ÃüÁî£¬»òÕßÏìÓ¦ÆäËûÃüÁîÊ±¹Ø±Õ»ØÔ­µã¿ª¹Ø
		if(Read429Short(IDX_VACTUAL|(axisNum<<5))==0)
		{
			if(motorGetLimitCNT[axisNum]++>=3)
			{
				u8 SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);
				if((SwitchStatus& (0x02<<axisNum*2)) ? 1:0)											 	  //´¥·¢×óÏŞÎ»
				{		

					if(HomeSpeed>0)	TMC429_MotorRotate(axisNum,HomeSpeed);				  //ÏòÓÒ×ª
					else 						TMC429_MotorRotate(axisNum,-HomeSpeed);
				}
				if((SwitchStatus& (0x01<<axisNum*2)) ? 1:0)												//´¥·¢ÓÒÏŞÎ»
				{																													 		
					if(HomeSpeed>0)	TMC429_MotorRotate(axisNum,-HomeSpeed);					//×ó×ª
					else 						TMC429_MotorRotate(axisNum,HomeSpeed);
				}
				motorGetLimitCNT[axisNum]=0;
			}
		}
	  //µç»ú»¹ÔÚÔËĞĞÖĞ´¥·¢Ô­µã
		//road µÄZÖáÊ¹ÓÃÍâÖÃÔ­µã	XÖáÊ¹ÓÃÄÚÖÃÔ­µã
    else if((g_tParam.Project_ID==BUTTON_ROAD && ((axisNum==0 && pinRead(OriginSensorPin[axisNum])==IN_OFF) || \
			
																					 (axisNum!=0 && pinRead(OriginSensorPin[axisNum])==IN_ON ))) ||	\
			 (g_tParam.Project_ID!=BUTTON_ROAD && pinRead(OriginSensorPin[axisNum])==IN_ON ))		//ÍâÖÃÔ­µã
		
		{
			if((HomeSpeed>0 && (Read429Short(IDX_VACTUAL|(axisNum<<5)))<0 ) || (HomeSpeed<0 && (Read429Short(IDX_VACTUAL|(axisNum<<5)))>0 ))		 
			{				
				setPositionAsOrigin(axisNum);		
			}
		}
	}
}
void MOTOR_ResetPreset( void )
{	
	setChOutput(2,1);	 setChOutput(1,0);	
	
	motorsResetInOrder=TRUE;
	
	for(uint8_t i=0;i<3;i++)
	{
		StopMotorByRamp(i);
		homeInfo.Homed[i]=FALSE;
		homeInfo.GoHome[i]=FALSE;
		homeInfo.GoLimit[i]=FALSE;
	}	
	delay_ms(20);	
	switch(g_tParam.Project_ID)
	{
		case BUTTON_OFFLINE:				
		case BUTTON_ONLINE:
				 homeInfo.GoHome[AXIS_Y]=TRUE;
				 TMC429_MotorRotate(AXIS_Y,-homeInfo.HomeSpeed[AXIS_Y]);			//ÄæÊ±ÕëĞı×ª		L		YÖáÏÈ¸´Î»		YÖáÏÈºóºóÇ°	
  
				break;
				 
		case OQC_FLEX:
    case BUTTON_ROAD:
			
				 homeInfo.GoHome[AXIS_Z]=TRUE;
				 homeInfo.GoLimit[AXIS_Z]=FALSE;
				 homeInfo.Homed[AXIS_Z]=FALSE;
				 homeInfo.HomeSpeed[AXIS_Z]=-g_tParam.speed_home[AXIS_Z];
		
		     TMC429_MotorRotate(AXIS_Z, -g_tParam.speed_home[AXIS_Z]);			//ÄæÊ±ÕëĞı×ª		L		ZÖáÏÈ¸´Î»		ZÖáÏÈÉÏºóÏÂ
	
				 rt_kprintf("motor[%d] is start go home by searching home sensor\n",AXIS_Z);
		
				 start_timer_5ms();
		
		break;	
		
		default:
			break;
	}
	buttonRESETpressed=TRUE;	
}
//
#defined MOTOR_RESET_THREAD_PRIORITY	18

static rt_uint8_t MOTOR_ResetStack[ 512 ];
static struct 		rt_thread MOTOR_ResetThread;

/***************************************************************************/
static void MOTOR_ResetThread_entry(void *parameter);
/***************************************************************************/
static void MOTOR_ResetThread_entry(void *parameter)
{
	  static uint8_t motorStayLimitCNT=0;
	
    while (1)
    {  
			
			if(motorResetFlag == TRUE)
			{			
				MOTOR_ResetInSequence(g_tParam.Project_ID);				
			}
			
			if(homeInfo.GoHome[0] || homeInfo.GoHome[1] || homeInfo.GoHome[2] )
			{			
				for(uint8_t i=0;i<N_O_MOTORS;i++)
				{			
					MotorHomingWithHomeSensor(i,homeInfo.HomeSpeed[i]);
				}	
				rt_thread_delay(2);				
			}
				
			else if( homeInfo.GoLimit[0] || homeInfo.GoLimit[1] || homeInfo.GoLimit[2] )
			{
				for(uint8_t i=0;i<N_O_MOTORS;i++)
				{			
					MotorHomingWithLimitSwitch(i,homeInfo.HomeSpeed[i]);
				}	
				rt_thread_delay(2);
			}
			else rt_thread_delay(100);
			
		}		
}
static int MOTOR_Reset_thread_init(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&MOTOR_ResetThread,
                            "MOTOR_Reset",
                            MOTOR_ResetThread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&MOTOR_ResetStack[0],
                            sizeof(MOTOR_ResetStack),
                            25,
                            20);
    if (result == RT_EOK)
    {
       rt_thread_startup(&MOTOR_ResetThread);
    }
    return 0;
}

/*******************************************************************************************************************/


/*********************************************************************************************************************/
#include "app_motor_home.h"
/******************************************Origin*********************************************************************

default OriginSensor --> IN GPIO

»ØÔ­µã¶¯×÷ÖĞ,ĞèÒª¼°Ê±²É¼¯Ô­µãĞÅºÅ,½øĞĞµç»úÍ£Ö¹²¢ÉèÁ¢µ±Ç°Î»ÖÃÎª0,Õâ¸ö¶¯×÷Èç¹ûdelayÌ«¾Ã»áÓ°ÏìÆäËûÖáµÄ»Ø0¶¯×÷£¡£¡£¡

SO: 
-->ÉèÖÃµ¥Öá»ØÔ­µã-->ÉèÖÃÔ­µã´«¸ĞÆ÷ÎªÍâ²¿ÖĞ¶Ï£¨´¥·¢·½Ê½´®¿Ú´«¸ĞÆ÷ĞÍºÅ)-->²É¼¯µ½ĞÅºÅ£¬ÅĞ¶Ï»ØÁã·½ÏòÕıÈ·µÄ»°£¬ÉèÁ¢µ±Ç°Î»ÖÃÎª0£¬²¢¹Ø±Õ´ËÍâ²¿ÖĞ¶Ï¡£(¼Óµ÷ÊÔLOG)
-->»Ø0¶¯×÷ÖĞ¾¡Á¿²»ÒªÊ¹ÓÃ´®¿Ú²É¼¯ĞÅÏ¢£¬Íâ²¿ÖĞ¶ÏÓÅÏÈ¼¶´óÓÚ´®¿Ú£¬´®¿ÚÊı¾İ»á³öÏÖÒì³£

¶àÖá×Ô¶¯»ØÔ­µãÔÙµ¥Öá»ØÔ­µã»ù´¡ÉÏ½øĞ ĞhomeInfo.GoHomeºÍhomeInfo.Homed ½øĞĞ¹ÜÀí

*********************************************************************************************************************/
uint8_t OriginSensorPin[3]={	PB_9,	PE_0,	PE_1};
uint8_t OriginSensorON[3]= {	HIGH,	LOW,	LOW};

static void MOTOR_ResetInSequence(uint8_t actualProID)
{
	switch(actualProID)
	{
		//ZÖá×ó×ªÍùÉÏÕÒ¸ºÏŞÎ»,ÔÙÕı×ªÕÒÔ­µã
		case OQC_FLEX:
		case BUTTON_ROAD:
				 //Ğı×ªÔË¶¯
				 TMC429_MotorRotate(AXIS_Z, -g_tParam.speed_home[AXIS_Z]);
				 //Ô­µãÖĞ¶ÏÊ¹ÄÜ
				 OriginSensorIRQEnable(AXIS_Z);
			break;
		
		default:
			break;
}

static void OriginSensorIRQCall(void *args)
{
	rt_uint32_t actualMotor;
	actualMotor = (rt_uint32_t)args;
	
	if(actualMotor<3)
	{
		//´¥·¢Ô­µãĞÅºÅ
		if(OriginSensorON[actualMotor]==HIGH && pinRead(OriginSensorPin[actualMotor])==HIGH ||
			 OriginSensorON[actualMotor]==LOW && pinRead(OriginSensorPin[actualMotor])==LOW			 )
		{
			
			if((homeInfo.HomeSpeed[actualMotor] >0 && (Read429Short(IDX_VACTUAL|(actualMotor<<5)))<0 ) || 
				 (homeInfo.HomeSpeed[actualMotor] <0 && (Read429Short(IDX_VACTUAL|(actualMotor<<5)))>0 )    )
			{	
				//´¥·¢Ô­µãµÄ×´Ì¬Âú×ã»ØÔ­·½Ê½,Í£Ö¹µç»ú°Ñµ±Ç°Î»ÖÃ¼Ä´æÆ÷Ğ´0,²¢¹Ø±ÕÔ­µãÖĞ¶Ï¹¦ÄÜ
				HardStop(actualMotor); delay_ms(20);
				Set429RampMode(MOTOR_NUMBER(actualMotor), RM_VELOCITY);
				Write429Int(IDX_XTARGET|(actualMotor<<5), 0);
				Write429Int(IDX_XACTUAL|(actualMotor<<5), 0);
		
				OriginSensorIRQDisable(actualMotor);
				homeInfo.Homed[actualMotor]=TRUE;
				
				rt_kprintf("motor[%d] go home ok\n",actualMotor);
			}
		}	
	}
}
	
	if(INPUT8==0 && buttonRESETpressed==0 && homeInfo.GoHome[2]==FALSE)	 	
	{			
		closeSerial();

		HardStop(2);//HardStop(1);HardStop(0);

		homeInfo.Homed[2]=FALSE;
		homeInfo.GoHome[2]=FALSE;	
		homeInfo.GoLimit[2]=FALSE;	

		rt_kprintf("stop motor z due to pressure overhigh!!!\n");	
	} 
}
static void OriginSensorIRQEnable(uint8_t actualMotor)
{
	rt_pin_irq_enable(OriginSensorPin[actualMotor], PIN_IRQ_ENABLE);
}
static void OriginSensorIRQDisable(uint8_t actualMotor)
{
	rt_pin_irq_enable(OriginSensorPin[actualMotor], PIN_IRQ_DISABLE);
}
static void InitOriginSensorAsEXTI(void)
{
	uint8_t i;
	for(i=0l;i<3;i++)
	{
		if(OriginSensorON[i]==HIGH)
			rt_pin_attach_irq(OriginSensorPin[i], PIN_IRQ_MODE_RISING,  press_overhigh_check, (void*)i);	
		else
			rt_pin_attach_irq(OriginSensorPin[i], PIN_IRQ_MODE_FALLING, press_overhigh_check, (void*)i);	
	}
	rt_kprintf("InitOriginSensorAsEXTI()...ok\n");
}





void OriginSensorProcess(void)
{
	

	
}
/******************************************Limit*********************************************************************

»ØÔ­µã¶¯×÷ÖĞ»á´¥·¢ÏŞÎ»ĞÅºÅ£¬¶Ô¼ì²âÊ±¼äÒªÇó²»¸ß£¬¿É¼ÆÊı¼ì²âµ½50msºó½øĞĞ·´×ªÔÙÑ°Ô­µãĞÅºÅ

*********************************************************************************************************************/

void LimitSensorProcess(void)
{
	
}
//

