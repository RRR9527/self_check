#ifndef CHECK_PROCESS_H
#define CHECK_PROCESS_H

#include "main.h"
#include "check_led.h"

#define Timer_ON      1U
#define Timer_OFF     0U
#define LED_NUM       4U
#define TIME_OUT_TICK 5000U

typedef enum
{
    CHECK_OK   = 0U,
    CHECK_BUSY = 1U,
    CHECK_TIME_OUT,
} check_result;

typedef enum 
{
    CHECKING_DEFUALT = 0U,
    CHECKING_DISABLE,
    CHECKING_TIME_OUT,
    CHECKING_LED,    
    CHECKING_MASTERSLAVE_CONNECTION,
    CHECKING_BOARDMOTOR_CONNECTION,
}checking_state;

typedef struct
{
    uint8_t timer_state;
    uint32_t begin_tick;
    uint16_t duration;
}timer;

typedef struct
{
    volatile checking_state check_cur_state;
    timer Check_Timer;
}check_param;

uint8_t time_expire(timer Timer);
check_result check_handling(void);

#endif