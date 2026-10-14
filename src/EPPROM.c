#include "EEPROM.h"
#include "sdk_project_config.h"
#include "lpi2c_driver.h"
#include "osif.h"
#include <string.h>

#define EEPROM_BASE_ADDR   0x50U
#define EEPROM_BLOCK_SIZE  256U
#define I2C_TIMEOUT        100U

char txBuff[EEPROM_SIZE + 1U] = "abcdefghijklmnopqrstuvwxyz";
char rxBuff[EEPROM_SIZE + 1U];

static uint8_t slave_of(uint16_t memAddr)
{
    return (uint8_t)(EEPROM_BASE_ADDR | ((memAddr >> 8U) & 0x01U));
}

status_t EEPROM_WriteBytes(uint16_t addr, const uint8_t *data, uint16_t len)
{
    uint8_t  frame[1U + EEPROM_PAGE_SIZE];
    uint16_t chunk;
    status_t st;

    if (((uint32_t)addr + len) > EEPROM_SIZE) {
    	return STATUS_ERROR;
    }

    while (len > 0U)
    {

        chunk = (uint16_t)(EEPROM_PAGE_SIZE - (addr % EEPROM_PAGE_SIZE));
        if (chunk > len) {
        	chunk = len;
        }

        frame[0] = (uint8_t)(addr & 0xFFU);
        memcpy(&frame[1], data, chunk);

        LPI2C_DRV_MasterSetSlaveAddr(INST_LPI2C0, slave_of(addr), false);
        st = LPI2C_DRV_MasterSendDataBlocking(INST_LPI2C0, frame,
                                              (uint32_t)(chunk + 1U), true, I2C_TIMEOUT);
        if (st != STATUS_SUCCESS) {
        	return st;
        }

        OSIF_TimeDelay(EEPROM_WRITE_MS);

        addr = (uint16_t)(addr + chunk);
        data += chunk;
        len  = (uint16_t)(len - chunk);
    }
    return STATUS_SUCCESS;
}

status_t EEPROM_ReadBytes(uint16_t addr, uint8_t *data, uint16_t len)
{
    uint8_t  wordAddr;
    uint16_t chunk;
    status_t st;

    if (((uint32_t)addr + len) > EEPROM_SIZE) {
    	return STATUS_ERROR;
    }

    while (len > 0U)
    {

        chunk = (uint16_t)(EEPROM_BLOCK_SIZE - (addr % EEPROM_BLOCK_SIZE));
        if (chunk > len) { chunk = len; }

        wordAddr = (uint8_t)(addr & 0xFFU);

        LPI2C_DRV_MasterSetSlaveAddr(INST_LPI2C0, slave_of(addr), false);

        st = LPI2C_DRV_MasterSendDataBlocking(INST_LPI2C0, &wordAddr, 1U, false, I2C_TIMEOUT);
        if (st != STATUS_SUCCESS) { return st; }

        st = LPI2C_DRV_MasterReceiveDataBlocking(INST_LPI2C0, data, (uint32_t)chunk, true, I2C_TIMEOUT);
        if (st != STATUS_SUCCESS) { return st; }

        addr = (uint16_t)(addr + chunk);
        data += chunk;
        len  = (uint16_t)(len - chunk);
    }
    return STATUS_SUCCESS;
}

status_t EEPROM_WritePage(uint8_t page, const uint8_t *data)
{
    if ((page == 0U) || (page > EEPROM_PAGE_COUNT)) {
    	return STATUS_ERROR;
    }
    return EEPROM_WriteBytes((uint16_t)((page - 1U) * EEPROM_PAGE_SIZE), data, EEPROM_PAGE_SIZE);
}

status_t EEPROM_ReadPage(uint8_t page, uint8_t *data)
{
    if ((page == 0U) || (page > EEPROM_PAGE_COUNT)) { return STATUS_ERROR; }
    return EEPROM_ReadBytes((uint16_t)((page - 1U) * EEPROM_PAGE_SIZE), data, EEPROM_PAGE_SIZE);
}

void EEPROM_WRITE(void)
{
    EEPROM_WriteBytes(0U, (const uint8_t *)txBuff, (uint16_t)strlen(txBuff));
}

void EEPROM_READ(void)
{
    memset(rxBuff, 0, sizeof(rxBuff));
    EEPROM_ReadBytes(0U, (uint8_t *)rxBuff, (uint16_t)strlen(txBuff));
}

void uart_display(void)
{
    LPUART_DRV_SendDataBlocking(INST_LPUART_1,
                                (const uint8_t *)rxBuff,
                                (uint32_t)strlen(rxBuff),
                                100U);
}
