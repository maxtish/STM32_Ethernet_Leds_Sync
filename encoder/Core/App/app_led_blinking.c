#include "app_led_blinking.h"
#include "main.h" // Нужно для доступа к HAL_GPIO и HAL_GetTick

void Led_blinking(void) {

    static uint32_t last_tick = 0;


    if (HAL_GetTick() - last_tick >= 200) {
        last_tick = HAL_GetTick();


        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
}
