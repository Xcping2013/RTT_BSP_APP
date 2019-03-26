#include "gpio.h"
#include "delay.h"
#include "TMC429.h"
#include "Commands.h"
#include "SysControl.h"
#include "usart.h"

/**********************电机回原点相关***************************/
#define	SENSOR_HOME_PIN(n)	BSP_INT_GetState(n+10)	//@@	IN10 IN11 IN12
#define	TOUCH								0
#define	POSITION_REF1				1000										//@@

u8 HomeStep[3]		=	{0,0,0};			
u8 MotorIsHomed[3]=	{0,0,0};

static void HomingUseREFL(u8 MotorNum);
static void HomingUseREFR(u8 MotorNum);
static void HomingUseREFML(u8 MotorNum);
static void HomingUseREFMR(u8 MotorNum);
static void ZeroAfterHomed(u8 axis);

/*************************************电机回原点相关函数定义***************************/
//
uint8_t MyCLI_MotorGoHome(char *Commands)
{
	if(strcmp("home?",Commands)==0)
  {
		u8 i;
		for(i=0;i<3;i++)	printf("HOME[%d]<%d>\r\n",i,MotorIsHomed[i] );	
		return CMD_GET_OK;
  }
	else if( strncmp(Commands,"home[",5)==0 && Commands[6]==']' ) 
	{		
		ActualMotor = Commands[5]-'0';
		if(ActualMotor<3)
		{
			HomeStep[ActualMotor]=1;
			return CMD_SET_OK;
		}
		else return CMD_PAR_ERR;
	}
	else if(strcmp("home!",Commands)==0)
  {
		HomeStep[0]=1;
		HomeStep[1]=1;
		HomeStep[2]=1;
		return CMD_SET_OK;
  }
	else return CMD_NEXT;
}
//
/*电机停止，并设置当前位置为原点0  
*/
static void ZeroAfterHomed(u8 axis) 
{
	//之前已经处于速度模式，无需再进行模式切换
	Write429Zero(IDX_VTARGET|(axis<<5));
	delay_ms(100);
	Write429Int(IDX_XACTUAL|(axis<<5), 0);
	Write429Int(IDX_XTARGET|(axis<<5), 0);
}
/*回原点的速度和加速度默认是使用上一个状态参数	且左右运动寻点时都是一样的参数
  如果不符合项目，可在此函数中自己设置匹配参数
  回原点模式：1.REFL为原点	2.以REFR为原点 3.添加Home开关为原点
*/
void HomingProcess(u8 WhichMotor,u8 SearchMode)
{
	switch(SearchMode)
	{
		case REFL_SEARCH:	HomingUseREFL(WhichMotor);
			break;
		
		case REFR_SEARCH:	HomingUseREFR(WhichMotor);
			break;
		
		case REFML_SEARCH:	HomingUseREFML(WhichMotor);
			break;		
		
		case REFMR_SEARCH:	HomingUseREFMR(WhichMotor);
			break;		

		default:
			break;
	}
}
//Vh-Rotate->REFL; Vl-move ref->Positon_1; Vl-Rotate->REFL; SetVal
static void HomingUseREFL(u8 MotorNum)
{
	if(	HomeStep[MotorNum] )		//不为0，才进行以下回原点动作
	{	
		switch(HomeStep[MotorNum])
		{
							/* #1 先高速匀速向左运动寻找LEFT限位点 */
			case 1:							
							SetAMax(MotorNum, MotorConfig[MotorNum].AMax);													//按需设置	
							Set429RampMode(MotorNum, RM_VELOCITY);
							Write429Short(IDX_VMAX|(MotorNum<<5), 2047);
							Write429Short(IDX_VTARGET|(MotorNum<<5), -MotorConfig[MotorNum].HomeV1);	//按需设置						
							printf("motor<%d> is homing...>>\r\n",MotorNum);	
							HomeStep[MotorNum]=2;						
				break;
							/* #2 等待到位后低速向右相对运动 */
			case 2:
							if((Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<MotorNum*2)) ? 1:0 )
							{
								delay_ms(100);
								//按需设置
								Write429Short(IDX_VMAX|(MotorNum<<5), MotorConfig[MotorNum].VMax);
								SetAMax(MotorNum, MotorConfig[MotorNum].AMax);
								Write429Int(IDX_XTARGET|(MotorNum<<5), Read429Int(IDX_XACTUAL|(MotorNum<<5))+POSITION_REF1);
								Set429RampMode(MotorNum, RM_RAMP);
								HomeStep[MotorNum]=3;	
							}
				break;
							/* #3 等待到位后低速匀速向左运动寻找负限位点  */
			case 3:
							if(((Read429Status() & (0x01<<MotorNum*2)) ? 1:0) )
							{
								delay_ms(100);
								SetAMax(MotorNum, MotorConfig[MotorNum].AMax);
								Set429RampMode(MotorNum, RM_VELOCITY);
								Write429Short(IDX_VMAX|(MotorNum<<5), 2047);
								Write429Short(IDX_VTARGET|(MotorNum<<5), -MotorConfig[MotorNum].HomeV2);	//按需设置												
								HomeStep[MotorNum]=4;	
							}
				break;
			/* #4 等待到位并设置左限位为原点  */
			case 4:
							if((Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<MotorNum*2)) ? 1:0 )
							{
								ZeroAfterHomed(MotorNum);						  																	//原点参数设置
								printf("motor<%d> homed<OK>>>\r\n",MotorNum);	
								MotorIsHomed[MotorNum]=1;
								HomeStep[MotorNum]=0;	
							}
				break;
			default :
				break;
		}
	}
}
		
