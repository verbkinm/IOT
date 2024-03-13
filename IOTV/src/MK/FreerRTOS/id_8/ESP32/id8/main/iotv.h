#ifndef MAIN_IOTV_H_
#define MAIN_IOTV_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <stdint.h>
#include <sys/socket.h>
#include <stdbool.h>

#include "Protocols/embedded/iotv_types.h"
#include "Protocols/embedded/creatorpkgs.h"
#include "Protocols/embedded/IOTV_SH.h"
#include "Protocols/embedded/iotv_server_embedded.h"
#include "Global_def.h"
#include "service/tcp/tcp.h"
#include "camera/camera.h"

void iotv_service_task(void *pvParameters);
void iotv_data_recived(const char *data, int size, int sock);

void iotv_clear_buf_data(void);

#endif /* MAIN_IOTV_H_ */
