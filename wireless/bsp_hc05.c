#include "bsp_include.h" 		 
#include "bsp_hc05.h" 
//HC05调试：数据接收模式正常还需要确定硬件上的EN连接方式，内部是上拉还是下拉，是否能稳定进入AT模式
#include "math.h"

#define USART3_MAX_RECV_LEN		600					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		600					//最大发送缓存字节数


u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 			//发送缓冲,最大USART3_MAX_SEND_LEN字节
u16 USART3_RX_STA=0;  

#define HC05_STATE_PIN		PD_13
#define HC05_EN_PIN				PE_14
#define printf	rt_kprintf
//返回值:0,成功;1,失败.

void uart3_printf(char* fmt,...)  
{  
	u16 i; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);		//此次发送数据的长度
	HAL_UART_Transmit(&huart3, (uint8_t *)&USART3_TX_BUF, i, 0xFFFF);
}

u8 HC05_Init(void)
{
	u8 retry=10,t;	  		 
	u8 temp=1;
	
	pinMode(HC05_STATE_PIN, PIN_MODE_INPUT_PULLUP);
	pinMode(HC05_EN_PIN, PIN_MODE_OUTPUT_PP);
	
	pinWrite(HC05_EN_PIN,PIN_HIGH);
	
	MX_USART3_UART_Init();
	
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
	
	MX_TIM7_Init();								//10ms中断
	USART3_RX_STA=0;							//清零
	HAL_TIM_Base_Stop_IT(&htim7);		
	
	while(retry--)
	{
		pinWrite(HC05_EN_PIN,PIN_HIGH);					//置高,进入AT模式
		drv_mdelay(10);
		uart3_printf("AT\r\n");												//发送AT测试指令
		pinWrite(HC05_EN_PIN,PIN_LOW);					//退出AT模式
		
		for(t=0;t<10;t++) 			//最长等待50ms,来接收HC05模块的回应
		{
			if(USART3_RX_STA&0X8000)break;
			drv_mdelay(5);
		}		
		if(USART3_RX_STA&0X8000)	//接收到一次数据了
		{
			temp=USART3_RX_STA&0X7FFF;	//得到数据长度
			USART3_RX_STA=0;			 
			if(temp==4&&USART3_RX_BUF[0]=='O'&&USART3_RX_BUF[1]=='K')
			{
				temp=0;//接收到OK响应
				break;
			}
		}			    		
	}		    
	if(retry==0)temp=1;	//检测失败
	return temp;	  
}	 
//获取ATK-HC05模块的角色
//返回值:0,从机;1,主机;0XFF,获取失败.							  
u8 HC05_Get_Role(void)
{	 		    
	u8 retry=0X0F;
	u8 temp,t;
	while(retry--)
	{
		pinWrite(HC05_EN_PIN,PIN_HIGH);					//KEY置高,进入AT模式
		drv_mdelay(10);
		uart3_printf("AT+ROLE?\r\n");	//查询角色
		for(t=0;t<20;t++) 			//最长等待200ms,来接收HC05模块的回应
		{
			drv_mdelay(10);
			if(USART3_RX_STA&0X8000)break;
		}		
		pinWrite(HC05_EN_PIN,PIN_LOW);					//KEY拉低,退出AT模式
		if(USART3_RX_STA&0X8000)	//接收到一次数据了
		{
			temp=USART3_RX_STA&0X7FFF;	//得到数据长度
			USART3_RX_STA=0;			 
			if(temp==13&&USART3_RX_BUF[0]=='+')//接收到正确的应答了
			{
				temp=USART3_RX_BUF[6]-'0';//得到主从模式值
				break;
			}
		}		
	}
	if(retry==0)temp=0XFF;//查询失败.
	return temp;
} 							   
//ATK-HC05设置命令
//此函数用于设置ATK-HC05,适用于仅返回OK应答的AT指令
//atstr:AT指令串.比如:"AT+RESET"/"AT+UART=9600,0,0"/"AT+ROLE=0"等字符串
//返回值:0,设置成功;其他,设置失败.							  
u8 HC05_Set_Cmd(u8* atstr)
{	 		    
	u8 retry=0X0F;
	u8 temp,t;
	while(retry--)
	{
		pinWrite(HC05_EN_PIN,PIN_HIGH);					//KEY置高,进入AT模式
		drv_mdelay(10);
		uart3_printf("%s\r\n",atstr);	//发送AT字符串
		pinWrite(HC05_EN_PIN,PIN_LOW);					//KEY拉低,退出AT模式
		for(t=0;t<20;t++) 			//最长等待100ms,来接收HC05模块的回应
		{
			if(USART3_RX_STA&0X8000)break;
			drv_mdelay(5);
		}		
		if(USART3_RX_STA&0X8000)	//接收到一次数据了
		{
			temp=USART3_RX_STA&0X7FFF;	//得到数据长度
			USART3_RX_STA=0;			 
			if(temp==4&&USART3_RX_BUF[0]=='O')//接收到正确的应答了
			{			
				temp=0;
				break;			 
			}
		}		
	}
	if(retry==0)temp=0XFF;//设置失败.
	return temp;
} 
///////////////////////////////////////////////////////////////////////////////////////////////////
//通过该函数,可以利用USMART,调试接在串口3上的ATK-HC05模块
//str:命令串.(这里注意不再需要再输入回车符)
void HC05_CFG_CMD(u8 *str)
{					  
	u8 temp;
	u8 t;		  
	pinWrite(HC05_EN_PIN,PIN_HIGH);						//KEY置高,进入AT模式
	drv_mdelay(10);
	uart3_printf("%s\r\n",(char*)str); //发送指令
	for(t=0;t<50;t++) 				//最长等待500ms,来接收HC05模块的回应
	{
		if(USART3_RX_STA&0X8000)break;
		drv_mdelay(10);
	}									    
	pinWrite(HC05_EN_PIN,PIN_LOW);						//KEY拉低,退出AT模式
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{
		temp=USART3_RX_STA&0X7FFF;	//得到数据长度
		USART3_RX_STA=0;
		USART3_RX_BUF[temp]=0;		//加结束符		 
		printf("\r\n%s",USART3_RX_BUF);//发送回应数据到串口1
	} 				 
}

