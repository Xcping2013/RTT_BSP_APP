/*
 * File      : app_uart.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 *2017-12-15      DQL          the first version 
 */

#include <rthw.h>
#include <rtthread.h>

#include "app_uart.h"

#define UARTs_RX_EVENT(UART_COUNT) (1 << UART_COUNT)

char *uarts[UART_COUNT]={"uart2"};//,"uart3","uart4","uart5","uart6","uart7","uart8"};

uint8_t uartsDelay[UART_COUNT]={0};
uint8_t uartsGot[UART_COUNT]={0};

static struct rt_event event;

static rt_device_t uart_device[UART_COUNT]  = {RT_NULL};

uint8_t uart_no=0;

rt_uint8_t	DataFromUart2[512];
rt_uint8_t	Data2_Index=0;	
rt_uint8_t	Data2_len=0;

static rt_err_t uart_intput(rt_device_t dev, rt_size_t size)
{
    //rt_event_send(&event, UARTs_RX_EVENT(uart_no));
		Data2_len=size;
    return RT_EOK;
}

rt_uint8_t uart_getchar(rt_device_t dev, rt_uint8_t no)
{
    rt_uint32_t e;
    rt_uint8_t ch;

		if (dev != RT_NULL)
		{
			/* 读取1字节数据 */
			while (rt_device_read(dev, 0, &ch, 1) != 1)
			{
				//rt_event_recv(&event, UARTs_RX_EVENT(no),RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER, &e);
			}
			return ch;
		}
    else 
		{
			rt_kprintf("device no find\n");
			return ch;
		}
}
void uart_putchar(rt_device_t dev, const rt_uint8_t c)
{
    rt_size_t len = 0;
    rt_uint32_t timeout = 0;
		if (dev != RT_NULL)
		{
			do
			{
					len = rt_device_write(dev, 0, &c, 1);
					timeout++;
			}
			while (len != 1 && timeout < 500);
		}
}

void uart_putstring(rt_device_t dev, const rt_uint8_t *s)
{
    while(*s)
    {
        uart_putchar(dev,*s++);
    }
}

void uart2_send(rt_uint8_t no, rt_uint8_t *s)
{
	uart_putstring(uart_device[no],s);
}

rt_err_t uart_open(const char *name)
{
    rt_err_t res;
		uint8_t i;
	
		for(i=0;i<UART_COUNT;i++)
		{
			if(rt_strcmp(uarts[i],name)==0) 
			{ 
				uart_device[i] = rt_device_find(name);  uart_no=i; 
				if (uart_device[i] != RT_NULL)
				{   
					res = rt_device_set_rx_indicate(uart_device[i], uart_intput);
			
					if (res != RT_EOK)
					{
            rt_kprintf("set %s rx indicate error.%d\n",name,res);
            return -RT_ERROR;
					}
					res = rt_device_open(uart_device[i], RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX );       

					if (res != RT_EOK)
					{
            rt_kprintf("open %s device error.%d\n",name,res);
            return -RT_ERROR;
					}
					rt_event_init(&event, "event", RT_IPC_FLAG_FIFO); 
				}
				else
				{
					rt_kprintf("can't find %s device.\n",name);
					return -RT_ERROR;
				}
				return RT_EOK;
			}
		}
		return RT_ERROR;
}


void uarts_buffer_entry(void* parameter)
{
	rt_uint32_t e;
	rt_uint8_t ch;
	rt_memset(DataFromUart2,'\0',512);
	while(1)
	{
//		while (rt_device_read(uart_device[0], 0, &ch, 1) !=1)
//		{
//			//rt_event_recv(&event, UARTs_RX_EVENT(0),RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER, &e);
			rt_thread_delay(1);
//		}
			//if(Data2_len) DataFromUart2[Data2_Index++]=ch;
	}
}
void uarts_thread_entry(void* parameter)
{    
    //rt_uint8_t uart_rx_data;
		rt_uint8_t	i;
	
		for(i=0;i<UART_COUNT;i++)
		{
			if (uart_open(uarts[i]) != RT_EOK)
			{
					rt_kprintf("%s open error.\n",uarts[i]);
					while (1)
					{
							rt_thread_delay(10);
				  }
			}
			else rt_kprintf("%s open ok.\n",uarts[i]);
		}
//    /* 单个字符写 */
//    uart_putchar('2');
//    uart_putchar('0');
//    uart_putchar('1');
//    uart_putchar('8');
//    uart_putchar('\n');
//    /* 写字符串 */
//    uart_putstring(uart_device[0],"Hello RT-Thread!\r\n");

    while (1)
    {   
        //uart_rx_data = uart_getchar(uart_device[0],0);
			
        //uart_putchar(uart_device[0],uart_rx_data);
				
				
				if(uartsDelay[0]!=0) uartsDelay[0]--;
				if(uartsDelay[0]==0 && uartsGot[0]== 1)
				{
					for(Data2_Index=0;Data2_Index<Data2_len;Data2_Index++)
					{
						rt_device_read(uart_device[0], 0, &DataFromUart2[Data2_Index], 1);
					}
					rt_kprintf("uart_device[0]_data=%s\n",DataFromUart2);
					uartsGot[0]=0;	Data2_Index=0;
					rt_memset(DataFromUart2,'\0',512);
				}
				rt_thread_delay(1);
				rt_kprintf("uartsDelay[0]=%d,uartsGot[0]=%d\n",uartsDelay[0],uartsGot[0]);
    }            
}

int uarts_thread_init(void)
{
    rt_thread_t uarts_process_thread;
		rt_thread_t uarts_rx_thread;

    uarts_process_thread = rt_thread_create("uarts_process",
                    uarts_thread_entry, 
                    RT_NULL,
                    1024, 
                    2, 
                    10);
	
    if (uarts_process_thread != RT_NULL)
        rt_thread_startup(uarts_process_thread);
		
    uarts_rx_thread = rt_thread_create("uart_get_buf",
                    uarts_buffer_entry, 
                    RT_NULL,
                    1024, 
                    2, 
                    10);
	
    if (uarts_rx_thread != RT_NULL)
        rt_thread_startup(uarts_rx_thread);

    return 0;
}


