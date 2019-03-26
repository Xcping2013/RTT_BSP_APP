#include "gpio.h"
#include "delay.h"
#include "TMC429.h"
#include "Commands.h"
#include "SysControl.h"
#include "usart.h"

/**********************�����ԭ�����***************************/
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

/*************************************�����ԭ����غ�������***************************/
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
/*���ֹͣ�������õ�ǰλ��Ϊԭ��0  
*/
static void ZeroAfterHomed(u8 axis) 
{
	//֮ǰ�Ѿ������ٶ�ģʽ�������ٽ���ģʽ�л�
	Write429Zero(IDX_VTARGET|(axis<<5));
	delay_ms(100);
	Write429Int(IDX_XACTUAL|(axis<<5), 0);
	Write429Int(IDX_XTARGET|(axis<<5), 0);
}
/*��ԭ����ٶȺͼ��ٶ�Ĭ����ʹ����һ��״̬����	�������˶�Ѱ��ʱ����һ���Ĳ���
  �����������Ŀ�����ڴ˺������Լ�����ƥ�����
  ��ԭ��ģʽ��1.REFLΪԭ��	2.��REFRΪԭ�� 3.���Home����Ϊԭ��
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
	if(	HomeStep[MotorNum] )		//��Ϊ0���Ž������»�ԭ�㶯��
	{	
		switch(HomeStep[MotorNum])
		{
							/* #1 �ȸ������������˶�Ѱ��LEFT��λ�� */
			case 1:							
							SetAMax(MotorNum, MotorConfig[MotorNum].AMax);													//��������	
							Set429RampMode(MotorNum, RM_VELOCITY);
							Write429Short(IDX_VMAX|(MotorNum<<5), 2047);
							Write429Short(IDX_VTARGET|(MotorNum<<5), -MotorConfig[MotorNum].HomeV1);	//��������						
							printf("motor<%d> is homing...>>\r\n",MotorNum);	
							HomeStep[MotorNum]=2;						
				break;
							/* #2 �ȴ���λ�������������˶� */
			case 2:
							if((Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<MotorNum*2)) ? 1:0 )
							{
								delay_ms(100);
								//��������
								Write429Short(IDX_VMAX|(MotorNum<<5), MotorConfig[MotorNum].VMax);
								SetAMax(MotorNum, MotorConfig[MotorNum].AMax);
								Write429Int(IDX_XTARGET|(MotorNum<<5), Read429Int(IDX_XACTUAL|(MotorNum<<5))+POSITION_REF1);
								Set429RampMode(MotorNum, RM_RAMP);
								HomeStep[MotorNum]=3;	
							}
				break;
							/* #3 �ȴ���λ��������������˶�Ѱ�Ҹ���λ��  */
			case 3:
							if(((Read429Status() & (0x01<<MotorNum*2)) ? 1:0) )
							{
								delay_ms(100);
								SetAMax(MotorNum, MotorConfig[MotorNum].AMax);
								Set429RampMode(MotorNum, RM_VELOCITY);
								Write429Short(IDX_VMAX|(MotorNum<<5), 2047);
								Write429Short(IDX_VTARGET|(MotorNum<<5), -MotorConfig[MotorNum].HomeV2);	//��������												
								HomeStep[MotorNum]=4;	
							}
				break;
			/* #4 �ȴ���λ����������λΪԭ��  */
			case 4:
							if((Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<MotorNum*2)) ? 1:0 )
							{
								ZeroAfterHomed(MotorNum);						  																	//ԭ���������
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
	if(	HomeStep[MotorNum] )		//��Ϊ0���Ž������»�ԭ�㶯��
	{
		switch(HomeStep[MotorNum])
		{
							/* #1 �ȸ������������˶�Ѱ��RIGHT��λ�� */
			case 1:							
							SetAMax(MotorNum, MotorConfig[MotorNum].AMax);													//��������	
							Set429RampMode(MotorNum, RM_VELOCITY);
							Write429Short(IDX_VMAX|(MotorNum<<5), 2047);
							Write429Short(IDX_VTARGET|(MotorNum<<5), MotorConfig[MotorNum].HomeV1);	//��������						
							printf("motor<%d> is homing...>>\r\n",MotorNum);	
							HomeStep[MotorNum]=2;						
				break;
							/* #2 �ȴ���λ�������������˶� */
			case 2:	//��ȡ��λ״̬
							if((Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x01<<MotorNum*2)) ? 1:0 )
							{
								delay_ms(100);
								//��������
								Write429Short(IDX_VMAX|(MotorNum<<5), MotorConfig[MotorNum].VMax);
								SetAMax(MotorNum, MotorConfig[MotorNum].AMax);
								Write429Int(IDX_XTARGET|(MotorNum<<5), Read429Int(IDX_XACTUAL|(MotorNum<<5))-POSITION_REF1);
								Set429RampMode(MotorNum, RM_RAMP);
								HomeStep[MotorNum]=3;	
							}
				break;
							/* #3 �ȴ���λ��������������˶�Ѱ�Ҹ���λ��  */
			case 3:	//��ȡ��λ״̬
							if(((Read429Status() & (0x01<<MotorNum*2)) ? 1:0) )
							{
								delay_ms(100);
								SetAMax(MotorNum, MotorConfig[MotorNum].AMax);
								Set429RampMode(MotorNum, RM_VELOCITY);
								Write429Short(IDX_VMAX|(MotorNum<<5), 2047);
								Write429Short(IDX_VTARGET|(MotorNum<<5), MotorConfig[MotorNum].HomeV2);	//��������												
								HomeStep[MotorNum]=4;	
							}
				break;
			/* #4 �ȴ���λ����������λΪԭ��  */
			case 4:
							if((Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x01<<MotorNum*2)) ? 1:0 )
							{
								ZeroAfterHomed(MotorNum);						  																	//ԭ���������
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
		



//1.�˵�������λ	2.����Ѱ��ԭ�㿪��
static void HomingUseREFML(u8 MotorNum)
{
	if(	HomeStep[MotorNum] )		//��Ϊ0���Ž������»�ԭ�㶯��
	{	
		switch(HomeStep[MotorNum])
		{
							/* #1 ���������˶�Ѱ��LEFT��λ�� */
			case 1:							
							SetAMax(MotorNum, MotorConfig[MotorNum].AMax);													//��������	
							Set429RampMode(MotorNum, RM_VELOCITY);
							Write429Short(IDX_VMAX|(MotorNum<<5), 2047);
							Write429Short(IDX_VTARGET|(MotorNum<<5), -MotorConfig[MotorNum].HomeV1);	//��������						
							printf("motor<%d> is homing...>>\r\n",MotorNum);	
							HomeStep[MotorNum]=2;						
				break;
							/* #2 ���������˶�Ѱ��HOME ��ѯ�ɼ�ԭ�㴫����״̬��������*/
			case 2:
							if((Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<MotorNum*2)) ? 1:0 )
							{
								delay_ms(100);
								//��������
								Write429Short(IDX_VTARGET|(MotorNum<<5), MotorConfig[MotorNum].HomeV2);	//��������						
								HomeStep[MotorNum]=3;	
							}
				break;
							/* #3 ��ȡԭ�㴫�����ź�  */
			case 3:
							if(SENSOR_HOME_PIN(MotorNum)==TOUCH)
							{
								ZeroAfterHomed(MotorNum);						  																	//ԭ���������
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

//1.�˵�������λ	2.����Ѱ��ԭ�㿪��
static void HomingUseREFMR(u8 MotorNum)
{
	if(	HomeStep[MotorNum] )		//��Ϊ0���Ž������»�ԭ�㶯��
	{	
		switch(HomeStep[MotorNum])
		{
							/* #1 ���������˶�Ѱ��LEFT��λ�� */
			case 1:							
							SetAMax(MotorNum, MotorConfig[MotorNum].AMax);													//��������	
							Set429RampMode(MotorNum, RM_VELOCITY);
							Write429Short(IDX_VMAX|(MotorNum<<5), 2047);
							Write429Short(IDX_VTARGET|(MotorNum<<5), MotorConfig[MotorNum].HomeV1);	//��������						
							printf("motor<%d> is homing...>>\r\n",MotorNum);	
							HomeStep[MotorNum]=2;						
				break;
							/* #2 ���������˶�Ѱ��HOME ��ѯ�ɼ�ԭ�㴫����״̬��������*/
			case 2:
							if((Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<MotorNum*2)) ? 1:0 )
							{
								delay_ms(100);
								//��������
								Write429Short(IDX_VTARGET|(MotorNum<<5), -MotorConfig[MotorNum].HomeV2);	//��������						
								HomeStep[MotorNum]=3;	
							}
				break;
							/* #3 ��ȡԭ�㴫�����ź�  */
			case 3:
							if(SENSOR_HOME_PIN(MotorNum)==TOUCH)
							{
								ZeroAfterHomed(MotorNum);						  																	//ԭ���������
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


/******************************************���������׶������***************************/

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
//��ʼ��������--�ֶ�һ�� PCʹ���´�	ָʾ��
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
	if( INPUT(Button)==TOUCH && KeyPressed_RESET==0)			//��һ�ΰ�����Ч
	{
		delay_ms(20);
		if( INPUT(Button)==TOUCH )													//ȥ����
		{
			KeyPressed_START=0;										  							//�����ʼ����	
			KeyPressed_RESET=1;			
			/*********do something****KeyPressed_RESET=0 when done*******/										
		}
	}
}
//



