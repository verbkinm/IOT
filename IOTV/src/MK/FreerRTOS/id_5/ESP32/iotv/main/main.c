#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "nvs_flash.h"

#include "Global_def.h"
#include "I2C/i2c.h"
#include "iotv.h"
#include "I2C/BME280.h"
#include "I2C/VL6180X_Simple.h"
#include "I2C/DS3231.h"
#include "Net/Tcp.h"
#include "Net/WIFI.h"

//extern QueueHandle_t xQueueInData, xQueueOutData, xQueueLedSignals;

static const char *TAG = "main";

void wifi_init_sta_Task(void *pvParameters)
{
	wifi_init_sta();
	vTaskDelete(NULL);
}

void app_main(void)
{
	//	//Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	xTaskCreate(wifi_init_sta_Task, "wifi_init_sta", 4096, NULL, 1, NULL);
	ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

	// Инициализациия глобальных очередей
//	xQueueInData = xQueueCreate(100, sizeof(struct DataPkg));
//	xQueueOutData = xQueueCreate(100, sizeof(struct DataPkg));
//	xQueueLedSignals = xQueueCreate(10, sizeof(struct LedSignalPkg));

//	if (xQueueInData == NULL  || xQueueOutData == NULL || xQueueLedSignals == NULL)
//	{
//		ESP_LOGE(TAG, "Queue was not created and must not be used");
//		esp_restart();
//	}

	i2c_init();
	iotvInit();

//	xTaskCreate(iotvTask, "iotvTask", 4096, NULL, 2, NULL);
	xTaskCreate(tcp_server_task, "tcp_server", 8192, (void*)AF_INET, 2, NULL);
	xTaskCreate(OLED_Task, "oledTask", 8192, NULL, 2, NULL);

	xTaskCreate(Vl6180X_Task, "Vl6180X_Task", 4096, NULL, 3, NULL);
	xTaskCreate(BME280_Task, "BME280_Task", 4096, NULL, 3, NULL);
	xTaskCreate(DS3231_Task, "DS3132_Task", 4096, NULL, 3, NULL);
}

