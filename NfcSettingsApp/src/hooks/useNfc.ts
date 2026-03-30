import { useState, useEffect } from 'react';
import NfcManager, { NfcEvents } from 'react-native-nfc-manager';
import { NfcData } from '../types/settings';
import { parseNfcPayload } from '../utils/parser';

export const useNfc = () => {
  const [nfcData, setNfcData] = useState<NfcData | null>(null);
  const [loading, setLoading] = useState(false);

  useEffect(() => {
    NfcManager.setEventListener(NfcEvents.DiscoverTag, (tag: any) => {
      let payload: number[] = [];

      // Ищем NDEF данные более тщательно
      if (tag.ndefMessage && tag.ndefMessage.length > 0) {
        payload = Array.from(tag.ndefMessage[0].payload);
      }

      const settings = parseNfcPayload(payload);

      setNfcData({
        raw: JSON.stringify(tag, null, 2),
        isCompatible: !!settings,
        settings: settings || undefined,
      });

      NfcManager.unregisterTagEvent().catch(() => null);
      setLoading(false);
    });

    return () => {
      NfcManager.setEventListener(NfcEvents.DiscoverTag, null);
    };
  }, []);

  const scanTag = async () => {
    try {
      setLoading(true);
      setNfcData(null);
      // Регистрируем событие для любого типа тегов
      await NfcManager.registerTagEvent();
    } catch (ex) {
      console.warn('NFC Scan Error:', ex);
      setLoading(false);
    }
  };

  return { nfcData, loading, scanTag };
};
