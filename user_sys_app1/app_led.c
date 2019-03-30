//#include "app_led.h"
//#include "app_sys_control.h"

#include "bsp_include.h"	
#include "app_include.h"

#if defined(USING_RTT)

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_stack[ 256 ];

static struct rt_thread led_thread;

static void rt_hw_led_init(void)
{
    pinMode(LED_PIN, PIN_MODE_OUTPUT);
}

static void led_thread_entry(void *parameter)
{
    rt_hw_led_init();

    while (1)
    {
        pinWrite(LED_PIN, 0);
        rt_thread_delay(RT_TICK_PER_SECOND / 4); /* sleep 0.5 second and switch to other thread */

        pinWrite(LED_PIN, 1);
        rt_thread_delay(RT_TICK_PER_SECOND / 4);
    }
}

int led_thread_init(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&led_thread,
                            "led",
                            led_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&led_stack[0],
                            sizeof(led_stack),
                            20,
                            5);
    if (result == RT_EOK)
    {
       rt_thread_startup(&led_thread);
    }
    return 0;
}

#else 
int led_thread_init(void)
{
	static uint32_t tickLEDstart;	
	
	tickLEDstart=HAL_GetTick();
	
	if(abs((int)(HAL_GetTick()-tickLEDstart))>350)
	{
		pinToggle(LED_PIN);
		tickLEDstart=HAL_GetTick();
	}	
}
#endif
//
#if 0
static rt_timer_t timer1;
static rt_timer_t timer2;


static uint8_t OutputPin[OUTPUT_COUNT]={PA_15,PC_10,PC_11,PC_12,PD_0,PD_1,PD_2,PD_3};

static uint8_t en_led_blink[OUTPUT_COUNT];
static uint8_t cnt_led_blink[OUTPUT_COUNT];
static uint8_t set_led_blink[OUTPUT_COUNT];

static int cnt = 0;
static uint8_t run_mode = 0;
static uint16_t run_time = 0;

void rt_hw_leds_init(void)
{
	rt_uint8_t i;
	for( i=0;i<OUTPUT_COUNT;i++)
	{
		rt_pin_mode(OutputPin[i],PIN_MODE_OUTPUT);
		rt_pin_write(OutputPin[i],PIN_LOW);
		en_led_blink[i]=0;
		cnt_led_blink[i]=0;
		set_led_blink[i]=0;
	}
}
//
void led_on_off(rt_uint8_t led_no, rt_uint8_t status)
{
	if( 0<led_no && led_no < OUTPUT_COUNT+1)
	{
		if(status==0)
		{
			rt_pin_write(OutputPin[led_no-1],PIN_LOW);
		}
		else rt_pin_write(OutputPin[led_no-1],PIN_HIGH);
	}
}
//
static void led_Toggle(rt_uint8_t led_no)
{
	switch(led_no)
	{
		case 1:			GPIOA->ODR ^= BIT15;
			break;
		case 2:			GPIOC->ODR ^= BIT10;
			break;
		case 3:			GPIOC->ODR ^= BIT11;
			break;
		case 4:			GPIOC->ODR ^= BIT12;
			break;
		case 5:			GPIOD->ODR ^= BIT0;
			break;
		case 6:			GPIOD->ODR ^= BIT1;
			break;
		case 7:			GPIOD->ODR ^= BIT2;
			break;	
		case 8:			GPIOD->ODR ^= BIT3;
			break;
		default:
			break;
	}
}
//
static void led_run_Left2Right(rt_uint8_t led_no)
{
	if(led_no==0)
	{
		led_on_off(led_no,1);
		led_on_off(OUTPUT_COUNT,0);
	}
	else 
	{
		led_on_off(led_no,1);
		led_on_off(led_no-1,0);
	}
}
//
static void led_run_Right2Left(rt_uint8_t led_no)
{
	if(led_no==OUTPUT_COUNT)
	{
		led_on_off(OUTPUT_COUNT,1);
		led_on_off(0,0);
	}
	else 
	{
		led_on_off(led_no,1);
		led_on_off(led_no+1,0);
	}
}
//
static void led_allOn_One2Right(rt_uint8_t led_no)
{
	rt_uint8_t i;
	for( i=0;i<OUTPUT_COUNT;i++)
	{
		rt_pin_write(OutputPin[i],1);
	}	
	if(led_no==0)
	{
		led_on_off(led_no,1);
		led_on_off(OUTPUT_COUNT,0);
	}
	else 
	{
		led_on_off(led_no,1);
		led_on_off(led_no-1,0);
	}
}
//
static void led_allOn_One2Left(rt_uint8_t led_no)
{
	rt_uint8_t i;
	for( i=0;i<OUTPUT_COUNT;i++)
	{
		rt_pin_write(OutputPin[i],1);
	}	
	if(led_no==OUTPUT_COUNT)
	{
		led_on_off(OUTPUT_COUNT,1);
		led_on_off(0,0);
	}
	else 
	{
		led_on_off(led_no,1);
		led_on_off(led_no+1,0);
	}
}
//
/* 定时器1超时函数 */
static void timeout1(void *parameter)
{
	//rt_tick_t timeout = 300;
	rt_uint8_t i;

	for( i=0;i<OUTPUT_COUNT;i++)
	{
		if(en_led_blink[i] && (cnt_led_blink[i]++==set_led_blink[i]))	
		{
			led_Toggle(i+1);
			cnt_led_blink[i]=0;
		}
	}

	switch(run_mode)
	{
		case 1:		led_run_Left2Right(cnt++);if(cnt==OUTPUT_COUNT+1) cnt=0;	
			break;
		case 2:		if(cnt==0) cnt=OUTPUT_COUNT+1;	led_run_Right2Left(--cnt);
			break;
		case 3:		led_allOn_One2Right(cnt++);if(cnt==OUTPUT_COUNT+1) cnt=0;	
			break;
		case 4:		if(cnt==0) cnt=OUTPUT_COUNT+1;	led_allOn_One2Left(--cnt);
			break;		
		default:
			break;
	}
//    if (cnt++ >= 10)
//    {
//        rt_timer_control(timer1, RT_TIMER_CTRL_SET_ONESHOT, (void *)&timeout);
//    }
}

