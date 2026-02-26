#ifndef APP_ETHERNET_H_
#define APP_ETHERNET_H_

#include "main.h"
#include "wizchip_conf.h"
#include "socket.h"
#include <string.h>
#include "app_usb_log.h"

extern EditTarget_t current_edit_target;

void App_Ethernet_Init(void);
void App_Ethernet_Process(void);
void Ethernet_UDP_Periodic_Task(void);

#endif
