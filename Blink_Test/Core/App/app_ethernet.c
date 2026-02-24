#include "dhcp.h"
#include "app_ethernet.h"
#include <stdbool.h> // <--- ОБЯЗАТЕЛЬНО для работы bool
#include <string.h>  // <--- ОБЯЗАТЕЛЬНО для memcpy и strncmp
#include <stdio.h>   // <--- ОБЯЗАТЕЛЬНО для sprintf

#define PORT_PC      5000
#define PORT_ENCODER 5001

/* Импорт переменных управления из main.c */
extern EditTarget_t current_edit_target;
extern volatile uint8_t global_brightness;
extern volatile uint8_t global_hue;

// Прототипы функций (чтобы компилятор не ругался на порядок)
void Get_Unique_MAC(uint8_t *mac_array);
void Process_UDP_Beacon(void);
void Listen_For_Neighbor(void);
void cb_ip_assigned(void);
void cb_ip_conflict(void);

// Внешние переменные
extern SPI_HandleTypeDef hspi1;
extern uint8_t rx_buf[1024];

// Глобальные переменные этого файла
static uint8_t dhcp_buffer[1024];
static bool ip_assigned = false;

// Сосед (если объявлен в main.c, то здесь только extern БЕЗ присваивания)
extern uint8_t neighbor_ip[4];
// Выбор чипа (CS в 0)
void W5500_Select(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
}

// Отмена выбора (CS в 1)
void W5500_Unselect(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

// Передача и прием одного байта
uint8_t W5500_ReadByte(void) {
	uint8_t dummy = 0xFF; // Байт-пустышка, чтобы мастер генерировал такты
	uint8_t rs = 0;
	// Используем TransmitReceive для одновременной записи и чтения
	HAL_SPI_TransmitReceive(&hspi1, &dummy, &rs, 1, 10);
	return rs;
}

void W5500_WriteByte(uint8_t wb) {
	HAL_SPI_Transmit(&hspi1, &wb, 1, 10);
}

void W5500_Reset(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_Delay(200);
}

void App_Ethernet_Init(void) {
	W5500_Unselect(); // CS = 1
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // RST = 0
	HAL_Delay(50);  // Держим сброс дольше
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // RST = 1
	HAL_Delay(200); // Ждем, пока чип "прогрузится"

	// Регистрация функций в библиотеке
	reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
	reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);

	// ПРОВЕРКА СВЯЗИ ПЕРЕД SOCKET
	uint8_t version = getVERSIONR();
	if (version != 0x04) {
		USB_Printf("SPI ERROR\r\n");
		return;
	}

	// ВАЖНО: Настройка памяти (2KB на каждый из 8 сокетов)
	uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2,
			2 } };
	wizchip_init(memsize[0], memsize[1]);

	wiz_NetInfo netInfo = { .dhcp = NETINFO_DHCP };
	Get_Unique_MAC(netInfo.mac);

	// Сначала применяем MAC, остальное заполнит DHCP
	setSHAR(netInfo.mac);

	DHCP_init(7, dhcp_buffer);
	reg_dhcp_cbfunc(cb_ip_assigned, cb_ip_assigned, cb_ip_conflict);

	USB_Printf("My MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", netInfo.mac[0],
			netInfo.mac[1], netInfo.mac[2], netInfo.mac[3], netInfo.mac[4],
			netInfo.mac[5]);
}

void App_Ethernet_Process(void) {
	static uint32_t last_dhcp_tick = 0;


	// 1. Таймер DHCP (раз в секунду)
	if (HAL_GetTick() - last_dhcp_tick > 1000) {
		DHCP_time_handler();

	}

	uint8_t ret = DHCP_run();

	if (ret == DHCP_IP_LEASED || ret == DHCP_IP_CHANGED) {
		Process_Ethernet_Server();


	}
}

void Process_Ethernet_Server(void) {
	// --- СОКЕТ 0: РАБОТА С ПК ---
	handle_pc_socket(0, PORT_PC);

	// --- СОКЕТ 1: РАБОТА С ЭНКОДЕРОМ ---
	handle_encoder_socket(1, PORT_ENCODER);
}

