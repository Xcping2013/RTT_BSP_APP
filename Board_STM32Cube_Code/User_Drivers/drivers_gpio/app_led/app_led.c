#include "bsp.h"

static rt_timer_t timer1;
static rt_timer_t timer2;

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
		rt_pin_mode(OutputPin[i],PIN_MODE_OUTPUT_PP);
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
/* ��ʱ��1��ʱ���� */
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

/* ��ʱ��2��ʱ���� */
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
    /* ������ʱ��1 */
    timer1 = rt_timer_create("timer1", /* ��ʱ�������� timer1 */
                             timeout1, /* ��ʱʱ�ص��Ĵ������� */
                             RT_NULL, /* ��ʱ��������ڲ��� */
                             10, /* ��ʱ���ȣ���OS TickΪ��λ����10��OS Tick */
                             RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER); /* �����Զ�ʱ�� */

    /* ������ʱ�� */
    if (timer1 != RT_NULL) rt_timer_start(timer1);

    /* ������ʱ��2 */
    timer2 = rt_timer_create("timer2", /* ��ʱ�������� timer2 */
                             timeout2, /* ��ʱʱ�ص��Ĵ������� */
                             RT_NULL, /* ��ʱ��������ڲ��� */
                             10, /* ��ʱ����Ϊ10��OS Tick */
                             RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER); /* ���ζ�ʱ�� */

    /* ������ʱ�� */
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