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

struct Open_Meteo_City {
    char *country;
    char *city_name;
    char *admin1;

    float latitude;
    float longitude;
};
typedef struct Open_Meteo_City open_meteo_city_t;

struct Open_Meteo_Data {
    char *city_name;

    time_t time;

    float latitude;
    float longitude;

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

void weather_service_task(void *pvParameters);
void http_meteo_get(void);

#endif /* MAIN_SERVICE_WEATHER_WEATHER_H_ */
