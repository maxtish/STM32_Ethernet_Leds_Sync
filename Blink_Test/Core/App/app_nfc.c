#include "app_nfc.h"
#include "app_usb_log.h"
#include <stdbool.h>
#include "main.h"

#define ST25_ADDR_SYS  0xAE

void App_NFC_Init(void) {
    if (HAL_I2C_IsDeviceReady(&hi2c1, ST25_ADDR_DATA, 3, 100) == HAL_OK) {

        // 0x90: GPO по окончанию записи + включение GPO
        uint8_t gpo_config = 0x90;
        HAL_I2C_Mem_Write(&hi2c1, ST25_ADDR_SYS, 0x0000, I2C_MEMADD_SIZE_16BIT, &gpo_config, 1, 100);

        // ЛАЙФХАК: Разрешаем I2C доступ даже во время RF сессии (регистр IT_TIME)
        // Если это не поможет, будем просто ждать в цикле
        USB_Printf("[NFC] ST25DV Initialized\r\n");
    }
}

void App_NFC_Process_Settings(void) {
    if (!nfc_event_flag) return;
    nfc_event_flag = 0;

    uint8_t buf[64]; // Читаем 64 байта, на случай если данные далеко
    HAL_StatusTypeDef status;

    uint8_t retry = 0;
    while (retry < 15) {
        // Читаем 64 байта с самого начала (0x0000)
        status = HAL_I2C_Mem_Read(&hi2c1, ST25_ADDR_DATA, 0x0000, I2C_MEMADD_SIZE_16BIT, buf, 64, 100);
        if (status == HAL_OK) break;
        retry++;
        HAL_Delay(100);
    }

    if (status != HAL_OK) {
        USB_Printf("[NFC] Read Error: %d\r\n", status);
        return;
    }

    // --- ОТЛАДОЧНЫЙ ВЫВОД ---
    USB_Printf("[NFC] MEMORY DUMP (first 32 bytes):\r\n");
    for (int i = 0; i < 32; i++) {
        USB_Printf("%02X ", buf[i]);
        if ((i + 1) % 8 == 0) USB_Printf("\r\n");
    }
    // ------------------------

    bool found = false;
    // Ищем теперь в окне 64 байт
    for (int i = 0; i < (64 - 4); i++) {
        uint8_t calc_crc = buf[i] ^ buf[i+1] ^ buf[i+2];

        if (calc_crc == buf[i+3] && buf[i] != 0 && buf[i] != 0xFF && buf[i] != 0x44) {
            global_speed      = buf[i];
            global_hue        = buf[i+1];
            global_brightness = buf[i+2];

            USB_Printf("[NFC] FOUND AT OFFSET: %d\r\n", i);
            USB_Printf("[NFC] APPLY: Spd:%d Hue:%d Br:%d\r\n", global_speed, global_hue, global_brightness);
            found = true;
            break;
        }
    }

    if (!found) USB_Printf("[NFC] Search finished: No valid frame found\r\n");

    __HAL_GPIO_EXTI_CLEAR_IT(ST25DV_GPO_Pin);
}
