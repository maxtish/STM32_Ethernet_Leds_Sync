import React from 'react';
import { View, Text, StyleSheet } from 'react-native';

export const NfcRawDetails = ({ data }: { data: any }) => {
  // Базовая проверка на наличие объекта данных
  if (!data) return null;

  // Безопасное извлечение записей (если их нет, будет пустой массив)
  const records = data.allRecords || [];
  const techTypes = data.techTypes || [];

  return (
    <View style={styles.container}>
      {/* Основная информация о чипе */}
      <View style={styles.mainInfo}>
        <Text style={styles.label}>
          ID метки: <Text style={styles.value}>{data.id || 'Неизвестно'}</Text>
        </Text>
        <Text style={styles.label}>
          Технологии:{' '}
          <Text style={styles.value}>
            {techTypes.length > 0 ? techTypes.join(', ') : 'Не определены'}
          </Text>
        </Text>
      </View>

      {/* Секция NDEF записей */}
      <Text style={[styles.label, { marginTop: 15, marginBottom: 5 }]}>
        NDEF Данные: {records.length > 0 ? `(${records.length})` : '(Пусто)'}
      </Text>

      {records.length > 0 ? (
        records.map((rec: any, index: number) => (
          <View key={index} style={styles.recordBox}>
            <Text style={styles.recTitle}>
              Запись #{index + 1} {rec.type ? `[${rec.type}]` : ''}
            </Text>
            {rec.text && <Text style={styles.recText}>Текст: {rec.text}</Text>}
            {rec.payloadRaw && (
              <Text style={styles.recBytes}>
                Байты: {rec.payloadRaw.join(', ')}
              </Text>
            )}
          </View>
        ))
      ) : (
        <View style={styles.emptyBox}>
          <Text style={styles.emptyText}>NDEF-сообщения не найдены</Text>
        </View>
      )}

      {/* Полный дамп для отладки */}
      <Text style={styles.debugTitle}>Полный дамп (JSON):</Text>
      <View style={styles.jsonBox}>
        <Text style={styles.debugText}>{JSON.stringify(data, null, 2)}</Text>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    padding: 12,
    backgroundColor: '#f8f9fa',
    borderRadius: 10,
    borderWidth: 1,
    borderColor: '#e0e0e0',
  },
  mainInfo: {
    borderBottomWidth: 1,
    borderBottomColor: '#eee',
    paddingBottom: 10,
  },
  label: { fontWeight: 'bold', color: '#444', fontSize: 14 },
  value: { fontWeight: '500', color: '#007AFF' },
  recordBox: {
    backgroundColor: '#fff',
    padding: 10,
    marginTop: 8,
    borderRadius: 6,
    borderLeftWidth: 4,
    borderLeftColor: '#007AFF',
    elevation: 1, // Тень для Android
    shadowColor: '#000', // Тень для iOS
    shadowOffset: { width: 0, height: 1 },
    shadowOpacity: 0.1,
  },
  recTitle: {
    fontSize: 12,
    fontWeight: 'bold',
    color: '#666',
    marginBottom: 4,
  },
  recText: {
    color: '#28a745',
    fontSize: 14,
    fontWeight: '500',
    marginBottom: 4,
  },
  recBytes: { fontSize: 11, color: '#999', fontFamily: 'monospace' },
  emptyBox: {
    padding: 15,
    alignItems: 'center',
    backgroundColor: '#eee',
    borderRadius: 6,
    marginTop: 5,
  },
  emptyText: { color: '#888', fontStyle: 'italic' },
  debugTitle: {
    marginTop: 20,
    fontSize: 11,
    color: '#999',
    fontWeight: 'bold',
    textTransform: 'uppercase',
  },
  jsonBox: {
    marginTop: 5,
    padding: 8,
    backgroundColor: '#222',
    borderRadius: 5,
  },
  debugText: { fontSize: 10, color: '#00ff00', fontFamily: 'monospace' },
});
