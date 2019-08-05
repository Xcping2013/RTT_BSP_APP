//#include "app_serial.h"
//#include "app_sys_control.h"
//applications\Doc\ChangeLists.txt
/*******************************************
1.去除函数内调用delay_ms            -->X
  static -->  volatile
2.更改中断优先级 串口3最低          -->X
3.修改RT_TICK_PER_SECOND和线程优先级-->X
4.修改开关串口，无需开关锁					-->X

现象照旧：
卡机后UART_FLAG_RXNE无变化，调用closeSerial无问题，调用openSerial就卡，再中断调用closeSerial恢复
5.串口3接收标志已经无法自动置1,也无法配置中断使能标志  程序异常后串口重新初始化是否OK

																		-->X
 
确定关键代码：HardStop(2)? closeSerial() --- openSerial

6.串口不再一直开关中断，而是一直中断取值，按提取标识来判断是否处理数据, 其他正常操作会被打断，如果X则换回
																		-->X
7.msh 不发送返回值									-->X

8.串口透传,一直从UART3收收发数据    -->Y<--

9.MCU内部控制跑流程		debug_code		-->Y<--

10.增加MSH缓存空间									---
		#define RT_CONSOLEBUF_SIZE 128-->512
		
11. overhigh!!!  \r\n --->\n

stop motor z due to pressure overhigh!!!\r\n
stop motor z due to pressure overhigh!!!\n

12.HAL_SPI_TransmitReceive(&hspi2, &aTxBuffer, &aRxBuffer, 1,5000); 5000->1000

13.spi dma?
*******************************************/
#include "bsp_include.h"	
#include "app_include.h"


/**********************************************************************************************************************/
static uint16_t input8Cnt=0;
static int next_posititon=0;	//uint32 负位置会出现错误
static struct rt_semaphore rx_sem;	

static uint8_t DataRemoveCnt=0;					//第一個字符串有可能不全
static uint8_t AxisSpeedKeepZeroCnt=0;

volatile uint8_t SerialOpened=0;

static rt_device_t serial;

/**********************************************************************************************************************/
//static void openSerial(void);

static void serial_thread_entry(void *parameter);
/**********************************************************************************************************************/
void  openSerial(void)
{
	//if(SerialOpened==FALSE && Read429Short(IDX_VACTUAL|(AXIS_Z<<5)) !=0)	
	//if(SerialOpened==FALSE)	
	//if(Read429Short(IDX_VACTUAL|(AXIS_Z<<5)) !=0)	
	{		
		#if defined(RT_USING_UART3)
				rt_device_control(serial, RT_DEVICE_CTRL_SET_INT, NULL);	
		#else
				HAL_NVIC_EnableIRQ(USART3_IRQn);
				__HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
		#endif
		DataRemoveCnt=1;	
		SerialOpened=TRUE; 
	}
}

void  closeSerial(void)
{
	//if(SerialOpened==TRUE)	
	{
		#if defined(RT_USING_UART3)
				rt_device_control(serial, RT_DEVICE_CTRL_CLR_INT, NULL);	
		#else		
				HAL_NVIC_DisableIRQ(USART3_IRQn);
				__HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
		#endif
		SerialOpened=FALSE; 	
	}
}


int printdata(int argc, char **argv)
{
	if (argc !=1)
	{
		CMD_TRACE("Please input: printdata\n");
		return RT_ERROR;
	}	
	AxisSpeedKeepZeroCnt=0;
	next_posititon=Read429Int(IDX_XTARGET|(2<<5));				
	openSerial();
	return 0;
}
int printdata_disable(int argc, char **argv)
{
	if (argc !=2)
	{
		CMD_TRACE("Please input: stop printdata\n");
		return RT_ERROR;
	}	
	if (!strcmp(argv[1], "printdata"))
	{
		closeSerial();
		printf("stop printing the pressure value ok\n");		
	}
	return 0;
}
/**********************************************************************************************************************/
#if defined(RT_USING_UART3)
#if defined(USING_INC_MB1616DEV6) 
static char uart_name[]="uart2";  
#endif
#if defined(USING_INC_MBTMC429) 
static char uart_name[]="uart3";
#endif

#define UART3_THREAD_PRIORITY 19
#define uart_rx_len_max 20
static char uart_rx_buff[30];
static uint8_t uart_rx_len_index=0;

struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; 
static void serial3_data_processing(void);

static void serial3_data_processing(void)
{
	if(Read429Short(IDX_VACTUAL|(AXIS_Z<<5)) ==0 )				//读取信息的时候如果被其他中断调用（比如一直UART1中断命令），则SPI读取值出问题
	{
		if(AxisSpeedKeepZeroCnt++ > 1)
		{
			AxisSpeedKeepZeroCnt=0;
			closeSerial();
			rt_kprintf("motor[2] is stop and stop printing data\n>>");
			
		}
	}
	else 
	{
		AxisSpeedKeepZeroCnt=0;
		if(DataRemoveCnt<2)	DataRemoveCnt++;
		else 
		{			
			motorPosition[AXIS_Z]=Read429Int(IDX_XACTUAL|(AXIS_Z<<5));
			if(next_posititon > motorPosition[AXIS_Z] && buttonRESETpressed==FALSE)
			{
				if(INPUT8==0) input8Cnt++; else input8Cnt=0;
			
				if(input8Cnt>=3) {HardStop(2);input8Cnt=0;}
			}
			rt_kprintf("P[2]=%d,Press%s\n",motorPosition[AXIS_Z],&uart_rx_buff);			
			
		}
		uart_rx_len_index=0; 
		memset(uart_rx_buff, '\0', uart_rx_len_max);
		
	}		
}
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
//	if(rt_device_read(serial, -1, &ch, 1) == 1 )
//	{
//		if(ch==0x0d) rt_sem_release(&rx_sem);
//	}
	//if(SerialOpened)
		rt_sem_release(&rx_sem);
	return RT_EOK;
}
static void serial_thread_entry(void *parameter)
{
    char ch;
    while (1)
    {
				
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }	
				rt_enter_critical();
				if(ch==0x0d || ch==0x0d) 
				{
					if(uart_rx_len_index!=0)
					serial3_data_processing();
				}
				else
				{
					if(ch!=0x0a && ch!=0x0a) 
					{
						uart_rx_buff[uart_rx_len_index]=ch;
						uart_rx_len_index++;
					}
					if(uart_rx_len_index > uart_rx_len_max) uart_rx_len_index=0; 
				}	
				rt_exit_critical();				
    }
}
//
int uart_stream_thread_init(void)
{
    rt_err_t ret = RT_EOK;
	
		memset(uart_rx_buff, '\0', uart_rx_len_max);
    serial = rt_device_find(uart_name);
	
    if (!serial)
    {
        rt_kprintf("find %s failed!\n",uart_name);
        return RT_ERROR;
    }

    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);	

		rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);		
		
		rt_device_control(serial, RT_DEVICE_CTRL_CLR_INT, NULL);	
	
		rt_device_set_rx_indicate(serial, uart_input);				

		//MX_USART3_UART_Init();
		
    rt_thread_t thread = rt_thread_create("serial", serial_thread_entry, RT_NULL, 1024, UART3_THREAD_PRIORITY, 100);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }
    return ret;	
}

#else

void USART3_IRQHandler(void)
{
	rt_interrupt_enter();
	
	int ch;
	if ((__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_RXNE) != RESET))
	{
		if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE) != RESET)
		{
			
#if defined(USING_STM32F0_HAL) 
			ch = huart3.Instance->RDR & 0xff;
#else
			ch = huart3.Instance->DR & 0xff;
#endif
			if(ch==0x0d) 
			{
				rt_sem_release(&rx_sem);
			}
			else
			{
				USART_RX_BUF[USART_RX_STA&0X3FFF]=ch;
				USART_RX_STA++;
				if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0; 
			}		 
		}
		
    __HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_RXNE);
  }
	rt_interrupt_leave();
}


