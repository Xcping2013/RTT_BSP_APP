#include "bsp.h"

#define LED_SYS_PIN  PB2//37//
//#define LED_SYS_PIN  PD7

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_stack[256];

static struct rt_thread led_thread;

static void led_thread_entry(void *parameter)
{
    //rt_pin_mode(LED_SYS_PIN, PIN_MODE_OUTPUT_PP);
		pinMode(&LED_SYS_PIN,GPIO_MODE_OUTPUT_PP);
    while (1)
    {
        pinToggle(&LED_SYS_PIN);
        rt_thread_delay(RT_TICK_PER_SECOND / 2); 
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

