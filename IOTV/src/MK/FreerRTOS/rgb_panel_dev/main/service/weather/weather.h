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

#include "../../Global_def.h"
#include "../../json/json_config.h"
#include "../../Local_Lib/local_lib.h"

#define OPEN_METEO_WEEK_SIZE (7 * 24)

struct Open_Meteo_Data {
    time_t time;

    float temperature;
    float apparent_temperature;
    float surface_pressure;

    float precipitation;
    float rain;
    float showers;
    float snowfall;

    float wind_speed;
    float wind_gusts;

    uint8_t relative_humidity;

    uint8_t cloud_cover;
    uint16_t wind_direction;
};
typedef struct Open_Meteo_Data open_meteo_data_t;

struct Open_Meteo_City {
    char *country;
    char *city_name;
    char *admin1;

    float latitude;
    float longitude;
};
typedef struct Open_Meteo_City open_meteo_city_t;

void weather_service_task(void *pvParameters);
void service_weather_set_city(const char* city);
const char *service_weather_get_city(void);
void service_weather_parse_meteo_data(void);
const open_meteo_data_t *service_weather_get_current_meteo_data(void);
const open_meteo_data_t *service_weather_get_meteo_data(void);

#endif /* MAIN_SERVICE_WEATHER_WEATHER_H_ */
