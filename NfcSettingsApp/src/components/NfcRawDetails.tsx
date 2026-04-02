import React from 'react';
import { View, Text, StyleSheet, ScrollView } from 'react-native';

const ARCTOS_RED = '#D32F2F';
const ARCTOS_DARK_BG = '#1A1A1A';
const ARCTOS_PANEL = '#252525';

export const NfcRawDetails = ({ data }: { data: any }) => {
  if (!data) return null;

  const records = data.allRecords || [];
  const techTypes = data.techTypes || [];

  return (
    <View style={styles.container}>
      {/* Шапка остается зафиксированной сверху */}
      <View style={styles.headerRow}>
        <Text style={styles.headerTitle}>HARDWARE DIAGNOSTICS</Text>
        <View style={styles.statusDot} />
      </View>

      {/* Единый скролл для всего контента */}
      <ScrollView
        nestedScrollEnabled={true}
        style={styles.mainScroll}
        contentContainerStyle={styles.scrollContent}
      >
        <View style={styles.mainInfo}>
          <View style={styles.infoRow}>
            <Text style={styles.label}>UUID:</Text>
            <Text style={styles.value}>{data.id || 'NULL_PTR'}</Text>
          </View>
          <View style={styles.infoRow}>
            <Text style={styles.label}>PROT:</Text>
            <Text style={styles.techValue}>
              {techTypes.length > 0 ? techTypes.join(' | ') : 'NO_TECH_FOUND'}
            </Text>
          </View>
        </View>

        <Text style={styles.sectionDivider}>NDEF DATA STREAM</Text>

        {records.length > 0 ? (
          records.map((rec: any, index: number) => (
            <View key={index} style={styles.recordBox}>
              <View style={styles.recHeader}>
                <Text style={styles.recIndex}>ENTRY_0{index + 1}</Text>
                {rec.type && <Text style={styles.recType}>{rec.type}</Text>}
              </View>
              {rec.text && (
                <Text style={styles.recText}>
                  <Text style={styles.syntax}>&gt; STR: </Text>
                  {rec.text}
                </Text>
              )}
              {rec.payloadRaw && (
                <View style={styles.bytesContainer}>
                  <Text style={styles.syntax}>&gt; RAW: </Text>
                  <Text style={styles.recBytes}>
                    {rec.payloadRaw
                      .map((b: number) =>
                        b.toString(16).toUpperCase().padStart(2, '0'),
                      )
                      .join(' ')}
                  </Text>
                </View>
              )}
            </View>
          ))
        ) : (
          <View style={styles.emptyBox}>
            <Text style={styles.emptyText}>NO DATA BLOCKS DETECTED</Text>
          </View>
        )}

        <Text style={styles.debugTitle}>SYSTEM_JSON_DUMP</Text>
        <View style={styles.jsonBox}>
          {/* Теперь текст просто лежит внутри View, скроллится вместе со всей секцией */}
          <Text style={styles.debugText}>{JSON.stringify(data, null, 2)}</Text>
        </View>
      </ScrollView>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    padding: 15,
    backgroundColor: ARCTOS_PANEL,
    borderRadius: 4,
    borderWidth: 1,
    borderColor: '#333',
    marginVertical: 10,
    maxHeight: 550, // Максимальная высота всего окна диагностики
  },
  mainScroll: {
    flex: 1,
  },
  scrollContent: {
    paddingBottom: 10,
  },
  headerRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: 10,
    borderBottomWidth: 1,
    borderBottomColor: '#444',
    paddingBottom: 5,
  },
  headerTitle: {
    color: '#FFF',
    fontSize: 10,
    fontWeight: '900',
    letterSpacing: 2,
  },
  statusDot: {
    width: 6,
    height: 6,
    borderRadius: 3,
    backgroundColor: ARCTOS_RED,
  },
  mainInfo: { marginBottom: 10 },
  infoRow: { flexDirection: 'row', marginBottom: 4 },
  label: {
    width: 50,
    fontSize: 12,
    color: '#666',
    fontWeight: 'bold',
    fontFamily: 'monospace',
  },
  value: {
    fontSize: 12,
    color: ARCTOS_RED,
    fontWeight: 'bold',
    fontFamily: 'monospace',
  },
  techValue: { fontSize: 11, color: '#AAA', fontFamily: 'monospace' },
  sectionDivider: {
    fontSize: 9,
    color: '#444',
    fontWeight: '900',
    marginVertical: 10,
    textAlign: 'center',
    letterSpacing: 1,
  },
  recordBox: {
    backgroundColor: ARCTOS_DARK_BG,
    padding: 10,
    marginBottom: 8,
    borderLeftWidth: 2,
    borderLeftColor: ARCTOS_RED,
  },
  recHeader: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    marginBottom: 5,
  },
  recIndex: { fontSize: 10, color: ARCTOS_RED, fontWeight: 'bold' },
  recType: { fontSize: 10, color: '#444', fontFamily: 'monospace' },
  syntax: { color: ARCTOS_RED, fontWeight: 'bold' },
  recText: { color: '#AAA', fontSize: 12, fontFamily: 'monospace' },
  bytesContainer: { flexDirection: 'row', marginTop: 5 },
  recBytes: { fontSize: 11, color: '#666', fontFamily: 'monospace', flex: 1 },
  emptyBox: {
    padding: 15,
    alignItems: 'center',
    borderWidth: 1,
    borderColor: '#333',
    borderStyle: 'dashed',
  },
  emptyText: { color: '#444', fontSize: 10 },
  debugTitle: { marginTop: 15, fontSize: 9, color: '#444', fontWeight: '900' },
  jsonBox: {
    marginTop: 5,
    padding: 8,
    backgroundColor: '#000',
    borderRadius: 2,
    borderWidth: 1,
    borderColor: '#333',
    // Убрали высоту height: 150, теперь блок растянется под JSON
  },
  debugText: { fontSize: 10, color: '#0F0', fontFamily: 'monospace' },
});
