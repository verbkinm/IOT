#include "TFT_touchscreen/TFT_touch_screen.h"
#include "I2C/i2c.h"
#include "sd/sd_spi.h"

#include "service/service.h"

void app_main(void)
{
	sd_spi_init();
	i2c_init();
	TFT_init(); // Перед этой функцией обязательно выполнить i2c_init();

	rgb_panel_start_services();

	while (1)
	{
		// raise the task priority of LVGL and/or reduce the handler period can improve the performance
		vTaskDelay(10 / portTICK_PERIOD_MS);

		// The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
		lv_timer_handler();
	}
}

/*
 *
 *
а бвгдеёжзийклмнопрстуфхцчшщъыьэюя
АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ
abcdefghijklmnopqrstuvwxyz
ABCDEFGHIJKLMNOPQRSTUVWXYZ
0123456789
.,°
`~!@#$%^&*()_+-=
!"№;:?<>/\|

 */
