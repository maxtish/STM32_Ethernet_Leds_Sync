import React from 'react';
import { Text, ScrollView, StyleSheet } from 'react-native';

export const NfcRawDetails = ({ data }: { data: string }) => (
  <ScrollView style={styles.rawBox}>
    <Text style={styles.title}>Raw NFC Data:</Text>
    <Text style={styles.code}>{data}</Text>
  </ScrollView>
);

const styles = StyleSheet.create({
  rawBox: {
    backgroundColor: '#eee',
    padding: 10,
    borderRadius: 5,
    maxHeight: 200,
  },
  title: { fontWeight: 'bold', marginBottom: 5 },
  code: { fontFamily: 'monospace', fontSize: 12 },
});
