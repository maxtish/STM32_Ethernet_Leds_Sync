#ifndef APP_LED_H_
#define APP_LED_H_

#include "main.h" // Нужно для доступа к HAL_GPIO и HAL_GetTick

// Объявляем функцию
void LED_Process_Simple(uint8_t f1, uint8_t f2, uint16_t pin_num, uint32_t interval);


#endif
