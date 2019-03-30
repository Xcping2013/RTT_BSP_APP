#include "bsp_include.h" 		 
#include "bsp_hc05.h" 
//HC05���ԣ����ݽ���ģʽ��������Ҫȷ��Ӳ���ϵ�EN���ӷ�ʽ���ڲ������������������Ƿ����ȶ�����ATģʽ
#include "math.h"

#define USART3_MAX_RECV_LEN		600					//�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		600					//����ͻ����ֽ���


u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.
u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 			//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
u16 USART3_RX_STA=0;  

#define HC05_STATE_PIN		PD_13
#define HC05_EN_PIN				PE_14
#define printf	rt_kprintf
//����ֵ:0,�ɹ�;1,ʧ��.

void uart3_printf(char* fmt,...)  
{  
	u16 i; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);		//�˴η������ݵĳ���
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
	
	MX_TIM7_Init();								//10ms�ж�
	USART3_RX_STA=0;							//����
	HAL_TIM_Base_Stop_IT(&htim7);		
	
	while(retry--)
	{
		pinWrite(HC05_EN_PIN,PIN_HIGH);					//�ø�,����ATģʽ
		drv_mdelay(10);
		uart3_printf("AT\r\n");												//����AT����ָ��
		pinWrite(HC05_EN_PIN,PIN_LOW);					//�˳�ATģʽ
		
		for(t=0;t<10;t++) 			//��ȴ�50ms,������HC05ģ��Ļ�Ӧ
		{
			if(USART3_RX_STA&0X8000)break;
			drv_mdelay(5);
		}		
		if(USART3_RX_STA&0X8000)	//���յ�һ��������
		{
			temp=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
			USART3_RX_STA=0;			 
			if(temp==4&&USART3_RX_BUF[0]=='O'&&USART3_RX_BUF[1]=='K')
			{
				temp=0;//���յ�OK��Ӧ
				break;
			}
		}			    		
	}		    
	if(retry==0)temp=1;	//���ʧ��
	return temp;	  
}	 
//��ȡATK-HC05ģ��Ľ�ɫ
//����ֵ:0,�ӻ�;1,����;0XFF,��ȡʧ��.							  
u8 HC05_Get_Role(void)
{	 		    
	u8 retry=0X0F;
	u8 temp,t;
	while(retry--)
	{
		pinWrite(HC05_EN_PIN,PIN_HIGH);					//KEY�ø�,����ATģʽ
		drv_mdelay(10);
		uart3_printf("AT+ROLE?\r\n");	//��ѯ��ɫ
		for(t=0;t<20;t++) 			//��ȴ�200ms,������HC05ģ��Ļ�Ӧ
		{
			drv_mdelay(10);
			if(USART3_RX_STA&0X8000)break;
		}		
		pinWrite(HC05_EN_PIN,PIN_LOW);					//KEY����,�˳�ATģʽ
		if(USART3_RX_STA&0X8000)	//���յ�һ��������
		{
			temp=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
			USART3_RX_STA=0;			 
			if(temp==13&&USART3_RX_BUF[0]=='+')//���յ���ȷ��Ӧ����
			{
				temp=USART3_RX_BUF[6]-'0';//�õ�����ģʽֵ
				break;
			}
		}		
	}
	if(retry==0)temp=0XFF;//��ѯʧ��.
	return temp;
} 							   
//ATK-HC05��������
//�˺�����������ATK-HC05,�����ڽ�����OKӦ���ATָ��
//atstr:ATָ�.����:"AT+RESET"/"AT+UART=9600,0,0"/"AT+ROLE=0"���ַ���
//����ֵ:0,���óɹ�;����,����ʧ��.							  
u8 HC05_Set_Cmd(u8* atstr)
{	 		    
	u8 retry=0X0F;
	u8 temp,t;
	while(retry--)
	{
		pinWrite(HC05_EN_PIN,PIN_HIGH);					//KEY�ø�,����ATģʽ
		drv_mdelay(10);
		uart3_printf("%s\r\n",atstr);	//����AT�ַ���
		pinWrite(HC05_EN_PIN,PIN_LOW);					//KEY����,�˳�ATģʽ
		for(t=0;t<20;t++) 			//��ȴ�100ms,������HC05ģ��Ļ�Ӧ
		{
			if(USART3_RX_STA&0X8000)break;
			drv_mdelay(5);
		}		
		if(USART3_RX_STA&0X8000)	//���յ�һ��������
		{
			temp=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
			USART3_RX_STA=0;			 
			if(temp==4&&USART3_RX_BUF[0]=='O')//���յ���ȷ��Ӧ����
			{			
				temp=0;
				break;			 
			}
		}		
	}
	if(retry==0)temp=0XFF;//����ʧ��.
	return temp;
} 
///////////////////////////////////////////////////////////////////////////////////////////////////
//ͨ���ú���,��������USMART,���Խ��ڴ���3�ϵ�ATK-HC05ģ��
//str:���.(����ע�ⲻ����Ҫ������س���)
void HC05_CFG_CMD(u8 *str)
{					  
	u8 temp;
	u8 t;		  
	pinWrite(HC05_EN_PIN,PIN_HIGH);						//KEY�ø�,����ATģʽ
	drv_mdelay(10);
	uart3_printf("%s\r\n",(char*)str); //����ָ��
	for(t=0;t<50;t++) 				//��ȴ�500ms,������HC05ģ��Ļ�Ӧ
	{
		if(USART3_RX_STA&0X8000)break;
		drv_mdelay(10);
	}									    
	pinWrite(HC05_EN_PIN,PIN_LOW);						//KEY����,�˳�ATģʽ
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{
		temp=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
		USART3_RX_STA=0;
		USART3_RX_BUF[temp]=0;		//�ӽ�����		 
		printf("\r\n%s",USART3_RX_BUF);//���ͻ�Ӧ���ݵ�����1
	} 				 
}

