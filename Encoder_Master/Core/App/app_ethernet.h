#ifndef APP_ETHERNET_H_
#define APP_ETHERNET_H_

#include <stdint.h>
#include <stdbool.h>


#include "main.h"



void App_Ethernet_Init(void);
void App_Ethernet_Process(void);
void Ethernet_Send_Command_To_LED(char* cmd);
void Ethernet_Master_UDP_Receiver_Task(void);
#endif
