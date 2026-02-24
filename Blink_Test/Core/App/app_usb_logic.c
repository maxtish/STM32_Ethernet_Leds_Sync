/*
 * app_usb_logic.c
 *
 *  Created on: 21.02.2026
 *      Author: maxti
 */

#include "app_usb_logic.h"

// Сообщаем компилятору, что эти переменные созданы в другом месте (в main.c)
extern UART_HandleTypeDef huart2;
extern volatile uint8_t rx_flag;
extern uint8_t rx_data;
extern uint8_t ethernet_tx_wait_text_flag;
extern uint8_t ethernet_text_index;
extern uint8_t ethernet_text_buffer[128];

///Модуль USB (Команды от ПК)
void Process_USB_To_Interfaces(void) {

	if (rx_flag) {
		rx_flag = 0;
		HAL_UART_Transmit(&huart2, &rx_data, sizeof(rx_data), 10);
		USB_Printf("%c", rx_data);
		if (rx_data == 'm') {

			if (ethernet_tx_wait_text_flag == 0) {
				ethernet_tx_wait_text_flag = 1;
				ethernet_text_index = 0; // Сбрасываем индекс для нового сообщения
				memset(ethernet_text_buffer, 0, sizeof(ethernet_text_buffer)); // Очищаем буфер
				USB_Printf("\r\n Мигаю! Пиши сообщение или команду LED_ON\r\n");
				HAL_Delay(10);
			} else {
				// ВТОРОЕ НАЖАТИЕ 'm': выводим массив и выключаем режим
				ethernet_tx_wait_text_flag = 0;
				// Убеждаемся, что строка закончена для корректной работы strlen и USB_Printf
				if (ethernet_text_index < sizeof(ethernet_text_buffer)) {
					ethernet_text_buffer[ethernet_text_index] = '\0';
				}

				USB_Printf("\r\n[USB->TCP] Отправляю соседу: %s",
						ethernet_text_buffer);

				// Вызываем отправку
				//Ethernet_Send_To_Neighbor((char*) ethernet_text_buffer);

				USB_Printf("\r\n[OK] Пакет передан!\r\n");

			}
		} else {
			// ЕСЛИ ПРИШЕЛ НЕ m
			if (ethernet_tx_wait_text_flag == 1) {
				// Режим записи: складываем байт в массив
				if (ethernet_text_index < sizeof(ethernet_text_buffer) - 1) {
					ethernet_text_buffer[ethernet_text_index++] = rx_data;

				}
			} else if (rx_data == '1') {
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

			} else if (rx_data == '0') {
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
			}
		}
	}

}
