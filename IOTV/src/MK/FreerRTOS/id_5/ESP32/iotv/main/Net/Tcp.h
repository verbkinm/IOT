#ifndef MAIN_NET_TCP_H_
#define MAIN_NET_TCP_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "Global_def.h"
#include "LedSignals.h"
#include "I2C/oled.h"

void tcp_server_task(void *pvParameters);

#endif /* MAIN_NET_TCP_H_ */
