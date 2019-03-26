#include "bsp.h"
//#include "app_tmc429.h"

#define AXIS_X	0
#define AXIS_Y	1
#define AXIS_Z	2

#if	(1)
    #define MOTION_TRACE         rt_kprintf
#else
    #define MOTION_TRACE(...)
#endif

u8 OriginSignal_LowEN=TRUE;

uint8_t OriginPin[3]={3,4,5};  						//PE_4	PE5	 PE6

u8 SpeedDirection[3]={DIR_NONE,DIR_NONE,DIR_NONE};
u8 MotorStop=FALSE;
u8 MotorStop_PIN=NULL;
u8 MotorStop_VALUE=LOW;

home_t Homming=
{
	{FALSE,FALSE,FALSE},
	{FALSE,FALSE,FALSE},
	{DIR_NONE,DIR_NONE,DIR_NONE},
	{DIR_NONE,DIR_NONE,DIR_NONE},
	{0,0,0},
};

UCHAR	SpeedChangedFlag[3]={FALSE,FALSE,FALSE};

void TMC429_RampSetup(void);
void SetAmaxAuto(u8 ActualMotor,int speed);
void SetCurPositionAsOrigin(u8 axis) ;
void motor_RotateRight(UCHAR Motor, int speed);
void motor_RotateLeft(UCHAR Motor, int speed);
void MotorStop_SignalGet(void);
void TriggerLimitProcess(void);
void TriggerOriginProcess(void);

void TMC429_RampSetup(void)
{
  uint8_t i;
	for(i=0; i<N_O_MOTORS; i++) 	//默认电机主要参数配置，主要为速度和加速度
	{																  
		MotorConfig[i].VMax=100;   //  3R/S  
		MotorConfig[i].AMax=500;
		MotorConfig[i].PulseDiv=5;
		MotorConfig[i].RampDiv=8;	
		SpeedChangedFlag[i]=TRUE;
	}
}

void rt_hw_tmc429_init(void)
{
	pinMode(&PE12, GPIO_MODE_INPUT);
  pinMode(&PE13, GPIO_MODE_INPUT);	
	time_pwm_hw_init();	 //12MHZ
	drv_mdelay(10);
	spi_hw_init();

	TMC429_RampSetup();
	Init429();
}

//12MHZ|10000steps//2047-->17.57/M;					0.29R/S			100--0.01R/S
void SetAmaxAuto(u8 ActualMotor,int speed)
{
	if(MotorConfig[ActualMotor].PulseDiv==7&&	MotorConfig[ActualMotor].RampDiv==10)
	{
		if (speed<25)
		{		SetAMax(ActualMotor, speed+5);	MotorConfig[ActualMotor].AMax=speed+5;}			
		else if (speed<100)
		{		SetAMax(ActualMotor, speed+10);	MotorConfig[ActualMotor].AMax=speed+10;}
		else if (speed<150)
		{		SetAMax(ActualMotor, speed+30);MotorConfig[ActualMotor].AMax=speed+30;}
		else if (speed<280)
		{		SetAMax(ActualMotor, speed+60);MotorConfig[ActualMotor].AMax=speed+60;}
		else if (speed<420)
		{		SetAMax(ActualMotor, speed+100);MotorConfig[ActualMotor].AMax=speed+100;}
		else if (speed<560)
		{		SetAMax(ActualMotor, speed+150);MotorConfig[ActualMotor].AMax=speed+150;}
		else if (speed<700)
		{		SetAMax(ActualMotor, speed+200);MotorConfig[ActualMotor].AMax=speed+200;}
		else if (speed<840)
		{		SetAMax(ActualMotor, speed+250);MotorConfig[ActualMotor].AMax=speed+250;}
		else if (speed<980)
		{		SetAMax(ActualMotor, speed+300);MotorConfig[ActualMotor].AMax=speed+300;}
		else if (speed<=1180)
		{		SetAMax(ActualMotor, speed+350);MotorConfig[ActualMotor].AMax=speed+350;}
		else if (speed<2047)
		{		SetAMax(ActualMotor, speed+600);MotorConfig[ActualMotor].AMax=speed+600;}	
	}
	else if(MotorConfig[ActualMotor].PulseDiv==5 &&	MotorConfig[ActualMotor].RampDiv==8)
	{
		if (speed<25)	//1~24
		{		SetAMax(ActualMotor, speed);	MotorConfig[ActualMotor].AMax=speed;}			
		else if (speed<50)
		{		SetAMax(ActualMotor, speed+5);	MotorConfig[ActualMotor].AMax=speed+5;}
		else if (speed<100)
		{		SetAMax(ActualMotor, 100);	MotorConfig[ActualMotor].AMax=100;}
		else if (speed<150)
		{		SetAMax(ActualMotor, 500);MotorConfig[ActualMotor].AMax=500;}
		else if (speed<250)
		{		SetAMax(ActualMotor, 1000);MotorConfig[ActualMotor].AMax=1000;}
		else if (speed<2047)
		{		SetAMax(ActualMotor, 2047);MotorConfig[ActualMotor].AMax=2047;}	
	}
}

