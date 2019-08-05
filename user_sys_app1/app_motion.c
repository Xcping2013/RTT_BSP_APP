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

//#include "app_motion.h"
//#include "app_sys_control.h"

#include "bsp_include.h"	
#include "app_include.h"

/*********************************************************************************************************************/

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
allLimit_SwS,
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
is_reach,
is_reset,

Type_none,

};

//
typedef struct
{
  UCHAR Type;        //!< type parameter
  UCHAR Motor;       //!< motor/bank parameter
  union
  {
    long Int32;      //!< value parameter as 32 bit integer
    UCHAR Byte[4];   //!< value parameter as 4 bytes
  } Value;           //!< value parameter
	
} TMC429_Command;

TMC429_Command CMDGetFromUart;

typedef struct
{
  uint8_t Status;               
  union
  {
    long Int32;      
    uint8_t Byte[4];   
  } Value;   
	
} TMC429_Parameter;

TMC429_Parameter TMC429_ParameterGet;

TMC429_Parameter TMC429_ParameterSet;

static UCHAR SpeedChangedFlag[N_O_MOTORS];

static uint8_t MotorStopByLimit[N_O_MOTORS]={ENABLE,ENABLE,ENABLE};								//碰到限位处理一次
		
/*********************************************************************************************************************/
static void 		TMC429_ParameterPresetToRam(void);
static uint8_t 		TMC429_MoveToPosition(uint8_t motor_number, uint8_t motion_mode, int32_t position);

static void 		TMC429_GetAxisParameter(uint8_t motor_number, uint8_t parameter_type);
static void 		TMC429_SetAxisParameter(uint8_t motor_number, uint8_t parameter_type, int32_t parameter_value);
static void 		MotorKeepStop_removeLimitSigal(uint8_t motor_number);

static void 		printf_cmdList_motor(void);
static void 		printf_cmdList_motor_set(void);
static void 		printf_cmdList_motor_get(void);

/*********************************************************************************************************************/
static void TMC429_ParameterPresetToRam(void)
{
  uint8_t i;
	for(i=0; i<N_O_MOTORS; i++) 					
	{																  
		MotorConfig[i].VMax 		= g_tParam.tmc429_VMax[i];   				
		MotorConfig[i].AMax 		=	g_tParam.tmc429_AMax[i];
		MotorConfig[i].PulseDiv	=	g_tParam.tmc429_PulseDiv[i];	
		MotorConfig[i].RampDiv	=	g_tParam.tmc429_RampDiv[i];	
		
		SpeedChangedFlag[i]			=	TRUE;
		homeInfo.Homed[i]				=	FALSE;
		homeInfo.GoHome[i]			=	FALSE;
		homeInfo.GoLimit[i]			=	FALSE;
		homeInfo.HomeSpeed[i]		=	g_tParam.speed_home[i];
		
		pinMode(homeSensorPin[i], PIN_MODE_INPUT_PULLUP);
	}
}

/***************************************************************//**
  \fn MoveToPosition(void)
  \brief Command MVP

  MVP (Move To Position) command (see TMCL manual).
********************************************************************/
static uint8_t TMC429_MoveToPosition(uint8_t motor_number, uint8_t motion_mode, int32_t position)
{
  if(motor_number<N_O_MOTORS)
  {
    if(motion_mode==MVP_ABS || motion_mode==MVP_REL)
    {
			//MotorStopByLimit[motor_number]=1;
			
      if(SpeedChangedFlag[motor_number])
      {
        Write429Short(IDX_VMAX|(motor_number<<5), MotorConfig[motor_number].VMax);
			  SetAmaxAutoByspeed(motor_number,MotorConfig[motor_number].VMax);
        SpeedChangedFlag[motor_number]=FALSE;
      }
      if(motion_mode==MVP_ABS)
	  {
				CMD_TRACE("motor[%d] is start to make absolute motion\n",motor_number);
        Write429Int(IDX_XTARGET|(motor_number<<5), position);
	  }
      else
	 {
				CMD_TRACE("motor[%d] is start to make relative motion\n",motor_number);
        Write429Int(IDX_XTARGET|(motor_number<<5), position + Read429Int(IDX_XACTUAL|(motor_number<<5)));
	 }
      Set429RampMode(motor_number, RM_RAMP);
	 return 0;
    }
    else 
	{
		return 1;
	}
  }
  else TMC429_ParameterGet.Status=REPLY_INVALID_VALUE;
  return 1;
}



