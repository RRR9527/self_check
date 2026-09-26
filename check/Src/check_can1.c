#include "check_can1.h"

static can1_check_ctx c1;

/* 发一帧 0x200,4 个电机电流全 0:电机不动,仅验证 TX + 喂狗 */
static void can1_tx_zero_current(void)
{
    CAN_TxHeaderTypeDef txheader = {0};
    uint8_t  data[8] = {0};
    uint32_t mailbox = 0U;

    txheader.StdId = CAN1_TX_ID;
    txheader.IDE   = CAN_ID_STD;
    txheader.RTR   = CAN_RTR_DATA;
    txheader.DLC   = 8U;
    txheader.TransmitGlobalTime = DISABLE;

    if (HAL_CAN_AddTxMessage(&hcan1, &txheader, data, &mailbox) == HAL_OK)
    {
        c1.tx_sent++;
    }
}

/* TX 是否健康:有节点 ACK 时发送错误计数器(TEC)恒为 0;任一发送失败都会抬升它 */
static uint8_t can1_tx_healthy(void)
{
    uint8_t tec = (uint8_t)((CAN1->ESR >> CAN_ESR_TEC_Pos) & 0xFFU);

    return (tec == 0U) ? 1U : 0U;
}

check_result can1_checking(void)
{
    uint8_t received;
    uint8_t rx_pass;
    uint8_t i;

    if (!c1.started)
    {
        c1.started = 1U;
        c1.tx_sent = 0U;
        c1.received_mask = 0U;

        c1.rx_timer.begin_tick  = HAL_GetTick();
        c1.rx_timer.duration    = CAN1_RX_WINDOW_MS;
        c1.rx_timer.timer_state = Timer_ON;

        c1.tx_timer.begin_tick  = HAL_GetTick();
        c1.tx_timer.duration    = CAN1_TX_PERIOD_MS;
        c1.tx_timer.timer_state = Timer_ON;

        /* 借用 DJmotor[i].error.lastRxTime 当"收到过"标志:
         * DJmotor_Receive 每收到一帧会把它清零,这里先全部置成非零哨兵。 */
        for (i = 0; i < USE_DJNUM; i++)
        {
            DJmotor[i].error.lastRxTime = CAN1_NOT_RECV;
        }
    }

    /* 周期发 0x200 零电流 */
    if (time_expire(c1.tx_timer))
    {
        c1.tx_timer.begin_tick = HAL_GetTick();
        can1_tx_zero_current();
    }

    /* 统计本次自检收到过反馈的电机 ID 掩码(bit i = ID i+1) */
    received = 0U;
    for (i = 0; i < USE_DJNUM; i++)
    {
        if (DJmotor[i].error.lastRxTime == 0U)
        {
            received |= (uint8_t)(1U << i);
        }
    }
    c1.received_mask = received;

    /* 判定 RX:未配置(0)时任意回应即算通;配置了则必须精确匹配期望 ID */
    rx_pass = (MOTOR_DJI_CONNECTED_MASK == 0U)
                ? (received != 0U)
                : (received == MOTOR_DJI_CONNECTED_MASK);

    /* 快路径: 发够了且物理层已坏(无节点 ACK)→ TxError，不必检查后续了 */
    if ((c1.tx_sent >= CAN1_TX_MIN_SEND) && !can1_tx_healthy())
    {
        return CHECK_CAN1_TX_ERROR;
    }

    /* 快路径: 发够、物理层干净、链路满足 → OK */
    if ((c1.tx_sent >= CAN1_TX_MIN_SEND) && rx_pass)
    {
        return CHECK_OK;
    }
    
    /* 窗口到期: 从下到上裁决 */
    if (time_expire(c1.rx_timer))
    {
        if (!can1_tx_healthy())              
        {
            return CHECK_CAN1_TX_ERROR; /* 物理 */
        }
        if (!rx_pass)                        
        {
            return CHECK_CAN1_RX_ERROR; /* 链路 */
        }
        if (c1.tx_sent < CAN1_TX_MIN_SEND)   
        {
            return CHECK_CAN1_TX_ERROR; /* 发送卡住 */
        }

        return CHECK_OK;
    }

    return CHECK_BUSY;
}
