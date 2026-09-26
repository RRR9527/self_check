#ifndef CHECK_PROCESS_H
#define CHECK_PROCESS_H

#include "main.h"

#define Timer_ON      1U
#define Timer_OFF     0U
#define LED_NUM       4U
#define TIME_OUT_TICK 5000U

typedef enum
{
    CHECK_OK   = 0U,
    CHECK_BUSY = 1U,
    CHECK_SKIPPED,
    CHECK_TIME_OUT,
    CHECK_CAN1_RX_ERROR,
    CHECK_CAN1_TX_ERROR,
    CHECK_ALL_OK,
} check_result;

typedef enum 
{
    CHECKING_DEFUALT = 0U,
    CHECKING_DISABLE,
    CHECKING_TIME_OUT,
    CHECKING_LED,    
    CHECKING_MASTERSLAVE_CONNECTION,
    CHECKING_CAN1,  // 对应大疆的CAN口
    CHECKING_CAN2,  // 对应Zdrive的CAN口
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

/* 依赖上面的 timer/check_param/check_result,故放在类型定义之后 */
#include "check_led.h"
#include "check_can1.h"

uint8_t time_expire(timer Timer);
check_result check_handling(void);

#endif