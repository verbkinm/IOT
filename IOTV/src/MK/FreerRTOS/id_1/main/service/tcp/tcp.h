#ifndef MAIN_NET_TCP_H_
#define MAIN_NET_TCP_H_

#include "freertos/FreeRTOS.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "global/global_def.h"

#include "iotv.h"
#include "Local_Lib/local_lib.h"

void tcp_server_task(void *pvParameters);
const char *tcp_server_task_name(void);

#endif /* MAIN_NET_TCP_H_ */
