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

///Модуль USB (Команды от ПК)
void Process_USB_To_Interfaces(void) {

	if (rx_flag) {
		rx_flag = 0;
		HAL_UART_Transmit(&huart2, &rx_data, sizeof(rx_data), 10);
		USB_Printf("%c", rx_data);

	}

}
