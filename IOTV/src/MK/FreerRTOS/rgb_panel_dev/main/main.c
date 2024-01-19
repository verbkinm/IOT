#include "TFT_touchscreen/TFT_touch_screen.h"
#include "I2C/i2c.h"
#include "sd/sd_spi.h"

#include "service/service.h"

void app_main(void)
{
	sd_spi_init();
	i2c_init();
	TFT_init(); // Перед этой функцией обязательно выполнить i2c_init();

	start_services();

	while (1)
	{
		lv_timer_handler();
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

//FIXME: О продукте

/* Online Font Converter https://lvgl.io/tools/fontconverter
а бвгдеёжзийклмнопрстуфхцчшщъыьэюя
АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ
abcdefghijklmnopqrstuvwxyz
ABCDEFGHIJKLMNOPQRSTUVWXYZ
0123456789
.,°
`~!@#$%^&*()_+-=
!"№;:?<>/\|

 */
