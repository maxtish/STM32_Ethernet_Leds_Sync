import { DeviceSettings } from '../types/settings';

export const encodeSettings = (settings: DeviceSettings): number[] => {
  const speed = Math.floor(settings.speed);
  const hue = Math.floor(settings.hue);
  const brightness = Math.floor(settings.brightness);
  const signature = 0xaa; // "Магическое число"

  // Считаем CRC по всем 4 компонентам
  const checksum = signature ^ speed ^ hue ^ brightness;

  // Возвращаем ПЯТЬ байт
  return [signature, speed, hue, brightness, checksum];
};

export const parseNfcPayload = (payload: number[]): DeviceSettings | null => {
  // Теперь ищем кадр из 5 байт
  if (!payload || payload.length < 5) return null;

  for (let i = 0; i <= payload.length - 5; i++) {
    const sig = payload[i];
    const speed = payload[i + 1];
    const hue = payload[i + 2];
    const brightness = payload[i + 3];
    const checksum = payload[i + 4];

    // Проверяем подпись ПЕРВОЙ
    if (sig === 0xaa) {
      const calculated = sig ^ speed ^ hue ^ brightness;

      if (calculated === checksum) {
        console.log(`[NFC] Valid Arctos frame found at offset ${i}`);
        return { speed, hue, brightness };
      }
    }
  }
  return null;
};
