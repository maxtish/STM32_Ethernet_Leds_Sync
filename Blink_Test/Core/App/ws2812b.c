#include <string.h>
#include <ws2812b.h>

extern TIM_HandleTypeDef htim2; // таймер
extern volatile uint8_t global_brightness;
extern volatile uint8_t global_hue;
// Общий буфер для DMA: 24 бита на каждый диод + пауза Reset
uint16_t led_buffer[(LED_COUNT * 24) + RESET_LEN];

void WS2812_Init(void) {
    memset(led_buffer, 0, sizeof(led_buffer));
}

void WS2812_SetColor(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= LED_COUNT) return;

    // WS2812B ждет данные в формате G-R-B
    for (int i = 0; i < 8; i++) {
        // Green
        led_buffer[index * 24 + i]      = (g << i) & 0x80 ? PWM_HI : PWM_LO;
        // Red
        led_buffer[index * 24 + i + 8]  = (r << i) & 0x80 ? PWM_HI : PWM_LO;
        // Blue
        led_buffer[index * 24 + i + 16] = (b << i) & 0x80 ? PWM_HI : PWM_LO;
    }
}

void WS2812_Send(void) {
    // Запускаем передачу через DMA. TIM_CHANNEL_1 должен совпадать с CubeMX
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t*)led_buffer, (LED_COUNT * 24) + RESET_LEN);
}

void WS2812_Clear(void) {
    for (int i = 0; i < LED_COUNT; i++) WS2812_SetColor(i, 0, 0, 0);
    WS2812_Send();
}





static uint16_t current_pos = 0;
static uint32_t last_step_time = 0;

void WS2812_Process_Dynamic_Run(void) {
    // Регулируем скорость бега (например, каждые 30 мс)
    if (HAL_GetTick() - last_step_time < 30) {
        return;
    }
    last_step_time = HAL_GetTick();

    // 1. Рассчитываем текущий RGB на основе внешних параметров
    uint8_t r, g, b;
    // Используем функцию преобразования цвета (которую мы обсуждали ранее)
    // Она берет global_hue (0-255) и global_brightness (0-255)
    Get_Color_Data(global_hue, global_brightness, &r, &g, &b);

    // 2. Очищаем буфер (выключаем все диоды в памяти)
    for (int i = 0; i < LED_COUNT; i++) {
        WS2812_SetSimpleColor(i, 0, 0, 0); // Специальная функция без Send внутри
    }

    // 3. Зажигаем один текущий диод рассчитанным цветом
    WS2812_SetSimpleColor(current_pos, r, g, b);

    // 4. Отправляем данные на ленту (один раз на весь кадр)
    WS2812_Send();

    // 5. Двигаем позицию для следующего вызова
    current_pos++;
    if (current_pos >= LED_COUNT) {
        current_pos = 0;
    }
}



// Функция установки цвета В БУФЕР (без немедленного вызова DMA)
// Это критично для анимаций, чтобы не прерывать текущую передачу
void WS2812_SetSimpleColor(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= LED_COUNT) return;

    for (int i = 0; i < 8; i++) {
        // Формат WS2812B: G7..G0, R7..R0, B7..B0
        led_buffer[index * 24 + i]      = (g << i) & 0x80 ? PWM_HI : PWM_LO;
        led_buffer[index * 24 + i + 8]  = (r << i) & 0x80 ? PWM_HI : PWM_LO;
        led_buffer[index * 24 + i + 16] = (b << i) & 0x80 ? PWM_HI : PWM_LO;
    }
}

// Преобразование "Радуги" (Hue) и Яркости в понятные диоду R, G, B
void Get_Color_Data(uint8_t hue, uint8_t bright, uint8_t *r, uint8_t *g, uint8_t *b) {
    uint8_t r_raw, g_raw, b_raw;

    // Секторный расчет радуги (0-255)
    if (hue < 85) {
        r_raw = 255 - hue * 3; g_raw = hue * 3; b_raw = 0;
    } else if (hue < 170) {
        hue -= 85;
        r_raw = 0; g_raw = 255 - hue * 3; b_raw = hue * 3;
    } else {
        hue -= 170;
        r_raw = hue * 3; g_raw = 0; b_raw = 255 - hue * 3;
    }

    // Применяем глобальную яркость
    // Математика: (Цвет * Яркость) / 255
    *r = (uint8_t)((r_raw * bright) >> 8);
    *g = (uint8_t)((g_raw * bright) >> 8);
    *b = (uint8_t)((b_raw * bright) >> 8);
}


