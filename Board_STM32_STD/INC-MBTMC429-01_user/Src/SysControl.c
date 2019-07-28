/**
  \file SysControl.c
														20180105 ‘≠µ„œﬁŒª≤…”√÷–∂œ∑Ω Ω Œ¥¥¶¿Ì
  \version 1.00

  \brief Motor monitoring

  This file contains the SystemControl function which does all necessary motor
  monitoring tasks.
*/

#include <limits.h>
#include <stdlib.h>
#include "bits.h"
#include "main.h"
#include "Globals.h"
#include "IO.h"
#include "SysTick.h"
#include "TMC429.h"
#include "Commands.h"
#include "SysControl.h"
#include "UART.h"
#include "TMC262.h"
#include "EEPROM.h"

u8 	KEY_START_pressed=0;
u8 	KEY_RESET_pressed=0;

u16 Homing_speed[3]={839,839,839};
u8 	MotorIsHomed=0;
u8 	Homing_step=NO_ACTION;

static u8 MotorIsHoming[3]={0};	
static u8 DoOnce=0;	
static U32_T SensorDelay;


static u8 SensorCheck[3]={0,0,0};
static void SetUpAfterHomed(u8 axis);

/*
static void RotateRight(u8 axis, int velocity);
static void RotateLeft(u8 axis, int velocity);
static void SetAmaxAuto(u8 ActualMotor,int speed);
*/
/*
/ø™ º∞¥º¸∂Ø◊˜-- ÷∂Ø“ª¥Œ PC πƒ‹œ¬¥Œ	÷∏ æµ∆
*/
void KeySTART_EXE(void)
{
	if(KEY_RESET_pressed==0)																											//µÁª˙‘⁄∏¥Œª÷–£¨STARTŒﬁ–ß
	{
		if( KEY_START_IN==0 && KEY_START_pressed==0 )					
		{
			delay_ms(20);		
			if( KEY_START_IN==0 )											 																//ø™ º∞¥º¸
			{
				 KEY_START_LED=1;	 KEY_RESET_LED=0;										
				 while(KEY_START_IN==0) {;} 																						//µ»¥˝À…ø™									 									 			
				 KEY_START_pressed=1;  KEY_RESET_pressed=0;	KEY_RESET_LED=0;						//ø™ º∞¥º¸◊‘À¯ ∏¥Œª∞¥º¸Ω‚À¯	
				 UART_SendStr("START<1>\n>>");
			}
		}
	}
}

/*
∏¥Œª πƒ‹START,µÁª˙ªÿ‘≠µ„
*/
void KeyRESET_EXE(void)
{
	if( KEY_RESET_IN==0 && KEY_RESET_pressed==0)									//µ⁄“ª¥Œ∞¥º¸”––ß
	{
		delay_ms(20);
		if( KEY_RESET_IN==0 )																				//»•∂∂∂Ø
		{														
			KEY_START_pressed=0;	LED_BLUE_ON();				KEY_START_LED=0;	//Ω‚≥˝ø™ º◊‘À¯,øÿ÷∆÷∏ æµ∆
			KEY_RESET_pressed=1;	KEY_RESET_LED=1;			MotorIsHomed=0;				
			SensorCheck[0]=0;			SensorCheck[1]=0;			SensorCheck[2]=0;
			MotorHoming_preset(ProID);			
		}
	}
}

