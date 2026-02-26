#include "app_usb_log.h"

void USB_Printf(const char *format, ...) {
	char buf[256];
	va_list args;
	va_start(args, format);
	int len = vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);

	if (len > 0) {
		// Ждем, пока USB освободится (предыдущая отправка завершена)
		// Это важно, чтобы данные не терялись при быстрой отправке
		uint32_t timeout = 1000;
		while (CDC_Transmit_FS((uint8_t*) buf, len) == USBD_BUSY && timeout > 0) {
			timeout--;
		}
	}
}
