
#include "bsp_include.h"	
#include "app_include.h"

//#define RT_USING_UART2

#if defined(USING_INC_MB1616DEV6) 

uint8_t LED_PIN=PB_2;
//MB1616DEV6
at24cxx_t at24c256=
{
	{0},
	{PC_14,PC_15},
	0xA2,	//1 0 1 0 0 0 1 0
};

int mb1616dev6_hw_init(void)
{		
	SysRunLed_thread_init();
	dido_hw_init();
	at24cxx_hw_init();
	
	tmc429_hw_init();
	MotorSensorCheck_timer_init();	
	MotorLimitCheck_thread_init();

	uart_stream_thread_init();	
	
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
	
	rt_kprintf("fixture init ok\n");

  return 0;
}

INIT_APP_EXPORT(mb1616dev6_hw_init);

MSH_CMD_EXPORT(readinput, read the state of the input channel);
MSH_CMD_EXPORT(output, set the output on-off-toggle);
//MSH_CMD_EXPORT(rgb, set the rgb led);
//MSH_CMD_EXPORT(beep, set the beeper);
MSH_CMD_EXPORT(savedata, save data to page 1~200);
MSH_CMD_EXPORT(readdata, read data from page 1~200);
//MSH_CMD_EXPORT(FunctionTest, Check whether the control is normal);
MSH_CMD_EXPORT(reboot, cpu reboot);
MSH_CMD_EXPORT(motor, control motor motion by commands);
MSH_CMD_EXPORT(printdata, auto print position and press);
MSH_CMD_EXPORT(ParamSave, save parameter as set and load it after board restart);


//MSH_CMD_EXPORT(memcheck, check memory data);
//MSH_CMD_EXPORT(memtrace, dump memory trace information);
//MSH_CMD_EXPORT(version, show RT-Thread version information);
//MSH_CMD_EXPORT(list_thread, list thread);
//MSH_CMD_EXPORT(list_sem, list semaphore in system);
//MSH_CMD_EXPORT(list_event, list event in system);
//MSH_CMD_EXPORT(list_mutex, list mutex in system);
//MSH_CMD_EXPORT(list_mailbox, list mail box in system);
//MSH_CMD_EXPORT(list_msgqueue, list message queue in system);
//MSH_CMD_EXPORT(list_memheap, list memory heap in system);
//MSH_CMD_EXPORT(list_mempool, list memory pool in system);
//MSH_CMD_EXPORT(list_timer, list timer in system);
//MSH_CMD_EXPORT(list_device, list device in system);

/*		board seft_debug
*/
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
    rt_kprintf("err\n>>");
  }	
	else rt_kprintf("ok\n>>");
}

static void IoTest(void)
{
	for(u8 channel=1;channel<17;channel++)
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
      rt_thread_delay(50);
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
                            20);

		if (tid1 != RT_NULL)
    {
        //tid1->cleanup = thread1_cleanup;
        rt_thread_startup(tid1);
    }
		debug_inited=1;
	}
  return 0;
}

MSH_CMD_EXPORT(board_debug, list device in system);
#endif
