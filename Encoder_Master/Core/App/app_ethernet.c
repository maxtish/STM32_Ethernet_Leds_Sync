#include "app_ethernet.h"
#include "dhcp.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "app_usb_log.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

extern UART_HandleTypeDef huart2;


static uint8_t dhcp_buffer[1024];
static bool ip_assigned = false;
static uint8_t neighbor_ip[4];

#define PORT_SYNC_IN 5001

extern SPI_HandleTypeDef hspi1;

static void W5500_Select(void);
static void W5500_Unselect(void);
static uint8_t W5500_ReadByte(void);
static void Get_Unique_MAC(uint8_t *mac);
static void cb_ip_conflict(void);
static void cb_ip_assigned(void);
static void Ethernet_Master_UDP_Receiver_Task(void);


// --- W5500 ---
static void W5500_Select(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

}
;
static void W5500_Unselect(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}
;

static uint8_t W5500_ReadByte(void) {
	uint8_t dummy = 0xFF, rs = 0;
	HAL_SPI_TransmitReceive(&hspi1, &dummy, &rs, 1, 10);
	return rs;
}
;

void W5500_WriteByte(uint8_t wb) {
	HAL_SPI_Transmit(&hspi1, &wb, 1, 10);
}
;

static void Get_Unique_MAC(uint8_t *mac) {
	uint32_t uid0 = HAL_GetUIDw0();
	mac[0] = 0x00;
	mac[1] = 0x08;
	mac[2] = 0xDC;
	mac[3] = (uint8_t) (uid0 & 0xFF);
	mac[4] = (uint8_t) ((uid0 >> 8) & 0xFF);
	mac[5] = (uint8_t) ((uid0 >> 16) & 0xFF);
}
;

static void cb_ip_assigned(void) {
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
static void cb_ip_conflict(void) {
	USB_Printf("IP Conflict!\r\n");
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

void App_Ethernet_Process(void) {
	static uint32_t last_dhcp_tick = 0;

	if (HAL_GetTick() - last_dhcp_tick > 1000) {
		DHCP_time_handler();
		last_dhcp_tick = HAL_GetTick();
	}

	uint8_t ret = DHCP_run();

	if (ret == DHCP_IP_LEASED || ret == DHCP_IP_CHANGED) {
		Ethernet_Master_UDP_Receiver_Task();
	}
}
;


// --- КЛИЕНТ: Отправляем команду на Исполнитель stm32 с лентой (Порт 5001) ---
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


/////прием СТАТУСА и ip сервера
static void Ethernet_Master_UDP_Receiver_Task(void) {
	uint8_t sn = 4;
	uint16_t port = 5005;
	uint8_t udp_buf[128];
	uint32_t now = HAL_GetTick();
	static uint32_t last_beacon_time = 0;


	if (getSn_SR(sn) != SOCK_UDP) {
		socket(sn, Sn_MR_UDP, port, 0);
		return;
	}

	uint16_t size = getSn_RX_RSR(sn);
	if (size > 0) {
		uint8_t pack_ip[4];
		uint16_t pack_port;
		int32_t len = recvfrom(sn, udp_buf, sizeof(udp_buf) - 1, pack_ip,
				&pack_port);
		if (len > 0) {
			udp_buf[len] = '\0';
			char *msg = (char*) udp_buf;

			if (strncmp(msg, "I_AM_STM32_IP", 13) == 0) {
				memcpy(neighbor_ip, pack_ip, 4);

				static uint32_t last_log = 0;
				if (now - last_log > 10000) {
					USB_Printf("Master: Found LED at %d.%d.%d.%d\r\n",
							neighbor_ip[0], neighbor_ip[1], neighbor_ip[2],
							neighbor_ip[3]);
					last_log = now;
				}
			}
			char m_rx;

			int b_rx, c_rx;

			int akt;
			// Получение статуса (M:B|B:100|C:255) ---
			if (sscanf((char*) udp_buf, "M:%c|B:%d|C:%d", &m_rx, &b_rx, &c_rx)
					== 3) {
				USB_Printf("UDP Status: Mode=%c, VAL=%d\r\n", m_rx, b_rx);
				char tx_buf_send[12];
				if (m_rx == 'B')
					akt = b_rx;
				else if (m_rx == 'C')
					akt = c_rx;
				int uart_len_send = sprintf(tx_buf_send, "$VAL:%d\n", akt);
				HAL_UART_Transmit(&huart2, (uint8_t*) tx_buf_send,
						uart_len_send, 100);
			}

		}
	}

	if (now - last_beacon_time >= 5000) {
		char msg[64];
		uint8_t my_ip[4];
		getSIPR(my_ip);
		int len = sprintf(msg, "I_AM_Encoder_IP:%d.%d.%d.%d", my_ip[0],
				my_ip[1], my_ip[2], my_ip[3]);
		uint8_t broadcast_ip[4] = { 255, 255, 255, 255 };
		sendto(sn, (uint8_t* )msg, len, broadcast_ip, port);
		last_beacon_time = now;
	}

}

