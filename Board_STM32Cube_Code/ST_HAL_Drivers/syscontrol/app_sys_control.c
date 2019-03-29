//#include "app_sys_control.h"

#include "bsp_include.h"	
#include "app_include.h"

PARAM_T g_tParam;

uint8_t TimerOpened=0;

void xstrcat(char *str1,char *str2)
{
	int i,len1;
	for(i=0;str1[i]!='\0';i++);
	len1=i;
	for(i=0;str2[i]!='\0';i++)
	str1[i+len1]=str2[i];
}

/*
*********************************************************************************************************
*	函 数 名: LoadParam
*	功能说明: 从Flash读参数到g_tParam
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void LoadParam(void)
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
		g_tParam.Project_ID = COMMON;
	
		SaveParam();						
	}
	switch(g_tParam.Project_ID)
	{
		case OQC_FLEX: 
			DEBUG_TRACE("\nfixture init ok --- type: button road\n>>");
			break;
		default:
			DEBUG_TRACE("\nfixture init ok --- type: common user\n>>");
		break;
	}
}

void SaveParam(void)
{
	at24cxx.write(at24c256,PARAM_ADDR,(uint8_t *)&g_tParam,sizeof(PARAM_T));
	//(pageNum+55)*64
	//ee_WriteBytes((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));
}





