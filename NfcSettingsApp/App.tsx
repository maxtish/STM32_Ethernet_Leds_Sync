import React, { useState } from 'react';
import {
  View,
  StyleSheet,
  Button,
  Text,
  ScrollView,
  TouchableOpacity,
  Alert,
} from 'react-native';
import { useNfc } from './src/hooks/useNfc';
import { NfcRawDetails } from './src/components/NfcRawDetails';
import { DeviceSettingsEditor } from './src/components/DeviceSettingsEditor';
import { DeviceSettings } from './src/types/settings';
const defaultSettings: DeviceSettings = {
  speed: 800,
  hue: 155,
  brightness: 128,
};
const App = () => {
  const { nfcData, loading, scanTag, writeTag } = useNfc();
  const [showEditor, setShowEditor] = useState(false);

  // Сбрасываем состояние редактора при новом сканировании
  const handleScan = () => {
    setShowEditor(false);
    scanTag();
  };

  const handleInitTag = async () => {
    const success = await writeTag(defaultSettings);
    if (success) {
      Alert.alert('Метка успешно прошита! Теперь нажми Скан.');
    } else {
      Alert.alert('Ошибка записи. Убедись, что метка вплотную к телефону.');
    }
  };

  return (
    <ScrollView style={styles.container}>
      <Text style={styles.header}>NFC Universal Reader</Text>

      <Button
        title={loading ? 'Ожидание метки...' : 'Сканировать любое устройство'}
        onPress={handleScan}
      />

      {nfcData && (
        <View style={styles.resultContainer}>
          {/* Секция 1: Сырые данные (показываем всегда) */}
          <Text style={styles.sectionTitle}>Информация о чипе:</Text>
          <NfcRawDetails data={nfcData} />

          {/* Секция 2: Проверка совместимости */}
          <View style={styles.statusBox}>
            <Text style={nfcData.isCompatible ? styles.ok : styles.fail}>
              {nfcData.isCompatible
                ? '✅ Настройки STM32 обнаружены'
                : '❌ Данные не распознаны как настройки'}
            </Text>
          </View>
          <Button
            title="Прошить эту метку под STM32"
            onPress={handleInitTag}
            color="#28a745"
          />
          {/* Секция 3: Кнопка входа в настройки */}
          {nfcData.isCompatible && !showEditor && (
            <TouchableOpacity
              style={styles.openButton}
              onPress={() => setShowEditor(true)}
            >
              <Text style={styles.openButtonText}>ОТКРЫТЬ НАСТРОЙКИ</Text>
            </TouchableOpacity>
          )}

          {/* Секция 4: Редактор (по кнопке) */}
          {showEditor && nfcData.settings && (
            <DeviceSettingsEditor
              initialSettings={nfcData.settings}
              onSave={data => {
                console.log('Saved:', data);
                setShowEditor(false); // Закрываем после сохранения
              }}
            />
          )}
        </View>
      )}
    </ScrollView>
  );
};

const styles = StyleSheet.create({
  container: { flex: 1, backgroundColor: '#f5f5f5', padding: 20 },
  header: {
    fontSize: 20,
    fontWeight: 'bold',
    textAlign: 'center',
    marginBottom: 20,
    marginTop: 20,
  },
  resultContainer: { marginTop: 20, paddingBottom: 40 },
  sectionTitle: {
    fontSize: 16,
    fontWeight: 'bold',
    marginBottom: 10,
    color: '#444',
  },
  statusBox: {
    marginVertical: 15,
    padding: 10,
    borderRadius: 8,
    backgroundColor: '#fff',
    alignItems: 'center',
  },
  ok: { color: 'green', fontWeight: 'bold' },
  fail: { color: '#888' },
  warning: { color: 'red', marginTop: 10, textAlign: 'center' },
  openButton: {
    backgroundColor: '#FF9500',
    padding: 15,
    borderRadius: 10,
    alignItems: 'center',
    marginVertical: 10,
    elevation: 2,
  },
  openButtonText: { color: 'white', fontWeight: 'bold', fontSize: 16 },
});

export default App;
