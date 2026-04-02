export interface DeviceSettings {
  speed: number;
  hue: number;
  brightness: number;
}

export type NfcData = {
  raw: string;
  isCompatible: boolean;
  settings?: DeviceSettings;
};
