/*
 * Докуменация по разработке ESP32-CAM:
 * https://docs.espressif.com/projects/esp-idf/en/release-v5.2/esp32/api-reference/peripherals/sdmmc_host.html
 * https://randomnerdtutorials.com/esp32-cam-ai-thinker-pinout/
 */

#include "Global_def.h"
#include "sd/sd_mmc.h"
#include "service/service.h"

static const char *TAG = "main";

void app_main(void)
{
//    uint16_t value = 0x1;
//    if ( *((char *) &value) == 1)
//    	ESP_LOGE(TAG, "litlle endian");
//    	ESP_LOGI(TAG, "big endian");

	//	sd_mmc_init();
	start_services();

//	ESP_LOGI(TAG, "Taking picture...");
//	camera_fb_t *pic = esp_camera_fb_get();

//	// use pic->buf to access the image
//	ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes", pic->len);
//	esp_camera_fb_return(pic);
}

//void app_main(void)
//{
//#if ESP_CAMERA_SUPPORTED
//    if(ESP_OK != init_camera()) {
//        return;
//    }
//
//    while (1)
//    {
//        ESP_LOGI(TAG, "Taking picture...");
//        camera_fb_t *pic = esp_camera_fb_get();
//
//        // use pic->buf to access the image
//        ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes", pic->len);
//        esp_camera_fb_return(pic);
//
//        vTaskDelay(100 / portTICK_RATE_MS);
//    }
//#else
//    ESP_LOGE(TAG, "Camera support is not available for this chip");
//    return;
//#endif
//}
