import React, { useState } from 'react';
import { View, Text, StyleSheet, TouchableOpacity, Alert } from 'react-native';
import Slider from '@react-native-community/slider';
import { DeviceSettings } from '../types/settings';
import { encodeSettings } from '../utils/parser';
import NfcManager, { Ndef, NfcTech } from 'react-native-nfc-manager';

// Описываем пропсы для основного компонента
interface EditorProps {
  initialSettings: DeviceSettings;
  onSave?: (data: DeviceSettings) => void;
}

export const DeviceSettingsEditor = ({
  initialSettings,
  onSave, // Добавляем деструктуризацию onSave
}: EditorProps) => {
  const [settings, setSettings] = useState<DeviceSettings>(initialSettings);
  const [isSaving, setIsSaving] = useState(false);

  const updateField = (field: keyof DeviceSettings, value: number) => {
    setSettings(prev => ({ ...prev, [field]: Math.floor(value) }));
  };

  const handleSave = async () => {
    try {
      setIsSaving(true);
      const bytes = encodeSettings(settings);

      const record = Ndef.record(
        Ndef.TNF_MIME_MEDIA,
        'application/octet-stream',
        [],
        bytes,
      );
      const bytesToWrite = Ndef.encodeMessage([record]);

      await NfcManager.requestTechnology(NfcTech.Ndef);
      await NfcManager.ndefHandler.writeNdefMessage(bytesToWrite);

      Alert.alert('Успех', 'Настройки переданы на STM32');

      // Вызываем коллбэк, если он передан из App.tsx
      if (onSave) {
        onSave(settings);
      }
    } catch (ex) {
      Alert.alert('Ошибка', 'Убедитесь, что телефон плотно прижат к антенне');
    } finally {
      NfcManager.cancelTechnologyRequest();
      setIsSaving(false);
    }
  };

  return (
    <View style={styles.card}>
      <Text style={styles.title}>Управление лентой</Text>

      <SettingSlider
        label="Задержка (скорость)"
        value={settings.speed}
        onValueChange={(v: number) => updateField('speed', v)} // Явно указываем (v: number)
      />

      <View style={styles.colorGroup}>
        <Text style={styles.label}>Цвет (Hue): {settings.hue}</Text>
        <View
          style={[
            styles.preview,
            {
              backgroundColor: `hsl(${(settings.hue / 255) * 360}, 100%, 50%)`,
            },
          ]}
        />
        <Slider
          style={{ width: '100%', height: 40 }}
          minimumValue={0}
          maximumValue={255}
          value={settings.hue}
          onValueChange={(v: number) => updateField('hue', v)} // Явно указываем (v: number)
          minimumTrackTintColor="#FF0000"
        />
      </View>

      <SettingSlider
        label="Общая яркость"
        value={settings.brightness}
        onValueChange={(v: number) => updateField('brightness', v)} // Явно указываем (v: number)
      />

      <TouchableOpacity
        style={[styles.saveButton, isSaving && { opacity: 0.5 }]}
        onPress={handleSave}
        disabled={isSaving}
      >
        <Text style={styles.saveText}>
          {isSaving ? 'ПЕРЕДАЧА...' : 'ОБНОВИТЬ ПО NFC'}
        </Text>
      </TouchableOpacity>
    </View>
  );
};

// Вспомогательный компонент с типизацией
interface SettingSliderProps {
  label: string;
  value: number;
  onValueChange: (v: number) => void;
}

const SettingSlider = ({ label, value, onValueChange }: SettingSliderProps) => (
  <View style={styles.sliderRow}>
    <Text style={styles.label}>
      {label}: {value}
    </Text>
    <Slider
      style={{ width: '100%', height: 40 }}
      minimumValue={1}
      maximumValue={255}
      value={value}
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
  },
  title: { fontSize: 18, fontWeight: 'bold', marginBottom: 15 },
  sliderRow: { marginVertical: 10 },
  label: { fontSize: 14, color: '#333', marginBottom: 5 },
  colorGroup: {
    padding: 10,
    backgroundColor: '#f9f9f9',
    borderRadius: 8,
    marginVertical: 10,
  },
  preview: {
    height: 40,
    borderRadius: 20,
    marginBottom: 10,
    borderWidth: 2,
    borderColor: '#fff',
  },
  saveButton: {
    backgroundColor: '#007AFF',
    padding: 15,
    borderRadius: 8,
    alignItems: 'center',
    marginTop: 10,
  },
  saveText: { color: '#fff', fontWeight: 'bold' },
});
