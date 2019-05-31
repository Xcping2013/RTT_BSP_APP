//#include "app_sys_control.h"
/* 	C内置宏
ANSI C标准中有几个标准预定义宏（也是常用的）：

__DATE__：在源文件中插入当前的编译日期

__TIME__：在源文件中插入当前编译时间；

__FILE__：在源文件中插入当前源文件名；

__LINE__：在源代码中插入当前源代码行号；

__FUNCTION__:在源代码中插入当前所在函数名称；

__STDC__：当要求程序严格遵循ANSI C标准时该标识被赋值为1；

__cplusplus：当编写C++程序时该标识符被定义
*/
#include "bsp_include.h"	
#include "app_include.h"

PARAM_T g_tParam;

int motorPosition[3];
uint8_t autoRESETmotor=FALSE;
uint8_t TimerOpened=0;

//对于限位安全要求高的项目：HARD_REF + 原点 HardStop  其他使用RampStop
void RampStop(UINT Motor)
{
  Set429RampMode(MOTOR_NUMBER(Motor), RM_VELOCITY);
  Write429Zero((MOTOR_NUMBER(Motor)<<5)|IDX_VTARGET);
}

void LoadSettingViaProID(void)
{
	
	
}
//
uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

UCHAR ReadWriteSPI1(UCHAR DeviceNumber, UCHAR aTxBuffer, UCHAR LastTransfer)
{
	u8 aRxBuffer=0;
  switch(DeviceNumber)
  {
    case SPI_DEV_TMC429:
				 SELECT_TMC429();

			   HAL_SPI_TransmitReceive(&hspi1, &aTxBuffer, &aRxBuffer, 1,5000);
				 if(LastTransfer) DESELECT_TMC429();

      return aRxBuffer;

    default:	
      return 0;
  }
}

UCHAR ReadWriteSPI2(UCHAR DeviceNumber, UCHAR aTxBuffer, UCHAR LastTransfer)
{
	u8 aRxBuffer=0;
  switch(DeviceNumber)
  {
    case SPI_DEV_TMC429:
				 SELECT_TMC429();

			   HAL_SPI_TransmitReceive(&hspi2, &aTxBuffer, &aRxBuffer, 1,5000);
				 if(LastTransfer) DESELECT_TMC429();

      return aRxBuffer;

    default:	
      return 0;
  }
}

void xstrcat(char *str1,char *str2)
{
	int i,len1;
	for(i=0;str1[i]!='\0';i++);
	len1=i;
	for(i=0;str2[i]!='\0';i++)
	str1[i+len1]=str2[i];
}

void reboot(void)
{
    rt_hw_cpu_reset();
}

