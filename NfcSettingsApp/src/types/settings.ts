export interface DeviceSettings {
  speed: number;
  red: number;
  green: number;
  blue: number;
  brightness: number;
}

export type NfcData = {
  raw: string;
  isCompatible: boolean;
  settings?: DeviceSettings;
};
