/*
 * bme280.h
 *
 *  Created on: 26 янв. 2024 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_BME280_BME280_H_
#define MAIN_SERVICE_BME280_BME280_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Global_def.h"
#include "Local_Lib/local_lvgl_lib.h"
#include "I2C/BME280.h"

void BME280_service_task(void *pvParameters);
const struct THP *BME280_service_get_value(void);

#endif /* MAIN_SERVICE_BME280_BME280_H_ */
