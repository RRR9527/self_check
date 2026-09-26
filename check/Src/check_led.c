#include "check_led.h"

static all_leds ALL_leds;

check_result led_checking(void)
{
    if (!ALL_leds.led_start)
    {
        for (uint8_t i = 0; i < LED_NUM; i++)
        {
            ALL_leds.ind_led[i].blink_count = 0U;
            ALL_leds.ind_led[i].led_id      = i;
            ALL_leds.ind_led[i].led_state   = OFF;
        }

        ALL_leds.led_start = 1U;
        ALL_leds.cur_led   = 0U;
        ALL_leds.led_timer.begin_tick  = HAL_GetTick();
        ALL_leds.led_timer.duration    = 200U;
        ALL_leds.led_timer.timer_state = Timer_ON;
    }

    /*逐个闪烁，人眼判断LED的好坏*/
    if (!time_expire(ALL_leds.led_timer))
    {
        return CHECK_BUSY;
    }

    if 
    (
        ALL_leds.ind_led[ALL_leds.cur_led].led_state == OFF
    )
    {
        Led_ON(ALL_leds.cur_led);
        ALL_leds.ind_led[ALL_leds.cur_led].led_state = ON;
        ALL_leds.led_timer.begin_tick = HAL_GetTick();
    }
    else
    {
        Led_OFF(ALL_leds.cur_led);
        ALL_leds.ind_led[ALL_leds.cur_led].led_state = OFF;
        ALL_leds.led_timer.begin_tick = HAL_GetTick();

        if (++ ALL_leds.ind_led[ALL_leds.cur_led].blink_count >= CHECK_LED_COUNTS)
        {
            ALL_leds.ind_led[ALL_leds.cur_led].blink_count = 0U;
            if (++ ALL_leds.cur_led >= LED_NUM)
            {
                return CHECK_OK;
            }
        }
    }

    return CHECK_BUSY;

}
