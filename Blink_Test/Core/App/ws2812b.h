#ifndef WS2812B_H
#define WS2812B_H

#include "main.h"
#include "app_usb_log.h"

#define LED_COUNT 144
#define PWM_HI    38
#define PWM_LO    17
#define RESET_LEN 50

extern volatile uint8_t global_brightness;
extern volatile uint8_t global_hue;


void WS2812_Init(void);
void WS2812_Process_Dynamic_Run(void);


#endif
