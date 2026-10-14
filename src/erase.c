/*
 * erase.c
 *
 * Page-by-page erase of a 24C04 I2C EEPROM with a progress/status callback.
 *
 * 24C04 addressing: 512 bytes, 8-bit word address. Address bit 8 is carried
 * in the slave address (P0 bit):
 *     0x000 - 0x0FF -> 7-bit slave 0x50
 *     0x100 - 0x1FF -> 7-bit slave 0x51
 */
#include "erase.h"       /* generated config: INST_LPI2C0 and driver headers */
#include "lpi2c_driver.h"
#include "osif.h"
#include <string.h>

#define EEPROM_BASE_ADDR          0x50U   /* 7-bit address */
#define I2C_TIMEOUT               100U    /* ms */

static NvM_EraseCallback_t s_eraseCb = NULL;

void NvM_SetEraseCallback(NvM_EraseCallback_t cb)
{
    s_eraseCb = cb;
}


static void NvM_WaitWriteDone(void)
{
    OSIF_TimeDelay(NVM_WRITE_CYCLE_MS);
}

static status_t NvM_ErasePageIndex(uint8_t page)
{
    uint8_t  frame[1U + NVM_PAGE_SIZE];
    uint16_t memAddr;
    uint8_t  slave;
    status_t st;

    if (page >= NVM_PAGE_COUNT)
    {
        return STATUS_ERROR;
    }

    memAddr  = (uint16_t)((uint16_t)page * NVM_PAGE_SIZE);
    slave    = (uint8_t)(EEPROM_BASE_ADDR | ((memAddr >> 8U) & 0x01U)); /* bit 8 -> P0 */
    frame[0] = (uint8_t)(memAddr & 0xFFU);                              /* word address */
    memset(&frame[1], NVM_ERASE_VALUE, NVM_PAGE_SIZE);

    LPI2C_DRV_MasterSetSlaveAddr(INST_LPI2C0, slave, false);
    st = LPI2C_DRV_MasterSendDataBlocking(INST_LPI2C0, frame,
                                          (uint32_t)sizeof(frame),
                                          true, I2C_TIMEOUT);
    if (st != STATUS_SUCCESS)
    {
        return st;
    }

    NvM_WaitWriteDone();
    return STATUS_SUCCESS;
}

/* Erase the specified page, numbered from 1:
 *   NvM_ErasePage(1)  -> 1st page (bytes 0..7)
 *   NvM_ErasePage(2)  -> 2nd page (bytes 8..15)
 *   NvM_ErasePage(64) -> last page (bytes 504..511)               */
status_t NvM_ErasePage(uint8_t page)
{
    if ((page == 0U) || (page > NVM_PAGE_COUNT))
    {
        return STATUS_ERROR;
    }

    return NvM_ErasePageIndex((uint8_t)(page - 1U));
}

status_t NvM_EraseAll(void)
{
    uint8_t  page;
    status_t st = STATUS_SUCCESS;

    if (s_eraseCb != NULL)
    {
        s_eraseCb(NVM_ERASE_STARTED, 0U, (uint8_t)NVM_PAGE_COUNT);
    }

    for (page = 0U; page < NVM_PAGE_COUNT; page++)
    {
        st = NvM_ErasePageIndex(page);

        if (st != STATUS_SUCCESS)
        {
            if (s_eraseCb != NULL)
            {
                s_eraseCb(NVM_ERASE_FAILED, page, (uint8_t)NVM_PAGE_COUNT);
            }
            return st;
        }

        if (s_eraseCb != NULL)
        {
            s_eraseCb(((page + 1U) == NVM_PAGE_COUNT) ? NVM_ERASE_DONE
                                                      : NVM_ERASE_PROGRESS,
                      (uint8_t)(page + 1U),
                      (uint8_t)NVM_PAGE_COUNT);
        }
    }

    return st;
}
