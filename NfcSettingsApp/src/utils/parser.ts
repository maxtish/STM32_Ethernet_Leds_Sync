import { DeviceSettings } from '../types/settings';

export const parseNfcPayload = (payload: number[]): DeviceSettings | null => {
  if (payload.length < 6) return null;

  const data = payload.slice(0, 5);
  const checksum = payload[5];
  const calculated = data.reduce((acc, byte) => acc ^ byte, 0);

  if (calculated !== checksum) return null;

  return {
    speed: data[0],
    red: data[1],
    green: data[2],
    blue: data[3],
    brightness: data[4],
  };
};

export const encodeSettings = (settings: DeviceSettings): number[] => {
  const payload = [
    settings.speed,
    settings.red,
    settings.green,
    settings.blue,
    settings.brightness,
  ];
  // CRC (XOR)
  const checksum = payload.reduce((acc, byte) => acc ^ byte, 0);
  payload.push(checksum);

  return payload;
};
