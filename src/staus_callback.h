#ifndef STATUS_CALLBACK_H
#define STATUS_CALLBACK_H

#include <stdint.h>
#include "erase.h"

void uart_print(const char *s);
void OnErase(NvM_EraseEvent_t evt, uint8_t done, uint8_t total);

#endif
