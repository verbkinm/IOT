#ifndef MAIN_IOTV_H_
#define MAIN_IOTV_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdint.h>

#include "Protocols/embedded/iotv_types.h"
#include "Protocols/embedded/creatorpkgs.h"
#include "Protocols/embedded/IOTV_SH.h"
#include "Protocols/embedded/iotv_server_embedded.h"
#include "Global_def.h"

#include "service/tcp/tcp.h"
#include "nvs/local_nvs.h"

esp_err_t iotv_init(void);
void iotv_data_recived(const char *data, int size, int sock);

void iotv_clear_buf_data(void);

iotv_obj_t*iotv_get(void);

#endif /* MAIN_IOTV_H_ */