/***************************************************************//**
  \fn GetAxisParameter(void)
  \brief Command GAP

  GAP (Get Axis Parameter) command (see TMCL manual).
********************************************************************/
static void TMC429_GetAxisParameter(uint8_t motor_number, uint8_t parameter_type)
{	
  TMC429_ParameterGet.Value.Int32=0;
	
  if(motor_number < N_O_MOTORS)	
  {
    switch(parameter_type)
    {
      case next_position:
        TMC429_ParameterGet.Value.Int32=Read429Int(IDX_XTARGET|(motor_number<<5));				
				CMD_TRACE("motor[%d] next position=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
				break;

      case actual_position:
        TMC429_ParameterGet.Value.Int32=Read429Int(IDX_XACTUAL|(motor_number<<5));
				CMD_TRACE("motor[%d] actual position=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case next_speed:
        TMC429_ParameterGet.Value.Int32=Read429Short(IDX_VTARGET|(motor_number<<5));
			  CMD_TRACE("motor[%d] next speed=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case actual_speed:
        TMC429_ParameterGet.Value.Int32=Read429Short(IDX_VACTUAL|(motor_number<<5));
			  CMD_TRACE("motor[%d] actual speed=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case max_v_positioning:
        TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].VMax;
			  CMD_TRACE("motor[%d] max positioning speed=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case max_acc:
        TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].AMax;
			  CMD_TRACE("motor[%d] max acceleration=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case max_current:
        //TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].IRun;
				CMD_TRACE("motor[%d] max current=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case standby_current:
        //TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].IStandby;
				CMD_TRACE("motor[%d] standby current=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case position_reached:
//        if(Read429Status() & 0x01) TMC429_ParameterGet.Value.Byte[0]=1;
//				CMD_TRACE("motor[%d] position reached=%d\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
			
				CMD_TRACE("motor[%d] position reached=%d\n",motor_number,(Read429Status() & (0x01<<motor_number*2)) ? 1:0);
        break;

      case leftLimit_SwS:
        TMC429_ParameterGet.Value.Int32=(Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<motor_number*2)) ? 1:0;
			  CMD_TRACE("motor[%d] left limit switch status=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case rightLimit_SwS:
        TMC429_ParameterGet.Value.Int32=(Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x01<<motor_number*2)) ? 1:0;
			  CMD_TRACE("motor[%d] right limit switch status=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;
			
      case allLimit_SwS:
				{
						uint8_t limit_char[6];
						uint8_t i;
						uint8_t limit = Read429SingleByte(IDX_REF_SWITCHES, 3);
						for(i=0;i<6;i++)
						{
							limit_char[i]=(limit&(0x20>>i)) ? '1':'0';
						}
						CMD_TRACE("motor limit switch status L2R2L1R1L0R0=%.6s\n",limit_char);
				}
        break;

      case rightLimit_disable:
        TMC429_ParameterGet.Value.Byte[0]=(Read429SingleByte(IDX_REFCONF_RM|(motor_number<<5), 2) & 0x02) ? 1:0;
			  CMD_TRACE("motor[%d] right limit switch disable=%d\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
				break;

      case leftLimit_disable:
        TMC429_ParameterGet.Value.Byte[0]=(Read429SingleByte(IDX_REFCONF_RM|(motor_number<<5), 2) & 0x01) ? 1:0;
				CMD_TRACE("motor[%d] left limit switch disable=%d\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
        break;

      case min_speed:
        TMC429_ParameterGet.Value.Int32=Read429Short(IDX_VMIN|(motor_number<<5));
				CMD_TRACE("motor[%d] minimum speed=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case ramp_mode:
        TMC429_ParameterGet.Value.Byte[0]=Read429SingleByte(IDX_REFCONF_RM|(motor_number<<5), 3);
			  CMD_TRACE("motor[%d] ramp mode=%d\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
        break;

      case ref_sw_tolerance:
        TMC429_ParameterGet.Value.Int32=Read429Short(MOTOR_NUMBER(motor_number)<<5|IDX_DX_REFTOLERANCE);
				CMD_TRACE("motor[%d] ref switch tolerance=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case softStop_flag:
        TMC429_ParameterGet.Value.Int32=(Read429SingleByte(IDX_REFCONF_RM|MOTOR_NUMBER(motor_number)<<5, 0) & 0x04) ? 1:0;
				CMD_TRACE("motor[%d] soft stop status=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
			  break;

      case ramp_divisor:
        TMC429_ParameterGet.Value.Byte[0]=Read429SingleByte(IDX_PULSEDIV_RAMPDIV|(motor_number<<5), 2) & 0x0f;
		  	CMD_TRACE("motor[%d] ramp divisor=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case pulse_divisor:
        TMC429_ParameterGet.Value.Byte[0]=Read429SingleByte(IDX_PULSEDIV_RAMPDIV|(motor_number<<5), 2) >> 4;
				CMD_TRACE("motor[%d] pulse divisor=%d\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

			case is_homed:
				CMD_TRACE("motor[%d] homed=%d\n",motor_number,homeInfo.Homed[motor_number]);
				break;
			case is_stop:
				CMD_TRACE("motor[%d] stop=%d\n",motor_number,Read429Short(IDX_VACTUAL|(motor_number<<5))? 0:1) ;
				break;
			case is_reach:

				CMD_TRACE("motor[%d] reach=%d\n",motor_number,(Read429Status() & (0x01<<motor_number*2)) ? 1:0);

				break;
			case home_SenS:
				CMD_TRACE("motor[%d] homeSensor=%d\n",motor_number, rt_pin_read(homeSensorPin[motor_number]) ? 0:1) ;
				break;						
      default:
        TMC429_ParameterGet.Status=REPLY_WRONG_TYPE;
        break;
    }
  } else TMC429_ParameterGet.Status=REPLY_INVALID_VALUE;
}
static void TMC429_SetAxisParameter(uint8_t motor_number, uint8_t parameter_type, int32_t parameter_value)
{
  UCHAR Read[4], Write[4];
  if(motor_number < N_O_MOTORS)
  {
		TMC429_ParameterSet.Value.Int32=parameter_value;
		
    switch(parameter_type)
    {
      case next_position:
        Write429Int(IDX_XTARGET|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				CMD_TRACE("set motor[%d] next position=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case actual_position:
        Write429Int(IDX_XACTUAL|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				CMD_TRACE("set motor[%d] actual position=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case next_speed:
        Write429Short(IDX_VTARGET|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				CMD_TRACE("set motor[%d] next speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case actual_speed:
        Write429Short(IDX_VACTUAL|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				CMD_TRACE("set motor[%d] actual speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);		
        break;

      case max_v_positioning:
        MotorConfig[motor_number].VMax=TMC429_ParameterSet.Value.Int32;
        Write429Short(IDX_VMAX|(motor_number<<5), MotorConfig[motor_number].VMax);
				SetAmaxAutoByspeed(motor_number,MotorConfig[motor_number].VMax);
				CMD_TRACE("set motor[%d] speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);		
        break;

      case max_acc:
        MotorConfig[motor_number].AMax=TMC429_ParameterSet.Value.Int32;
        SetAMax(motor_number, MotorConfig[motor_number].AMax);
				CMD_TRACE("set motor[%d] max acceleration speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);		
        break;

      case rightLimit_disable:	//12
        Write[0]=IDX_REFCONF_RM|TMC429_READ|(motor_number<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        if(CMDGetFromUart.Value.Byte[0]!=0)
          Write[2]=Read[2]|0x02;
        else
          Write[2]=Read[2]&  ~0x02;
        Write[3]=Read[3];
        Write[0]=IDX_REFCONF_RM|(motor_number<<5);
        ReadWrite429(Read, Write);
				if(CMDGetFromUart.Value.Byte[0]!=0)
					CMD_TRACE("motor[%d] right limit switch is disable\n",motor_number);	
				else 
					CMD_TRACE("motor[%d] right limit switch is enable\n",motor_number);	
        break;

      case leftLimit_disable: //13
        Write[0]=IDX_REFCONF_RM|TMC429_READ|(motor_number<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        if(CMDGetFromUart.Value.Byte[0]!=0)
          Write[2]=Read[2]|0x01;
        else
          Write[2]=Read[2]&  ~0x01;
        Write[3]=Read[3];
        Write[0]=IDX_REFCONF_RM|(motor_number<<5);
        ReadWrite429(Read, Write);
				if(CMDGetFromUart.Value.Byte[0]!=0)
					CMD_TRACE("motor[%d] left limit switch is disable\n",motor_number);	
				else 
					CMD_TRACE("motor[%d] left limit switch is enable\n",motor_number);	
        break;

      case min_speed:
        Write429Short(IDX_VMIN|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				CMD_TRACE("set motor[%d] minimum speed=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case ramp_mode:
        Set429RampMode(motor_number, TMC429_ParameterSet.Value.Byte[0]);
				CMD_TRACE("set motor[%d] ramp mode=%d ok\n",motor_number,TMC429_ParameterSet.Value.Byte[0]);
        break;

      case ref_sw_tolerance:
        Write429Short(MOTOR_NUMBER(motor_number)<<5|IDX_DX_REFTOLERANCE, TMC429_ParameterSet.Value.Int32);
				CMD_TRACE("set motor[%d] ref switch tolerance=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
			  break;

      case softStop_flag:
        Read429Bytes(IDX_REFCONF_RM|MOTOR_NUMBER(motor_number)<<5, Read);
        if(TMC429_ParameterSet.Value.Int32!=0)
          Read[1]|=0x04;
        else
          Read[1]&= ~0x04;
        Write429Bytes(IDX_REFCONF_RM|MOTOR_NUMBER(motor_number)<<5, Read);
				CMD_TRACE("set motor[%d] soft stop status=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case ramp_divisor:
        Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(motor_number<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        Write[2]=(Read[2] & 0xf0) | (CMDGetFromUart.Value.Byte[0] & 0x0f);
        Write[3]=Read[3];
        Write[0]=IDX_PULSEDIV_RAMPDIV|(motor_number<<5);
        ReadWrite429(Read, Write);
        MotorConfig[motor_number].RampDiv=CMDGetFromUart.Value.Byte[0] & 0x0f;
				CMD_TRACE("set motor[%d] ramp divisor=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case pulse_divisor:
        Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(motor_number<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        Write[2]=(Read[2] & 0x0f) | (CMDGetFromUart.Value.Byte[0] << 4);
        Write[3]=Read[3];
        Write[0]=IDX_PULSEDIV_RAMPDIV|(motor_number<<5);
        ReadWrite429(Read, Write);
        MotorConfig[motor_number].PulseDiv=CMDGetFromUart.Value.Byte[0]& 0x0f;
				CMD_TRACE("set motor[%d] pulse divisor=%d ok\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;
			case limitSignal:
				if(TMC429_ParameterSet.Value.Int32!=0) 
				{
				  Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR|IFCONF_INV_REF);
				}
				else Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR);
				CMD_TRACE("set limit signal effective trigger level=%d ok\n",TMC429_ParameterSet.Value.Int32);
				break;
				
      default:
        TMC429_ParameterGet.Status=REPLY_WRONG_TYPE;
        break;
    }
  } else TMC429_ParameterGet.Status=REPLY_INVALID_VALUE;
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
static void	printf_cmdList_motor(void)
{
		rt_kprintf("Usage: \n");
		rt_kprintf("motor stop <axisNum>                  - stop motor\n");		
		rt_kprintf("motor rotate <axisNum> <speed>        - rotate motor\n");	
		rt_kprintf("motor moveto <axisNum> <position>     - absolute motion of the motor\n");
		rt_kprintf("motor move <axisNum> <position>       - relative motion of the motor\n");

		rt_kprintf("motor get <type> <axisNum>            - get axis parameter\n");	
		rt_kprintf("motor set <type> <axisNum> <value>    - set axis parameter\n");	
		
		rt_kprintf("motor gohome <axisNum> <speed>        - home sensor as origin position\n");	
		rt_kprintf("motor golimit <axisNum> <speed>       - limit sensor as origin position\n");	
		
		rt_kprintf("motor reset                           - motor reset\n");	
		rt_kprintf("motor stop reset                      - motor reset\n");	
		rt_kprintf("motor reset?                          - is axis reset or not\n");		
}
static void printf_cmdList_motor_set(void)
{
		rt_kprintf("Usage: \n");
		rt_kprintf("motor set speed <axis> <value>             -set the rotate speed \n");				
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
}
static void printf_cmdList_motor_get(void)
{
	rt_kprintf("Usage: \n");
	//用户接口
	rt_kprintf("motor get speed <axis>             -get the current speed \n");
	rt_kprintf("motor get position <axis>          -get the current position\n");
	 
	rt_kprintf("motor get limit all                -get all limit switch status\n");	
	rt_kprintf("motor get rightLimit <axis>        -get right switch status\n");
	rt_kprintf("motor get leftLimit  <axis>        -get left switch status\n");	
	
	rt_kprintf("motor get is_homed <axis>          -is axis homed or not\n");	
	rt_kprintf("motor get is_stop <axis>           -is axis stop or not\n");	
	rt_kprintf("motor get is_reach <axis>          -is axis reach the position\n");	
	rt_kprintf("motor get homeSensor <axis>        -get home sensor status\n");	
	//调试接口
	CMD_TRACE("motor get next_speed <axis>        -get the target speed \n");
	CMD_TRACE("motor get next_position <axis>     -get the target position to move\n");
	
	CMD_TRACE("motor get VMAX <axis>              -get max positioning speed\n");
	CMD_TRACE("motor get AMAX <axis>              -get max acceleration\n");

	CMD_TRACE("motor get position_reached <axis>  -is reach positon or not\n");
	CMD_TRACE("motor get ramp_div <axis>          -get ramp divisor value\n");
	CMD_TRACE("motor get pulse_div <axis>         -get pulse divisor value\n");			
	
}
/*********************************************************************************************************************/
void  tmc429_hw_init(void)
{
	pinMode(POSCMP1_PIN, PIN_MODE_INPUT_PULLUP);
    pinMode(INTOUT1_PIN, PIN_MODE_INPUT_PULLUP);	
	
	MX_SPI_Init();
	
#if defined(USING_INC_MBTMC429) 
	MX_TIM3_Init();	
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
#endif
	
	TMC429_ParameterPresetToRam();
	
	Init429();

//板卡上电后已当前位置为原点，正常应用需要回原点动作
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
********************************************************************/
uint8_t TMC429_MotorRotate(uint8_t motor_number, int32_t motor_speed)
{
  if(motor_number < N_O_MOTORS)
  {
		//MotorStopByLimit[motor_number]=1;	
    SpeedChangedFlag[motor_number]=TRUE;
		
    Set429RampMode(motor_number, RM_VELOCITY);
		
	SetAmaxAutoByspeed(motor_number,abs(motor_speed));
	  
    Write429Short(IDX_VMAX|(motor_number<<5), 2047);
	  
    Write429Short(IDX_VTARGET|(motor_number<<5), motor_speed);
	

	rt_kprintf("motor%d is rotate at %d\n",motor_number,motor_speed);
	return 0;	
  }
  return 1;
}
/***************************************************************//**
  \fn MotorStop(void)
  \brief Command MST

  MST (Motor StoP) command (see TMCL manual).
********************************************************************/
uint8_t TMC429_MotorStop(uint8_t motor_number)
{
  if(motor_number < N_O_MOTORS)
  {	
		/*  立刻停止  */
		HardStop(motor_number);
		//StopMotorByRamp(motor_number);
		/* 回原点过程中响应停止命令的变量置位 */
		homeInfo.GoHome[motor_number]=0;
		homeInfo.GoLimit[motor_number]=0;
		/* 电机停止会自动返回当前位置 */
		rt_kprintf("motor[%d] is stop and P[%d]=%d\n",motor_number,motor_number,Read429Int(IDX_XACTUAL|(motor_number<<5)));
		return 0;
  }
	return 1;
}
void StopMotorByRamp(UINT Motor)
{
  Set429RampMode(MOTOR_NUMBER(Motor), RM_VELOCITY);
  Write429Zero((MOTOR_NUMBER(Motor)<<5)|IDX_VTARGET);
}

uint8_t SPI_LockByCMD=0;

int motor(int argc, char **argv)
{
	 CMDGetFromUart.Type=Type_none;
	
	 TMC429_ParameterGet.Status=REPLY_OK;
	
    int result = RT_ERROR;
		
		if (argc == 2 )
		{
			if (!strcmp(argv[1], "reset?"))
			{
				CMD_TRACE("motor reset=%d\n",motorsReset_InOrder==OK_2 ? 1:0) ;
				return RT_EOK	; 				
			}
			else	if (!strcmp(argv[1], "reset"))
			{
				MotorAutoReset_preset();
				return RT_EOK	; 
			}
			else	if (!strcmp(argv[1], "set"))
			{
				printf_cmdList_motor_set();
				return RT_EOK	; 
			}
			else	if (!strcmp(argv[1], "get"))
			{
				printf_cmdList_motor_get();
				return RT_EOK	; 
			}
      else
			{		
				result =RT_ERROR;//REPLY_INVALID_CMD;
			}     
		}
		if (argc == 3 )
		{
			CMDGetFromUart.Motor=atoi(argv[2]);
			if (!strcmp(argv[1], "stop"))
			{
				if (!strcmp(argv[2], "reset"))
				{
					buttonRESETpressed=FALSE;
					homeInfo.GoHome[0]=0;
					homeInfo.GoHome[1]=0;
					homeInfo.GoHome[2]=0;
					StopMotorByRamp(0);StopMotorByRamp(1);StopMotorByRamp(2);return RT_EOK;//TMC429_MotorStop(2);
				}
				if(buttonRESETpressed==TRUE || homeInfo.GoHome[2]==1) {CMD_TRACE("motor is reseting\n");return RT_EOK;}
				if(CMDGetFromUart.Motor==AXIS_Z) 
				{	
					closeSerial();
				}	

				if(!TMC429_MotorStop(CMDGetFromUart.Motor))	//电机停止后自动会关闭串口打印压力
				{
//					homeInfo.GoHome[0]=FALSE;		homeInfo.GoHome[1]=FALSE;		homeInfo.GoHome[2]=FALSE;	
//					homeInfo.GoLimit[0]=FALSE;	homeInfo.GoLimit[1]=FALSE;	homeInfo.GoLimit[2]=FALSE;						
//					buttonRESETpressed=FALSE;								  //只要有轴停止啊，就可以开启下次复位功能，复位过去被停止的话									
					return RT_EOK	;
				}
				result = RT_EINVAL;
			}
		}
		else if (argc == 4)
		{	
			CMDGetFromUart.Motor=atoi(argv[2]);
			CMDGetFromUart.Value.Int32=atoi(argv[3]);
			if (!strcmp(argv[1], "get"))
			{
				if (!strcmp(argv[2], "speed")) 	 					CMDGetFromUart.Type=actual_speed;
				if (!strcmp(argv[2], "position")) 				CMDGetFromUart.Type=actual_position;
				if (!strcmp(argv[2], "is_homed")) 				CMDGetFromUart.Type=is_homed;
				if (!strcmp(argv[2], "is_stop")) 					CMDGetFromUart.Type=is_stop;
				if (!strcmp(argv[2], "is_reach")) 				CMDGetFromUart.Type=is_reach;
			
				if (!strcmp(argv[2], "next_speed")) 			CMDGetFromUart.Type=next_speed;
				if (!strcmp(argv[2], "next_position")) 	 	CMDGetFromUart.Type=next_position;		
				if (!strcmp(argv[2], "VMAX")) 					 	CMDGetFromUart.Type=max_v_positioning;
				if (!strcmp(argv[2], "AMAX")) 		 				CMDGetFromUart.Type=max_acc;
				if (!strcmp(argv[2], "position_reached")) CMDGetFromUart.Type=position_reached;		
				
				if (!strcmp(argv[2], "rightLimit")) 			CMDGetFromUart.Type=rightLimit_SwS;
				if (!strcmp(argv[2], "leftLimit")) 				CMDGetFromUart.Type=leftLimit_SwS;
				if (!strcmp(argv[2], "limit")) 			      CMDGetFromUart.Type=allLimit_SwS;
				
				if (!strcmp(argv[2], "homeSensor")) 			CMDGetFromUart.Type=home_SenS;
				
				if (!strcmp(argv[2], "rightLimit?"))			CMDGetFromUart.Type=rightLimit_disable;
				if (!strcmp(argv[2], "leftLimit?")) 			CMDGetFromUart.Type=leftLimit_disable;
				
				if (!strcmp(argv[2], "ramp_div")) 			  CMDGetFromUart.Type=ramp_divisor;
				if (!strcmp(argv[2], "pulse_div")) 				CMDGetFromUart.Type=pulse_divisor;	
				
//				if (!strcmp(argv[2], "max_current")) 	   	CMDGetFromUart.Type=max_current;
//				if (!strcmp(argv[2], "standby_current")) 	CMDGetFromUart.Type=standby_current;
//				if (!strcmp(argv[2], "Llimit_off")) 			CMDGetFromUart.Type=leftLimit_disable;
				
				if(CMDGetFromUart.Type!=Type_none)
				{
					CMDGetFromUart.Motor=atoi(argv[3]);
					TMC429_GetAxisParameter(CMDGetFromUart.Motor,CMDGetFromUart.Type);
					if(TMC429_ParameterGet.Status==REPLY_OK) return RT_EOK;
					result=RT_EINVAL;
				}
				else 
				{
					printf_cmdList_motor_get();
					result =REPLY_INVALID_CMD;
				}
			}		
			//报警后所有的正向运动都屏蔽
//			if(pressureAlarm == TRUE && CMDGetFromUart.Value.Int32>0 && CMDGetFromUart.Motor==2 )  
//			{
//				CMD_TRACE("pressure overhigh, motor z is forbidden to move downward\n");	
//				return RT_EOK;				
//			}
			//复位状态下屏蔽电机运行控制
			if(buttonRESETpressed==TRUE )
			{	
				CMD_TRACE("motor is resetting\n");
				return RT_EOK;
			}
			else 
			{
				if (!strcmp(argv[1], "rotate"))
				{
					if(!(TMC429_MotorRotate(CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32)))	return RT_EOK	;
					result = RT_EINVAL;
				}
				if (!strcmp(argv[1], "move") )
				{			
						if(!TMC429_MoveToPosition(CMDGetFromUart.Motor,  MVP_REL, CMDGetFromUart.Value.Int32))	return RT_EOK;
						result =RT_EINVAL;
				}
				if (!strcmp(argv[1], "moveto"))
				{
					if( (pressureAlarm == TRUE || INPUT8==0 )  &&  CMDGetFromUart.Motor==2 )  
					{
						if(CMDGetFromUart.Value.Int32 > Read429Int(IDX_XACTUAL|(CMDGetFromUart.Motor<<5)))
						{
							CMD_TRACE("pressure overhigh, motor z is forbidden to move downward\n");	
							return RT_EOK;				
						}
					}
					closeSerial();
					if(!TMC429_MoveToPosition(CMDGetFromUart.Motor, MVP_ABS, CMDGetFromUart.Value.Int32))	return RT_EOK;
					result =RT_EINVAL;
				}
				if (!strcmp(argv[1], "gohome"))
				{
						closeSerial();
						
						Stop_HardTimer();		
			
						homeInfo.GoHome[CMDGetFromUart.Motor] = TRUE;
						homeInfo.GoLimit[CMDGetFromUart.Motor]= FALSE;
						homeInfo.Homed[CMDGetFromUart.Motor]	= FALSE;

						homeInfo.HomeSpeed[CMDGetFromUart.Motor]=CMDGetFromUart.Value.Int32;	
					
						SetAmaxAutoByspeed(CMDGetFromUart.Motor,abs(CMDGetFromUart.Value.Int32));
			
						TMC429_MotorRotate(CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32);
						
						CMD_TRACE("motor[%d] is start go home by searching home sensor\n",CMDGetFromUart.Motor);
										
						Start_HardTimer();
					
						return RT_EOK	;
				}
				if (!strcmp(argv[1], "golimit"))
				{			
						Stop_HardTimer();					
						homeInfo.GoHome[CMDGetFromUart.Motor]	=FALSE;
						homeInfo.GoLimit[CMDGetFromUart.Motor]=TRUE;
						homeInfo.Homed[CMDGetFromUart.Motor]	=FALSE;
						homeInfo.HomeSpeed[CMDGetFromUart.Motor]=CMDGetFromUart.Value.Int32;	
						SetAmaxAutoByspeed(CMDGetFromUart.Motor,abs(CMDGetFromUart.Value.Int32));
						TMC429_MotorRotate(CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32);
						Start_HardTimer();					
						return RT_EOK	;
				}
			}	

		}
		else if (argc == 5)
		{
			if (!strcmp(argv[1], "set"))
			{
				//user
				if (!strcmp(argv[2], "speed")) 					 	CMDGetFromUart.Type=max_v_positioning;
					
				//debug
				if (!strcmp(argv[2], "next_speed")) 			CMDGetFromUart.Type=next_speed;		
				if (!strcmp(argv[2], "next_position")) 	 	CMDGetFromUart.Type=next_position;
				if (!strcmp(argv[2], "actual_position")) 	CMDGetFromUart.Type=actual_position;
				if (!strcmp(argv[2], "actual_speed")) 	 	CMDGetFromUart.Type=actual_speed;
				if (!strcmp(argv[2], "VMAX")) 					  CMDGetFromUart.Type=max_v_positioning;
				if (!strcmp(argv[2], "AMAX")) 		 				CMDGetFromUart.Type=max_acc;
				//设置限位失能无效，需要调试原因
				if (!strcmp(argv[2], "rightLimit"))				CMDGetFromUart.Type=rightLimit_disable;
				if (!strcmp(argv[2], "leftLimit")) 				CMDGetFromUart.Type=leftLimit_disable;	  		
				if (!strcmp(argv[2], "limitSignal")) 		  CMDGetFromUart.Type=limitSignal;		
				if (!strcmp(argv[2], "ramp_div")) 			  CMDGetFromUart.Type=ramp_divisor;
				if (!strcmp(argv[2], "pulse_div")) 				CMDGetFromUart.Type=pulse_divisor;
//				if (!strcmp(argv[2], "max_current")) 	   	CMDGetFromUart.Type=max_current;
//				if (!strcmp(argv[2], "standby_current")) 	CMDGetFromUart.Type=standby_current;
				if(CMDGetFromUart.Type!=Type_none)	
				{
					CMDGetFromUart.Motor=atoi(argv[3]);
					CMDGetFromUart.Value.Int32=atoi(argv[4]);
					TMC429_SetAxisParameter(CMDGetFromUart.Motor, CMDGetFromUart.Type, CMDGetFromUart.Value.Int32);
					if(TMC429_ParameterGet.Status==REPLY_OK) return RT_EOK;
					else result =RT_EINVAL;
				}
				else 
				{
					printf_cmdList_motor_set();
					result =REPLY_INVALID_CMD;														
				}				
			}

		}		
		if(	result == RT_EINVAL )
		{
			rt_kprintf("motor number must be 0~2\n",CMDGetFromUart.Motor);
		}
		else if( result == RT_ERROR )
		{
			printf_cmdList_motor();
		}
	  return result;
}

//
static rt_uint8_t motion_stack[ 512 ];
static struct rt_thread motion_thread;
static void motion_thread_entry(void *parameter);
//////////////////////////////////////////////////////////
static void MotorKeepStop_removeLimitSigal(uint8_t motor_number)
{
	if(homeInfo.GoHome[motor_number]==FALSE && homeInfo.GoLimit[motor_number]==FALSE)	//不是回原点动作
	{	
		if(MotorStopByLimit[motor_number]==1)
		{
			uint8_t SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);				//限位状态读取
			//CMD_TRACE("%d GoHome=%d,GoLimit=%d,SwitchStatus=%d,V=%d\n",motor_number,homeInfo.GoHome[motor_number],homeInfo.GoLimit[motor_number],SwitchStatus,Read429Short(IDX_VACTUAL|(motor_number<<5)));

			//左				需要再判断速度													右              //碰到限位停止，需要再次发命令运动
			if(((((SwitchStatus& (0x02<<motor_number*2))?1:0) ==1)	||  (((SwitchStatus& (0x01<<motor_number*2)) ?1:0)==1))	\
				   && ( Read429Short(IDX_VACTUAL|(motor_number<<5))==0 )	)						 
			{					
					
					TMC429_MotorStop(motor_number);		MotorStopByLimit[motor_number]=0;
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
					MotorKeepStop_removeLimitSigal(i);
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
                            20);
    if (result == RT_EOK)
    {
       rt_thread_startup(&motion_thread);
    }
    return 0;
}

//



