//#include "app_motion.h"
//#include "app_sys_control.h"

#include "bsp_include.h"	
#include "app_include.h"

/*  TMCL Instruction
1-ROR rotate right
2-ROL rotate left
3-MST Motor  stop
4-MVP move to position
5-SAP set axis parameter
6-GAP get axis parameter
7-STAP store axis parameter
8-RSAP restore axis parameter
9-SGP set global parameter
10-SGP get global parameter
11-STGP store global parameter
12-RSGP restore global parameter
13-REF reference search
14-SIO set output
15-GIO get input

*/



enum axisParameter {

next_position, 
actual_position, 
	
next_speed, 				//int
actual_speed,				//int 
max_v_positioning, 	//int
max_acc, 						//int

position_reached,
ref_SwStatus,				//=leftLimit_SwS
rightLimit_SwS,
leftLimit_SwS,
rightLimit_disable,	
leftLimit_disable,

max_current,
standby_current,
	
min_speed,					//int
actual_acc,	
ramp_mode,

ref_sw_tolerance,
softStop_flag,

ramp_divisor,
pulse_divisor,	
limitSignal,
home_SenS,
is_homed,
is_stop,
is_reset,

Type_none,

};

//
TTMCLCommand ActualCommand;
TTMCLReply ActualReply;

static UCHAR SpeedChangedFlag[N_O_MOTORS];
static uint8_t MotorStopByLimit[3]={1,1,1};														//碰到限位处理一次
		

static void RampInit(void);
//static uint8_t TMCL_MotorRotate(void);
static uint8_t TMCL_MotorStop(void);
static uint8_t TMCL_MoveToPosition(void);
static void TMCL_GetAxisParameter(void);
static void TMCL_SetAxisParameter(void);
static void MotorLimitProcess(uint8_t axisNum);


static void RampInit(void)
{
  uint8_t i;
	for(i=0; i<N_O_MOTORS; i++) 					
	{																  
		MotorConfig[i].VMax=g_tParam.tmc429_VMax[i];   				
		MotorConfig[i].AMax=g_tParam.tmc429_AMax[i];
		MotorConfig[i].PulseDiv=g_tParam.tmc429_PulseDiv[i];	
		MotorConfig[i].RampDiv=g_tParam.tmc429_RampDiv[i];	
		
		SpeedChangedFlag[i]=TRUE;
		homeInfo.Homed[i]=FALSE;
		homeInfo.GoHome[i]=FALSE;
		homeInfo.GoLimit[i]=FALSE;
		homeInfo.HomeSpeed[i]=g_tParam.speed_home[i];
		
		pinMode(homeSensorPin[i], PIN_MODE_INPUT_PULLUP);
	}
}

void  tmc429_hw_init(void)
{
	pinMode(POSCMP1_PIN, PIN_MODE_INPUT_PULLUP);
  pinMode(INTOUT1_PIN, PIN_MODE_INPUT_PULLUP);	
	
	MX_SPI_Init();
#if defined(USING_INC_MBTMC429) 
	MX_TIM3_Init();	
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
#endif
	RampInit();
	Init429();
	for(uint8_t i=0;i<3;i++)
	{
		HardStop(i);
		delay_ms(50);
		Write429Short(IDX_VMAX|(i<<5), 0);	
		delay_ms(50);
		Write429Int(IDX_XTARGET|(i<<5), 0);
		Write429Int(IDX_XACTUAL|(i<<5), 0);
	}
}

/***************************************************************//**
  \fn RotateRight(void)
  \brief Command ROR (see TMCL manual)

  ROR (ROtate Right) command (see TMCL manual).
********************************************************************/
void RotateRight(uint8_t axisNum , int speed)
{
	SpeedChangedFlag[axisNum]=TRUE;
	Set429RampMode(axisNum, RM_VELOCITY);
	Write429Short(IDX_VMAX|(axisNum<<5), 2047);
	Write429Short(IDX_VTARGET|(axisNum<<5), speed);
}
/***************************************************************//**
  \fn RotateLeft(void)
  \brief Command ROL

  ROL (ROtate Left) command (see TMCL manual).
********************************************************************/
void RotateLeft(uint8_t axisNum , int speed)
{
	SpeedChangedFlag[axisNum]=TRUE;
	Set429RampMode(axisNum, RM_VELOCITY);
	Write429Short(IDX_VMAX|(axisNum<<5), 2047);
	Write429Short(IDX_VTARGET|(axisNum<<5), -speed);
}
/***************************************************************//**
  \fn MotorStop(void)
  \brief Command MST

  MST (Motor StoP) command (see TMCL manual).
********************************************************************/
void MotorStop(uint8_t axisNum)
{
//	Set429RampMode(axisNum, RM_VELOCITY);
//	Write429Zero(IDX_VTARGET|(axisNum<<5));

	HardStop(axisNum);
	delay_ms(3);
}


