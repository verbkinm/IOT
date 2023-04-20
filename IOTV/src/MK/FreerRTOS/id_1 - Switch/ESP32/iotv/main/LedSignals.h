/*
 * errorBlink.h
 *
 *  Created on: 18 апр. 2023 г.
 *      Author: user
 */

#ifndef MAIN_LEDSIGNALS_H_
#define MAIN_LEDSIGNALS_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_WIFI							GPIO_NUM_1
#define LED_TCP								GPIO_NUM_2
#define LED_ERROR							GPIO_NUM_2

// Номер ошибки
#define I2C_INIT_FAIL						1
#define I2C_DEINIT_FAIL						2
#define I2C_WRITE_FAIL						3
#define I2C_READ_FAIL						4
#define RESULT__INTERRUPT_STATUS_GPIO_LOOP 	5
#define RESULT__RANGE_STATUS_LOOP			6
#define SYSTEM__FRESH_OUT_OF_RESET_LOOP		7

#define WIFI_CONNECT						1
#define	TCP_CONNECT							2

struct LedSignalPkg {
	const char *TAG;
	int errorNumber;
};

void LedSignals_Task(void *pvParameters);
//void ledSignal(gpio_num_t gpio, bool sate);

#endif /* MAIN_LEDSIGNALS_H_ */
