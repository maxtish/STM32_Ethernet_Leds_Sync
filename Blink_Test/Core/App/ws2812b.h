#ifndef WS2812B_H
#define WS2812B_H

#include "main.h"

#define LED_COUNT 144
#define PWM_HI    40
#define PWM_LO    20
#define RESET_LEN 50



// Прототипы функций
void WS2812_Init(void);
void WS2812_SetColor(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void WS2812_SetSimpleColor(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void WS2812_Send(void);
void WS2812_Clear(void);
void WS2812_Process_Dynamic_Run(void);
void Get_Color_Data(uint8_t hue, uint8_t bright, uint8_t *r, uint8_t *g, uint8_t *b);

#endif
