
#ifndef ERASE_H
#define ERASE_H

#include"sdk_project_config.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "status.h"

#define NVM_EEPROM_SIZE     512U
#define NVM_PAGE_SIZE       8U
#define NVM_PAGE_COUNT      (NVM_EEPROM_SIZE / NVM_PAGE_SIZE)
#define NVM_WRITE_CYCLE_MS  5U
#define NVM_ERASE_ALL_MS    (NVM_PAGE_COUNT * NVM_WRITE_CYCLE_MS)
#define NVM_ERASE_VALUE     0xFFU

typedef enum
{
	NVM_ERASE_STARTED,
    NVM_ERASE_PROGRESS,
    NVM_ERASE_DONE,
    NVM_ERASE_FAILED
} NvM_EraseEvent_t;

typedef void (*NvM_EraseCallback_t)(NvM_EraseEvent_t event,
                                    uint8_t pagesDone,
                                    uint8_t pagesTotal);
void NvM_SetEraseCallback(NvM_EraseCallback_t cb);
status_t NvM_ErasePage(uint8_t page);
status_t NvM_EraseAll(void);

#endif
