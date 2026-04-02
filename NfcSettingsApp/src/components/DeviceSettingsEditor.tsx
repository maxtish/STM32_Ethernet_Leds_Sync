import React, { useState } from 'react';
import { View, Text, StyleSheet, TouchableOpacity, Alert } from 'react-native';
import Slider from '@react-native-community/slider';
import { DeviceSettings } from '../types/settings';
import { encodeSettings } from '../utils/parser';
import NfcManager, { Ndef, NfcTech } from 'react-native-nfc-manager';

const ARCTOS_RED = '#D32F2F';
const ARCTOS_DARK_CARD = '#222222';

interface EditorProps {
  initialSettings: DeviceSettings;
}

export const DeviceSettingsEditor = ({ initialSettings }: EditorProps) => {
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
    } catch (ex) {
      Alert.alert(
        'IO ERROR',
        'NFC connection lost. Secure the device closer to the antenna.',
      );
    } finally {
      NfcManager.cancelTechnologyRequest();
      setIsSaving(false);
    }
  };

  return (
    <View style={styles.card}>
      <Text style={styles.title}>PARAMETER OVERRIDE</Text>

      <SettingSlider
        label="ENGINE SPEED (delay)"
        value={settings.speed}
        onValueChange={(v: number) => updateField('speed', v)}
      />

      <View style={styles.colorGroup}>
        <View style={styles.labelRow}>
          <Text style={styles.label}>COLOR HUE</Text>
          <Text style={styles.valueLabel}>{settings.hue} nm/idx</Text>
        </View>

        <View style={styles.previewContainer}>
          <View
            style={[
              styles.preview,
              {
                backgroundColor: `hsl(${
                  (settings.hue / 255) * 360
                }, 100%, 50%)`,
                shadowColor: `hsl(${(settings.hue / 255) * 360}, 100%, 50%)`,
              },
            ]}
          />
        </View>

        <Slider
          style={{ width: '100%', height: 40 }}
          minimumValue={0}
          maximumValue={255}
          value={settings.hue}
          onValueChange={(v: number) => updateField('hue', v)}
          minimumTrackTintColor={ARCTOS_RED}
          maximumTrackTintColor="#444"
          thumbTintColor="#FFF"
        />
      </View>

      <SettingSlider
        label="BEAM INTENSITY"
        value={settings.brightness}
        onValueChange={(v: number) => updateField('brightness', v)}
      />

      <TouchableOpacity
        style={[styles.saveButton, isSaving && { opacity: 0.5 }]}
        onPress={handleSave}
        disabled={isSaving}
      >
        <Text style={styles.saveText}>
          {isSaving ? 'UPLOADING...' : 'COMMIT CHANGES'}
        </Text>
      </TouchableOpacity>
    </View>
  );
};

interface SettingSliderProps {
  label: string;
  value: number;
  onValueChange: (v: number) => void;
}

const SettingSlider = ({ label, value, onValueChange }: SettingSliderProps) => (
  <View style={styles.sliderRow}>
    <View style={styles.labelRow}>
      <Text style={styles.label}>{label}</Text>
      <Text style={styles.valueLabel}>{value}</Text>
    </View>
    <Slider
      style={{ width: '100%', height: 40 }}
      minimumValue={1}
      maximumValue={255}
      value={value}
      onValueChange={onValueChange}
      minimumTrackTintColor={ARCTOS_RED}
      maximumTrackTintColor="#444"
      thumbTintColor="#FFF"
    />
  </View>
);

const styles = StyleSheet.create({
  card: {
    padding: 20,
    backgroundColor: ARCTOS_DARK_CARD,
    borderRadius: 15,
    borderWidth: 1,
    borderColor: '#333',
    marginVertical: 10,
  },
  title: {
    fontSize: 14,
    fontWeight: '900',
    marginBottom: 20,
    color: '#FFF',
    letterSpacing: 2,
    borderLeftWidth: 3,
    borderLeftColor: ARCTOS_RED,
    paddingLeft: 10,
  },
  sliderRow: { marginVertical: 12 },
  labelRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: 5,
  },
  label: { fontSize: 11, color: '#AAA', letterSpacing: 1, fontWeight: 'bold' },
  valueLabel: {
    fontSize: 12,
    color: ARCTOS_RED,
    fontWeight: 'bold',
    fontFamily: 'monospace',
  },
  colorGroup: {
    padding: 15,
    backgroundColor: '#1A1A1A',
    borderRadius: 10,
    marginVertical: 15,
    borderWidth: 1,
    borderColor: '#2A2A2A',
  },
  previewContainer: {
    alignItems: 'center',
    marginVertical: 10,
  },
  preview: {
    height: 16,
    width: '100%',
    borderRadius: 3,
    borderWidth: 0,
    // Свечение а-ля лазерный луч
    shadowOffset: { width: 0, height: 0 },
    shadowOpacity: 1,
    shadowRadius: 8,
    elevation: 5,
  },
  saveButton: {
    backgroundColor: ARCTOS_RED,
    padding: 18,
    borderRadius: 4,
    alignItems: 'center',
    marginTop: 15,
    borderBottomWidth: 4,
    borderBottomColor: '#8B0000',
  },
  saveText: {
    color: '#fff',
    fontWeight: '900',
    letterSpacing: 2,
    fontSize: 14,
  },
});
