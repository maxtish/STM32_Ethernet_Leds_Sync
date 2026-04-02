import React from 'react';
import {
  View,
  StyleSheet,
  Text,
  ScrollView,
  TouchableOpacity,
  Alert,
  SafeAreaView,
  StatusBar,
} from 'react-native';
import { useNfc } from './src/hooks/useNfc';
import { NfcRawDetails } from './src/components/NfcRawDetails';
import { DeviceSettingsEditor } from './src/components/DeviceSettingsEditor';
import { DeviceSettings } from './src/types/settings';

const ARCTOS_RED = '#D32F2F'; // Цвет лазерного диода
const ARCTOS_DARK = '#1A1A1A'; // Индустриальный черный

const defaultSettings: DeviceSettings = {
  speed: 100,
  hue: 155,
  brightness: 128,
};

const App = () => {
  const { nfcData, loading, scanTag, writeTag } = useNfc();

  const handleInitTag = async () => {
    const success = await writeTag(defaultSettings);
    if (success) {
      Alert.alert(
        'System Initialized',
        'Чип успешно сконфигурирован для Arctos Laser Controller.',
      );
    } else {
      Alert.alert('Error', 'Ошибка записи. Убедитесь в стабильности NFC-поля.');
    }
  };

  return (
    <SafeAreaView style={styles.container}>
      <StatusBar barStyle="light-content" />

      {/* Header в стиле Arctos */}
      <View style={styles.brandHeader}>
        <Text style={styles.brandText}>ARCTOS</Text>
        <Text style={styles.subBrandText}>NFC SYSTEM CONTROLLER</Text>
      </View>

      <ScrollView contentContainerStyle={styles.scrollContent}>
        {/* Центральная круглая кнопка сканирования */}
        {!nfcData && (
          <View style={styles.centerContainer}>
            <TouchableOpacity
              activeOpacity={0.8}
              style={[styles.scanButton, loading && styles.scanButtonActive]}
              onPress={() => scanTag()}
              disabled={loading}
            >
              <View style={styles.scanButtonInner}>
                <Text style={styles.scanButtonText}>
                  {loading ? 'READING...' : 'START SCAN'}
                </Text>
              </View>
            </TouchableOpacity>
            <Text style={styles.hintText}>
              Place smartphone near the module NFC tag
            </Text>
          </View>
        )}

        {nfcData && (
          <View style={styles.resultArea}>
            {/* ЕСЛИ НАСТРОЙКИ НАЙДЕНЫ - СРАЗУ ЭДИТОР */}
            {nfcData.isCompatible && nfcData.settings ? (
              <View>
                <View style={styles.successBadge}>
                  <Text style={styles.successBadgeText}>
                    ACTIVE CONNECTION: STM32 OK
                  </Text>
                </View>
                <DeviceSettingsEditor initialSettings={nfcData.settings} />
                <TouchableOpacity
                  style={styles.resetBtn}
                  onPress={() => scanTag()}
                >
                  <Text style={styles.resetBtnText}>SCAN ANOTHER DEVICE</Text>
                </TouchableOpacity>
              </View>
            ) : (
              /* ЕСЛИ НАСТРОЙКИ НЕ НАЙДЕНЫ - ИНФО И КНОПКА ПРОШИВКИ */
              <View style={styles.infoCard}>
                <Text style={styles.errorText}>⚠️ NON FIRMWARE DETECTED</Text>
                <NfcRawDetails data={nfcData} />

                <TouchableOpacity
                  style={styles.initButton}
                  onPress={handleInitTag}
                >
                  <Text style={styles.initButtonText}>
                    INITIALIZE FOR SYSTEM
                  </Text>
                </TouchableOpacity>

                <TouchableOpacity
                  style={styles.retryBtn}
                  onPress={() => scanTag()}
                >
                  <Text style={styles.retryBtnText}>RETRY SCAN</Text>
                </TouchableOpacity>
              </View>
            )}
          </View>
        )}
      </ScrollView>
    </SafeAreaView>
  );
};

const styles = StyleSheet.create({
  container: { flex: 1, backgroundColor: ARCTOS_DARK },
  brandHeader: {
    padding: 20,
    alignItems: 'center',
    borderBottomWidth: 1,
    borderBottomColor: '#333',
  },
  brandText: {
    fontSize: 28,
    fontWeight: '900',
    color: '#FFF',
    letterSpacing: 5,
  },
  subBrandText: {
    fontSize: 10,
    color: ARCTOS_RED,
    letterSpacing: 2,
    marginTop: 5,
  },
  scrollContent: { flexGrow: 1, padding: 20, justifyContent: 'center' },
  centerContainer: { alignItems: 'center', justifyContent: 'center' },

  // Круглая кнопка
  scanButton: {
    width: 200,
    height: 200,
    borderRadius: 100,
    backgroundColor: '#333',
    justifyContent: 'center',
    alignItems: 'center',
    borderWidth: 2,
    borderColor: ARCTOS_RED,
    shadowColor: ARCTOS_RED,
    shadowOffset: { width: 0, height: 0 },
    shadowOpacity: 0.5,
    shadowRadius: 15,
    elevation: 10,
  },
  scanButtonActive: {
    borderColor: '#FFF',
    shadowColor: '#FFF',
  },
  scanButtonInner: {
    width: 170,
    height: 170,
    borderRadius: 85,
    backgroundColor: ARCTOS_DARK,
    justifyContent: 'center',
    alignItems: 'center',
    borderWidth: 1,
    borderColor: '#444',
  },
  scanButtonText: {
    color: '#FFF',
    fontSize: 18,
    fontWeight: 'bold',
    letterSpacing: 1,
  },
  hintText: {
    color: '#888',
    marginTop: 30,
    textAlign: 'center',
    fontSize: 12,
  },

  // Результаты
  resultArea: { width: '100%' },
  successBadge: {
    backgroundColor: 'rgba(40, 167, 69, 0.2)',
    padding: 10,
    borderRadius: 5,
    borderWidth: 1,
    borderColor: '#28a745',
    marginBottom: 15,
    alignItems: 'center',
  },
  successBadgeText: { color: '#28a745', fontWeight: 'bold', fontSize: 12 },
  infoCard: {
    backgroundColor: '#222',
    padding: 20,
    borderRadius: 15,
    borderWidth: 1,
    borderColor: '#444',
  },
  errorText: {
    color: ARCTOS_RED,
    fontWeight: 'bold',
    textAlign: 'center',
    marginBottom: 20,
  },
  initButton: {
    backgroundColor: ARCTOS_RED,
    padding: 18,
    borderRadius: 8,
    alignItems: 'center',
    marginTop: 20,
  },
  initButtonText: { color: '#FFF', fontWeight: 'bold', letterSpacing: 1 },

  // Вспомогательные кнопки
  resetBtn: { marginTop: 20, alignItems: 'center' },
  resetBtnText: {
    color: '#888',
    fontSize: 12,
    textDecorationLine: 'underline',
  },
  retryBtn: { marginTop: 15, alignItems: 'center' },
  retryBtnText: { color: '#FFF', fontSize: 12 },
});

export default App;
