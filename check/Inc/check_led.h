#ifndef CHECK_LED_H
#define CHECK_LED_H

#include "main.h"
#include "check_process.h"
#include "Led.h"
#include "show_error.h"

#define OFF              0U
#define ON               1U
#define CHECK_LED_COUNTS 3U

extern volatile check_param c_params;

typedef struct
{
    volatile uint8_t led_state;
    volatile uint8_t led_id;
    volatile uint8_t blink_count;
}leds_param;

typedef struct
{
    volatile uint8_t cur_led;
    leds_param       ind_led[LED_NUM];
    timer            led_timer;
    uint8_t          led_start;
}all_leds;

check_result led_checking(void);

#endif