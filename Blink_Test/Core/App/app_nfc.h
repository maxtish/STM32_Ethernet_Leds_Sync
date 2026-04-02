#ifndef APP_NFC_H
#define APP_NFC_H

#include "main.h"

#define ST25_ADDR_DATA 0xA6
#define ST25_ADDR_SYS  0xAE


// Компактная структура: 3 байта данных + 1 байт CRC
typedef struct {
    uint8_t speed;
    uint8_t hue;
    uint8_t brightness;
    uint8_t crc;
} NFC_Settings_t;

void App_NFC_Init(void);
void App_NFC_Process_Settings(void);
void App_NFC_Sync_Settings(void);

#endif
