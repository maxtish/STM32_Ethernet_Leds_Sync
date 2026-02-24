/*
 * app_ethernet.h
 *
 *  Created on: 21.02.2026
 *      Author: maxti
 */
#ifndef APP_ETHERNET_H_
#define APP_ETHERNET_H_

#include "main.h"
#include "wizchip_conf.h"
#include "socket.h"
#include "app_usb_log.h"
#include <string.h>

extern EditTarget_t current_edit_target;


// Прототип основной функции
void Process_Ethernet_Traffic(void);

// Прототипы функций инициализации (те, что были в начале main)
void W5500_Init(void);
void W5500_Select(void);
void W5500_Unselect(void);
uint8_t W5500_ReadByte(void);
void W5500_WriteByte(uint8_t wb);
void App_Ethernet_Init(void);
void App_Ethernet_Process(void);
void Process_Ethernet_Server(void);
void Ethernet_Send_Status_To_Encoder(void);
void Get_Unique_MAC(uint8_t *mac_array);
void Process_UDP_Beacon(void);
void Listen_For_Neighbor(void);
void cb_ip_assigned(void);
void cb_ip_conflict(void);
void handle_encoder_socket(uint8_t sn, uint16_t port);
void handle_pc_socket(uint8_t sn, uint16_t port);
void Parse_Ethernet_Commands(char *data_ptr, uint8_t sn);
void Ethernet_UDP_Periodic_Task(void);
void Ethernet_UDP_Status(void);
#endif
