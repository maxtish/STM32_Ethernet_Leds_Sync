#ifndef APP_USB_LOG_H_
#define APP_USB_LOG_H_

#include "main.h"
#include "usbd_cdc_if.h"
#include <stdarg.h>
#include <stdio.h>

void USB_Printf(const char *format, ...);

#endif
