//#include "app_serial.h"
//#include "app_sys_control.h"

#include "bsp_include.h"	
#include "app_include.h"

#define uart3_rx_len_max 12

uint8_t autoPULLdata=FALSE;


static struct rt_semaphore rx_sem;	
static rt_device_t serial;
struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; 

//char uart3_name[]="uart3";
static char uart3_name[]="uart2";
static char uart3_rx_buff[200];
static uint8_t uart3_rx_len_index=0;
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
				if(Read429Short(IDX_VACTUAL|(1<<5)) ==0)				//读取信息的时候如果被其他中断调用（比如一直UART1中断命令），则SPI读取值出问题
				{
					if(motorSpeedCnt++>3)
					{
						motorSpeedCnt=0;
						rt_device_close(serial);
						if(autoPULLdata==TRUE)	autoPULLdata=FALSE;
					}
				}				
				if(uart3_rx_len_index < uart3_rx_len_max)
        {
					if(ch=='\r' || ch=='\n' )	
					{
						//DEBUG_TRACE("read data from uart3:%s\n", uart3_rx_buff);				
						uart3_rx_buff[uart3_rx_len_index]='\0';
//						get_motor_position();
						motorPosition[1]=Read429Int(IDX_XACTUAL|(1<<5));
						if(FirstDataOut<3)	FirstDataOut++;
						else 
						rt_kprintf("P[1]=%d,Press%s\n",motorPosition[1],&uart3_rx_buff);								
						uart3_rx_len_index=0;
						memset(uart3_rx_buff, '\0', uart3_rx_len_max);
					}	
					else 	uart3_rx_buff[uart3_rx_len_index++]=ch;	//读取串口缓冲器
				}
				else 
				{
					uart3_rx_len_index=0;
					memset(uart3_rx_buff,'\0', uart3_rx_len_max);	
				}

    }
}
//
int uart_PressPos_thread_init(void)
{
    rt_err_t ret = RT_EOK;
		memset(uart3_rx_buff, '\0', uart3_rx_len_max);
    serial = rt_device_find(uart3_name);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n",uart3_name);
        return RT_ERROR;
    }
//		config.baud_rate = g_tParam.SlaveBaudrate;
//		rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);	
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);		
    //rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);				/* 以中断接收及轮询发送方式打开串口设备 */
    rt_device_set_rx_indicate(serial, uart_input);				/* 设置接收回调函数 */
    //优先级需大于FINSH，防止处理数据时被MSH命令打断后采集到误数据
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