void HC05_Role_Show(void)
{
	if(HC05_Get_Role()==1)rt_kprintf("ROLE:Master\n");	//主机
	else rt_kprintf("ROLE:Slave\n");			 							//从机
}

void HC05_Sta_Show(void)
{												 
	if(pinRead(HC05_STATE_PIN))
	{
		rt_kprintf("STA:Connected\n");//连接成功
	}
	else rt_kprintf("STA:Disconnect\n");	 			//未连接				 
}	


ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t hc05_stack[ 512 ];

static struct rt_thread hc05_thread;

static void hc05_thread_entry(void *parameter)
{
	uint16_t count=0;
	u8 sendbuf[20];	
	u8 reclen=0;  
	pinMode(PD_7, PIN_MODE_OUTPUT_PP);
	pinWrite(PD_7,PIN_LOW); 
	drv_mdelay(500);
	rt_kprintf("HC05_Init start\n"); 	  
	pinMode(HC05_STATE_PIN, PIN_MODE_INPUT_PULLUP);
	pinMode(HC05_EN_PIN, PIN_MODE_OUTPUT_PP);
	
	pinWrite(HC05_EN_PIN,PIN_HIGH);
	
	MX_USART3_UART_Init();
	rt_kprintf("HC05_Init finsh\n"); 	
//	HC05_Role_Show();
	drv_mdelay(100);
	USART3_RX_STA=0;

	while (1)
	{
		count++;
		if(count==50)
		{
			uart3_printf("ALIENTEK HC05 run +1s\r\n");		//发送到蓝牙模块
			count=0;
			HC05_Sta_Show();  	  
		}
			
		if(USART3_RX_STA&0X8000)			//接收到一次数据了
		{
 			reclen=USART3_RX_STA&0X7FFF;//得到数据长度
		  USART3_RX_BUF[reclen]=0;	 	//加入结束符
			if(reclen==9||reclen==8) 		//控制DS1检测
			{
				if(strcmp((const char*)USART3_RX_BUF,"+LED1 ON")==0)	pinWrite(PD_7,PIN_LOW); 
				if(strcmp((const char*)USART3_RX_BUF,"+LED1 OFF")==0)	pinWrite(PD_7,PIN_HIGH); 
			}
 			rt_kprintf("%s\n",USART3_RX_BUF);//显示接收到的数据
 			USART3_RX_STA=0;	 
		}	 
		rt_thread_delay(2);
	}
}

int hc05_sample_init(void)
{
    rt_err_t result;

    /* init hc05 thread */
    result = rt_thread_init(&hc05_thread,
                            "hc05",
                            hc05_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&hc05_stack[0],
                            sizeof(hc05_stack),
                            23,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&hc05_thread);
    }
    return 0;
}

MSH_CMD_EXPORT(HC05_Set_Cmd,...);
MSH_CMD_EXPORT(HC05_Sta_Show,...);
MSH_CMD_EXPORT(hc05_sample_init,...);

void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */
	rt_interrupt_enter();	
  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */
	rt_interrupt_leave();	
  /* USER CODE END TIM7_IRQn 1 */
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance==TIM7)
  {
		USART3_RX_STA|=1<<15;			//标记接收完成
		HAL_TIM_Base_Stop_IT(&htim7);	
  }	
}
void MX_TIM7_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig;

  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 7199;								//psc：时钟预分频数
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 99;											//自动重装值。
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	HAL_TIM_Base_Start_IT(&htim7);
}

void USART3_IRQHandler(void)
{ 
	rt_interrupt_enter();	
	
	if ((__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_RXNE) != RESET))
	{
		if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE) != RESET)
		{ 
			int ch = huart3.Instance->DR & 0xff;
			//huart1.Instance->DR = ch;
			if((USART3_RX_STA&(1<<15))==0)					//接收完的一批数据,还没有被处理,则不再接收其他数据
			{ 
				if(USART3_RX_STA<USART3_MAX_RECV_LEN)	//还可以接收数据
				{
					TIM7->CNT = 0;								   		//计数器清空          				
					if(USART3_RX_STA==0) 								//使能定时器7的中断 
					{
						HAL_TIM_Base_Start_IT(&htim7);			//使能定时器7
					}
					USART3_RX_BUF[USART3_RX_STA++]=ch;	//记录接收到的值	 
				}else 
				{
					USART3_RX_STA|=1<<15;								//强制标记接收完成
				} 
			}
		}
	} 
	__HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_RXNE);
	
	rt_interrupt_leave();	
}  
//void MX_USART3_UART_Init(void)
//{

//  huart3.Instance = USART3;
//  huart3.Init.BaudRate = 9600;
//  huart3.Init.WordLength = UART_WORDLENGTH_8B;
//  huart3.Init.StopBits = UART_STOPBITS_1;
//  huart3.Init.Parity = UART_PARITY_NONE;
//  huart3.Init.Mode = UART_MODE_TX_RX;
//  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
//  if (HAL_UART_Init(&huart3) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }
//	__HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
//	
//	MX_TIM7_Init();								//10ms中断
//	USART3_RX_STA=0;							//清零
//	HAL_TIM_Base_Stop_IT(&htim7);	
//	//__HAL_TIM_DISABLE(TIM7);			//关闭定时器7	
//	
//}

