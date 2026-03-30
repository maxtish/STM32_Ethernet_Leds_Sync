import React, { useState } from 'react';
import { View, Text, StyleSheet, TouchableOpacity, Alert } from 'react-native';
import Slider from '@react-native-community/slider';
import { DeviceSettings } from '../types/settings';
import { encodeSettings } from '../utils/parser';
import NfcManager, { Ndef, NfcTech } from 'react-native-nfc-manager';

interface SettingSliderProps {
  label: string;
  value: number;
  onValueChange: (value: number) => void; // Типизируем функцию обратного вызова
  color?: string;
}

interface Props {
  initialSettings: DeviceSettings;
  onSave?: (data: DeviceSettings) => void;
}

export const DeviceSettingsEditor = ({ initialSettings }: Props) => {
  const [settings, setSettings] = useState<DeviceSettings>(initialSettings);
  const [isSaving, setIsSaving] = useState(false);

  const updateField = (field: keyof DeviceSettings, value: number) => {
    setSettings(prev => ({ ...prev, [field]: Math.floor(value) }));
  };

  const handleSave = async () => {
    try {
      setIsSaving(true);
      // 1. Подготавливаем байты (наша логика с CRC)
      const bytes = encodeSettings(settings);

      // 2. Создаем NDEF запись
      const record = Ndef.record(
        Ndef.TNF_MIME_MEDIA,
        'application/octet-stream',
        [],
        bytes,
      );
      const bytesToWrite = Ndef.encodeMessage([record]);

      // 3. Запись
      await NfcManager.requestTechnology(NfcTech.Ndef);
      await NfcManager.ndefHandler.writeNdefMessage(bytesToWrite);

      Alert.alert('Успех', 'Настройки сохранены в память ST25DV');
    } catch (ex) {
      console.warn(ex);
      Alert.alert('Ошибка', 'Не удалось записать данные. Попробуйте еще раз.');
    } finally {
      NfcManager.cancelTechnologyRequest();
      setIsSaving(false);
    }
  };

  return (
    <View style={styles.card}>
      <Text style={styles.title}>Настройки устройства</Text>

      <SettingSlider
        label="Скорость"
        value={settings.speed}
        onValueChange={v => updateField('speed', v)}
      />

      <View style={styles.colorGroup}>
        <View
          style={[
            styles.preview,
            {
              backgroundColor: `rgb(${settings.red},${settings.green},${settings.blue})`,
            },
          ]}
        />
        <SettingSlider
          label="R"
          color="#ff4444"
          value={settings.red}
          onValueChange={v => updateField('red', v)}
        />
        <SettingSlider
          label="G"
          color="#44ff44"
          value={settings.green}
          onValueChange={v => updateField('green', v)}
        />
        <SettingSlider
          label="B"
          color="#4444ff"
          value={settings.blue}
          onValueChange={v => updateField('blue', v)}
        />
      </View>

      <SettingSlider
        label="Яркость"
        value={settings.brightness}
        onValueChange={v => updateField('brightness', v)}
      />

      <TouchableOpacity
        style={[styles.saveButton, isSaving && { opacity: 0.5 }]}
        onPress={handleSave}
        disabled={isSaving}
      >
        <Text style={styles.saveText}>
          {isSaving ? 'ЗАПИСЬ...' : 'СОХРАНИТЬ В МЕТКУ'}
        </Text>
      </TouchableOpacity>
    </View>
  );
};

const SettingSlider = ({
  label,
  value,
  onValueChange,
  color = '#007AFF',
}: SettingSliderProps) => (
  <View style={styles.sliderRow}>
    <Text style={styles.label}>
      {label}: {value}
    </Text>
    <Slider
      style={{ width: '100%', height: 40 }}
      minimumValue={0}
      maximumValue={255}
      value={value}
      minimumTrackTintColor={color}
      onValueChange={onValueChange}
    />
  </View>
);

const styles = StyleSheet.create({
  card: {
    marginTop: 20,
    padding: 15,
    backgroundColor: '#fff',
    borderRadius: 12,
    elevation: 3,
    shadowColor: '#000',
    shadowOpacity: 0.1,
    shadowRadius: 10,
  },
  title: { fontSize: 18, fontWeight: 'bold', marginBottom: 15, color: '#333' },
  sliderRow: { marginVertical: 8 },
  label: { fontSize: 14, color: '#666' },
  colorGroup: {
    padding: 10,
    backgroundColor: '#f0f0f0',
    borderRadius: 8,
    marginVertical: 10,
  },
  preview: {
    height: 30,
    borderRadius: 15,
    marginBottom: 10,
    borderWidth: 1,
    borderColor: '#ccc',
  },
  saveButton: {
    backgroundColor: '#28a745',
    padding: 15,
    borderRadius: 8,
    alignItems: 'center',
    marginTop: 15,
  },
  saveText: { color: '#fff', fontWeight: 'bold' },
});
