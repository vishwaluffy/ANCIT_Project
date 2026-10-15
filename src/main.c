#include "sdk_project_config.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "EEPROM.h"
#include "erase.h"
#include "staus_callback.h"

lpi2c_master_state_t lpi2c_master_state;

int main(void)
{
    CLOCK_DRV_Init(&clockMan1_InitConfig0);
    PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);
    LPUART_DRV_Init(INST_LPUART_1, &lpUartState0, &lpuart_0_InitConfig0);
    LPI2C_DRV_MasterInit(INST_LPI2C0, &lpi2c0_MasterConfig0, &lpi2c_master_state);

    uart_print("Input: ");
    EEPROM_WRITE();
    EEPROM_READ();
    uart_display();
    uart_print("\r\n\n");

    uart_print("After Erasing specific page: ");
    NvM_ErasePage(2);
    EEPROM_READ();
    uart_display();
    uart_print("\r\n\n");


    uart_print("Erasing Entire EEPROM Memory\r\n\n");
    NvM_SetEraseCallback(OnErase);
    NvM_EraseAll();
    uart_print("\nEntire Erasing time of all the 64 pages : 320ms\r\n");
    uart_print("\r\n");

    uart_print("After Erasing Entire pages: ");
    EEPROM_READ();
    uart_display();
    uart_print("\r\n\n");

}
