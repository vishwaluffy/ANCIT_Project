#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>
#include "status.h"

#define EEPROM_SIZE        512U
#define EEPROM_PAGE_SIZE   8U
#define EEPROM_PAGE_COUNT  (EEPROM_SIZE / EEPROM_PAGE_SIZE)
#define EEPROM_WRITE_MS    5U

extern char txBuff[EEPROM_SIZE + 1U];
extern char rxBuff[EEPROM_SIZE + 1U];

status_t EEPROM_WriteBytes(uint16_t addr, const uint8_t *data, uint16_t len);
status_t EEPROM_ReadBytes(uint16_t addr, uint8_t *data, uint16_t len);

status_t EEPROM_WritePage(uint8_t page, const uint8_t *data);
status_t EEPROM_ReadPage(uint8_t page, uint8_t *data);

void EEPROM_WRITE(void);
void EEPROM_READ(void);
void uart_display(void);

#endif /* EEPROM_H */
