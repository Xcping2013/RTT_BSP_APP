#include "bsp.h" 		 
//#include "bsp_hc05.h" 
//HC05���ԣ����ݽ���ģʽ��������Ҫȷ��Ӳ���ϵ�EN���ӷ�ʽ���ڲ������������������Ƿ����ȶ�����ATģʽ

#include "string.h"	 
#include "math.h"

#define HC05_STATE_PIN		PD_13
#define HC05_EN_PIN				PE_14
#define printf	rt_kprintf
//����ֵ:0,�ɹ�;1,ʧ��.
u8 HC05_Init(void)
{
	u8 retry=10,t;	  		 
	u8 temp=1;
	
	rt_pin_mode(HC05_STATE_PIN, PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(HC05_EN_PIN, PIN_MODE_OUTPUT_PP);
	
	rt_pin_write(HC05_EN_PIN,PIN_HIGH);
	
	MX_USART3_UART_Init();
	
	while(retry--)
	{
		rt_pin_write(HC05_EN_PIN,PIN_HIGH);					//�ø�,����ATģʽ
		drv_mdelay(10);
		uart3_printf("AT\r\n");												//����AT����ָ��
		rt_pin_write(HC05_EN_PIN,PIN_LOW);					//�˳�ATģʽ
		
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
		rt_pin_write(HC05_EN_PIN,PIN_HIGH);					//KEY�ø�,����ATģʽ
		drv_mdelay(10);
		uart3_printf("AT+ROLE?\r\n");	//��ѯ��ɫ
		for(t=0;t<20;t++) 			//��ȴ�200ms,������HC05ģ��Ļ�Ӧ
		{
			drv_mdelay(10);
			if(USART3_RX_STA&0X8000)break;
		}		
		rt_pin_write(HC05_EN_PIN,PIN_LOW);					//KEY����,�˳�ATģʽ
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
		rt_pin_write(HC05_EN_PIN,PIN_HIGH);					//KEY�ø�,����ATģʽ
		drv_mdelay(10);
		uart3_printf("%s\r\n",atstr);	//����AT�ַ���
		rt_pin_write(HC05_EN_PIN,PIN_LOW);					//KEY����,�˳�ATģʽ
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
	rt_pin_write(HC05_EN_PIN,PIN_HIGH);						//KEY�ø�,����ATģʽ
	drv_mdelay(10);
	uart3_printf("%s\r\n",(char*)str); //����ָ��
	for(t=0;t<50;t++) 				//��ȴ�500ms,������HC05ģ��Ļ�Ӧ
	{
		if(USART3_RX_STA&0X8000)break;
		drv_mdelay(10);
	}									    
	rt_pin_write(HC05_EN_PIN,PIN_LOW);						//KEY����,�˳�ATģʽ
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
	if(rt_pin_read(HC05_STATE_PIN))
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
	rt_pin_mode(PD_7, PIN_MODE_OUTPUT_PP);
	rt_pin_write(PD_7,PIN_LOW); 
	drv_mdelay(500);
	rt_kprintf("HC05_Init start\n"); 	  
	rt_pin_mode(HC05_STATE_PIN, PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(HC05_EN_PIN, PIN_MODE_OUTPUT_PP);
	
	rt_pin_write(HC05_EN_PIN,PIN_HIGH);
	
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
				if(strcmp((const char*)USART3_RX_BUF,"+LED1 ON")==0)	rt_pin_write(PD_7,PIN_LOW); 
				if(strcmp((const char*)USART3_RX_BUF,"+LED1 OFF")==0)	rt_pin_write(PD_7,PIN_HIGH); 
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






