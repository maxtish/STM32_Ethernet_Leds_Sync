#ifndef APP_NFC_H
#define APP_NFC_H

#include "main.h"

// Адрес для данных (0x53 << 1)
#define ST25_ADDR_DATA 0xA6

// Компактная структура: 3 байта данных + 1 байт CRC
typedef struct {
    uint8_t speed;
    uint8_t hue;
    uint8_t brightness;
    uint8_t crc;
} NFC_Settings_t;

void App_NFC_Init(void);
void App_NFC_Process_Settings(void);

#endif