void MotorHoming_preset( uint8_t ID)
{														
	//…Ë÷√ªÿ‘≠µ„ÀŸ∂»ƒ£ Ωœ¬µƒ◊Ó¥ÛÀŸ∂»∫Õº”ÀŸ∂»
	Write429Int(IDX_VMAX|MOTOR0, 2047);			SetAMax(0, 2000);
	Write429Int(IDX_VMAX|MOTOR1, 2047);			SetAMax(1, 2000);
	Write429Int(IDX_VMAX|MOTOR2, 2047);			SetAMax(2, 2000);
	UART_Printf("motor homing...\n");
	
	pressureAlarm=0;printStr5times=0;
	
	switch(ID)
	{
		case BUTTON_ONLINE:
				 Homing_step=Y_GoHome;
				 RotateLeft(AXIS_Y,Homing_speed[AXIS_Y]);	//ƒÊ ±’Î–˝◊™		L		Y÷·œ»∏¥Œª		Y÷·œ»∫Û∫Û«∞
				break;
		
		case BUTTON_OFFLINE:				Cylinder_Reset_check();
				 Homing_step=Y_GoHome;
				 RotateLeft(AXIS_Y,Homing_speed[AXIS_Y]);	//ƒÊ ±’Î–˝◊™		L		Y÷·œ»∏¥Œª		Y÷·œ»∫Û∫Û«∞
				 //delay_ms(5);
				 break;
		
		case BUTTON_VER3:
				 if(INPUT3==1 && INPUT4==0)	
				 {				 
					Homing_step=Z_GoHome;
				  RotateLeft(AXIS_Z,Homing_speed[AXIS_Z]);	//ƒÊ ±’Î–˝◊™		L		Z÷·œ»∏¥Œª		Z÷·œ»…œ∫Ûœ¬
				 } 
				 break;		
				 
		case BUTTON_ROAD:
				 Homing_step=Z_GoHome;
				 RotateLeft(AXIS_Z,Homing_speed[AXIS_Z]);	//ƒÊ ±’Î–˝◊™		L		Z÷·œ»∏¥Œª		Z÷·œ»…œ∫Ûœ¬
				break;	
		default:
			break;
	}
}
/*
Bx35_button: Y÷·œ»¿Ô∫Û≥ˆ ---> Z÷·œ»…œ∫Ûœ¬ ---> X÷·œ»◊Û∫Û”“

B445_button: Z÷·œ»…œ∫Ûœ¬ªÿ‘≠µ„∫Û‘ŸÕ˘…œ‘À––“ª∂ŒŒª÷√∑≈÷√¥´∏–∆˜◊≤µΩHolder---> Y÷·œ»¿Ô∫Û≥ˆ---> X÷·œ»◊Û∫Û”“

ROAD			 £∫Z÷·œ»…œ∫Ûœ¬				
	
*/
void MotorHoming_WithHomeSensor(void)
{
	if(KEY_RESET_pressed==1)															
	{	
		u8 SwitchStatus;	
		if(DoOnce==0) {	SensorDelay=GetSysTimer(); 	DoOnce=1;	}
		SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3); 		
		switch(Homing_step)
		{
				case X_GoHome:	
					if( (Read429Short(IDX_VACTUAL|(0<<5)))<0  && X_HomeSensor==1)	//X÷·œ»À≥ ±’ÎœÚ◊Û—∞’“◊ÛœﬁŒª£¨µΩŒª∫ÛƒÊ ±’ÎœÚ”“‘À––÷–≤…ºØµΩ‘≠µ„–≈∫
					//ROAD Z AIXS ; ADD X AXIS  20190516
					//if(((Read429Short(IDX_VACTUAL|(0<<5)))<0 ) && ((ProID==BUTTON_ROAD && X_HomeSensor==0) || (ProID!=BUTTON_ROAD && X_HomeSensor==1)) )						
					{
						SetUpAfterHomed(AXIS_X);	Homing_step=GoHome_OK;					//X÷·ªÿ‘≠µ„OK
						UART_Printf("Axis X Homed<OK>\n>>");	
						break;
					}																													  //÷ª“™≤…ºØ≤ªµΩ‘≠µ„–≈∫≈,µÁª˙‘⁄¡Ω∏ˆœﬁŒª÷Æº‰¿¥ªÿ‘À∂Ø,¥•∑¢”≤º˛œﬁŒª◊‘∂ØÕ£÷π
					if((SwitchStatus& (0x02<<AXIS_X*2)) ? 1:0)								  //REFL1	ƒÊ ±’Î¥•∑¢◊ÛœﬁŒª ±∑¥◊™
					{																													 
						RotateRight(AXIS_X,Homing_speed[AXIS_X]);								  //À≥ ±’Î ’˝◊™ R
						if(SensorCheck[0]==1) SensorCheck[0]=2;  
					}
					if((SwitchStatus& (0x01<<AXIS_X*2)) ? 1:0)								  //’˝≥£«Èøˆœ¬≤ªª·¥•∑¢”“œﬁŒª;‘≠µ„–≈∫≈“ª÷±≤…ºØ≤ªµΩ ±≤≈ª·¥•∑¢
					{																													  //REFR1  À≥ ±’Î¥•∑¢”“œﬁŒª ±∑¥◊™
						RotateLeft(AXIS_X,Homing_speed[AXIS_X]);		
						if(SensorCheck[0]==0) SensorCheck[0]=1; 
						else if(SensorCheck[0]==2) SensorCheck[0]=3; 
					}
					break;
				
				case Y_GoHome:																									//Y÷·œ»ƒÊ ±’ÎœÚ”“‘À––—∞’“”“œﬁŒª,
					//∂¡»°ÀŸ∂»∫Ø ˝‘⁄«∞£¨≤ª»ªY÷·»Áπ˚÷Æ«∞‘⁄‘≠µ„∫Û–¯æÕ≤ªª·‘Ÿ’“‘≠µ„£¨”–ŒÛ≤Ó
					if(((Read429Short(IDX_VACTUAL|(1<<5))))>0 && Y_HomeSensor==1)	//µΩŒª∫ÛÀ≥ ±’ÎœÚ◊Û‘À––÷–≤…ºØµΩ‘≠µ„–≈∫≈	
					{
						SetUpAfterHomed(AXIS_Y);				  		
						UART_Printf("Axis Y Homed<OK>\n>>");											
						if(ProID==BUTTON_VER3)	
						{
							RotateRight(AXIS_X,Homing_speed[AXIS_X]);
							Homing_step=X_GoHome;
							break ;
						}
						else 
						{
							RotateLeft(AXIS_Z,Homing_speed[AXIS_Z]);
							Homing_step=Z_GoHome;
							break ;
						}									    //Z÷·œ»…œ∫Ûœ¬			
					}
					if((SwitchStatus& (0x02<<AXIS_Y*2)) ? 1:0 )											//L2
					{
						RotateRight(AXIS_Y,Homing_speed[AXIS_Y]);		
						if(SensorCheck[1]==0) SensorCheck[1]=1; 
						else if(SensorCheck[1]==2) SensorCheck[1]=3; 
					}
					if((SwitchStatus& (0x01<<AXIS_Y*2)) ? 1:0 )											//R2
					{
						RotateLeft(AXIS_Y,Homing_speed[AXIS_Y]);		
						if(SensorCheck[1]==1) SensorCheck[1]=2;  
					}			
					break;				
			 case Z_GoHome:
				  if(ProID==BUTTON_ROAD)	
					{
						//if(INPUT3==1) HardStop(2);KEY_RESET_pressed=0;	MotorIsHomed=0;	Homing_step=NO_ACTION; break;
					}
					if(((Read429Short(IDX_VACTUAL|(2<<5)))>0 ) && ((ProID==BUTTON_ROAD && Z_HomeSensor==0) || (ProID!=BUTTON_ROAD && Z_HomeSensor==1)) )
					{
						SetUpAfterHomed(AXIS_Z);	
						UART_Printf("Axis Z Homed<OK>\n>>");				
						if(ProID==BUTTON_VER3)	
						{
							Write429Int(IDX_XTARGET|(2<<5), Read429Int(IDX_XACTUAL|(2<<5))-12000);
							Set429RampMode(2, RM_RAMP);	
							delay_ms(500);OUTPUT3=0;OUTPUT4=1;
							RotateLeft(AXIS_Y,Homing_speed[AXIS_Y]);	Homing_step=Y_GoHome;	break ;
						}
//						else if(ProID==BUTTON_ROAD)					//ROAD Z AIXS ; ADD X AXIS  20190516
//						{
//							Homing_step=GoHome_OK;	break ;
//						}
						else {Homing_step=X_GoHome;	 RotateRight(AXIS_X,Homing_speed[AXIS_X]);break;}			//X÷·œ»◊Û∫Û”“		
					}
					if((SwitchStatus& (0x02<<AXIS_Z*2)) ? 1:0	)		//L3
					{		
						RotateRight(AXIS_Z,Homing_speed[AXIS_Z]);		
						if(SensorCheck[2]==0) SensorCheck[2]=1; 
						else if(SensorCheck[2]==2) SensorCheck[2]=3; 
					}
					if((SwitchStatus& (0x01<<AXIS_Z*2)) ? 1:0	)		//R3
					{
						RotateLeft(AXIS_Z,Homing_speed[AXIS_Z]);		
						if(SensorCheck[2]==1) SensorCheck[2]=2; 
					}
				 break;
			 case GoHome_OK:
						KEY_RESET_pressed=0;	KEY_RESET_LED=0;	KEY_START_pressed=0;	KEY_START_LED=0;	LED_GREEN_ON(); 
						MotorIsHomed=1;
						UART_Printf("All Axis Homed<OK>\n>>");
						//…Ë÷√∏¥Œª∫Ûµƒƒ¨»œ‘À––ÀŸ∂»”Îº”ÀŸ∂»	
						Write429Int(IDX_VMAX|MOTOR0, 280);MotorConfig[0].VMax=280;
						SetAMax(0, 120);MotorConfig[0].AMax=120;
						Write429Int(IDX_VMAX|MOTOR1, 140);MotorConfig[1].VMax=140;
						SetAMax(1, 30);MotorConfig[1].AMax=30;
						Write429Int(IDX_VMAX|MOTOR2, 280);MotorConfig[2].VMax=280;
						SetAMax(2, 120);MotorConfig[2].AMax=120;
						Homing_step=NO_ACTION;
			 
				 break ;
			 default:
				 break;	
			if(abs((int)(GetSysTimer()-SensorDelay))>1000 )
			{
				if(SensorCheck[0]==3)	{ SensorCheck[0]=0; UART_SendStr("Please check home sensor >>motor X<<\n");	LED_BED_ON();}
				if(SensorCheck[1]==3)	{	SensorCheck[1]=0; UART_SendStr("Please check home sensor >>motor Y<<\n");	LED_BED_ON();}
				if(SensorCheck[2]==3)	{ SensorCheck[2]=0; UART_SendStr("Please check home sensor >>motor Z<<\n");	LED_BED_ON();}
				SensorDelay=GetSysTimer();
			}
		}	
	}
}
void MotorHoming_WithoutHomeSensor( uint8_t ID)
{
	
	
}
//
void RotateRight(u8 axis, int velocity)
{
	Set429RampMode(axis, RM_VELOCITY);
	Write429Short(IDX_VTARGET|(axis<<5), velocity);
}
//
void RotateLeft(u8 axis, int velocity)
{
	Set429RampMode(axis, RM_VELOCITY);
	Write429Short(IDX_VTARGET|(axis<<5), -velocity);
}
//
static void SetUpAfterHomed(u8 axis) 
{
	//÷Æ«∞“—æ≠¥¶”⁄ÀŸ∂»ƒ£ Ω
	HardStop(axis);
	delay_ms(100);					//”≈ªØªÿ¡„∫Û∂¡µΩµƒŒª÷√≤ªŒ™0
	//delay_ms(200);
	Write429Int(IDX_XACTUAL|(axis<<5), 0);
	Write429Int(IDX_XTARGET|(axis<<5), 0);
	
}
//
void CommandCheckAndExeViaUart1(void)
{
	u8 len;
	if(USART_RX_STA&0x8000)									//Ω” ’µΩ\r\n						
	{	
		len=USART_RX_STA&0x3fff;							//µ√µΩ¥À¥ŒΩ” ’µΩµƒ ˝æ›≥§∂»
		USART_RX_BUF[len]='\0';								//‘⁄ƒ©Œ≤º”»ÎΩ· ¯∑˚. 
		UART_SendStr((char*)USART_RX_BUF);
		UART_SendChar('\n');
		if(USART_RX_BUF[0]=='\0')	
			UART_SendStr("\n");
		else if(Command_Analysis_Process((char*)USART_RX_BUF))	;
		else 
			UART_SendStr("<NA>\n");								
		
		UART_SendStr(">>");	
		USART_RX_STA=0;
	}	
}