//Vh-Rotate->REFR; Vl-move reR->Positon_1; Vl-Rotate->REFR; SetVal
static void HomingUseREFR(u8 MotorNum)
{
	if(	HomeStep[MotorNum] )		//不为0，才进行以下回原点动作
	{
		switch(HomeStep[MotorNum])
		{
							/* #1 先高速匀速向右运动寻找RIGHT限位点 */
			case 1:							
							SetAMax(MotorNum, MotorConfig[MotorNum].AMax);													//按需设置	
							Set429RampMode(MotorNum, RM_VELOCITY);
							Write429Short(IDX_VMAX|(MotorNum<<5), 2047);
							Write429Short(IDX_VTARGET|(MotorNum<<5), MotorConfig[MotorNum].HomeV1);	//按需设置						
							printf("motor<%d> is homing...>>\r\n",MotorNum);	
							HomeStep[MotorNum]=2;						
				break;
							/* #2 等待到位后低速向右相对运动 */
			case 2:	//读取限位状态
							if((Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x01<<MotorNum*2)) ? 1:0 )
							{
								delay_ms(100);
								//按需设置
								Write429Short(IDX_VMAX|(MotorNum<<5), MotorConfig[MotorNum].VMax);
								SetAMax(MotorNum, MotorConfig[MotorNum].AMax);
								Write429Int(IDX_XTARGET|(MotorNum<<5), Read429Int(IDX_XACTUAL|(MotorNum<<5))-POSITION_REF1);
								Set429RampMode(MotorNum, RM_RAMP);
								HomeStep[MotorNum]=3;	
							}
				break;
							/* #3 等待到位后低速匀速向右运动寻找负限位点  */
			case 3:	//读取到位状态
							if(((Read429Status() & (0x01<<MotorNum*2)) ? 1:0) )
							{
								delay_ms(100);
								SetAMax(MotorNum, MotorConfig[MotorNum].AMax);
								Set429RampMode(MotorNum, RM_VELOCITY);
								Write429Short(IDX_VMAX|(MotorNum<<5), 2047);
								Write429Short(IDX_VTARGET|(MotorNum<<5), MotorConfig[MotorNum].HomeV2);	//按需设置												
								HomeStep[MotorNum]=4;	
							}
				break;
			/* #4 等待到位并设置左限位为原点  */
			case 4:
							if((Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x01<<MotorNum*2)) ? 1:0 )
							{
								ZeroAfterHomed(MotorNum);						  																	//原点参数设置
								printf("motor<%d> homed<OK>>>\r\n",MotorNum);	
								MotorIsHomed[MotorNum]=1;
								HomeStep[MotorNum]=0;	
							}
				break;
			default :
				break;
		}
	}
}
		