void SetCurPositionAsOrigin(u8 axis) 
{
	HardStop(axis);
	drv_mdelay(10);
	
	Write429Int(IDX_XACTUAL|(axis<<5), 0);
	Write429Int(IDX_XTARGET|(axis<<5), 0);
	
	Homming.Enable[axis]=FALSE;
	Homming.completed[axis]=TRUE;
	Homming.SearchOriginDirection[axis]=DIR_NONE;
	Homming.SearchLimitDirection[axis]=DIR_NONE;
	SpeedDirection[axis]=DIR_NONE;
	
	MOTION_TRACE("<motor%d reset ok>\r\n>>",axis+1);
}
void motor_RotateRight(UCHAR Motor, int speed)
{
	if(MotorStop==FALSE)
	{
		if(Motor<N_O_MOTORS && (abs(speed)<2048))
		{
			SpeedChangedFlag[Motor]=TRUE;
			Set429RampMode(Motor, RM_VELOCITY);
			Write429Short((IDX_VMAX|Motor<<5), 2047);
			Write429Short(IDX_VTARGET|(Motor<<5), speed);
			SpeedDirection[Motor]=DIR_R;
		}
	}
}
void motor_RotateLeft(UCHAR Motor, int speed)
{
	if(MotorStop==FALSE)
	{
		if(Motor<N_O_MOTORS && (abs(speed)<2048))
		{
			SpeedChangedFlag[Motor]=TRUE;
			Set429RampMode(Motor, RM_VELOCITY);
			Write429Short((IDX_VMAX|Motor<<5), 2047);
			Write429Short(IDX_VTARGET|(Motor<<5),-speed);
			SpeedDirection[Motor]=DIR_L;
		}
	}
}
//
void MotorStop_SignalGet(void)
{
	switch(MotorStop_PIN)
	{
		case 1:		if(MotorStop_VALUE==0){if(pinRead(&PB5)==0) MotorStop=1;else MotorStop=0;}
							if(MotorStop_VALUE==1){if(pinRead(&PB5)==1) MotorStop=1;else MotorStop=0;}	
			break;
							
		case 2:		if(MotorStop_VALUE==0){if(pinRead(&PB6)==0) MotorStop=1;else MotorStop=0;}
							if(MotorStop_VALUE==1){if(pinRead(&PB6)==1) MotorStop=1;else MotorStop=0;}	
			break;	
														
		case 3:		if(MotorStop_VALUE==0){if(pinRead(&PB7)==0) MotorStop=1;else MotorStop=0;}
							if(MotorStop_VALUE==1){if(pinRead(&PB7)==1) MotorStop=1;else MotorStop=0;}	
			break;
							
		case 4:		if(MotorStop_VALUE==0){if(pinRead(&PB8)==0) MotorStop=1;else MotorStop=0;}
							if(MotorStop_VALUE==1){if(pinRead(&PB8)==1) MotorStop=1;else MotorStop=0;}	
			break;
							
		case 5:		if(MotorStop_VALUE==0){if(pinRead(&PB9)==0) MotorStop=1;else MotorStop=0;}
							if(MotorStop_VALUE==1){if(pinRead(&PB9)==1) MotorStop=1;else MotorStop=0;}	
			break;
							
		case 6:		if(MotorStop_VALUE==0){if(pinRead(&PE0)==0) MotorStop=1;else MotorStop=0;}
							if(MotorStop_VALUE==1){if(pinRead(&PE0)==1) MotorStop=1;else MotorStop=0;}	
			break;
							
		case 7:		if(MotorStop_VALUE==0){if(pinRead(&PE1)==0) MotorStop=1;else MotorStop=0;}
							if(MotorStop_VALUE==1){if(pinRead(&PE1)==1) MotorStop=1;else MotorStop=0;}	
			break;
							
		case 8:		if(MotorStop_VALUE==0){if(pinRead(&PE2)==0) MotorStop=1;else MotorStop=0;}
							if(MotorStop_VALUE==1){if(pinRead(&PE2)==1) MotorStop=1;else MotorStop=0;}	
			break;
							
		case 9:		if(MotorStop_VALUE==0){if(pinRead(&PE3)==0) MotorStop=1;else MotorStop=0;}
							if(MotorStop_VALUE==1){if(pinRead(&PE3)==1) MotorStop=1;else MotorStop=0;}	
			break;
							
		default:	MotorStop=0;
			break;
		
	}
}
//
void TriggerLimitProcess(void)
{
	u8 SwitchStatus;
	u8 i;
	if(MotorStop==FALSE)
	{
		SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3); 					//读取限位状态	
		for(i=0;i<3;i++)
		{
			if(((SwitchStatus & (0x02<<i*2))?1:0) ==TRUE)								//触发左限位
			{
				if(Homming.Enable[i]==TRUE)																//收到回原点命令
				{
					if(Homming.SearchLimitDirection[i]==DIR_L)							//收到的是找左限位命令
					{
						SetCurPositionAsOrigin(i);														//找到左限位，回原点OK
					}													
					else 
					{
						motor_RotateRight(i,Homming.HommingSpeed[i]);					//否则右转,寻找原点信号
					}
				}
				else if(SpeedDirection[i]==DIR_L)												  //电机正常运动碰到左限位,需要设置软件停止，防止限位消除后又继续运动
				{
					SpeedDirection[i]=DIR_NONE;	HardStop(i);
				} 	
			}
			if(((SwitchStatus & (0x01<<i*2))?1:0) ==TRUE)								//触发右限位
			{
				if(Homming.Enable[i]==TRUE)
				{
					if(Homming.SearchLimitDirection[i]==DIR_R) 							//收到的是找右限位命令
					{
						SetCurPositionAsOrigin(i);														//找到右限位，回原点OK		
					}
					else 
					{
						motor_RotateLeft(i,Homming.HommingSpeed[i]);					//否则左转,寻找原点信号
					}
				}
				else if(SpeedDirection[i]==DIR_R)													//电机正常运动碰到右限位,需要设置软件停止，防止限位消除后又继续运动
				{
					SpeedDirection[i]=DIR_NONE;	HardStop(i);						
				} 
			}		
		}
	}
	else																														//外部输入控制电机运动停止
	{
		HardStop(0);	HardStop(1);	HardStop(2);
	}
}
//
void TriggerOriginProcess(void)
{
	u8 i;
	if(MotorStop==FALSE)
	{
		for(i=0;i<3;i++)
		{
			if(Homming.Enable[i]==TRUE && OriginSignal_LowEN==TRUE  && rt_pin_read(OriginPin[i])==0 || \
				 Homming.Enable[i]==TRUE && OriginSignal_LowEN==FALSE && rt_pin_read(OriginPin[i])==1
				)																
			//已收到回原点命令且触发原点信号
			{
				if(Homming.SearchOriginDirection[i]==DIR_L)							//收到的是找左限位命令
				{
					if((Read429Short(IDX_VACTUAL|(i<<5)))<0);
					else if((Read429Short(IDX_VACTUAL|(i<<5)))>0)
					{
						SetCurPositionAsOrigin(i);			
					}													
				}													

				else if(Homming.SearchOriginDirection[i]==DIR_R)							//收到的是找左限位命令
				{
					if((Read429Short(IDX_VACTUAL|(i<<5)))>0);
					else if((Read429Short(IDX_VACTUAL|(i<<5)))<0)
					{
						SetCurPositionAsOrigin(i);			
					}														
				}	
			}
		}
	}
}
//
/********************call by shell*********************************
*/
/* 配置输入信号为控制电机停止的条件
*/
void motor_disable(char* input,u8 value)
{
	if(strcmp("input1",input)==0) 		 MotorStop_PIN=1;	
	else if(strcmp("input2",input)==0) MotorStop_PIN=2;	
	else if(strcmp("input3",input)==0) MotorStop_PIN=3;	
	else if(strcmp("input4",input)==0) MotorStop_PIN=4;	
	else if(strcmp("input5",input)==0) MotorStop_PIN=5;	
	else if(strcmp("input6",input)==0) MotorStop_PIN=6;	
	else if(strcmp("input7",input)==0) MotorStop_PIN=7;	
	else if(strcmp("input8",input)==0) MotorStop_PIN=8;	
	else if(strcmp("input9",input)==0) MotorStop_PIN=9;
	else 															 MotorStop_PIN=NULL;		
	at24cxx.writeU8(at24c256,ADDR_EN_PIN,MotorStop_PIN);
	at24cxx.writeU8(at24c256,ADDR_EN_VALUE,value);
	MOTION_TRACE("<ok>");
}


