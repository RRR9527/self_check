#include "show_error.h"

void Show_Error(check_result ch_re)
{
    switch (ch_re)
    {
        case CHECK_ALL_OK:
        case CHECK_SKIPPED:
            break;

        case CHECK_CAN1_TX_ERROR:

        case CHECK_CAN1_RX_ERROR:
    }
}