// Логика для ПК (Порт 5000)
void handle_pc_socket(uint8_t sn, uint16_t port) {
	uint8_t status = getSn_SR(sn);
	switch (status) {
	case SOCK_CLOSED:
		socket(sn, Sn_MR_TCP, port, 0);
		break;
	case SOCK_INIT:
		listen(sn);
		break;
	case SOCK_ESTABLISHED:
		if (getSn_RX_RSR(sn) > 0) {
			int32_t len = recv(sn, rx_buf, 1024);
			if (len > 0) {
				rx_buf[len] = '\0';
				char *data_ptr = (char*) rx_buf;
				// Обработка команд
				Parse_Ethernet_Commands(data_ptr, sn);

			}
		}
		break;
	case SOCK_CLOSE_WAIT:
		disconnect(sn);
		break;
	}
}

// Логика для Энкодера (Порт 5001)
void handle_encoder_socket(uint8_t sn, uint16_t port) {
	uint8_t status = getSn_SR(sn);
	switch (status) {
	case SOCK_CLOSED:
		socket(sn, Sn_MR_TCP, port, 0);
		break;
	case SOCK_INIT:
		listen(sn);
		break;
	case SOCK_ESTABLISHED:
		if (getSn_RX_RSR(sn) > 0) {
			int32_t len = recv(sn, rx_buf, 1024);
			if (len > 0) {
				rx_buf[len] = '\0';
				char *data_ptr = (char*) rx_buf;
				// Обработка команд
				Parse_Ethernet_Commands(data_ptr, sn);

			}
		}
		break;
	case SOCK_CLOSE_WAIT:
		disconnect(sn);
		break;
	}
}

void Parse_Ethernet_Commands(char *data_ptr, uint8_t sn) {

	// 1. Обработка команды смены режима настройки ($BTN)
	if (strstr(data_ptr, "$BTN") != NULL) {
		if (current_edit_target == EDIT_BRIGHTNESS) {
			current_edit_target = EDIT_COLOR;
			USB_Printf("[Eth] Режим: Смена ЦВЕТА\r\n");
		} else {
			current_edit_target = EDIT_BRIGHTNESS;
			USB_Printf("[Eth] Режим: Смена ЯРКОСТИ\r\n");
		}
		send(sn, (uint8_t*) "MODE_CHANGED", 12);
	}

	// 2. Обработка значения ($VAL:число)
	int val_received;
	if (sscanf(data_ptr, "$VAL:%d", &val_received) == 1) {
		// Ограничиваем диапазон 0-255
		if (val_received < 0)
			val_received = 0;
		if (val_received > 255)
			val_received = 255;

		if (current_edit_target == EDIT_BRIGHTNESS) {
			global_brightness = (uint8_t) val_received;
			USB_Printf("[Eth] Яркость установлена: %d\r\n", global_brightness);
		} else {
			global_hue = (uint8_t) val_received;
			USB_Printf("[Eth] Цвет (Hue) установлен: %d\r\n", global_hue);
		}
		send(sn, (uint8_t*) "VAL_OK", 6);
	}

	Ethernet_UDP_Status();

}

// Callback: вызывается при успешном получении IP
void cb_ip_assigned(void) {
	wiz_NetInfo ni;

	// 1. Читаем то, что выдал DHCP сервер
	getIPfromDHCP(ni.ip);
	getGWfromDHCP(ni.gw);
	getSNfromDHCP(ni.sn);
	getDNSfromDHCP(ni.dns);

	// 2. MAC адрес DHCP не возвращает, берем его из регистров W5500
	getSHAR(ni.mac);

	ni.dhcp = NETINFO_DHCP;

	// 3. Записываем всё это обратно в конфиг чипа
	wizchip_setnetinfo(&ni);

	USB_Printf("DHCP: IP Assigned! %d.%d.%d.%d\r\n", ni.ip[0], ni.ip[1],
			ni.ip[2], ni.ip[3]);
	ip_assigned = true;
}

