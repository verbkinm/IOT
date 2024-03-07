#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "service/service.h"
//#include "Global_def.h"
#include "I2C/i2c.h"
#include "iotv.h"
#include "I2C/DS3231.h"

static const char *TAG = "main";

void app_main(void)
{
//	xTaskCreate(wifi_init_sta_Task, "wifi_init_sta", 4096, NULL, 1, NULL);
	ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

	iotv_init();
	i2c_init();
	DS3231_set_system_time();

	start_services();

//	xTaskCreate(tcp_server_task, "tcp_server", 8192, (void*)AF_INET, 2, NULL);
	xTaskCreate(OLED_Task, "oledTask", 8192, NULL, 2, NULL);

//	xTaskCreate(Vl6180X_Task, "Vl6180X_Task", 4096, NULL, 3, NULL);
//	xTaskCreate(BME280_Task, "BME280_Task", 4096, NULL, 3, NULL);
//	xTaskCreate(DS3231_Task, "DS3132_Task", 4096, NULL, 3, NULL);
}

