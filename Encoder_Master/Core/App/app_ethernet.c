#include "app_ethernet.h" // Свой заголовок всегда первым
#include "dhcp.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "app_usb_log.h"
#include <string.h>
#include <stdio.h>
extern UART_HandleTypeDef huart2;

// Приватные переменные (static) - их не должен видеть никто снаружи
static uint8_t dhcp_buffer[1024];
static bool ip_assigned = false;
#define PORT_SYNC_IN 5001 // Порт, где слушаем
// Внешние объекты, которые мы используем, но они созданы в main.c
extern SPI_HandleTypeDef hspi1;
extern uint8_t rx_buf[1024];
extern uint8_t neighbor_ip[4];

// Глобальные переменные,  main.c,
extern uint8_t global_brightness;
extern uint8_t global_hue;
extern EditTarget_t current_edit_target;

// Прототипы приватных функций (чтобы не мусорить в .h)
static void Process_Encoder_Sync_Server(void);
static void Get_Unique_MAC(uint8_t *mac);
static void cb_ip_conflict(void);
static void cb_ip_assigned(void);
// --- Низкоуровневая работа с W5500 ---
void W5500_Select(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

}
;
void W5500_Unselect(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}
;

uint8_t W5500_ReadByte(void) {
	uint8_t dummy = 0xFF, rs = 0;
	HAL_SPI_TransmitReceive(&hspi1, &dummy, &rs, 1, 10);
	return rs;
}
;

void W5500_WriteByte(uint8_t wb) {
	HAL_SPI_Transmit(&hspi1, &wb, 1, 10);
}
;

// --- Инициализация ---
void App_Ethernet_Init(void) {
	W5500_Unselect();
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // RST
	HAL_Delay(50);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_Delay(200);

	reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
	reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);

	if (getVERSIONR() != 0x04)
		return;

	uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2,
			2 } };
	wizchip_init(memsize[0], memsize[1]);

	wiz_NetInfo netInfo = { .dhcp = NETINFO_DHCP };
	Get_Unique_MAC(netInfo.mac);
	setSHAR(netInfo.mac);

	DHCP_init(7, dhcp_buffer);
	reg_dhcp_cbfunc(cb_ip_assigned, cb_ip_assigned, cb_ip_conflict);
}
;

// --- Основной процесс ---
void App_Ethernet_Process(void) {
	static uint32_t last_dhcp_tick = 0;

	;

	// 1. Таймер DHCP (раз в секунду)
	if (HAL_GetTick() - last_dhcp_tick > 1000) {
		DHCP_time_handler();

	}

	uint8_t ret = DHCP_run();

	if (ret == DHCP_IP_LEASED || ret == DHCP_IP_CHANGED) {
		// --- САМОЕ ВАЖНОЕ ТУТ ---

		// TCP СЕРВЕР вызываем максимально часто (каждый круг)
		// Чтобы он мгновенно реагировал на клик в C#
		Process_Encoder_Sync_Server();


	}
}
;

// --- СЕРВЕР: Слушаем обновления (Синхронизация) ---
void Process_Encoder_Sync_Server(void) {
	uint8_t sn = 0;
	uint8_t status = getSn_SR(sn);

	switch (status) {
	case SOCK_CLOSED:
		socket(sn, Sn_MR_TCP, PORT_SYNC_IN, 0);
		break;
	case SOCK_INIT:
		listen(sn);
		break;
	case SOCK_ESTABLISHED:
		if (getSn_RX_RSR(sn) > 0) {
			int32_t len = recv(sn, rx_buf, 1024);
			if (len > 0) {
				char tx_buf[12]; // Буфер для текста
				rx_buf[len] = '\0';
				int val_rx;
				char mode_rx[16];
				// Парсим пакет типа STAT:MODE=BRIGHT;VAL=150
				if (sscanf((char*) rx_buf, "STAT:MODE=%[^;];VAL=%d", mode_rx,
						&val_rx) == 2) {
					if (strcmp(mode_rx, "BRIGHT") == 0)
						global_brightness = (uint8_t) val_rx;

					else if (strcmp(mode_rx, "COLOR") == 0)
						global_hue = (uint8_t) val_rx;


					//  Формируем строку СТРОГО под  RxCpltCallback encoder
					// Формат: $VAL:число
					int uart_len = sprintf(tx_buf, "$VAL:%d\n", val_rx);
					USB_Printf("[Sync] Update:%s",tx_buf);
					//Отправляем
					HAL_UART_Transmit(&huart2, (uint8_t*) tx_buf, uart_len,
							100);
				}
			}
		}
		break;
	case SOCK_CLOSE_WAIT:
		disconnect(sn);
		break;
	}
}
;

// --- КЛИЕНТ: Отправляем команду на Исполнитель (Порт 5001) ---
void Ethernet_Send_Command_To_LED(char *cmd) {
	uint8_t sn = 1;
	if (neighbor_ip[0] == 0)
		return;

	if (socket(sn, Sn_MR_TCP, 15000, 0) == sn) {
		if (connect(sn, neighbor_ip, 5001) == SOCK_OK) {
			send(sn, (uint8_t*) cmd, strlen(cmd));
			disconnect(sn);
		}
		close(sn);
	}
}
;





