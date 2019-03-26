//#include "app_serial.h"
//#include "app_sys_control.h"

#include "bsp_include.h"	
#include "app_include.h"

#define uart_rx_len_max 12

uint8_t autoPULLdata=FALSE;


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
static uint8_t motorSpeedCnt=0;

static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&rx_sem);
	return RT_EOK;
}
//
static void serial_thread_entry(void *parameter)
{
    char ch;
    while (1)
    {
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }	
				if(Read429Short(IDX_VACTUAL|(AXIS_Z<<5)) ==0)				//��ȡ��Ϣ��ʱ������������жϵ��ã�����һֱUART1�ж��������SPI��ȡֵ������
				{
					if(motorSpeedCnt++>3)
					{
						motorSpeedCnt=0;
						rt_device_close(serial);
						if(autoPULLdata==TRUE)	autoPULLdata=FALSE;
						rt_kprintf("motor[2] is stop and stop printing data\n>>");
					}
				}
				else if(motorSpeedCnt!=0) motorSpeedCnt--;
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
int uart_PressPos_thread_init(void)
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
    rt_thread_t thread = rt_thread_create("serial", serial_thread_entry, RT_NULL, 1024, 19, 5);
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
	if(autoPULLdata==FALSE)	
	{
		autoPULLdata=TRUE;   
		FirstDataOut=1;
		rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);	
	}
	return 0;
}
//



