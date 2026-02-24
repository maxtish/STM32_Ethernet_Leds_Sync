/*
 * app_uart_logic.c
 *
 *  Created on: 21.02.2026
 *      Author: maxti
 */
#include "app_uart_logic.h"
#include <stdio.h>
// Ссылаемся на переменные из main.c
extern uint8_t uart_rx_buffer[64];
extern uint8_t uart_msg_ready;
extern uint8_t uart_rx_data;

///Модуль UART (Связь между платами)
void Process_UART_Communication(void) {



	if (uart_msg_ready) {
	        // В буфере будет лежать "VAL:125" (уже без $ и \n)
		int value;

		    // 1. Проверяем: не кнопка ли это?
		    // strcmp вернет 0, если строки полностью идентичны
		    if (strcmp((char*)uart_rx_buffer, "BTN") == 0) {

		        USB_Printf("\r\n[СОБЫТИЕ]: BTN\r\n");

		        // --- ТВОЕ ДЕЙСТВИЕ НА КНОПКУ ---
		        // Например: переключить цвет ленты
		        // toggle_led_color();
		    }

		    // 2. Если не кнопка, проверяем: не значение ли это энкодера?
		    else if (sscanf((char*)uart_rx_buffer, "VAL:%d", &value) == 1) {

		        USB_Printf("\r\n[ДАННЫЕ]: Энкодер = %d\r\n", value);

		        // --- ТВОЕ ДЕЙСТВИЕ НА КРУТИЛКУ ---
		        // Например: изменить яркость
		        // update_brightness(value);
		    }

		    // 3. Если пришло что-то неизвестное
		    else {
		        USB_Printf("\r\n[ОШИБКА]: Неизвестная команда: %s\r\n", uart_rx_buffer);
		    }

		    uart_msg_ready = 0; // Сбрасываем флаг в конце обработки любого пакета
	    }



/*

	// --- ОБРАБОТКА ДАННЫХ ИЗ UART2 (ОТ СОСЕДНЕЙ ПЛАТЫ) ---
	if (uart_rx_flag) {
		uart_rx_flag = 0;
		///показываем что пришло
		USB_Printf("\r\n[Получен байт]: %c\r\n", uart_rx_data);

		// ПРОВЕРКА НА ENTER
		if (uart_rx_data == 'm') {

			if (uart_rx_wait_text_flag == 0) {
				// ПЕРВОЕ НАЖАТИЕ m: включаем режим накопления
				uart_rx_wait_text_flag = 1;
				text_index = 0; // Сбрасываем индекс для нового сообщения
				memset(text_buffer, 0, sizeof(text_buffer)); // Очищаем буфер
				USB_Printf("\r\nФлаг накопления получен\r\n");
			} else {
				// ВТОРОЕ НАЖАТИЕ 'm': выводим массив и выключаем режим
				uart_rx_wait_text_flag = 0;
				// Выводим в USB то, что накопили

				char footer[] = "\r\nРежим записи текста ВЫКЛ\r\n";
				USB_Printf("\r\n[Накопили]: %.*s%s", text_index, text_buffer,
						footer);

			}
		} else {
			// ЕСЛИ ПРИШЕЛ НЕ m
			if (uart_rx_wait_text_flag == 1) {
				// Режим записи: складываем байт в массив
				if (text_index < sizeof(text_buffer) - 1) {
					text_buffer[text_index++] = uart_rx_data;
				}
			} else {
				// Обычный режиме: управляем светодиодом
				if (uart_rx_data == '1') {
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
					USB_Printf("\r\nLED ON\r\n");
				} else if (uart_rx_data == '0') {
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
					USB_Printf("\r\nLED OFF\r\n");

				}
			}
		}
	}


	*/
}