// Callback: вызывается при ошибке
void cb_ip_conflict(void) {
	USB_Printf("DHCP: IP Conflict!\r\n");
}

void Get_Unique_MAC(uint8_t *mac_array) {
	// Базовый адрес Unique ID для STM32F1 - 0x1FFFF7E8
	uint32_t uid0 = HAL_GetUIDw0();
	uint32_t uid1 = HAL_GetUIDw1();
	uint32_t uid2 = HAL_GetUIDw2();

	// Оставляем первые 3 байта стандартными (Wiznet),
	// а последние 3 делаем уникальными на основе ID чипа
	mac_array[0] = 0x00;
	mac_array[1] = 0x08;
	mac_array[2] = 0xDC;
	mac_array[3] = (uint8_t) (uid0 & 0xFF);
	mac_array[4] = (uint8_t) (uid1 & 0xFF);
	mac_array[5] = (uint8_t) (uid2 & 0xFF);
}

//// ОТПРАВКА СТАТУСА формат M:B|B:100|C:255
void Ethernet_UDP_Periodic_Task(void) {
	static uint32_t last_status_time = 0;
	static uint32_t last_beacon_time = 0;
	uint8_t sn = 4;
	uint16_t port = 5005;
	uint32_t now = HAL_GetTick();

	// 1. Инициализация
	if (getSn_SR(sn) != SOCK_UDP) {
		socket(sn, Sn_MR_UDP, port, 0);
		return;
	}

	// 2. ПРИЕМ (Слушаем соседей каждый цикл без задержек)
	uint16_t rx_size = getSn_RX_RSR(sn);
	if (rx_size > 0) {
		uint8_t target_ip[4];
		uint16_t target_port;
		uint8_t udp_buf[64];
		int32_t received = recvfrom(sn, udp_buf, rx_size, target_ip,
				&target_port);

		if (received > 0) {
			udp_buf[received] = '\0';
			// Если кто-то крикнул "Я энкодер" или "Я STM32"
			if (strstr((char*) udp_buf, "I_AM_Encoder_IP") != NULL) {

				uint8_t my_ip[4];
				getSIPR(my_ip); // Взять текущий IP из регистров W5500

				if (memcmp(target_ip, my_ip, 4) != 0) {
					memcpy(neighbor_ip, target_ip, 4);
					USB_Printf("Neighbor found");
				}
			}
		}
	}



	// 4. ОТПРАВКА МАЯКА (5 сек)
	if (now - last_beacon_time >= 5000) {
		char msg[64];
		uint8_t my_ip[4];
		getSIPR(my_ip);
		int len = sprintf(msg, "I_AM_STM32_IP:%d.%d.%d.%d", my_ip[0], my_ip[1],
				my_ip[2], my_ip[3]);
		uint8_t broadcast_ip[4] = { 255, 255, 255, 255 };
		sendto(sn, (uint8_t* )msg, len, broadcast_ip, port);
		last_beacon_time = now;
	}
}

//// ОТПРАВКА СТАТУСА формат M:B|B:100|C:255
void Ethernet_UDP_Status(void) {

	uint8_t sn = 4;
	uint16_t port = 5005;

	// 1. Инициализация
	if (getSn_SR(sn) != SOCK_UDP) {
		socket(sn, Sn_MR_UDP, port, 0);
		return;
	}

	// 3. ОТПРАВКА СТАТУСА (1 сек)

	char msg_stat[64];
	char mode_char = (current_edit_target == EDIT_COLOR) ? 'C' : 'B';

	// Формируем строку статуса
	int len_stat = sprintf(msg_stat, "M:%c|B:%d|C:%d", mode_char, global_brightness,
			global_hue);

	uint8_t broadcast_ip[4] = { 255, 255, 255, 255 };

	if (sendto(sn, (uint8_t*)msg_stat, len_stat, broadcast_ip, port) > 0) {
		// СООБЩЕНИЕ В ТЕРМИНАЛ
		USB_Printf("[UDP] Status Broadcast: %s\r\n", msg_stat);

	} else {
		USB_Printf("[UDP] Send Error!\r\n");
	}
}



