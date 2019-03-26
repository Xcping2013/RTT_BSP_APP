#include "bsp.h" 		 
//#include "bsp_hc05.h" 
//HC05调试：数据接收模式正常还需要确定硬件上的EN连接方式，内部是上拉还是下拉，是否能稳定进入AT模式

#include "string.h"	 
#include "math.h"

#define HC05_STATE_PIN		PD_13
#define HC05_EN_PIN				PE_14
#define printf	rt_kprintf
//返回值:0,成功;1,失败.
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
		rt_pin_write(HC05_EN_PIN,PIN_HIGH);					//置高,进入AT模式
		drv_mdelay(10);
		uart3_printf("AT\r\n");												//发送AT测试指令
		rt_pin_write(HC05_EN_PIN,PIN_LOW);					//退出AT模式
		
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
		rt_pin_write(HC05_EN_PIN,PIN_HIGH);					//KEY置高,进入AT模式
		drv_mdelay(10);
		uart3_printf("AT+ROLE?\r\n");	//查询角色
		for(t=0;t<20;t++) 			//最长等待200ms,来接收HC05模块的回应
		{
			drv_mdelay(10);
			if(USART3_RX_STA&0X8000)break;
		}		
		rt_pin_write(HC05_EN_PIN,PIN_LOW);					//KEY拉低,退出AT模式
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
		rt_pin_write(HC05_EN_PIN,PIN_HIGH);					//KEY置高,进入AT模式
		drv_mdelay(10);
		uart3_printf("%s\r\n",atstr);	//发送AT字符串
		rt_pin_write(HC05_EN_PIN,PIN_LOW);					//KEY拉低,退出AT模式
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
	rt_pin_write(HC05_EN_PIN,PIN_HIGH);						//KEY置高,进入AT模式
	drv_mdelay(10);
	uart3_printf("%s\r\n",(char*)str); //发送指令
	for(t=0;t<50;t++) 				//最长等待500ms,来接收HC05模块的回应
	{
		if(USART3_RX_STA&0X8000)break;
		drv_mdelay(10);
	}									    
	rt_pin_write(HC05_EN_PIN,PIN_LOW);						//KEY拉低,退出AT模式
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
	if(rt_pin_read(HC05_STATE_PIN))
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
				if(strcmp((const char*)USART3_RX_BUF,"+LED1 ON")==0)	rt_pin_write(PD_7,PIN_LOW); 
				if(strcmp((const char*)USART3_RX_BUF,"+LED1 OFF")==0)	rt_pin_write(PD_7,PIN_HIGH); 
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