//1.运到到左限位	2.向右寻找原点开关
static void HomingUseREFML(u8 MotorNum)
{
	if(	HomeStep[MotorNum] )		//不为0，才进行以下回原点动作
	{	
		switch(HomeStep[MotorNum])
		{
							/* #1 匀速向左运动寻找LEFT限位点 */
			case 1:							
							SetAMax(MotorNum, MotorConfig[MotorNum].AMax);													//按需设置	
							Set429RampMode(MotorNum, RM_VELOCITY);
							Write429Short(IDX_VMAX|(MotorNum<<5), 2047);
							Write429Short(IDX_VTARGET|(MotorNum<<5), -MotorConfig[MotorNum].HomeV1);	//按需设置						
							printf("motor<%d> is homing...>>\r\n",MotorNum);	
							HomeStep[MotorNum]=2;						
				break;
							/* #2 匀速向右运动寻找HOME 轮询采集原点传感器状态，需慢速*/
			case 2:
							if((Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<MotorNum*2)) ? 1:0 )
							{
								delay_ms(100);
								//按需设置
								Write429Short(IDX_VTARGET|(MotorNum<<5), MotorConfig[MotorNum].HomeV2);	//按需设置						
								HomeStep[MotorNum]=3;	
							}
				break;
							/* #3 获取原点传感器信号  */
			case 3:
							if(SENSOR_HOME_PIN(MotorNum)==TOUCH)
							{
								ZeroAfterHomed(MotorNum);						  																	//原点参数设置
								printf("motor<%d> homed<OK>>>\r\n",MotorNum);	
								MotorIsHomed[MotorNum]=1;
								HomeStep[MotorNum]=0;	
							}
				break;
			default :
				break;
		}
	}
}	

//1.运到到右限位	2.向左寻找原点开关
static void HomingUseREFMR(u8 MotorNum)
{
	if(	HomeStep[MotorNum] )		//不为0，才进行以下回原点动作
	{	
		switch(HomeStep[MotorNum])
		{
							/* #1 匀速向右运动寻找LEFT限位点 */
			case 1:							
							SetAMax(MotorNum, MotorConfig[MotorNum].AMax);													//按需设置	
							Set429RampMode(MotorNum, RM_VELOCITY);
							Write429Short(IDX_VMAX|(MotorNum<<5), 2047);
							Write429Short(IDX_VTARGET|(MotorNum<<5), MotorConfig[MotorNum].HomeV1);	//按需设置						
							printf("motor<%d> is homing...>>\r\n",MotorNum);	
							HomeStep[MotorNum]=2;						
				break;
							/* #2 匀速向左运动寻找HOME 轮询采集原点传感器状态，需慢速*/
			case 2:
							if((Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<MotorNum*2)) ? 1:0 )
							{
								delay_ms(100);
								//按需设置
								Write429Short(IDX_VTARGET|(MotorNum<<5), -MotorConfig[MotorNum].HomeV2);	//按需设置						
								HomeStep[MotorNum]=3;	
							}
				break;
							/* #3 获取原点传感器信号  */
			case 3:
							if(SENSOR_HOME_PIN(MotorNum)==TOUCH)
							{
								ZeroAfterHomed(MotorNum);						  																	//原点参数设置
								printf("motor<%d> homed<OK>>>\r\n",MotorNum);	
								MotorIsHomed[MotorNum]=1;
								HomeStep[MotorNum]=0;	
							}
				break;
			default :
				break;
		}
	}
}	


/******************************************按键与气缸动作相关***************************/

#define INPUT(n) 			  BSP_INT_GetState(n-1)
#define OUTPUTH(n)			BSP_OUT_On(n-1)
#define OUTPUTL(n)			BSP_OUT_Off(n-1)

#define PB_START_Pin		PEin(0)
#define PRESS						0

#define DUT_GO_INSIDE 	1
#define CLOSE_DOOR 			2
#define OPEN_DOOR 			3
#define DUT_GO_OUTSIDE 	4
#define STANDBY 				5

#define	INSIDE					0
#define	OUTSIDE					1
#define	UNDEFINE				3
#define	UPSIDE					0
#define DOWNSIDE				1