static void serial_thread_entry(void *parameter)
{
    char ch;
    while (1)
    {
        rt_sem_take(&rx_sem, RT_WAITING_FOREVER);

				u8 len=USART_RX_STA&0x3fff;	
				USART_RX_BUF[len]='\0';
									
				if(Read429Short(IDX_VACTUAL|(AXIS_Z<<5)) ==0 )				//读取信息的时候如果被其他中断调用（比如一直UART1中断命令），则SPI读取值出问题
				{
						if(AxisSpeedKeepZeroCnt++ > 1)
						{
							AxisSpeedKeepZeroCnt=0;
							closeSerial();
							rt_kprintf("motor[2] is stop and stop printing data\n>>");
						}					
				}
				else 
				{
					AxisSpeedKeepZeroCnt=0;
					if(DataRemoveCnt<2)	DataRemoveCnt++;
					else 
					{
						motorPosition[AXIS_Z]=Read429Int(IDX_XACTUAL|(AXIS_Z<<5));
						rt_kprintf("P[2]=%d,Press%s\n",motorPosition[AXIS_Z],&USART_RX_BUF);			
					}
					
					rt_pin_write(outputs_pin_num[7-1],PIN_HIGH);
					USART_RX_STA=0; 
					memset(USART_RX_BUF, '\0', USART_REC_LEN);
				}

    }
}
//
int uart_stream_thread_init(void)
{
    rt_err_t ret = RT_EOK;
		USART_RX_STA=0; 
		memset(USART_RX_BUF, '\0', USART_REC_LEN);

    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);		

    rt_thread_t thread = rt_thread_create("uart3", serial_thread_entry, RT_NULL, 1024, 19, 20);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }
    return ret;	
}
#endif

#if (ENABLE_DEBUG_CODE)

void getUART3_Status(void)
{

	CMD_TRACE("SerialOpened=%d,UART_FLAG_RXNE=%d,UART_IT_RXNE=%d\n",\
	SerialOpened,__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE),__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_RXNE));//
}

void debug_exit(void *args)
{
	if(INPUT7==0)	 	
	{			
		getUART3_Status();
		MX_USART3_UART_Init();
		getUART3_Status();
		HAL_NVIC_DisableIRQ(USART3_IRQn);
	   __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
//		list_thread();
//		list_sem();
//		list_event();
//	  list_mempool();
//	  list_timer();		
	} 
}
void InitIn7AsExti(void)
{
  rt_pin_attach_irq(inputs_pin_num[7-1], PIN_IRQ_MODE_FALLING, debug_exit, RT_NULL);
	rt_pin_irq_enable(inputs_pin_num[7-1], PIN_IRQ_ENABLE);
}
void uart3_reinit(void)
{
	//HAL_UART_AbortReceive_IT(&huart3);
	MX_SPI1_Init();
	MX_USART3_UART_Init();
}
MSH_CMD_EXPORT(openSerial,...);
MSH_CMD_EXPORT(closeSerial,...);
INIT_APP_EXPORT(InitIn7AsExti);
MSH_CMD_EXPORT(getUART3_Status,...);
MSH_CMD_EXPORT(uart3_reinit,...);
//

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t code_test_stack[ 1024 ];	//256

static struct rt_thread code_test_thread;

static void rt_hw_code_test_init(void)
{
    Write429Short(IDX_VMAX|(2<<5), MotorConfig[2].VMax);
  	SetAmaxAutoByspeed(2,MotorConfig[2].VMax);
	  
}

static void code_test_thread_entry(void *parameter)
{
		rt_hw_code_test_init();
    while (1)
    {
			 Write429Int(IDX_XTARGET|(2<<5), 3000);		 
			 Set429RampMode(2, RM_RAMP);
			 rt_thread_delay(RT_TICK_PER_SECOND / 50);
			 openSerial();
			 rt_thread_delay(RT_TICK_PER_SECOND);
			 OUTPUT8=1;
			 rt_thread_delay(RT_TICK_PER_SECOND/50);
			 OUTPUT8=0;
			 rt_thread_delay(RT_TICK_PER_SECOND/50);			
			 Write429Int(IDX_XTARGET|(2<<5), 6000);
			 Set429RampMode(2, RM_RAMP);
			 rt_thread_delay(RT_TICK_PER_SECOND / 50);
			 openSerial();
			 rt_thread_delay(RT_TICK_PER_SECOND);
			 OUTPUT8=1;
			 rt_thread_delay(RT_TICK_PER_SECOND/50);
			 OUTPUT8=0;
			 rt_thread_delay(RT_TICK_PER_SECOND/50);	
    }
}

int code_test_thread_init(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&code_test_thread,
                            "code_test",
                            code_test_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&code_test_stack[0],
                            sizeof(code_test_stack),
                            20,
                            20);
    if (result == RT_EOK)
    {
       rt_thread_startup(&code_test_thread);
    }
    return 0;
}
//INIT_APP_EXPORT(code_test_thread_init);
#endif

