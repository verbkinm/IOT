#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"
#include "esp_err.h"

#include "Led_RGB.h"

//#define LEDC_HS_TIMER          LEDC_TIMER_0
//#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
//#define LEDC_HS_CH0_GPIO       (32)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
//
//#define LEDC_TEST_DUTY         (LED_maxDuty)


void app_main(void)
{

	Lef_RGB_script_1_init();

	while (1)
	{
		Lef_RGB_script_1();
	}

	while (1)
	{
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