/* 定时器2超时函数 */
static void timeout2(void *parameter)
{
	rt_uint8_t i;
	run_mode=0;
	for( i=0;i<OUTPUT_COUNT;i++)
	{
		rt_pin_write(OutputPin[i],PIN_LOW);
	}
	//rt_kprintf("time2 out\n" );
}
//
int timer_led_init(void)
{
    /* 创建定时器1 */
    timer1 = rt_timer_create("timerLEDS1", /* 定时器名字是 timer1 */
                             timeout1, /* 超时时回调的处理函数 */
                             RT_NULL, /* 超时函数的入口参数 */
                             10, /* 定时长度，以OS Tick为单位，即10个OS Tick */
                             RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER); /* 周期性定时器 */

    /* 启动定时器 */
    if (timer1 != RT_NULL) rt_timer_start(timer1);

    /* 创建定时器2 */
    timer2 = rt_timer_create("timerLEDS2", /* 定时器名字是 timer2 */
                             timeout2, /* 超时时回调的处理函数 */
                             RT_NULL, /* 超时函数的入口参数 */
                             10, /* 定时长度为10个OS Tick */
                             RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER); /* 单次定时器 */

    /* 启动定时器 */
    if (timer2 != RT_NULL) rt_timer_start(timer2);
    return 0;
}
//
void led_blink(rt_uint8_t led_no, rt_uint8_t led_en, rt_uint8_t _100ms_delay)
{
	if( 0<led_no && led_no < OUTPUT_COUNT+1)		
	{
		en_led_blink[led_no-1]=led_en;
		cnt_led_blink[led_no-1]=0;
		set_led_blink[led_no-1]=_100ms_delay;	
		led_on_off(led_no,led_en);
	}
}
//
void led_running(rt_uint8_t mode, rt_uint8_t time)
{
	cnt=0;
	run_mode=mode;
	run_time=time*100;
	rt_timer_control(timer2, RT_TIMER_CTRL_SET_TIME, (void *)&run_time);
	rt_timer_start(timer2);
	//rt_kprintf("time2 start\n" );
}
//
MSH_CMD_EXPORT_ALIAS(timer_led_init,leds_init, ...);
MSH_CMD_EXPORT(led_blink, ...);
MSH_CMD_EXPORT(led_running, 1-L-R 2-R-L 3-A-R 4-A-L time);
#endif

