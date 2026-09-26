#ifndef CHECK_CAN_1_H
#define CHECK_CAN_1_H

#include "main.h"
#include "check_process.h"
#include "DJmotor.h"
#include "can.h"

/* CAN1 接大疆 2006 或 3508:反馈 ID 0x201-0x204,控制帧 0x200。 */
#define CAN1_TX_ID         0x200U /* 控制帧 ID,8 字节 = 4 个电机的电流(全 0) */
#define CAN1_RX_WINDOW_MS  500U   /* RX 统计窗口 */
#define CAN1_TX_PERIOD_MS  1U     /* 每 1ms 发一帧,保持电调在线 */
#define CAN1_TX_MIN_SEND   10U    /* 至少发够这么多帧才下 TX 结论 */
#define CAN1_NOT_RECV      0xFFU  /* lastRxTime 哨兵:尚未收到反馈 */

typedef struct
{
    uint8_t  started;
    uint16_t tx_sent;
    uint8_t  received_mask; /* 本次自检收到过反馈的电机 ID 掩码(bit i = ID i+1) */
    timer    rx_timer;
    timer    tx_timer;
}can1_check_ctx;

check_result can1_checking(void);

#endif