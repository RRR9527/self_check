#include "check_process.h"

check_param c_params = 
{
    .check_cur_state = CHECKING_DEFAULT,
    .Check_Timer = {.timer_state = Timer_OFF, .begin_tick = 0U, .duration = TIME_OUT_TICK}
};

uint8_t time_expire(timer Timer)
{
    if (Timer.timer_state != Timer_ON)
    {
        return 0U;
    }

    if (Timer.duration + Timer.begin_tick <= HAL_GetTick())
    {
        return 1U;
    }
    else
    {
        return 0U;
    }     
}

check_result check_handling(void)
{
    if 
    (
        c_params.check_cur_state == CHECKING_DISABLE
    )
    {
        return CHECK_SKIPPED;
    }

    if (!c_params.Check_Timer.timer_state)
    {
        c_params.Check_Timer.timer_state = Timer_ON;
        c_params.Check_Timer.begin_tick  = HAL_GetTick();
    }

    if (time_expire(c_params.Check_Timer))
    {
        return CHECK_TIME_OUT;  // 未来每完成一个步骤记得修改一次begin_tick
    }

    switch (c_params.check_cur_state)
    {
        case (CHECKING_DEFAULT):
        {
            c_params.check_cur_state = CHECKING_LED;
            c_params.Check_Timer.timer_state = Timer_ON;            
            c_params.Check_Timer.begin_tick  = HAL_GetTick();
            break;
        }

        case (CHECKING_LED):
        {
            if (led_checking() == CHECK_OK)
            {
                c_params.check_cur_state = CHECKING_CAN1;
            }
            c_params.Check_Timer.begin_tick  = HAL_GetTick();
            break;
        }

        case (CHECKING_CAN1):
        {
            check_result CAN1_re = can1_checking();

            switch (CAN1_re)
            {
                case CHECK_BUSY:
                {
                    break;
                }
                case CHECK_OK:
                {
                    c_params.check_cur_state = CHECKING_CAN2;
                    break;
                }
                case CHECK_CAN1_TX_ERROR:
                case CHECK_CAN1_RX_ERROR:
                {
                    return CAN1_re;
                    break;
                }
            }

            c_params.Check_Timer.begin_tick  = HAL_GetTick();
            break;
        }
    }

    return CHECK_BUSY;

}
