/*
 * weather.h
 *
 *  Created on: 18 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_WEATHER_WEATHER_H_
#define MAIN_SERVICE_WEATHER_WEATHER_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_tls.h"
#include "esp_log.h"
#include "stdlib.h"

#include "Global_def.h"
#include "json/json_config.h"
#include "Local_Lib/local_lib.h"
#include "Local_Lib/local_lvgl_lib.h"

#define OPEN_METEO_WEEK_SIZE (3 * 24)

enum {
	METEO_TEMPERATURE,
	METEO_APPARENT_TEMPERATURE,
	METEO_SURFACE_PRESSURE,
	METEO_PRECIPITATION,
	METEO_RAIN,
	METEO_SHOWERS,
	METEO_SNOWFALL,
	METEO_WIND_SPEED,
	METEO_WIND_GUSTS,
	METEO_RELATIVE_HUMIDITY,
	METEO_CLOUD_COVER,
	METEO_WIND_DIRECTION,
	METEO_TIME
};

typedef struct Open_Meteo_Data {
    float temperature;				// °С
    float apparent_temperature;		// °С
    float surface_pressure;			// мм.рт.ст.

    float precipitation;			// мм
    float rain;						// мм
    float showers;					// мм
    float snowfall;					// см

    float wind_speed;				// м/с
    float wind_gusts;				// м/с

    uint8_t relative_humidity;		// %

    uint8_t cloud_cover;			// %
    uint16_t wind_direction;		// °

    time_t time;
} open_meteo_data_t;

typedef struct Open_Meteo_City {
    char *country;
    char *city_name;
    char *admin1;

    float latitude;
    float longitude;
} open_meteo_city_t;

void weather_service_task(void *pvParameters);
void service_weather_set_city(const char* city);
const char *service_weather_get_city(void);
const open_meteo_data_t *service_weather_get_current_meteo_data(void);
const open_meteo_data_t *service_weather_get_meteo_data(void);

void service_weather_get_range(open_meteo_data_t *ret_min, open_meteo_data_t *ret_max);

#endif /* MAIN_SERVICE_WEATHER_WEATHER_H_ */
