
#include "bsp_include.h"	
#include "app_include.h"

#include "inc_mbtmc429.h"

uint8_t buttonSTARTpressed=0;
uint8_t buttonRESETpressed=0;

//气缸下压 4 [1] 3 [0]	IN4下压触发	光栅触发是0
#define BUTTON_START 	  INPUT1
#define BUTTON_RESET 		INPUT2
#define CYLINDER_START	INPUT3
#define CYLINDER_END	  INPUT4
#define SAFETY_LIGHT	  INPUT5
#define DUT_BUTTON  		INPUT6

#define BUTTON_START_LED 	  OUTPUT1
#define BUTTON_RESET_LED 	  OUTPUT2
#define CYLINDER_START_OUT	OUTPUT3
#define CYLINDER_END_OUT		OUTPUT4
#define ALARM_LED_RED   		OUTPUT5
#define ALARM_LED_GREEN   	OUTPUT6
#define ALARM_SPEEKER		   	OUTPUT7

rt_uint8_t beep_flash_on=0;

void cylinder_down(void)
{
	while(SAFETY_LIGHT==1)
	{
		delay_ms(1000);			
	}
	OUTPUT3=0;
	delay_ms(100);
	OUTPUT4=1;
}

void cylinder_up(void)
{
	while(SAFETY_LIGHT==1)
	{
		delay_ms(1000);			
	}
	OUTPUT3=1;
	delay_ms(100);
	OUTPUT4=0;
}
void FlexButtonTest_APP(void)
{
	if(buttonRESETpressed==FALSE)																											//电机在复位中，START无效
	{
		if( BUTTON_START==IN_ON && buttonSTARTpressed==FALSE )					
		{
			delay_ms(20);		
			if( BUTTON_START==IN_ON )											 																//开始按键
			{
				 BUTTON_START_LED=1;	 																	 									 			
				 buttonSTARTpressed=TRUE;  
				 cylinder_down();	
			}
		}
	}
	if(buttonSTARTpressed==TRUE)
	{
		if(CYLINDER_END==0 && CYLINDER_START==1)
		{
			delay_ms(20);
			if(CYLINDER_END==0 && CYLINDER_START==1)
			{
				buttonSTARTpressed=FALSE;
				BUTTON_START_LED=0;
				if(DUT_BUTTON==0)
				{
					delay_ms(20);
					if(DUT_BUTTON==0)
					{
						ALARM_LED_GREEN=1;
						ALARM_LED_RED=0;
						beep_flash_on=0;		
					}
					else 
					{
						ALARM_LED_GREEN=0;
						ALARM_LED_RED=1;	
						beep_flash_on=1;						
					}
				}
			}
		}
	}	
	if(buttonSTARTpressed==FALSE)		
	{
		if( BUTTON_RESET==IN_ON && buttonRESETpressed==FALSE )							
		{
			delay_ms(20);
			if( BUTTON_RESET==IN_ON )																				
			{		
				BUTTON_RESET_LED=1;	
				buttonRESETpressed=TRUE;	
				cylinder_up();
			}
		}
	}
	if(buttonRESETpressed==TRUE)
	{
		if(CYLINDER_END==1 && CYLINDER_START==0)
		{
			delay_ms(20);
			if(CYLINDER_END==1 && CYLINDER_START==0)
			{
				buttonRESETpressed=FALSE;
				BUTTON_RESET_LED=0;
			}
		}
	}		
}


ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t button_stack[ 512 ];

static struct rt_thread button_thread;

static void button_thread_entry(void *parameter)
{
		ALARM_LED_RED=0;
		ALARM_LED_GREEN=0;
		ALARM_SPEEKER=0;
    while (1)
    {
				FlexButtonTest_APP();
        rt_thread_delay(10);
    }
}

int button_thread_init(void)
{
    rt_err_t result;

    /* init button thread */
    result = rt_thread_init(&button_thread,
                            "button",
                            button_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&button_stack[0],
                            sizeof(button_stack),
                            20,
                            5);
    if (result == RT_EOK)
    {
       rt_thread_startup(&button_thread);
    }
    return 0;
}


static rt_timer_t timer2;

static rt_uint16_t beep_flash_delay;
static rt_uint16_t beep_flash_cnt;


static void timeout2(void *parameter)
{
	if (beep_flash_on==1)
	{
		if(beep_flash_delay++>300)
		{ 
			beep_flash_on=0;rt_pin_write(rgb_pin_num[1],0);
		}
		if(beep_flash_cnt++>50)
		{
			pinToggle(rgb_pin_num[1]);
			beep_flash_cnt=0;
		}	
	}
}
//
static int timer2_10ms_init(void)
{
	timer2 = rt_timer_create("timer2", timeout2,
                             RT_NULL, 1,
                             RT_TIMER_FLAG_PERIODIC);
  if (timer2 != RT_NULL) 
		rt_timer_start(timer2);	
	return 0;
}
//
uint8_t LED_PIN=PD_7;

at24cxx_t at24c256=
{
	{0},
	{PB_4,PB_3},	//SDA SCL
	0xA0,
};

int mbtmc429_hw_init(void)
{		
	led_thread_init();
	dido_gpio_init();
	at24cxx_hw_init();

	timer2_10ms_init();
	button_thread_init();
	
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
	
	rt_kprintf("fixture init ok\n");

  return 0;
}

INIT_APP_EXPORT(mbtmc429_hw_init);

MSH_CMD_EXPORT(readinput, read the state of the input channel);
MSH_CMD_EXPORT(output, set the output on-off-toggle);

MSH_CMD_EXPORT(beep, set the beeper);
MSH_CMD_EXPORT(savedata, save data to page 1~200);
MSH_CMD_EXPORT(readdata, read data from page 1~200);
MSH_CMD_EXPORT(reboot, cpu reboot);

