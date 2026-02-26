#include "app_led.h"


void LED_Process_Simple(uint8_t f1, uint8_t f2, uint16_t pin, uint32_t interval) {
	static uint32_t last_tick = 0;

	// Если активен хоть один режим
	if (f1 || f2) {
		if (HAL_GetTick() - last_tick >= interval) {
			last_tick = HAL_GetTick();
			// Используем стандартный порт GPIOC для BluePill
			HAL_GPIO_TogglePin(GPIOC, (1 << pin));

		}
	} else {
		// Если мигание не требуется
	}
}


