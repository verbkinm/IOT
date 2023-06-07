#include "LedSignals.h"

extern QueueHandle_t xQueueLedSignals;

static const char *TAG = "LedSignals";

static const char *failStr[] = {
		"I2C_INIT_FAIL",
		"I2C_DEINIT_FAIL",
		"I2C_WRITE_FAIL",
		"I2C_READ_FAIL",
		"RESULT__INTERRUPT_STATUS_GPIO_LOOP",
		"RESULT__RANGE_STATUS_LOOP",
		"SYSTEM__FRESH_OUT_OF_RESET_LOOP",
		"OLED_INIT_FAIL"
};

static SemaphoreHandle_t xSemaphore = NULL;

static void errorBlink(const char* errorTag, uint8_t value);
static void LedGPIO_State(gpio_port_t gpio_num, bool state);

void LedSignals_Task(void *pvParameters)
{
	ESP_LOGW(TAG, "LedSignals task created");

	gpio_reset_pin(LED_WIFI);
	gpio_reset_pin(LED_TCP);
	gpio_reset_pin(LED_ERROR);

	gpio_set_direction(LED_WIFI, GPIO_MODE_INPUT_OUTPUT);
	gpio_set_direction(LED_TCP, GPIO_MODE_INPUT_OUTPUT);
	gpio_set_direction(LED_ERROR, GPIO_MODE_INPUT_OUTPUT);

	xSemaphore = xSemaphoreCreateMutex();
	struct LedSignalPkg pkg = {NULL, 0};

	while (true)
	{
		xQueueReceive(xQueueLedSignals, (void *)&pkg, portMAX_DELAY);
		errorBlink(pkg.TAG, pkg.errorNumber);
	}
}

static void errorBlink(const char* errorTag, uint8_t value)
{
	if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE)
	{
		gpio_set_direction(LED_ERROR, GPIO_MODE_INPUT_OUTPUT);

		for (uint8_t counter = 0; counter < value; ++counter)
		{
			gpio_set_level(LED_ERROR, 1);
			vTaskDelay(100 / portTICK_PERIOD_MS);
			gpio_set_level(LED_ERROR, 0);
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}
		vTaskDelay(500 / portTICK_PERIOD_MS);

		if ((value > 0) && (value < OLED_INIT_FAIL)) // OLED_INIT_FAIL - крайний элемент
			ESP_LOGE(TAG, "%s FAIL: %s", errorTag, failStr[value - 1]);

		xSemaphoreGive( xSemaphore );
	}
}

static void LedGPIO_State(gpio_port_t gpio_num, bool state)
{
	gpio_set_level(gpio_num, state);
}

void LedSignals_setWifiState(bool state)
{
	ESP_LOGW(TAG, "wifi state: %d", state);
	LedGPIO_State(LED_WIFI, state);
}

void LedSignals_setTCPState(bool state)
{
	ESP_LOGW(TAG, "tcp session state: %d", state);
	LedGPIO_State(LED_TCP, state);
}
