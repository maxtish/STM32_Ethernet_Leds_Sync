#include "app_uart_logic.h"
#include <stdio.h>
#include <string.h>
#include "app_usb_log.h"
#include "app_led_blinking.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim2;

static uint32_t btn_timer = 0;
static char uart_tx_buf[32];

void Process_UART_Communication(void) {
	if (uart_msg_ready) {
		Led_blinking();
		int value;
		USB_Printf("[UART RX] Получили: %s\r\n", (char*) uart_rx_buffer);
		if (sscanf((char*) uart_rx_buffer, "VAL:%d", &value) == 1) {
			if (value < 0)
				value = 0;
			if (value > 255)
				value = 255;

			__HAL_TIM_SET_COUNTER(&htim2, value);
			encoder_value = (int16_t) value;
			last_value = encoder_value;
			USB_Printf("[UART] Установлено новое значение: %d\r\n", value);
		}
		memset(uart_rx_buffer, 0, sizeof(uart_rx_buffer));
		uart_msg_ready = 0;
	}

	encoder_value = (int16_t) __HAL_TIM_GET_COUNTER(&htim2);
	if (encoder_value != last_value) {
		int len = sprintf(uart_tx_buf, "$VAL:%d\n", encoder_value);
		HAL_UART_Transmit(&huart1, (uint8_t*) uart_tx_buf, len, 100);
		last_value = encoder_value;
	}

	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == GPIO_PIN_RESET) {
		if (HAL_GetTick() - btn_timer > 500) {
			int len = sprintf(uart_tx_buf, "$BTN\n");
			HAL_UART_Transmit(&huart1, (uint8_t*) uart_tx_buf, len, 100);

			USB_Printf("[BTN] Нажата кнопка\r\n");
			btn_timer = HAL_GetTick();
		}
	}
}