void SetAmaxAuto(u8 ActualMotor,int speed)
{
	//12MHZ  lidopen  10000steps//2047-->17.57/M;					0.29R/S			100--0.01R/S
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
	//usteps=1	steps=200 fullstep=1*200=200
	else if(MotorConfig[ActualMotor].PulseDiv==5 &&	MotorConfig[ActualMotor].RampDiv==8 && MotorConfig[ActualMotor].usteps==1)
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
	//usteps=8	steps=200 fullstep=8*200=1600  Ω®“ÈÀŸ∂»‘⁄60~810÷Æº‰
	else if(MotorConfig[ActualMotor].PulseDiv==5 &&	MotorConfig[ActualMotor].RampDiv==8 && MotorConfig[ActualMotor].usteps==8)
	{
		/*
		A=20	  	V=60~74	
		A=35	  	V=75~100
		A=60 			V=101~130
		A=100 		V=131~170
		A=180 		V=171~230
		A=330   	V=231~310
		A=550   	V=311~400
		A=1000   	V=401~550
		A=1800   	V=551~750
		A=2047   	V=751~810
		*/
		if 			(speed<75)					MotorConfig[ActualMotor].AMax=20;	
		else if (speed<101)					MotorConfig[ActualMotor].AMax=35;	
		else if (speed<131)					MotorConfig[ActualMotor].AMax=60;
		else if (speed<171)					MotorConfig[ActualMotor].AMax=100;
		else if (speed<231)					MotorConfig[ActualMotor].AMax=180;
		else if (speed<311)					MotorConfig[ActualMotor].AMax=330;
		else if (speed<401)					MotorConfig[ActualMotor].AMax=550;
		else if (speed<551)					MotorConfig[ActualMotor].AMax=1000;
		else if (speed<751)					MotorConfig[ActualMotor].AMax=1800;
		else if (speed<810)					MotorConfig[ActualMotor].AMax=2047;
		else if (speed<2048)				MotorConfig[ActualMotor].AMax=2047;
		
		SetAMax(ActualMotor, MotorConfig[ActualMotor].AMax);
	}
}
//-----------------------------------------------------------Lidopen
void SetAmaxAuto_old(u8 ActualMotor,int speed)
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
void MotorHoming_Lidopen(void)
{
	if(	Homing_step )
	{
		u8 i;
		static Data_4Bytes Speed1;
		static Data_4Bytes Speed2;
		static Data_4Bytes Speed3;
		switch(Homing_step)
		{
			/* #1 œ»∏ﬂÀŸ‘»ÀŸœÚ◊Û‘À∂Ø—∞’“∏∫œﬁŒªµ„ */
			case 1:	
							SERVO_ON_OUT=1;
							Write429Int(IDX_VMAX|MOTOR1, 300);								
							SetAMax(AXIS_Y, 300);
							UART_Printf("motor homing...\n");		
							for(i=0;i<4;i++)
							{
								Speed1.Byte[i]=AT24CXX_ReadOneByte(1*4+100+i);
							}
							SetAmaxAuto(AXIS_Y, (int)Speed1.f32);
							RotateLeft(AXIS_Y,(int)Speed1.f32);
							Homing_step=2;
							
				break;
			/* #2 µ»¥˝µΩŒª∫ÛµÕÀŸœÚ”“œ‡∂‘‘À∂Ø */
			case 2:
							if((Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<AXIS_Y*2)) ? 1:0 )
							{
								delay_ms(100);
								for(i=0;i<4;i++)
								{
									Speed2.Byte[i]=AT24CXX_ReadOneByte(2*4+100+i);
								}
								Write429Short(IDX_VMAX|AXIS_Y<<5, (int)Speed2.f32);
								SetAmaxAuto(AXIS_Y, (int)Speed2.f32);
								Set429RampMode(AXIS_Y, RM_VELOCITY);
								Write429Int(IDX_XTARGET|(AXIS_Y<<5), Read429Int(IDX_XACTUAL|(AXIS_Y<<5))+1000);
								Set429RampMode(AXIS_Y, RM_RAMP);
								Homing_step=3;
							}
				break;
			/* #3 µ»¥˝µΩŒª∫ÛµÕÀŸ‘»ÀŸœÚ◊Û‘À∂Ø—∞’“∏∫œﬁŒªµ„  */
			case 3:
							if(((Read429Status() & (0x01<<AXIS_Y*2)) ? 1:0) )
							{
								delay_ms(100);
								for(i=0;i<4;i++)
								{
									Speed3.Byte[i]=AT24CXX_ReadOneByte(3*4+100+i);
								}
								SetAmaxAuto(AXIS_Y, (int)Speed3.f32);
								RotateLeft(AXIS_Y,(int)Speed3.f32);	
								Homing_step=4;
							}
				break;
			/* #4 µ»¥˝µΩŒª≤¢…Ë÷√◊ÛœﬁŒªŒ™‘≠µ„  */
			case 4:
							if((Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<AXIS_Y*2)) ? 1:0 )
							{
								SetUpAfterHomed(AXIS_Y);						  														//‘≠µ„≤Œ ˝…Ë÷√
								UART_Printf("Axis Y Homed<OK>\n");																//¥Ú”°Ã· æ	
								MotorIsHomed=1;
								Homing_step=0;
							}
				break;
			default :
				break;
		}
	}
}
//20180118
void Cylinder_Reset_check(void)
{
	U32_T DebugDelay;
	DebugDelay=GetSysTimer();		
	UART_SendStr("\n>>Cylinder resetting...\n");
	OUTPUT5=0;OUTPUT6=1;delay_ms(100);
	while(	(INPUT8==1 && INPUT9==0) ? 0:1 )	
	{
		if(abs((int)(GetSysTimer()-DebugDelay))>1000) 
		{
			UART_SendStr("Please check pogo pin cylinder is >>BACK<< :  OUT6=1,OUT5=0;    IN8=0,IN9=1\n");
			CODE_RUN_LED_TOGGLE();LED_BED_ON();
			DebugDelay=GetSysTimer();
		}
	}
	OUTPUT8=0;OUTPUT7=1;delay_ms(100);
	while((INPUT11==1 && INPUT10==0) 	? 0:1 )	
	{
		if(abs((int)(GetSysTimer()-DebugDelay))>1000) 
		{
			UART_SendStr("Please check pogo pin cylinder is >>UPSIDE<< : OUT7=1,OUT8=0;   IN11=0,IN10=1\n");
			CODE_RUN_LED_TOGGLE();LED_BED_ON();
			DebugDelay=GetSysTimer();}
	}
	OUTPUT3=0;OUTPUT4=1;delay_ms(100);
	while((INPUT3==1 && INPUT4==0) 		? 0:1 )	
	{
		if(abs((int)(GetSysTimer()-DebugDelay))>1000) 
		{
			UART_SendStr("Please check dut cylinder is >>OUTSIDE<< : OUT4=1,OUT3=0;    IN3=0,IN4=1\n");
			CODE_RUN_LED_TOGGLE();LED_BED_ON();
			DebugDelay=GetSysTimer();
		}
	}
	LED_BLUE_ON();
}
//
void IO_DebugViaLedBlinking(void)
{
	static U32_T BlinkDelay_IO;
	static	u8 IO_debug=0;
	GPIO_InitTypeDef  GPIO_InitStructure;
	if(IO_debug==0)
	{
		IO_debug=1;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
		//IN1~5																											    
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		//IN6~12																										    
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOE, &GPIO_InitStructure);
		
	}
	if(abs((int)(GetSysTimer()-BlinkDelay_IO))>1000)
	{
		PBout(5)=1;PBout(7)=1;PBout(9)=1;PEout(1)=1;PEout(3)=1;PEout(5)=1;
		PBout(6)=0;PBout(8)=0;PEout(0)=0;PEout(2)=0;PEout(4)=0;PEout(6)=0;
		
		OUTPUT1=1;OUTPUT3=1;OUTPUT5=1;OUTPUT7=1;
		OUTPUT2=0;OUTPUT4=0;OUTPUT6=0;OUTPUT8=0;
		
		BlinkDelay_IO=GetSysTimer();
	}
	else if(abs((int)(GetSysTimer()-BlinkDelay_IO))>500) 
	{
		PBout(5)=0;PBout(7)=0;PBout(9)=0;PEout(1)=0;PEout(3)=0;PEout(5)=0;
		PBout(6)=1;PBout(8)=1;PEout(0)=1;PEout(2)=1;PEout(4)=1;PEout(6)=1;
		
		OUTPUT1=0;OUTPUT3=0;OUTPUT5=0;OUTPUT7=0;
		OUTPUT2=1;OUTPUT4=1;OUTPUT6=1;OUTPUT8=1;
	}	
	
}
//-----------------------------------------------------------add home exit 
/*
void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource9);

  EXTI_InitStructure.EXTI_Line=EXTI_Line9;	
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	 	
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource0);
  EXTI_InitStructure.EXTI_Line=EXTI_Line0;	
	EXTI_Init(&EXTI_InitStructure);	 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource1);
  EXTI_InitStructure.EXTI_Line=EXTI_Line1;	
	EXTI_Init(&EXTI_InitStructure);	 	

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x07;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;				
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							
	NVIC_Init(&NVIC_InitStructure); 
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;				
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							
	NVIC_Init(&NVIC_InitStructure); 

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
	NVIC_Init(&NVIC_InitStructure);
}
//
void EXTI0_IRQHandler(void)
{
	//OUTPUT6=~OUTPUT6;
	if((MotorIsHoming[AXIS_Y]==1)&&(INPUT6==1))	//Y÷·µΩ¥Ô‘≠µ„Œª÷√«“Œ¥∏¥Œªπ˝
	{
		if((Read429Short(IDX_VACTUAL|(1<<5)))>0)	  //Y÷·ªÿ‘≠µ„∑ΩœÚ’˝»∑			
		{
			SetUpAfterHomed(AXIS_Y);						  		//‘≠µ„≤Œ ˝…Ë÷√
			UART_Printf("Axis Y Homed<OK>\n>>");				//¥Ú”°Ã· æ
				
			MotorIsHoming[AXIS_Y]=0;									//Y÷·Ω· ¯ªÿ‘≠µ„
			MotorIsHoming[AXIS_Z]=1;									//Z÷·ø™ ºªÿ‘≠µ„
			RotateLeft(AXIS_Z,Homing_speed[AXIS_Z]);
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line0); 
}
void EXTI1_IRQHandler(void)
{
	//OUTPUT7=~OUTPUT7;
	if((MotorIsHoming[AXIS_Z]==1)&&INPUT7==1)
	{
		if((Read429Short(IDX_VACTUAL|(2<<5)))>0)
		{
			SetUpAfterHomed(AXIS_Z);	
			UART_Printf("Axis Z Homed<OK>\n>>");
			
			MotorIsHoming[AXIS_Z]=0;
			MotorIsHoming[AXIS_X]=1;
			RotateRight(AXIS_X,Homing_speed[AXIS_X]);			
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line1);  //«Â≥˝LINE4…œµƒ÷–∂œ±Í÷æŒª  
}
void EXTI9_5_IRQHandler(void)
{
	//OUTPUT5=~OUTPUT5;
		if((MotorIsHoming[AXIS_X]==1)&&(INPUT5==1))
	{
		if((Read429Short(IDX_VACTUAL|(0<<5)))<0)
		{
			SetUpAfterHomed(AXIS_X);
			UART_Printf("Axis X Homed<OK>\n>>");				
			MotorIsHoming[AXIS_X]=0;
		}
	}	
	EXTI_ClearITPendingBit(EXTI_Line9);  
}
*/



