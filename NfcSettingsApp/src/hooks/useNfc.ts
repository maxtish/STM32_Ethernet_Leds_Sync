import { useState } from 'react';
import NfcManager, { NfcTech, Ndef } from 'react-native-nfc-manager';
import { DeviceSettings, NfcData } from '../types/settings';
import { encodeSettings, parseNfcPayload } from '../utils/parser';

export const useNfc = () => {
  const [nfcData, setNfcData] = useState<any>(null);
  const [loading, setLoading] = useState(false);

  const writeTag = async (settings: DeviceSettings) => {
    try {
      setLoading(true);
      await NfcManager.requestTechnology(NfcTech.Ndef);

      // Подготавливаем наши 5 байт + 1 байт CRC
      const settingsBytes = encodeSettings(settings);

      // Используем TNF_MIME_MEDIA, чтобы минимизировать лишние заголовки
      // Тип 'application/octet-stream' — это стандарт для сырых байтов
      const record = Ndef.record(
        Ndef.TNF_MIME_MEDIA,
        'application/octet-stream',
        [],
        settingsBytes,
      );

      const bytesToWrite = Ndef.encodeMessage([record]);

      if (bytesToWrite) {
        await NfcManager.ndefHandler.writeNdefMessage(bytesToWrite);
        return true;
      }
      return false;
    } catch (ex) {
      console.warn('Write Error:', ex);
      return false;
    } finally {
      NfcManager.cancelTechnologyRequest();
      setLoading(false);
    }
  };

  const scanTag = async () => {
    try {
      setLoading(true);
      setNfcData(null);

      // Запрашиваем поддержку базовых технологий (Ndef для данных, NfcV для ST25DV)
      await NfcManager.requestTechnology([NfcTech.Ndef, NfcTech.NfcV]);

      const tag = await NfcManager.getTag();

      if (tag) {
        let payload: number[] = [];
        if (tag.ndefMessage && tag.ndefMessage.length > 0) {
          payload = Array.from(tag.ndefMessage[0].payload);
        }

        const settings = parseNfcPayload(payload);

        setNfcData({
          id: tag.id || 'Неизвестный ID',
          techTypes: tag.techTypes || [],
          isCompatible: !!settings,
          settings: settings || undefined,
          // Сохраняем весь текст для отладки в сыром виде
          raw: JSON.stringify(tag, null, 2),
        });
      }
    } catch (ex) {
      console.warn('Ошибка сканирования:', ex);
    } finally {
      // Обязательно закрываем сессию, чтобы антенна не работала впустую
      NfcManager.cancelTechnologyRequest();
      setLoading(false);
    }
  };

  return { nfcData, loading, scanTag, writeTag };
};