#define DutGoInside()		//OUTPUTH(n)
#define DutGoOutside()	//OUTPUTH(n)
#define CloseTheDoor()	//OUTPUTH(n)
#define OpenTheDoor()		//OUTPUTH(n)
						
uint8_t	OPStep_START=0;
uint8_t	StartActOK=0;
uint8_t PB_EnableStart=0;
u8 			KeyPressed_START=0;
u8 			KeyPressed_RESET=0;

static UCHAR DutPosition(void);
static UCHAR DoorPosition(void);
//
static UCHAR DutPosition(void)
{
	if		 (PEin(1)==1 && PEin(2)==0) return INSIDE;
	else if(PEin(1)==0 && PEin(2)==1) return OUTSIDE;	
	else {return UNDEFINE;}
}

static UCHAR DoorPosition(void)
{
	if		 (PEin(8)==0 && PEin(9)==1) return UPSIDE;
	else if(PEin(8)==1 && PEin(9)==0) return DOWNSIDE;
	else {return UNDEFINE;}	
}

uint8_t MyCLI_StatusEN(char *Commands)
{
	if(strcmp("button enable",Commands)==0)
	{
		PB_EnableStart=1;StartActOK=0;
		return CMD_SET_OK;
	}
	else if(strcmp("button disable",Commands)==0)
	{
		PB_EnableStart=0;StartActOK=0;
		return CMD_SET_OK;
	}
	else if(strcmp("ready?",Commands)==0)
	{
		if(StartActOK==1)
		{	
			printf("READY<1>\r\n");
			return CMD_GET_OK;
		}
		else 
		{	
			printf("READY<0>\r\n");	
			return CMD_GET_OK;
		}
	}
  else return CMD_NEXT; 
}
//开始按键动作--手动一次 PC使能下次	指示灯
void StartAction_Button(u8 Button)
{
	if(OPStep_START)
	{
		switch(OPStep_START)
		{
			case STANDBY:
				if(DutPosition()==OUTSIDE && DoorPosition()==UPSIDE )					
				{
					if(PB_START_Pin==PRESS) 																								
					{	
						HAL_Delay(20);
						if(PB_START_Pin==PRESS)	OPStep_START=DUT_GO_INSIDE;					
					}			
				}	
				break;
			
		case DUT_GO_INSIDE:	
			if(PB_START_Pin==PRESS)	
			{	
				 DutGoInside();HAL_Delay(100);
				 if(DutPosition()==INSIDE )	OPStep_START=CLOSE_DOOR;	
			}
			else	OPStep_START=DUT_GO_OUTSIDE;
			break;	
			
		case CLOSE_DOOR:
			if(PB_START_Pin==PRESS)	
			{	
				 CloseTheDoor();HAL_Delay(100);
				 if(DoorPosition()==DOWNSIDE )	
				 {
					 PB_EnableStart=0;StartActOK=1;OPStep_START=STANDBY;
					 printf("ready to test\r\n>>\r\n");
				 }
			}
			else	OPStep_START=OPEN_DOOR;
			break;
			
		case OPEN_DOOR:
			if(PB_START_Pin!=PRESS)	
			{
				 OpenTheDoor();HAL_Delay(100);
				 if(DoorPosition()==UPSIDE) OPStep_START=DUT_GO_OUTSIDE;
			}
			else OPStep_START=CLOSE_DOOR;
			break;
			
		case DUT_GO_OUTSIDE:		
			if(PB_START_Pin!=PRESS)	
			{	
				 DutGoOutside();HAL_Delay(100);
				 if(DutPosition()==OUTSIDE) OPStep_START=STANDBY;
				 else DutGoOutside();
			}
			else	OPStep_START=DUT_GO_INSIDE;
			break;
			
		default:
			break;		
		}
	}
}
//
void ResetAction_Button(u8 Button)
{
	if( INPUT(Button)==TOUCH && KeyPressed_RESET==0)			//第一次按键有效
	{
		delay_ms(20);
		if( INPUT(Button)==TOUCH )													//去抖动
		{
			KeyPressed_START=0;										  							//解除开始自锁	
			KeyPressed_RESET=1;			
			/*********do something****KeyPressed_RESET=0 when done*******/										
		}
	}
}
//



