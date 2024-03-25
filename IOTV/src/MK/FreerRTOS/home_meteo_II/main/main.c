#include "TFT_touchscreen/TFT_touch_screen.h"
#include "I2C/i2c.h"
#include "sd/sd_spi.h"

#include "GUI_manager/GUI_manager.h"

void app_main(void)
{
	sd_spi_init();

	i2c_init();
	DS3231_set_system_time();

	TFT_init(); // Перед этой функцией обязательно выполнить i2c_init();

	start_services();

	GUI_manager_init();

	while (1)
	{
		lv_timer_handler();
		vTaskDelay(5 / portTICK_PERIOD_MS);
	}
}

//ИСПРАВЛЕНО FIXME: При ошибках считывания BME280 графический интерфейс в настройках "Датчики" сильно тормозит!
//FIXME: О продукте
//FIXME: Везде, где проверяется strlen необходимо с начала проверить, что указатель на строку не равен NULL
//FIXME: Названия показателей погоды объеденить между homePage и meteoChartPage. Использовать replace_char для замены пробелов на символ переноса строки

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
