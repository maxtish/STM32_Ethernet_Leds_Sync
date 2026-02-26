#ifndef APP_UART_LOGIC_H_
#define APP_UART_LOGIC_H_

#include "main.h"

extern volatile int16_t encoder_value;
extern volatile int16_t last_value;
extern uint8_t uart_rx_buffer[64];
extern volatile uint8_t uart_msg_ready;


void Process_UART_Communication(void);

#endif
