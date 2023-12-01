#include "TFT_touch_screen.h"
#include "I2C/i2c.h"

//static const char *TAG = "example";


void app_main(void)
{
	i2c_init();
	TFT_init(); // Перед этой функцией обязательно выполнить i2c_init();

	while (1)
	{
		// raise the task priority of LVGL and/or reduce the handler period can improve the performance
		vTaskDelay(10 / portTICK_PERIOD_MS);

		// The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
		lv_timer_handler();
	}
}
