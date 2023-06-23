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
//#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
//
//#define LEDC_TEST_DUTY         (LED_maxDuty)

TaskHandle_t led_rgb_task = NULL;
extern uint8_t Led_RGB_scriptNumber;

void app_main(void)
{
	xTaskCreate(Led_RGB_Task, "Led_RGB_Task", 2048, NULL, 1, &led_rgb_task);

//	Led_RGB_script_1_init();
//
//	while (1)
//	{
//		Led_RGB_script_1();
//	}

	while (1)
	{
		vTaskDelay(3000 / portTICK_PERIOD_MS);
		Led_RGB_scriptNumber++;
		printf("Led_RGB_scriptNumber %d\n", Led_RGB_scriptNumber);
		fflush(stdout);
//		Led_RGB_scriptNumber = 2;
	}
}