void Get_Unique_MAC(uint8_t *mac) {
	uint32_t uid0 = HAL_GetUIDw0();
	mac[0] = 0x00;
	mac[1] = 0x08;
	mac[2] = 0xDC;
	mac[3] = (uint8_t) (uid0 & 0xFF);
	mac[4] = (uint8_t) ((uid0 >> 8) & 0xFF);
	mac[5] = (uint8_t) ((uid0 >> 16) & 0xFF);
}
;

void cb_ip_assigned(void) {
	wiz_NetInfo ni;
	getIPfromDHCP(ni.ip);
	getGWfromDHCP(ni.gw);
	getSNfromDHCP(ni.sn);
	getDNSfromDHCP(ni.dns);
	getSHAR(ni.mac);
	ni.dhcp = NETINFO_DHCP;
	wizchip_setnetinfo(&ni);
	ip_assigned = true;
}
void cb_ip_conflict(void) {
	USB_Printf("IP Conflict!\r\n");
}
;


/////прием СТАТУСА и ip сервера

void Ethernet_Master_UDP_Receiver_Task(void) {
    uint8_t sn = 4;
    uint16_t port = 5005;
    uint8_t udp_buf[128]; // Немного увеличим буфер для безопасности
    uint32_t now = HAL_GetTick();
    static uint32_t last_beacon_time = 0;

    // 1. Инициализация сокета, если он не в режиме UDP
    if (getSn_SR(sn) != SOCK_UDP) {
        socket(sn, Sn_MR_UDP, port, 0);
        return;
    }

    // 2. Проверяем наличие входящих данных
    uint16_t size = getSn_RX_RSR(sn);
    if (size > 0) {
        uint8_t pack_ip[4];
        uint16_t pack_port;

        int32_t len = recvfrom(sn, udp_buf, sizeof(udp_buf) - 1, pack_ip, &pack_port);
        if (len > 0) {
            udp_buf[len] = '\0'; // Терминируем строку для работы строковых функций
            char* msg = (char*)udp_buf;

            // --- ВАРИАНТ А: Обнаружение соседа (Маяк) ---
            if (strncmp(msg, "I_AM_STM32_IP", 13) == 0) {
                memcpy(neighbor_ip, pack_ip, 4);
                // Можно добавить флаг "Сосед найден", чтобы не спамить в USB
                static uint32_t last_log = 0;
                if (now - last_log > 10000) { // Лог раз в 10 сек
                    USB_Printf("Master: Found LED at %d.%d.%d.%d\r\n",
                               neighbor_ip[0], neighbor_ip[1], neighbor_ip[2], neighbor_ip[3]);
                    last_log = now;
                }
            }
            char m_rx;

            int b_rx, c_rx;

            int akt;
            // Получение статуса (M:B|B:100|C:255) ---
            // Парсинг формата M:B|B:100|C:255
            if (sscanf((char*)udp_buf, "M:%c|B:%d|C:%d", &m_rx, &b_rx, &c_rx) == 3) {
              // Теперь у тебя есть все данные в переменных m_rx, b_rx, c_rx
            // Можешь обновить локальные переменные или экран
            USB_Printf("UDP Status: Mode=%c, VAL=%d\r\n", m_rx, b_rx);
            char tx_buf_send[12]; // Буфер для текста
            if (m_rx == 'B') akt = b_rx;
            else if (m_rx == 'C') akt = c_rx;
            int uart_len_send = sprintf(tx_buf_send, "$VAL:%d\n", akt);
            HAL_UART_Transmit(&huart2, (uint8_t*) tx_buf_send, uart_len_send,100);
            }

        }
    }



	//ОТПРАВКА МАЯКА (5 сек)
	if (now - last_beacon_time >= 5000) {
		char msg[64];
		uint8_t my_ip[4];
		getSIPR(my_ip);
		int len = sprintf(msg, "I_AM_Encoder_IP:%d.%d.%d.%d", my_ip[0], my_ip[1],
				my_ip[2], my_ip[3]);
		uint8_t broadcast_ip[4] = { 255, 255, 255, 255 };
		sendto(sn, (uint8_t* )msg, len, broadcast_ip, port);
		last_beacon_time = now;
	}

}








/*void Ethernet_Listen_Status_UDP(void) {

uint8_t sn = 4;

uint16_t port = 5005;



if (getSn_SR(sn) != SOCK_UDP) {

socket(sn, Sn_MR_UDP, port, 0);

}



uint16_t size = getSn_RX_RSR(sn);

if (size > 0) {

uint8_t pack_ip[4];

uint16_t pack_port;

uint8_t udp_buf[64];



int32_t len = recvfrom(sn, udp_buf, size, pack_ip, &pack_port);

if (len > 0) {

udp_buf[len] = '\0'; // Завершаем строку



char m_rx;

int b_rx, c_rx;

int akt;



// Парсинг формата M:B|B:100|C:255

if (sscanf((char*)udp_buf, "M:%c|B:%d|C:%d", &m_rx, &b_rx, &c_rx) == 3) {

// Теперь у тебя есть все данные в переменных m_rx, b_rx, c_rx

// Можешь обновить локальные переменные или экран

USB_Printf("UDP Status: Mode=%c, VAL=%d\r\n", m_rx, b_rx);

char tx_buf_send[12]; // Буфер для текста

if (m_rx == 'B') akt = b_rx;

else if (m_rx == 'C') akt = c_rx;



int uart_len_send = sprintf(tx_buf_send, "$VAL:%d\n", akt);

HAL_UART_Transmit(&huart2, (uint8_t*) tx_buf_send, uart_len_send,

100);

}

}

}

}

*/
