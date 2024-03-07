#ifndef MAIN_IOTV_H_
#define MAIN_IOTV_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdint.h>
#include "nvs.h"

#include "Protocols/embedded/iotv_types.h"
#include "Protocols/embedded/creatorpkgs.h"
#include "Protocols/embedded/IOTV_SH.h"
#include "Protocols/embedded/iotv_server_embedded.h"
#include "Global_def.h"
#include "service/tcp/tcp.h"

#include "I2C/BME280.h"
#include "I2C/VL6180X_Simple.h"
#include "I2C/oled.h"
#include "I2C/DS3231.h"

void iotv_init(void);
void iotv_data_recived(const char *data, int size, int sock);

void iotv_clear_buf_data(void);

//void Vl6180X_Task(void *pvParameters);
//void BME280_Task(void *pvParameters);
//void DS3231_Task(void *pvParameters);
void OLED_Task(void *pvParameters);

#endif /* MAIN_IOTV_H_ */
