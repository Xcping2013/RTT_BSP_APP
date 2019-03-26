#ifndef __APP_LED_H
#define __APP_LED_H

void rt_hw_leds_init(void);
void led_on_off(rt_uint8_t led_no, rt_uint8_t status);

int timer_led_init(void);
void led_blink(rt_uint8_t led_no, rt_uint8_t led_en,rt_uint8_t _100ms_delay);
void led_running(rt_uint8_t mode, rt_uint8_t time);

#endif