void HC05_Role_Show(void)
{
	if(HC05_Get_Role()==1)rt_kprintf("ROLE:Master\n");	//����
	else rt_kprintf("ROLE:Slave\n");			 							//�ӻ�
}

void HC05_Sta_Show(void)
{												 
	if(pinRead(HC05_STATE_PIN))
	{
		rt_kprintf("STA:Connected\n");//���ӳɹ�
	}
	else rt_kprintf("STA:Disconnect\n");	 			//δ����				 
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
			uart3_printf("ALIENTEK HC05 run +1s\r\n");		//���͵�����ģ��
			count=0;
			HC05_Sta_Show();  	  
		}
			
		if(USART3_RX_STA&0X8000)			//���յ�һ��������
		{
 			reclen=USART3_RX_STA&0X7FFF;//�õ����ݳ���
		  USART3_RX_BUF[reclen]=0;	 	//���������
			if(reclen==9||reclen==8) 		//����DS1���
			{
				if(strcmp((const char*)USART3_RX_BUF,"+LED1 ON")==0)	pinWrite(PD_7,PIN_LOW); 
				if(strcmp((const char*)USART3_RX_BUF,"+LED1 OFF")==0)	pinWrite(PD_7,PIN_HIGH); 
			}
 			rt_kprintf("%s\n",USART3_RX_BUF);//��ʾ���յ�������
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
		USART3_RX_STA|=1<<15;			//��ǽ������
		HAL_TIM_Base_Stop_IT(&htim7);	
  }	
}
void MX_TIM7_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig;

  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 7199;								//psc��ʱ��Ԥ��Ƶ��
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 99;											//�Զ���װֵ��
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
			if((USART3_RX_STA&(1<<15))==0)					//�������һ������,��û�б�����,���ٽ�����������
			{ 
				if(USART3_RX_STA<USART3_MAX_RECV_LEN)	//�����Խ�������
				{
					TIM7->CNT = 0;								   		//���������          				
					if(USART3_RX_STA==0) 								//ʹ�ܶ�ʱ��7���ж� 
					{
						HAL_TIM_Base_Start_IT(&htim7);			//ʹ�ܶ�ʱ��7
					}
					USART3_RX_BUF[USART3_RX_STA++]=ch;	//��¼���յ���ֵ	 
				}else 
				{
					USART3_RX_STA|=1<<15;								//ǿ�Ʊ�ǽ������
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
//	MX_TIM7_Init();								//10ms�ж�
//	USART3_RX_STA=0;							//����
//	HAL_TIM_Base_Stop_IT(&htim7);	
//	//__HAL_TIM_DISABLE(TIM7);			//�رն�ʱ��7	
//	
//}

