#include "app_uart_logic.h"
#include "socket.h"
#include "app_ethernet.h"
#include <stdio.h>
#include <string.h>

// Внешние переменные из main.c (UART буфер)
extern uint8_t uart_rx_buffer[64];
extern uint8_t uart_msg_ready;

// Внешняя функция отладки
extern void USB_Printf(const char* format, ...);

/**
 * @brief Обработка команд, пришедших от физического энкодера по UART,
 * и их трансляция в Ethernet (TCP).
 */
void Process_UART_Communication(void) {
    if (uart_msg_ready) {
        int value;
        char msg_to_send[32];

        // 1. Обработка КНОПКИ
        // Используем strstr вместо strcmp, если в UART может прилететь лишний пробел или \r
        if (strstr((char*)uart_rx_buffer, "BTN") != NULL) {

            // Формируем четкую команду для TCP сервера
            strcpy(msg_to_send, "$BTN\n");

            // Отправляем на плату-исполнитель (Neighbor)
            Ethernet_Send_Command_To_LED(msg_to_send);

            USB_Printf("[Encoder->Net] Sent: $BTN\r\n");
        }

        // 2. Обработка ЗНАЧЕНИЯ (крутилка) "VAL:125"
        else if (sscanf((char*)uart_rx_buffer, "VAL:%d", &value) == 1) {

            // Ограничение диапазона (санитизация данных)
            if (value < 0) value = 0;
            if (value > 255) value = 255;

            // Формируем сетевой пакет
            sprintf(msg_to_send, "$VAL:%d\n", value);

            // Отправка на исполнитель
            Ethernet_Send_Command_To_LED(msg_to_send);

            USB_Printf("[Encoder->Net] Sent: VAL:%d\r\n", value);
        }

        // Очищаем флаг и буфер, чтобы не обрабатывать старые данные по кругу
        memset(uart_rx_buffer, 0, sizeof(uart_rx_buffer));
        uart_msg_ready = 0;
    }
}
