//#include "app_led.h"
//#include "app_sys_control.h"

#include "bsp_include.h"	
#include "app_include.h"

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_stack[ 512 ];

static struct rt_thread led_thread;

static void rt_hw_led_init(void)
{
    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);
}

static void led_thread_entry(void *parameter)
{
    rt_hw_led_init();

    while (1)
    {
        rt_pin_write(LED_PIN, 0);
        rt_thread_delay(RT_TICK_PER_SECOND / 4); /* sleep 0.5 second and switch to other thread */

        rt_pin_write(LED_PIN, 1);
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
