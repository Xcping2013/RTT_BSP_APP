//#include "app_serial.h"
//#include "app_sys_control.h"

#include "bsp_include.h"	
#include "app_include.h"

#define uart_rx_len_max 12

static struct rt_semaphore rx_sem;	
static rt_device_t serial;
struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; 


#if defined(USING_INC_MB1616DEV6) 
char uart_name[]="uart2";  
#endif
#if defined(USING_INC_MBTMC429) 
static char uart_name[]="uart3";
#endif
static char uart_rx_buff[200];
static uint8_t uart_rx_len_index=0;
static uint8_t FirstDataOut=0;
uint8_t AxisSpeedIsZeroCnt=0;
static uint8_t serialOpened=0;

void  openSerial(void)
{
	if(serialOpened==FALSE && Read429Short(IDX_VACTUAL|(AXIS_Z<<5)) !=0)	
	{
		serialOpened=TRUE; 
		FirstDataOut=1;		
		__HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
		//rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);	
	}
}

void  closeSerial(void)
{
	if(serialOpened==TRUE)	
	{
		serialOpened=FALSE; 	
		__HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
//		rt_device_close(serial);	
//		rt_kprintf("motor[2] is stop and stop printing data\n>>");
	}
}

static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&rx_sem);
	return RT_EOK;
}
//
static void serial_thread_entry1(void *parameter)
{
    char ch;
    while (1)
    {
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }	
				if(Read429Short(IDX_VACTUAL|(AXIS_Z<<5)) ==0 )				//��ȡ��Ϣ��ʱ������������жϵ��ã�����һֱUART1�ж��������SPI��ȡֵ������
				{
					if(AxisSpeedIsZeroCnt++ > 20)
					{
						AxisSpeedIsZeroCnt=0;
						closeSerial();
						rt_kprintf("motor[2] is stop and stop printing data\n>>");
					}
//					else if(0<AxisSpeedIsZeroCnt)
//						rt_kprintf("IDX_VACTUAL=0\n>>");
				}
//				else if(AxisSpeedIsZeroCnt!=0) AxisSpeedIsZeroCnt--;
				else AxisSpeedIsZeroCnt=0;
				if(uart_rx_len_index < uart_rx_len_max)
        {
					if(ch=='\r' || ch=='\n' )	
					{
						//DEBUG_TRACE("read data from uart3:%s\n", uart_rx_buff);				
						uart_rx_buff[uart_rx_len_index]='\0';
//						get_motor_position();
						motorPosition[AXIS_Z]=Read429Int(IDX_XACTUAL|(AXIS_Z<<5));
						if(FirstDataOut<3)	FirstDataOut++;
						else 
						rt_kprintf("P[2]=%d,Press%s\n",motorPosition[AXIS_Z],&uart_rx_buff);								
						uart_rx_len_index=0;
						memset(uart_rx_buff, '\0', uart_rx_len_max);
					}	
					else 	uart_rx_buff[uart_rx_len_index++]=ch;	//��ȡ���ڻ�����
				}
				else 
				{
					uart_rx_len_index=0;
					memset(uart_rx_buff,'\0', uart_rx_len_max);	
				}

    }
}
//
int uart_stream_thread_init1(void)
{
    rt_err_t ret = RT_EOK;
		memset(uart_rx_buff, '\0', uart_rx_len_max);
    serial = rt_device_find(uart_name);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n",uart_name);
        return RT_ERROR;
    }
//		config.baud_rate = g_tParam.SlaveBaudrate;
//		rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);	
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);		
    //rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);				/* ���жϽ��ռ���ѯ���ͷ�ʽ�򿪴����豸 */
    rt_device_set_rx_indicate(serial, uart_input);				/* ���ý��ջص����� */
    //���ȼ������FINSH����ֹ��������ʱ��MSH�����Ϻ�ɼ���������
    rt_thread_t thread = rt_thread_create("serial", serial_thread_entry1, RT_NULL, 1024, 19, 5);
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

int printdata(int argc, char **argv)
{
	if (argc !=1)
	{
		CMD_TRACE("Please input: printdata\n");
		return RT_ERROR;
	}	
	AxisSpeedIsZeroCnt=0;
	openSerial();
	return 0;
}
//

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

//			huart1.Instance->TDR = ch;

			if(ch==0x0d) 
			{
				rt_sem_release(&rx_sem);
				
				//USART_RX_STA|=0x8000; 
//				u8 len=USART_RX_STA&0x3fff;	
//				USART_RX_BUF[len]='\0';
//									
//				if(Read429Short(IDX_VACTUAL|(AXIS_Z<<5)) ==0 )				//��ȡ��Ϣ��ʱ������������жϵ��ã�����һֱUART1�ж��������SPI��ȡֵ������
//				{
//						if(AxisSpeedIsZeroCnt++ > 1)
//						{
//							AxisSpeedIsZeroCnt=0;
//							closeSerial();
//							rt_kprintf("motor[2] is stop and stop printing data\n>>");
//						}
//				}
//				else 
//				{
//					AxisSpeedIsZeroCnt=0;
//					if(FirstDataOut<2)	FirstDataOut++;
//					else 
//					{
//						motorPosition[AXIS_Z]=Read429Int(IDX_XACTUAL|(AXIS_Z<<5));
//						rt_kprintf("P[2]=%d,Press%s\n",motorPosition[AXIS_Z],&USART_RX_BUF);			
//					}
//					USART_RX_STA=0; 
//					memset(USART_RX_BUF, '\0', USART_REC_LEN);
//				}
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
									
				if(Read429Short(IDX_VACTUAL|(AXIS_Z<<5)) ==0 )				//��ȡ��Ϣ��ʱ������������жϵ��ã�����һֱUART1�ж��������SPI��ȡֵ������
				{
						if(AxisSpeedIsZeroCnt++ > 1)
						{
							AxisSpeedIsZeroCnt=0;
							closeSerial();
							rt_kprintf("motor[2] is stop and stop printing data\n>>");
						}
				}
				else 
				{
					AxisSpeedIsZeroCnt=0;
					if(FirstDataOut<2)	FirstDataOut++;
					else 
					{
						motorPosition[AXIS_Z]=Read429Int(IDX_XACTUAL|(AXIS_Z<<5));
						rt_kprintf("P[2]=%d,Press%s\n",motorPosition[AXIS_Z],&USART_RX_BUF);			
					}
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

    rt_thread_t thread = rt_thread_create("uart3", serial_thread_entry, RT_NULL, 1024, 19, 5);
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