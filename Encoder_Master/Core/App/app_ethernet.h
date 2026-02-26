#ifndef APP_ETHERNET_H_
#define APP_ETHERNET_H_


void App_Ethernet_Init(void);
void App_Ethernet_Process(void);
void Ethernet_Send_Command_To_LED(char* cmd);

#endif