/* 电机限位触发信号有效值取反
*/
void motor_limitINV(void)
{
	Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR|IFCONF_INV_REF);
	MOTION_TRACE("<ok>");
}
/* 电机原点信号有效值取反命令
*/
void motor_orgINV(void)
{
	OriginSignal_LowEN=FALSE;
	at24cxx.writeU8(at24c256,ADDR_ORG,HIGH);
	MOTION_TRACE("<ok>");
}



/* 电机回原点是否OK
*/
void motor_isRESET(UCHAR Motor) 
{
	if(0<Motor && Motor<N_O_MOTORS+1 )
	{
		Motor=Motor-1;
		MOTION_TRACE("<%d>",Homming.completed[Motor]);
	}
}

/* 电机旋转运动命令
*/
void motor_rotate(UCHAR Motor, int speed)
{
	if(MotorStop==FALSE)
	{
		if(0<Motor && Motor<N_O_MOTORS+1 && (abs(speed)<2048))
		{
			Motor=Motor-1;
			if(speed>0)	SpeedDirection[Motor]=DIR_R; else SpeedDirection[Motor]=DIR_L; 
			SpeedChangedFlag[Motor]=TRUE;
			Set429RampMode(Motor, RM_VELOCITY);
			Write429Short((IDX_VMAX|Motor<<5), 2047);
			Write429Short(IDX_VTARGET|(Motor<<5), speed);
			MOTION_TRACE("<motor is rotating>");
			if(Read429Short(IDX_VACTUAL|(Motor<<5))>0)			SpeedDirection[Motor]=DIR_R;
			else if(Read429Short(IDX_VACTUAL|(Motor<<5))<0) SpeedDirection[Motor]=DIR_L;
			else SpeedDirection[Motor]=DIR_NONE;
		}
		else MOTION_TRACE("<motor number must be 1~3, speed must be -2047~2047>");
	}
	else MOTION_TRACE("<motor is disable by input%d>",MotorStop_PIN);
}
/* 电机停止运动命令
*/
void motor_stop(UCHAR Motor)
{
  if(0<Motor && Motor<N_O_MOTORS+1 )
  {
		Motor=Motor-1;
		HardStop(Motor);
		MOTION_TRACE("<ok>");
  }
	else MOTION_TRACE("<motor number must be 1~3>");
}
/* 电机绝对运动和相对运动命令
*/
void motor_move(UCHAR Motor,char* Type, int Position)
{
	if(MotorStop==FALSE)
	{
		if(0<Motor && Motor<N_O_MOTORS+1 && (abs(Position)<8388607))
		{
			Motor=Motor-1;
			if(Type[0]==MVP_ABS || Type[0]==MVP_REL)
			{
				if(SpeedChangedFlag[Motor])
				{
					Write429Short(IDX_VMAX|(Motor<<5), MotorConfig[Motor].VMax);
					SetAMax(Motor, MotorConfig[Motor].AMax);
					SpeedChangedFlag[Motor]=FALSE;
				}

				if(Type[0]==MVP_ABS )
				{
					if(Type[1]=='-' ) Write429Int(IDX_XTARGET|(Motor<<5), -Position);
					else Write429Int(IDX_XTARGET|(Motor<<5), Position);
					
				}
				else 
				{
					if(Type[1]=='-' ) Write429Int(IDX_XTARGET|(Motor<<5), -Position+Read429Int(IDX_XACTUAL|(Motor<<5)));		
					else Write429Int(IDX_XTARGET|(Motor<<5), Position+Read429Int(IDX_XACTUAL|(Motor<<5)));		
				}
			}
			Set429RampMode(Motor, RM_RAMP);
			MOTION_TRACE("<ok>");
			if(Read429Short(IDX_VACTUAL|(Motor<<5))>0)			SpeedDirection[Motor]=DIR_R;
			else if(Read429Short(IDX_VACTUAL|(Motor<<5))<0) SpeedDirection[Motor]=DIR_L;
			else SpeedDirection[Motor]=DIR_NONE;
		}
		else MOTION_TRACE("<motor number must be 1~3,position must be 0~8388607>");
	}
	else MOTION_TRACE("<motor is disable by input%d>",MotorStop_PIN);
}
/* 设置电机运动参数命令
*/
void motor_set(UCHAR Motor, char* Type,long Value)
{
  UCHAR Read[4], Write[4];u8 flag=1;

  if(0<Motor && Motor<N_O_MOTORS+1 )
  {
		Motor=Motor-1;
		
    switch(Type[0])
    {
      //Write429Int(IDX_XTARGET|(Motor<<5), Value);
      //Write429Int(IDX_XACTUAL|(Motor<<5), Value);
			//Write429Short(IDX_VACTUAL|(Motor<<5), Value);
			//Write429Short(IDX_VTARGET|(Motor<<5), Value);
      case 'V':
        MotorConfig[Motor].VMax=Value;
        Write429Short(IDX_VMAX|Motor<<5, MotorConfig[Motor].VMax);
				SetAmaxAuto(Motor,abs(Value));
        break;

      case 'A':
        MotorConfig[Motor].AMax=Value;
        SetAMax(Motor, MotorConfig[Motor].AMax);
        break;

      case 'm':
        Set429RampMode(Motor, Value);
        break;

      case 'r':
        Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(Motor<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        Write[2]=(Read[2] & 0xf0) | (Value & 0x0f);
        Write[3]=Read[3];
        Write[0]=IDX_PULSEDIV_RAMPDIV|(Motor<<5);
        ReadWrite429(Read, Write);
        MotorConfig[Motor].RampDiv=Value & 0x0f;
        break;

      case 'p':
        Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(Motor<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        Write[2]=(Read[2] & 0x0f) | (Value << 4);
        Write[3]=Read[3];
        Write[0]=IDX_PULSEDIV_RAMPDIV|(Motor<<5);
        ReadWrite429(Read, Write);
        MotorConfig[Motor].PulseDiv=Value& 0x0f;
        break;

      default:	flag=0;
        break;
    }
		if(flag)	MOTION_TRACE("<ok>");
		else	  	MOTION_TRACE("<type err>");	
  } 
	else MOTION_TRACE("<motor number must be 1~3>");
}
/* 读取电机设置的参数命令
*/
void motor_get(UCHAR Motor,char* Type)
{
	long Value=0; u8 flag=1;
	if(0<Motor && Motor<N_O_MOTORS+1 )
	{
		Motor=Motor-1;
	
    switch(Type[0])
    {
      case 'X':
        Value=Read429Int(IDX_XACTUAL|(Motor<<5));
        break;
      case 'Y':
        Value=Read429Int(IDX_XTARGET|(Motor<<5));
        break;

      case 'V':
        Value=MotorConfig[Motor].VMax;
        break;

      case 'v':
        Value=Read429Short(IDX_VACTUAL|(Motor<<5));
        break;

      case 'A':
        Value=MotorConfig[Motor].AMax;
        break;

      case 'S':	//position reached
        if(Read429Status() & 0x01) Value=1;
        break;

      case 'L':
        Value=(Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<Motor*2)) ? 1:0;
        break;

      case 'R':
        Value=(Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x01<<Motor*2)) ? 1:0;
        break;

      case 'm':
        Value=Read429SingleByte(IDX_REFCONF_RM|(Motor<<5), 3);
        break;

      case 'r':
        Value=Read429SingleByte(IDX_PULSEDIV_RAMPDIV|(Motor<<5), 2) & 0x0f;
        break;

      case 'p':
        Value=Read429SingleByte(IDX_PULSEDIV_RAMPDIV|(Motor<<5), 2) >> 4;
        break;

      default:flag=0;
        
        break;
    }
		if(flag)	MOTION_TRACE("<%d>",Value);
		else	  	MOTION_TRACE("<type err>");	
  }
	else MOTION_TRACE("<motor number must be 1~3");
}
/* 电机以设置的速度向设置的方向运动，触碰到限位后再反发现找原点	
*/
void motor_goHOME(UCHAR Motor,char* Dir,int speed)
{
	if(MotorStop==FALSE)
	{
		if(0<Motor && Motor<N_O_MOTORS+1 )
		{
			Motor=Motor-1;
			Homming.completed[Motor]=FALSE;
			Homming.Enable[Motor]=TRUE;
			Homming.SearchOriginDirection[Motor]=Dir[0];
			Homming.HommingSpeed[Motor]=speed;
			HardStop(Motor);
			
			if(Dir[0]=='R')				{motor_RotateRight(Motor,speed);SpeedDirection[Motor]=DIR_R;}
			else if(Dir[0]=='L')	{motor_RotateLeft(Motor,speed);SpeedDirection[Motor]=DIR_L;}
			MOTION_TRACE("<motor%d start go home>",Motor+1);		
		}
	
	}
	else MOTION_TRACE("<motor is disable by input%d>",MotorStop_PIN);		
}
/* 电机以设置的速度向设置的方向运动，触碰到限位后停止
*/
void motor_goLIMIT(UCHAR Motor,char* Dir,int speed)
{
	if(MotorStop==FALSE)
	{
		if(0<Motor && Motor<N_O_MOTORS+1 )
		{
			Motor=Motor-1;
			Homming.completed[Motor]=FALSE;
			Homming.Enable[Motor]=TRUE;
			Homming.SearchLimitDirection[Motor]=Dir[0];
			Homming.HommingSpeed[Motor]=speed;
			if(Dir[0]=='R')				motor_RotateRight(Motor,speed);
			else if(Dir[0]=='L')	motor_RotateLeft(Motor,speed);
			MOTION_TRACE("<motor%d start go limit>",Motor+1);	
		}
	}
	else MOTION_TRACE("<motor is disable by input%d>",MotorStop_PIN);
}
/********************CALL BY FUNCTION*********************************
*/

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t motor_stack[ 512 ];

static struct rt_thread motor_thread;


void motor_thread_entry(void *parameter)
{
    while (1)
    {  

			MotorStop_SignalGet();
			TriggerLimitProcess();
			TriggerOriginProcess();
			rt_thread_delay(1);
    }
}

int motor_thread_init(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&motor_thread,
                            "motor",
                            motor_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&motor_stack[0],
                            sizeof(motor_stack),
                            20,
                            5);
    if (result == RT_EOK)
    {
       rt_thread_startup(&motor_thread);
    }
    return 0;
}
//