//** TMCL Commands **
/***************************************************************//**
  \fn RotateRight(void)
  \brief Command ROR (see TMCL manual)
********************************************************************/
uint8_t TMCL_MotorRotate(void)
{
  if(ActualCommand.Motor<N_O_MOTORS)
  {
		//MotorStopByLimit[ActualCommand.Motor]=1;
		
    SpeedChangedFlag[ActualCommand.Motor]=TRUE;
    Set429RampMode(ActualCommand.Motor, RM_VELOCITY);
    Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), 2047);
    Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), ActualCommand.Value.Int32);
		//rt_kprintf("motor%d is rotate at %d\n",ActualCommand.Motor,ActualCommand.Value.Int32);
		return 0;	
  }
	return 1;
}
/***************************************************************//**
  \fn MotorStop(void)
  \brief Command MST

  MST (Motor StoP) command (see TMCL manual).
********************************************************************/
static uint8_t TMCL_MotorStop(void)
{
  if(ActualCommand.Motor<N_O_MOTORS)
  {
		
//    Set429RampMode(ActualCommand.Motor, RM_VELOCITY);
//    Write429Zero(IDX_VTARGET|(ActualCommand.Motor<<5));
		
		HardStop(ActualCommand.Motor);
		delay_ms(3);
		homeInfo.GoHome[ActualCommand.Motor]=0;
		homeInfo.GoLimit[ActualCommand.Motor]=0;
		rt_kprintf("motor[%d] is stop and P[%d]=%d\n",ActualCommand.Motor,ActualCommand.Motor,Read429Int(IDX_XACTUAL|(ActualCommand.Motor<<5)));
		return 0;
  }
	return 1;
}
/***************************************************************//**
  \fn MoveToPosition(void)
  \brief Command MVP

  MVP (Move To Position) command (see TMCL manual).
********************************************************************/
static uint8_t TMCL_MoveToPosition(void)
{
  if(ActualCommand.Motor<N_O_MOTORS)
  {
    if(ActualCommand.Type==MVP_ABS || ActualCommand.Type==MVP_REL)
    {
			//MotorStopByLimit[ActualCommand.Motor]=1;
			
      if(SpeedChangedFlag[ActualCommand.Motor])
      {
        Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), MotorConfig[ActualCommand.Motor].VMax);
				SetAmaxAutoByspeed(ActualCommand.Motor,MotorConfig[ActualCommand.Motor].VMax);
        //SetAMax(ActualCommand.Motor, MotorConfig[ActualCommand.Motor].AMax);
        SpeedChangedFlag[ActualCommand.Motor]=FALSE;
      }
      if(ActualCommand.Type==MVP_ABS)
			{
				CMD_TRACE("motor[%d] is start to make absolute motion\n",ActualCommand.Motor);
        Write429Int(IDX_XTARGET|(ActualCommand.Motor<<5), ActualCommand.Value.Int32);
			}
      else
			{
				CMD_TRACE("motor[%d] is start to make relative motion\n",ActualCommand.Motor);
        Write429Int(IDX_XTARGET|(ActualCommand.Motor<<5), ActualCommand.Value.Int32+Read429Int(IDX_XACTUAL|(ActualCommand.Motor<<5)));
			}
      Set429RampMode(ActualCommand.Motor, RM_RAMP);
			return 0;
    }
    else 
		{
			ActualReply.Status=REPLY_WRONG_TYPE;
			return 1;
		}
  }
  else ActualReply.Status=REPLY_INVALID_VALUE;
	return 1;
}



