#include "app_nfc.h"
#include "app_usb_log.h"
#include <stdbool.h>
#include "main.h"


extern I2C_HandleTypeDef hi2c1;

static bool nfc_initialized = false;


static void Force_I2C_Reset_Internal(void) {
    USB_Printf("[NFC] Emergency I2C Reset...\r\n");
    HAL_I2C_DeInit(&hi2c1);
     __HAL_RCC_I2C1_FORCE_RESET();
    HAL_Delay(20);
    __HAL_RCC_I2C1_RELEASE_RESET();

    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        USB_Printf("[NFC] Critical: I2C Re-Init Failed\r\n");
    }
}


void App_NFC_Init(void) {

    if (HAL_I2C_IsDeviceReady(&hi2c1, ST25_ADDR_DATA, 3, 100) == HAL_OK) {
        uint8_t gpo_config = 0x90;
        HAL_I2C_Mem_Write(&hi2c1, ST25_ADDR_SYS, 0x0000, I2C_MEMADD_SIZE_16BIT, &gpo_config, 1, 100);
        nfc_initialized = true;

        App_NFC_Sync_Settings();

    } else {
    	Force_I2C_Reset_Internal();
    }
}

void App_NFC_Process_Settings(void) {
	if (!nfc_initialized) {
		App_NFC_Init();

	}

    if (!nfc_event_flag) return;
    nfc_event_flag = 0;
    App_NFC_Sync_Settings();

}


void App_NFC_Sync_Settings(void) {
    uint8_t buf[64];
    HAL_StatusTypeDef status;


    status = HAL_I2C_Mem_Read(&hi2c1, ST25_ADDR_DATA, 0x0000, I2C_MEMADD_SIZE_16BIT, buf, 64, 100);
    if (status != HAL_OK) return;

    bool found = false;
    // Ищем кадр из 5 байт в буфере 64 байта
    for (int i = 0; i < (64 - 5); i++) {

        // 1. Ищем магическое число 0xAA
        if (buf[i] == 0xAA) {

            // 2. Считаем CRC: Sig ^ Spd ^ Hue ^ Br
            uint8_t calc_crc = buf[i] ^ buf[i+1] ^ buf[i+2] ^ buf[i+3];

            // 3. Сверяем с байтом CRC в пакете
            if (calc_crc == buf[i+4]) {
                global_speed      = buf[i+1];
                global_hue        = buf[i+2];
                global_brightness = buf[i+3];

                USB_Printf("[NFC] MATCH! Sig:0x%02X Spd:%d Hue:%d Br:%d\r\n",
                            buf[i], global_speed, global_hue, global_brightness);
                found = true;
                break;
            }
        }
    }

    if (!found) USB_Printf("[NFC] No valid Arctos signature found\r\n");
    __HAL_GPIO_EXTI_CLEAR_IT(ST25DV_GPO_Pin);
}