/*
*********************************************************************************************************
*	函 数 名: LoadParamFromEeprom
*	功能说明: 从Flash读参数到g_tParam
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void LoadParamFromEeprom(void)
{
	at24cxx.read(at24c256 , PARAM_ADDR, (uint8_t *)&g_tParam, sizeof(PARAM_T));
	//ee_ReadBytes((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));
//	DEBUG_TRACE("read data from eeprom when power up,fw ver:%d\n",g_tParam.ParamVer);
//	DEBUG_TRACE("g_tParam.Baudrate=%d\n",g_tParam.SlaveBaudrate);
//	DEBUG_TRACE("g_tParam.Project_ID=%d\n",g_tParam.Project_ID);
//	DEBUG_TRACE("g_tParam.tmc429_VMax[0]=%d\n",g_tParam.tmc429_VMax[0]);
//	DEBUG_TRACE("g_tParam.speed_home[1]=%d\n",g_tParam.speed_home[1]);
//	DEBUG_TRACE("g_tParam.speed_home[2]=%d\n",g_tParam.speed_home[2]);
	if (g_tParam.ParamVer != PARAM_VER)
	{
		g_tParam.ParamVer   = PARAM_VER;
		g_tParam.MasterBaudrate  = 115200;
		g_tParam.SlaveBaudrate   = 115200;
		g_tParam.Project_ID = OQC_FLEX;
	
#if defined(USING_INC_MBTMC429) 
		
		g_tParam.tmc429_VMax[0] = 140;
		g_tParam.tmc429_AMax[0] = 1000;
		g_tParam.tmc429_PulseDiv[0]=5;
		g_tParam.tmc429_RampDiv[0]=8;
		g_tParam.speed_home[0]=839;

		g_tParam.tmc429_VMax[1] = 140;
		g_tParam.tmc429_AMax[1] = 1000;
		g_tParam.tmc429_PulseDiv[1]=5;
		g_tParam.tmc429_RampDiv[1]=8;
		g_tParam.speed_home[1]=839;


		g_tParam.tmc429_VMax[2] = 140;
		g_tParam.tmc429_AMax[2] = 1000;
		g_tParam.tmc429_PulseDiv[2]=5;
		g_tParam.tmc429_RampDiv[2]=8;
		g_tParam.speed_home[2]=839;
		
#endif

#if defined(USING_INC_MB1616DEV6) 
		
		g_tParam.tmc429_VMax[0] = 839;
		g_tParam.tmc429_AMax[0] = 1000;
		g_tParam.tmc429_PulseDiv[0]=5;
		g_tParam.tmc429_RampDiv[0]=8;
		g_tParam.speed_home[0]=839;

		g_tParam.tmc429_VMax[1] = 839;
		g_tParam.tmc429_AMax[1] = 1000;
		g_tParam.tmc429_PulseDiv[1]=5;
		g_tParam.tmc429_RampDiv[1]=8;
		g_tParam.speed_home[1]=839;


		g_tParam.tmc429_VMax[2] = 839;
		g_tParam.tmc429_AMax[2] = 1000;
		g_tParam.tmc429_PulseDiv[2]=5;
		g_tParam.tmc429_RampDiv[2]=8;
		g_tParam.speed_home[2]=839;
		
#endif



		SaveParamToEeprom();						
	}
	
	switch(g_tParam.Project_ID)
	{
		case OQC_FLEX: 
			DEBUG_TRACE("\ncontroller init ok --- type: OQC_FLEX\n");
			break;
		case BUTTON_ROAD: 
			DEBUG_TRACE("\ncontroller init ok --- type: BUTTON_ROAD\n");
			break;
		default:
			DEBUG_TRACE("\ncontroller init ok --- type: common user\n");
		break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: SaveParamToEeprom
*	功能说明: 将全局变量g_tParam 写入到CPU内部Flash
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void SaveParamToEeprom(void)
{
	at24cxx.write(at24c256,PARAM_ADDR,(uint8_t *)&g_tParam,sizeof(PARAM_T));
	//(pageNum+55)*64
	//ee_WriteBytes((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));
}
int ParamSave(int argc, char **argv)
{
	int result = RT_EOK;
	if (argc != 3)
	{
			CMD_TRACE("Please input: ParamSave <par> <value> ......\n");
			return -RT_ERROR;
	}
	else
	{
		if (!strcmp(argv[1], "MasterBaudrate")) 
			g_tParam.MasterBaudrate  = atoi(argv[2]);		
		else if (!strcmp(argv[1], "SlaveBaudrate")) 
			g_tParam.SlaveBaudrate  = atoi(argv[2]);		
		else if (!strcmp(argv[1], "ProjectType"))
		{
			if (!strcmp(argv[2], "button_online")) 				g_tParam.Project_ID  = BUTTON_ONLINE;	
			else if (!strcmp(argv[2], "button_offline")) 	g_tParam.Project_ID  = BUTTON_OFFLINE;	
			else if (!strcmp(argv[2], "OQC-Flex")) 		    g_tParam.Project_ID  = OQC_FLEX;	
			else if (!strcmp(argv[2], "lidopen")) 				g_tParam.Project_ID  = LIDOPEN;	
			else if (!strcmp(argv[2], "button_road")) 				g_tParam.Project_ID  = BUTTON_ROAD;	
			else 
			{
				rt_kprintf("Usage: \n");
				rt_kprintf("ParamSave ProjectType button_online      -set controller fit to button online fixture\n");	
				rt_kprintf("ParamSave ProjectType button_offline     -set controller fit to button offline fixture\n");	
				
				rt_kprintf("ParamSave ProjectType button_road        -set controller fit to button road fixture\n");
				
				rt_kprintf("ParamSave ProjectType OQC-Flex           -set controller fit to OQC-Flex fixture\n");
				rt_kprintf("ParamSave ProjectType lidopen            -set controller fit to lidopen fixture\n");
				result = REPLY_INVALID_CMD;
			}
		}
		else result = RT_ERROR;
	}
	if(result == RT_EOK ) 
	{
		SaveParamToEeprom();	
		CMD_TRACE("save parameter ok and controller start to load setting......");
		//reboot();
		LoadSettingViaProID();
		CMD_TRACE("ok\n");
	}
	else if( result == RT_ERROR ) 
	{
		rt_kprintf("Usage: \n");
		rt_kprintf("ParamSave MasterBaudrate <baudrate>      -set the controller baudrate\n");	
		rt_kprintf("ParamSave SlaveBaudrate <baudrate>       -set the device baudrate\n");		
		rt_kprintf("ParamSave ProjectType <project>          -set the project type\n");	
	
	}
	
	return -RT_ERROR;
}

//按项目需要设定几个固定的速度，再固定加速度,也可以用户自定义
//用户如果想自行使用其他速度，那么需要配置P_DIV;RAMP_DIV，且速度和加速度按表格加载
void SetAmaxAutoByspeed(u8 axisNum,int speed)
{
	//16MHZ|1600steps PulseDiv=6 //speed=105->0.25/s  15.02/m 2047=4.88/S  292.83/m
	//16MHZ|1600steps PulseDiv=7 //speed=210->0.25/s  15.02/m 2047=2.44/S  146.41/m
	if(MotorConfig[axisNum].PulseDiv==6 &&	MotorConfig[axisNum].RampDiv==8)
	{
		//按项目进行速度设置 0.5mm/S为最小速度  其他速度都大于此设置值
//  if (speed==105)									MotorConfig[axisNum].AMax=550;	//0.05s-->210---0.5/S
		if (speed==105)										MotorConfig[axisNum].AMax=275;	//0.05s-->210---0.5/S
		else if (speed<50)								MotorConfig[axisNum].AMax=10;	
		else if (49<speed && speed<105)		MotorConfig[axisNum].AMax=speed+10;	
		else if (105<speed && speed<210)	MotorConfig[axisNum].AMax=speed*2;	
		else if (209<speed && speed<400)	MotorConfig[axisNum].AMax=1000;	
		else 															MotorConfig[axisNum].AMax=2047;	
		SetAMax(axisNum, MotorConfig[axisNum].AMax);
	}
	else if(MotorConfig[axisNum].PulseDiv==5 &&	MotorConfig[axisNum].RampDiv==8)
	{
		if (speed==70)										MotorConfig[axisNum].AMax=200;	//0.05s-->210---0.5/S
		else if (speed<50)								MotorConfig[axisNum].AMax=speed;	
		else if (49<speed && speed<105)		MotorConfig[axisNum].AMax=speed*2;	
		else if (104<speed && speed<210)	MotorConfig[axisNum].AMax=speed*3;		
		else 															MotorConfig[axisNum].AMax=2000;	
		SetAMax(axisNum, MotorConfig[axisNum].AMax);
	}

	else if(MotorConfig[axisNum].PulseDiv==7&&	MotorConfig[axisNum].RampDiv==10)
	{
		if (speed<25)
		{		SetAMax(axisNum, speed+5);	MotorConfig[axisNum].AMax=speed+5;}			
		else if (speed<100)
		{		SetAMax(axisNum, speed+10);	MotorConfig[axisNum].AMax=speed+10;}
		else if (speed<150)
		{		SetAMax(axisNum, speed+30);MotorConfig[axisNum].AMax=speed+30;}
		else if (speed<280)
		{		SetAMax(axisNum, speed+60);MotorConfig[axisNum].AMax=speed+60;}
		else if (speed<420)
		{		SetAMax(axisNum, speed+100);MotorConfig[axisNum].AMax=speed+100;}
		else if (speed<560)
		{		SetAMax(axisNum, speed+150);MotorConfig[axisNum].AMax=speed+150;}
		else if (speed<700)
		{		SetAMax(axisNum, speed+200);MotorConfig[axisNum].AMax=speed+200;}
		else if (speed<840)
		{		SetAMax(axisNum, speed+250);MotorConfig[axisNum].AMax=speed+250;}
		else if (speed<980)
		{		SetAMax(axisNum, speed+300);MotorConfig[axisNum].AMax=speed+300;}
		else if (speed<=1180)
		{		SetAMax(axisNum, speed+350);MotorConfig[axisNum].AMax=speed+350;}
		else if (speed<2047)
		{		SetAMax(axisNum, speed+600);MotorConfig[axisNum].AMax=speed+600;}	
	}
	else if(MotorConfig[axisNum].PulseDiv==5 &&	MotorConfig[axisNum].RampDiv==8)
	{
		if (speed<25)	//1~24
		{		SetAMax(axisNum, speed);	MotorConfig[axisNum].AMax=speed;}			
		else if (speed<50)
		{		SetAMax(axisNum, speed+5);	MotorConfig[axisNum].AMax=speed+5;}
		else if (speed<100)
		{		SetAMax(axisNum, 100);	MotorConfig[axisNum].AMax=100;}
		else if (speed<150)
		{		SetAMax(axisNum, 500);MotorConfig[axisNum].AMax=500;}
		else if (speed<250)
		{		SetAMax(axisNum, 1000);MotorConfig[axisNum].AMax=1000;}
		else if (speed<2047)
		{		SetAMax(axisNum, 2047);MotorConfig[axisNum].AMax=2047;}	
	}
}




void get_motor_position(void)
{
	for(uint8_t i=0;i<N_O_MOTORS;i++)
	{
		motorPosition[i]=Read429Int(IDX_XACTUAL|(i<<5));
	}
}
/*
Bx35_button: Y轴先里后出 ---> Z轴先上后下 ---> X轴先左后右

B445_button: Z轴先上后下回原点后再往上运行一段位置放置传感器撞到Holder---> Y轴先里后出---> X轴先左后右

ROAD			 ：Z轴先上后下				
	
*/
void MotorAutoReset_preset( void )
{	
	pressureAlarm=0;
	
	timer_stop();	
	autoRESETmotor=TRUE;
	//设置回原点速度模式下的最大速度和加速度
	MotorStop(0);MotorStop(1);MotorStop(2);
	Write429Int(IDX_VMAX|MOTOR0, 2047);		
	Write429Int(IDX_VMAX|MOTOR1, 2047);			
	Write429Int(IDX_VMAX|MOTOR2, 2047);		
	
	for(uint8_t i=0;i<3;i++)
	{
//		SetAMax(i, 2000);
		homeInfo.Homed[i]=FALSE;
		homeInfo.GoHome[i]=FALSE;
		homeInfo.GoLimit[i]=FALSE;
		SetAmaxAutoByspeed(i,g_tParam.speed_home[i]);
		//		SetAMax(i, 2000);
//		homeInfo.HomeSpeed[i]=g_tParam.speed_home[i];	
	}
	
						
	switch(g_tParam.Project_ID)
	{
		case BUTTON_OFFLINE:				
				 //Cylinder_Reset_check();
		case BUTTON_ONLINE:
				 homeInfo.GoHome[AXIS_Y]=TRUE;
				 RotateLeft(AXIS_Y,homeInfo.HomeSpeed[AXIS_Y]);	  //逆时针旋转		L		Y轴先复位		Y轴先后后前
				break;
				 
		case OQC_FLEX:
    case BUTTON_ROAD:
			
				 ActualCommand.Motor=AXIS_Z;
				 homeInfo.GoHome[ActualCommand.Motor]=TRUE;
				 homeInfo.GoLimit[ActualCommand.Motor]=FALSE;
				 homeInfo.Homed[ActualCommand.Motor]=FALSE;
				 homeInfo.HomeSpeed[ActualCommand.Motor]=-g_tParam.speed_home[ActualCommand.Motor];
				 ActualCommand.Value.Int32=homeInfo.HomeSpeed[ActualCommand.Motor];	 
		     TMCL_MotorRotate();				
				//RotateLeft(AXIS_Z,-homeInfo.HomeSpeed[AXIS_Z]);		//逆时针旋转		L		Z轴先复位		Z轴先上后下
				 CMD_TRACE("motor[%d] is start go home by searching home sensor\n",ActualCommand.Motor);
				 timer_start();
		break;	
		
		default:
			break;
	}
}
/******************       TIM         **********************/
void TIM1_UP_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_IRQn 0 */
	rt_interrupt_enter();	
  /* USER CODE END TIM1_UP_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_IRQn 1 */
	rt_interrupt_leave();
  /* USER CODE END TIM1_UP_IRQn 1 */
}
//10ms
//static uint16_t cnt_delay;
//static uint16_t cnt;
/*
V-2000=2000/140=14.2mm/s   =0.14mm/10ms   840 0.05mm  20ms---0.01mm 使用软件定时器10ms也没问题 
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	for(uint8_t i=0;i<N_O_MOTORS;i++)
	{			
		MotorHomingWithHomeSensor(i,homeInfo.HomeSpeed[i]);
		MotorHomingWithLimitSwitch(i,homeInfo.HomeSpeed[i]);
	}		
}

void MotorSensorCheck_timer_init(void)
{
	MX_TIM1_Init();
	//timer_start();
}
void timer_start(void)
{
	if(TimerOpened==0)
	{
		TimerOpened=1;
		HAL_TIM_Base_Start_IT(&htim1);
	}
}
void timer_stop(void)
{
	if(TimerOpened==1)
	{
		TimerOpened=0;
		HAL_TIM_Base_Stop_IT(&htim1);
	}
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}

#endif /* USE_FULL_ASSERT */