/***************************************************************//**
  \fn GetAxisParameter(void)
  \brief Command GAP

  GAP (Get Axis Parameter) command (see TMCL manual).
********************************************************************/
static void TMCL_GetAxisParameter(void)
{
  if(ActualCommand.Motor<N_O_MOTORS)
  {
    ActualReply.Value.Int32=0;
    switch(ActualCommand.Type)
    {
      case next_position:
        ActualReply.Value.Int32=Read429Int(IDX_XTARGET|(ActualCommand.Motor<<5));				
				CMD_TRACE("motor[%d] next position=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
				break;

      case actual_position:
        ActualReply.Value.Int32=Read429Int(IDX_XACTUAL|(ActualCommand.Motor<<5));
				CMD_TRACE("motor[%d] actual position=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
        break;

      case next_speed:
        ActualReply.Value.Int32=Read429Short(IDX_VTARGET|(ActualCommand.Motor<<5));
			  CMD_TRACE("motor[%d] next speed=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
        break;

      case actual_speed:
        ActualReply.Value.Int32=Read429Short(IDX_VACTUAL|(ActualCommand.Motor<<5));
			  CMD_TRACE("motor[%d] actual speed=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
        break;

      case max_v_positioning:
        ActualReply.Value.Int32=MotorConfig[ActualCommand.Motor].VMax;
			  CMD_TRACE("motor[%d] max positioning speed=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
        break;

      case max_acc:
        ActualReply.Value.Int32=MotorConfig[ActualCommand.Motor].AMax;
			  CMD_TRACE("motor[%d] max acceleration=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
        break;

      case max_current:
        //ActualReply.Value.Int32=MotorConfig[ActualCommand.Motor].IRun;
				CMD_TRACE("motor[%d] max current=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
        break;

      case standby_current:
        //ActualReply.Value.Int32=MotorConfig[ActualCommand.Motor].IStandby;
				CMD_TRACE("motor[%d] standby current=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
        break;

      case position_reached:
        if(Read429Status() & 0x01) ActualReply.Value.Byte[0]=1;
				CMD_TRACE("motor[%d] position reached=%d\n",ActualCommand.Motor,ActualReply.Value.Byte[0]);
        break;

      case leftLimit_SwS:
        ActualReply.Value.Int32=(Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<ActualCommand.Motor*2)) ? 1:0;
			  CMD_TRACE("motor[%d] left limit switch status=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
        break;

      case rightLimit_SwS:
        ActualReply.Value.Int32=(Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x01<<ActualCommand.Motor*2)) ? 1:0;
			  CMD_TRACE("motor[%d] right limit switch status=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
        break;

      case rightLimit_disable:
        ActualReply.Value.Byte[0]=(Read429SingleByte(IDX_REFCONF_RM|(ActualCommand.Motor<<5), 2) & 0x02) ? 1:0;
			  CMD_TRACE("motor[%d] right limit switch disable=%d\n",ActualCommand.Motor,ActualReply.Value.Byte[0]);
				break;

      case leftLimit_disable:
        ActualReply.Value.Byte[0]=(Read429SingleByte(IDX_REFCONF_RM|(ActualCommand.Motor<<5), 2) & 0x01) ? 1:0;
				CMD_TRACE("motor[%d] left limit switch disable=%d\n",ActualCommand.Motor,ActualReply.Value.Byte[0]);
        break;

      case min_speed:
        ActualReply.Value.Int32=Read429Short(IDX_VMIN|(ActualCommand.Motor<<5));
				CMD_TRACE("motor[%d] minimum speed=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
        break;

      case ramp_mode:
        ActualReply.Value.Byte[0]=Read429SingleByte(IDX_REFCONF_RM|(ActualCommand.Motor<<5), 3);
			  CMD_TRACE("motor[%d] ramp mode=%d\n",ActualCommand.Motor,ActualReply.Value.Byte[0]);
        break;

      case ref_sw_tolerance:
        ActualReply.Value.Int32=Read429Short(MOTOR_NUMBER(ActualCommand.Motor)<<5|IDX_DX_REFTOLERANCE);
				CMD_TRACE("motor[%d] ref switch tolerance=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
        break;

      case softStop_flag:
        ActualReply.Value.Int32=(Read429SingleByte(IDX_REFCONF_RM|MOTOR_NUMBER(ActualCommand.Motor)<<5, 0) & 0x04) ? 1:0;
				CMD_TRACE("motor[%d] soft stop status=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
			  break;

      case ramp_divisor:
        ActualReply.Value.Byte[0]=Read429SingleByte(IDX_PULSEDIV_RAMPDIV|(ActualCommand.Motor<<5), 2) & 0x0f;
		  	CMD_TRACE("motor[%d] ramp divisor=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
        break;

      case pulse_divisor:
        ActualReply.Value.Byte[0]=Read429SingleByte(IDX_PULSEDIV_RAMPDIV|(ActualCommand.Motor<<5), 2) >> 4;
				CMD_TRACE("motor[%d] pulse divisor=%d\n",ActualCommand.Motor,ActualReply.Value.Int32);
        break;

			case is_homed:
				CMD_TRACE("motor[%d] homed=%d\n",ActualCommand.Motor,homeInfo.Homed[ActualCommand.Motor]);
				break;
			case is_stop:
				CMD_TRACE("motor[%d] stop=%d\n",ActualCommand.Motor,Read429Short(IDX_VACTUAL|(ActualCommand.Motor<<5))? 0:1) ;
				break;
			case home_SenS:
				CMD_TRACE("motor[%d] homeSensor=%d\n",ActualCommand.Motor, rt_pin_read(homeSensorPin[ActualCommand.Motor]) ? 0:1) ;
				break;						
      default:
        ActualReply.Status=REPLY_WRONG_TYPE;
        break;
    }
  } else ActualReply.Status=REPLY_INVALID_VALUE;
}



static void TMCL_SetAxisParameter(void)
{
  UCHAR Read[4], Write[4];
  if(ActualCommand.Motor<N_O_MOTORS)
  {
    switch(ActualCommand.Type)
    {
      case next_position:
        Write429Int(IDX_XTARGET|(ActualCommand.Motor<<5), ActualCommand.Value.Int32);
				CMD_TRACE("set motor[%d] next position=%d ok\n",ActualCommand.Motor,ActualCommand.Value.Int32);
        break;

      case actual_position:
        Write429Int(IDX_XACTUAL|(ActualCommand.Motor<<5), ActualCommand.Value.Int32);
				CMD_TRACE("set motor[%d] actual position=%d ok\n",ActualCommand.Motor,ActualCommand.Value.Int32);
        break;

      case next_speed:
        Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), ActualCommand.Value.Int32);
				CMD_TRACE("set motor[%d] next speed=%d ok\n",ActualCommand.Motor,ActualCommand.Value.Int32);
        break;

      case actual_speed:
        Write429Short(IDX_VACTUAL|(ActualCommand.Motor<<5), ActualCommand.Value.Int32);
				CMD_TRACE("set motor[%d] actual speed=%d ok\n",ActualCommand.Motor,ActualCommand.Value.Int32);		
        break;

      case max_v_positioning:
        MotorConfig[ActualCommand.Motor].VMax=ActualCommand.Value.Int32;
        Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), MotorConfig[ActualCommand.Motor].VMax);
				SetAmaxAutoByspeed(ActualCommand.Motor,MotorConfig[ActualCommand.Motor].VMax);
				CMD_TRACE("set motor[%d] speed=%d ok\n",ActualCommand.Motor,ActualCommand.Value.Int32);		
        break;

      case max_acc:
        MotorConfig[ActualCommand.Motor].AMax=ActualCommand.Value.Int32;
        SetAMax(ActualCommand.Motor, MotorConfig[ActualCommand.Motor].AMax);
				CMD_TRACE("set motor[%d] max acceleration speed=%d ok\n",ActualCommand.Motor,ActualCommand.Value.Int32);		
        break;

      case rightLimit_disable:	//12
        Write[0]=IDX_REFCONF_RM|TMC429_READ|(ActualCommand.Motor<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        if(ActualCommand.Value.Byte[0]!=0)
          Write[2]=Read[2]|0x02;
        else
          Write[2]=Read[2]&  ~0x02;
        Write[3]=Read[3];
        Write[0]=IDX_REFCONF_RM|(ActualCommand.Motor<<5);
        ReadWrite429(Read, Write);
				if(ActualCommand.Value.Byte[0]!=0)
					CMD_TRACE("motor[%d] right limit switch is disable\n",ActualCommand.Motor);	
				else 
					CMD_TRACE("motor[%d] right limit switch is enable\n",ActualCommand.Motor);	
        break;

      case leftLimit_disable: //13
        Write[0]=IDX_REFCONF_RM|TMC429_READ|(ActualCommand.Motor<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        if(ActualCommand.Value.Byte[0]!=0)
          Write[2]=Read[2]|0x01;
        else
          Write[2]=Read[2]&  ~0x01;
        Write[3]=Read[3];
        Write[0]=IDX_REFCONF_RM|(ActualCommand.Motor<<5);
        ReadWrite429(Read, Write);
				if(ActualCommand.Value.Byte[0]!=0)
					CMD_TRACE("motor[%d] left limit switch is disable\n",ActualCommand.Motor);	
				else 
					CMD_TRACE("motor[%d] left limit switch is enable\n",ActualCommand.Motor);	
        break;

      case min_speed:
        Write429Short(IDX_VMIN|(ActualCommand.Motor<<5), ActualCommand.Value.Int32);
				CMD_TRACE("set motor[%d] minimum speed=%d ok\n",ActualCommand.Motor,ActualCommand.Value.Int32);
        break;

      case ramp_mode:
        Set429RampMode(ActualCommand.Motor, ActualCommand.Value.Byte[0]);
				CMD_TRACE("set motor[%d] ramp mode=%d ok\n",ActualCommand.Motor,ActualCommand.Value.Byte[0]);
        break;

      case ref_sw_tolerance:
        Write429Short(MOTOR_NUMBER(ActualCommand.Motor)<<5|IDX_DX_REFTOLERANCE, ActualCommand.Value.Int32);
				CMD_TRACE("set motor[%d] ref switch tolerance=%d ok\n",ActualCommand.Motor,ActualCommand.Value.Int32);
			  break;

      case softStop_flag:
        Read429Bytes(IDX_REFCONF_RM|MOTOR_NUMBER(ActualCommand.Motor)<<5, Read);
        if(ActualCommand.Value.Int32!=0)
          Read[1]|=0x04;
        else
          Read[1]&= ~0x04;
        Write429Bytes(IDX_REFCONF_RM|MOTOR_NUMBER(ActualCommand.Motor)<<5, Read);
				CMD_TRACE("set motor[%d] soft stop status=%d ok\n",ActualCommand.Motor,ActualCommand.Value.Int32);
        break;

      case ramp_divisor:
        Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(ActualCommand.Motor<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        Write[2]=(Read[2] & 0xf0) | (ActualCommand.Value.Byte[0] & 0x0f);
        Write[3]=Read[3];
        Write[0]=IDX_PULSEDIV_RAMPDIV|(ActualCommand.Motor<<5);
        ReadWrite429(Read, Write);
        MotorConfig[ActualCommand.Motor].RampDiv=ActualCommand.Value.Byte[0] & 0x0f;
				CMD_TRACE("set motor[%d] ramp divisor=%d ok\n",ActualCommand.Motor,ActualCommand.Value.Int32);
        break;

      case pulse_divisor:
        Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(ActualCommand.Motor<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        Write[2]=(Read[2] & 0x0f) | (ActualCommand.Value.Byte[0] << 4);
        Write[3]=Read[3];
        Write[0]=IDX_PULSEDIV_RAMPDIV|(ActualCommand.Motor<<5);
        ReadWrite429(Read, Write);
        MotorConfig[ActualCommand.Motor].PulseDiv=ActualCommand.Value.Byte[0]& 0x0f;
				CMD_TRACE("set motor[%d] pulse divisor=%d ok\n",ActualCommand.Motor,ActualCommand.Value.Int32);
        break;
			case limitSignal:
				if(ActualCommand.Value.Int32!=0) 
				{
				  Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR|IFCONF_INV_REF);
				}
				else Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR);
				CMD_TRACE("set limit signal effective trigger level=%d ok\n",ActualCommand.Value.Int32);
				break;
				
      default:
        ActualReply.Status=REPLY_WRONG_TYPE;
        break;
    }
  } else ActualReply.Status=REPLY_INVALID_VALUE;
}

/*
 对轴号和参数范围先进行判断	内部调用函数不再进行判断
		0		 		1       		2 							3					4						5
argc=2
  motor   reset			//封装一系列动作，需要提取项目号来触发，不建议应用
argc=3
	motor 	stop  		<axisNum> 
argc=4
	motor 	rotate 		<axisNum> 	<axis_speed>  
  motor 	move	 		<axisNum> 	<position> 
	motor 	moveto	 	<axisNum> 	<position> 
	motor 	get 			<axisNum> 	<type>  

  motor 	gohome 		<axisNum> 	<speed> 
	motor 	golimit		<axisNum> 	<speed> 
argc=5
  motor 	set 			<type> 			<axisNum> 				<value>  

  	
*/
int motor(int argc, char **argv)
{
	  ActualCommand.Type=Type_none;
		ActualReply.Status=REPLY_OK;
    int result = RT_ERROR;
	
		if (argc == 2 )
		{
			if (!strcmp(argv[1], "reset?"))
			{
				CMD_TRACE("motor reset=%d\n",autoRESETmotor==OK_2 ? 1:0) ;
				return RT_EOK	; 				
			}
			else	if (!strcmp(argv[1], "reset"))
			{
				MotorAutoReset_preset();
				return RT_EOK	; 
			}
      else
			{
				rt_kprintf("Usage: \n");
				rt_kprintf("motor reset            -motor reset\n");		
				rt_kprintf("motor reset?           -is axis reset or not\n");					
				result =REPLY_INVALID_CMD;
			}     
		}
		if (argc == 3)
		{
			ActualCommand.Motor=atoi(argv[2]);
			if (!strcmp(argv[1], "stop"))
			{
				if(!TMCL_MotorStop())
				{
					if(ActualCommand.Motor==AXIS_Z) 
					{	
						closeSerial();
					}
					delay_ms(1);
					return RT_EOK	;
				}
				result = RT_EINVAL;
			}
		}
		else if (argc == 4)
		{	
			if (!strcmp(argv[1], "rotate"))
			{
				ActualCommand.Motor=atoi(argv[2]);
				ActualCommand.Value.Int32=atoi(argv[3]);
				if(!(TMCL_MotorRotate()))	return RT_EOK	;
				result = RT_EINVAL;
			}
			if (!strcmp(argv[1], "move"))
			{
				ActualCommand.Motor=atoi(argv[2]);
				ActualCommand.Type=MVP_REL;
				ActualCommand.Value.Int32=atoi(argv[3]);
				if(!TMCL_MoveToPosition())	return RT_EOK;
				result =RT_EINVAL;
			}
			if (!strcmp(argv[1], "moveto"))
			{
				ActualCommand.Motor=atoi(argv[2]);
				ActualCommand.Type=MVP_ABS;
				ActualCommand.Value.Int32=atoi(argv[3]);
				if(!TMCL_MoveToPosition())	return RT_EOK;
				result =RT_EINVAL;
			}
			if (!strcmp(argv[1], "gohome"))
			{
					timer_stop();		
				
					ActualCommand.Motor=atoi(argv[2]);
					homeInfo.GoHome[ActualCommand.Motor]=TRUE;
				  homeInfo.GoLimit[ActualCommand.Motor]=FALSE;
					homeInfo.Homed[ActualCommand.Motor]=FALSE;
					ActualCommand.Value.Int32=atoi(argv[3]);
					homeInfo.HomeSpeed[ActualCommand.Motor]=ActualCommand.Value.Int32;	
					if(ActualCommand.Value.Int32<0) {SetAmaxAutoByspeed(ActualCommand.Motor,-ActualCommand.Value.Int32);}
					else														{SetAmaxAutoByspeed(ActualCommand.Motor,ActualCommand.Value.Int32);}
				  TMCL_MotorRotate();
					CMD_TRACE("motor[%d] is start go home by searching home sensor\n",ActualCommand.Motor);
				  //电机回原点速度需要保存，下次复位按键需要调用，或者按键默认速度
					timer_start();
					return RT_EOK	;
			}
			if (!strcmp(argv[1], "golimit"))
			{
					timer_stop();
				
					ActualCommand.Motor=atoi(argv[2]);
				  homeInfo.GoHome[ActualCommand.Motor]=FALSE;
					homeInfo.GoLimit[ActualCommand.Motor]=TRUE;
					homeInfo.Homed[ActualCommand.Motor]=FALSE;
					ActualCommand.Value.Int32=atoi(argv[3]);
					homeInfo.HomeSpeed[ActualCommand.Motor]=ActualCommand.Value.Int32;	
					if(ActualCommand.Value.Int32<0) {SetAmaxAutoByspeed(ActualCommand.Motor,-ActualCommand.Value.Int32);}
					else														{SetAmaxAutoByspeed(ActualCommand.Motor,ActualCommand.Value.Int32);}
				  TMCL_MotorRotate();
					timer_start();
					return RT_EOK	;
			}
			else if (!strcmp(argv[1], "get"))
			{
				if (!strcmp(argv[2], "speed")) 	 					ActualCommand.Type=actual_speed;
				if (!strcmp(argv[2], "position")) 				ActualCommand.Type=actual_position;
				if (!strcmp(argv[2], "is_homed")) 				ActualCommand.Type=is_homed;
				if (!strcmp(argv[2], "is_stop")) 					ActualCommand.Type=is_stop;

			
				if (!strcmp(argv[2], "next_speed")) 			ActualCommand.Type=next_speed;
				if (!strcmp(argv[2], "next_position")) 	 	ActualCommand.Type=next_position;		
				if (!strcmp(argv[2], "VMAX")) 					 	ActualCommand.Type=max_v_positioning;
				if (!strcmp(argv[2], "AMAX")) 		 				ActualCommand.Type=max_acc;
				if (!strcmp(argv[2], "position_reached")) ActualCommand.Type=position_reached;			
				if (!strcmp(argv[2], "rightLimit")) 			ActualCommand.Type=rightLimit_SwS;
				if (!strcmp(argv[2], "leftLimit")) 				ActualCommand.Type=leftLimit_SwS;
				
				if (!strcmp(argv[2], "homeSensor")) 			ActualCommand.Type=home_SenS;
				
				if (!strcmp(argv[2], "rightLimit?"))			  ActualCommand.Type=rightLimit_disable;
				if (!strcmp(argv[2], "leftLimit?")) 			  ActualCommand.Type=leftLimit_disable;
				
				if (!strcmp(argv[2], "ramp_div")) 			  ActualCommand.Type=ramp_divisor;
				if (!strcmp(argv[2], "pulse_div")) 				ActualCommand.Type=pulse_divisor;	
				
//				if (!strcmp(argv[2], "max_current")) 	   	ActualCommand.Type=max_current;
//				if (!strcmp(argv[2], "standby_current")) 	ActualCommand.Type=standby_current;
//				if (!strcmp(argv[2], "Llimit_off")) 			ActualCommand.Type=leftLimit_disable;
				
				if(ActualCommand.Type!=Type_none)
				{
					ActualCommand.Motor=atoi(argv[3]);
					TMCL_GetAxisParameter();
					if(ActualReply.Status==REPLY_OK) return RT_EOK;
					result=RT_EINVAL;
				}
				else 
				{
						rt_kprintf("Usage: \n");
					//用户接口
					  rt_kprintf("motor get speed <axis>             -get the current speed \n");
					  rt_kprintf("motor get position <axis>          -get the current position\n");
					 
					  rt_kprintf("motor get rightLimit <axis>        -get right switch status\n");
					  rt_kprintf("motor get leftLimit  <axis>        -get left switch status\n");	
					
					  rt_kprintf("motor get is_homed <axis>          -is axis homed or not\n");	
					  rt_kprintf("motor get is_stop <axis>           -is axis stop or not\n");	
						rt_kprintf("motor get homeSensor <axis>        -get home sensor status\n");	
					//调试接口
					CMD_TRACE("motor get next_speed <axis>        -get the target speed \n");
					CMD_TRACE("motor get next_position <axis>     -get the target position to move\n");
					
					CMD_TRACE("motor get VMAX <axis>              -get max positioning speed\n");
					CMD_TRACE("motor get AMAX <axis>              -get max acceleration\n");
		
					CMD_TRACE("motor get position_reached <axis>  -is reach positon or not\n");
					CMD_TRACE("motor get ramp_div <axis>          -get ramp divisor value\n");
					CMD_TRACE("motor get pulse_div <axis>         -get pulse divisor value\n");		
					result =REPLY_INVALID_CMD;
				}
			}
		}
		else if (argc == 5)
		{
			if (!strcmp(argv[1], "set"))
			{
				//user
				if (!strcmp(argv[2], "speed")) 					 	ActualCommand.Type=max_v_positioning;
					
				//debug
				if (!strcmp(argv[2], "next_speed")) 			ActualCommand.Type=next_speed;		
				if (!strcmp(argv[2], "next_position")) 	 	ActualCommand.Type=next_position;
				if (!strcmp(argv[2], "actual_position")) 	ActualCommand.Type=actual_position;
				if (!strcmp(argv[2], "actual_speed")) 	 	ActualCommand.Type=actual_speed;
				if (!strcmp(argv[2], "VMAX")) 					ActualCommand.Type=max_v_positioning;
				if (!strcmp(argv[2], "AMAX")) 		 				ActualCommand.Type=max_acc;
				//设置限位失能无效，需要调试原因
				if (!strcmp(argv[2], "rightLimit"))				ActualCommand.Type=rightLimit_disable;
				if (!strcmp(argv[2], "leftLimit")) 				ActualCommand.Type=leftLimit_disable;	  		
				if (!strcmp(argv[2], "limitSignal")) 		  ActualCommand.Type=limitSignal;		
				if (!strcmp(argv[2], "ramp_div")) 			  ActualCommand.Type=ramp_divisor;
				if (!strcmp(argv[2], "pulse_div")) 				ActualCommand.Type=pulse_divisor;
//				if (!strcmp(argv[2], "max_current")) 	   	ActualCommand.Type=max_current;
//				if (!strcmp(argv[2], "standby_current")) 	ActualCommand.Type=standby_current;
				if(ActualCommand.Type!=Type_none)	
				{
					ActualCommand.Motor=atoi(argv[3]);
					ActualCommand.Value.Int32=atoi(argv[4]);
					TMCL_SetAxisParameter();
					if(ActualReply.Status==REPLY_OK) return RT_EOK;
					else result =RT_EINVAL;
				}
				else 
				{
				  rt_kprintf("Usage: \n");
				  rt_kprintf("motor set speed <axis> <value>              -set the target speed \n");				
					CMD_TRACE("motor set next_position <axis> <value>      -set the target position to move\n");
					CMD_TRACE("motor set actual_position <axis> <value>    -set the current position\n");				
					CMD_TRACE("motor set actual_speed <axis> <value>       -set the current speed \n");
					CMD_TRACE("motor set VMAX <axis> <value>               -set max positioning speed\n");
					CMD_TRACE("motor set AMAX <axis> <value>               -set max acceleration\n");
					CMD_TRACE("motor set ramp_div <axis> <value>           -set ramp divisor value\n");
					CMD_TRACE("motor set pulse_div <axis> <value>          -set pulse divisor value\n");	
					CMD_TRACE("motor set rightLimit <axis> <value>         -set right limit switch\n");
					CMD_TRACE("motor set leftLimit <axis> <value>          -set left limit switch\n");
					CMD_TRACE("motor set limitSignal <axis> <value>        -set limit signal effective trigger level\n");					
					result =REPLY_INVALID_CMD;														
				}				
			}

		}		
		if(	result == RT_EINVAL )
		{
			rt_kprintf("motor number must be 0~2\n",ActualCommand.Motor);
		}
		else if( result == RT_ERROR )
		{
			rt_kprintf("Usage: \n");
			rt_kprintf("motor stop <axisNum>                  - stop motor\n");		
			rt_kprintf("motor rotate <axisNum> <speed>        - rotate motor\n");	
			rt_kprintf("motor move <axisNum> <position>       - absolute motion of the motor\n");
			rt_kprintf("motor moveto <axisNum> <position>     -relative motion of the motor\n");

			rt_kprintf("motor get <type> <axisNum>            - get axis parameter\n");	
			rt_kprintf("motor set <type> <axisNum> <value>    - set axis parameter\n");	
			
			rt_kprintf("motor gohome <axisNum> <speed>        - home sensor as origin position\n");	
			rt_kprintf("motor golimit <axisNum> <speed>       - limit sensor as origin position\n");	
		
		}
	  return result;
}

//
static rt_uint8_t motion_stack[ 512 ];
static struct rt_thread motion_thread;
static void motion_thread_entry(void *parameter);
//////////////////////////////////////////////////////////
static void MotorLimitProcess(uint8_t axisNum)
{
	if(homeInfo.GoHome[axisNum]==FALSE && homeInfo.GoLimit[axisNum]==FALSE)	//不是回原点动作
	{	
		if(MotorStopByLimit[axisNum]==1)
		{
			uint8_t SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);				//限位状态读取
			//CMD_TRACE("%d GoHome=%d,GoLimit=%d,SwitchStatus=%d,V=%d\n",axisNum,homeInfo.GoHome[axisNum],homeInfo.GoLimit[axisNum],SwitchStatus,Read429Short(IDX_VACTUAL|(axisNum<<5)));

			//左				需要再判断速度													右              //碰到限位停止，需要再次发命令运动
			if((((SwitchStatus& (0x02<<axisNum*2))?1:0) ==1)	||  (((SwitchStatus& (0x01<<axisNum*2)) ?1:0)==1)	\
				   && ( Read429Short(IDX_VACTUAL|(axisNum<<5))==0 )	)						 
			{					
					
					MotorStop(axisNum);		MotorStopByLimit[axisNum]=0;
					//CMD_TRACE("MotorStop(%d) is act\n",axisNum);
			}		
		}
	}
}
//MSH_CMD_EXPORT(test,..);
static void motion_thread_entry(void *parameter)
{
	  static uint8_t cnt_delay=0;
    while (1)
    {  
//			for(uint8_t i=0;i<N_O_MOTORS;i++)
//			{			
//				MotorHomingWithHomeSensor(i,homeInfo.HomeSpeed[i]);
//				MotorHomingWithLimitSwitch(i,homeInfo.HomeSpeed[i]);
//			}	
			if(cnt_delay++>5) 
			{
				for(uint8_t i=0;i<N_O_MOTORS;i++)	
				{
					MotorLimitProcess(i);
					cnt_delay=0;
				}
			}
			rt_thread_delay(10);
		}		
}
int MotorLimitCheck_thread_init(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&motion_thread,
                            "motion",
                            motion_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&motion_stack[0],
                            sizeof(motion_stack),
                            25,
                            5);
    if (result == RT_EOK)
    {
       rt_thread_startup(&motion_thread);
    }
    return 0;
}

