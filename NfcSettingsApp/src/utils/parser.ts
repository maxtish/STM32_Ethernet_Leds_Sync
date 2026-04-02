import { DeviceSettings } from '../types/settings';

/**
 * Ищет и извлекает настройки из сырого массива байтов NFC метки.
 * Использует алгоритм поиска по контрольной сумме (CRC),
 * так как данные могут быть смещены из-за NDEF заголовков.
 */
export const parseNfcPayload = (payload: number[]): DeviceSettings | null => {
  // Нам нужно как минимум 4 байта (Speed, Hue, Brightness, CRC)
  if (!payload || payload.length < 4) return null;

  // Проходим циклом по всему массиву байтов, ища валидный кадр
  for (let i = 0; i <= payload.length - 4; i++) {
    const speed = payload[i];
    const hue = payload[i + 1];
    const brightness = payload[i + 2];
    const checksum = payload[i + 3];

    // Считаем XOR для первых трех байт
    const calculated = speed ^ hue ^ brightness;

    // Проверяем: совпала ли сумма и не являются ли данные пустыми (0x00 или 0xFF)
    if (calculated === checksum && speed !== 0 && speed !== 255) {
      console.log(`[NFC Parser] Данные успешно найдены на смещении: ${i}`);
      return {
        speed: speed,
        hue: hue,
        brightness: brightness,
      };
    }
  }

  console.log('[NFC Parser] Валидный пакет настроек не обнаружен');
  return null;
};

/**
 * Кодирует настройки в компактный 4-байтовый массив для записи.
 * Формат: [Speed, Hue, Brightness, CRC]
 */
export const encodeSettings = (settings: DeviceSettings): number[] => {
  const payload = [
    Math.floor(settings.speed),
    Math.floor(settings.hue),
    Math.floor(settings.brightness),
  ];

  // Считаем контрольную сумму (XOR)
  const checksum = payload[0] ^ payload[1] ^ payload[2];

  // Добавляем CRC четвертым байтом
  payload.push(checksum);

  return payload;
};
