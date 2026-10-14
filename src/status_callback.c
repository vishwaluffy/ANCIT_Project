#include "staus_callback.h"
#include "sdk_project_config.h"
#include <string.h>

void uart_print(const char *s)
{
    LPUART_DRV_SendDataBlocking(INST_LPUART_1,
                                (const uint8_t *)s,
                                (uint32_t)strlen(s),
                                100U);
}

void OnErase(NvM_EraseEvent_t evt, uint8_t done, uint8_t total)
{
    (void)total;

    switch (evt)
    {
        case NVM_ERASE_STARTED:
            uart_print("Erase Started\r\n");
            break;

        case NVM_ERASE_PROGRESS:
            if (done == 1U)
            {
                uart_print("Erase In Progress\r\n");
            }
            break;

        case NVM_ERASE_DONE:
            uart_print("Erase Done\r\n");
            break;

        case NVM_ERASE_FAILED:
            uart_print("Erase Failed\r\n");
            break;

        default:
            break;
    }
}
