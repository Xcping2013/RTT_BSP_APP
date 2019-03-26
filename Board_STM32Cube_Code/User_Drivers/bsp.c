#include "bsp.h"

void cpuReset(void)
{
	
}

void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

static void hw_eeprom_debug(void)
{
	u8 *save_info="this page data save for eeprom test";
	u8 read_info[64];
	u8 len=strlen(save_info);
					
	rt_kprintf("eeprom test..................");
	at24cxx.write(at24c256,100,save_info,len);
	at24cxx.read(at24c256 , 100, read_info, len);
	
  if(Buffercmp((uint8_t*)save_info,(uint8_t*)read_info,len))
  {
    rt_kprintf("err\r\n>>");
  }	
	else rt_kprintf("ok\r\n>>");
	
}

void IoTest(void)
{
	u8 channel;
	for(channel=1;channel<17;channel++)
	{
		SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
		SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	
		if(channel<9)	
		{
			pca9539a_1.out_data= ((pca9539a_1.in_data & (1 << (channel-1))? 1:0))? (pca9539a_1.out_data | (1 << (channel-1))) :(pca9539a_1.out_data & ((uint8_t) ~(1 << (channel-1))));
			SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
		}
		else
		{
			pca9539a_2.out_data= ((pca9539a_2.in_data & (1 << (channel-9))? 1:0))? (pca9539a_2.out_data | (1 << (16-channel))) :(pca9539a_2.out_data & ((uint8_t) ~(1 << (16-channel))));
			SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
		}				
	}
}
//
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t board_debug_stack[512];

static struct rt_thread board_debug_thread;

static void board_debug_thread_entry(void *parameter)
{
		hw_eeprom_debug();
    while (1)
    {
			IoTest();	    
      rt_thread_delay(5);
    }
}
static rt_thread_t tid1 = RT_NULL;
	
static void thread1_cleanup(struct rt_thread *tid)
{
    if (tid != tid1)
    {
        return ;
    }
    //rt_kprintf("thread1 end\n");
    tid1 = RT_NULL;
}

static u8 debug_inited=0;
int board_debug_thread_init(void)
{
//    rt_err_t result;

//    /* init board_debug thread */
//    result = rt_thread_init(&board_debug_thread,
//                            "board_debug",
//                            board_debug_thread_entry,
//                            RT_NULL,
//                            (rt_uint8_t *)&board_debug_stack[0],
//                            sizeof(board_debug_stack),
//                            20,
//                            5);
//    if (result == RT_EOK)
//    {
//        rt_thread_startup(&board_debug_thread);
//    }
	if(debug_inited==0)
	{
    /* init board_debug thread */
    tid1 = rt_thread_create("board_debug",
                            board_debug_thread_entry,
                            RT_NULL,
                            sizeof(board_debug_stack),
                            20,
                            5);

		if (tid1 != RT_NULL)
    {
        tid1->cleanup = thread1_cleanup;
        rt_thread_startup(tid1);
    }
		debug_inited=1;
	}
  return 0;
}
