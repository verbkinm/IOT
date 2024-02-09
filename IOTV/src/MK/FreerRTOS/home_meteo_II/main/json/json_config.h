/*
 * json_config.h
 *
 *  Created on: 11 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_JSON_JSON_CONFIG_H_
#define MAIN_JSON_JSON_CONFIG_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "esp_log.h"
#include "Global_def.h"
#include "cJSON.h"

bool set_wifi_config_value(const char *key, const char *value);
bool get_wifi_config_value(const char *key, char **value);

bool set_sntp_config_value(const char *key, const char *value);
bool get_sntp_config_value(const char *key, char **value);

bool set_meteo_config_value(const char *key, const char *value);
bool get_meteo_config_value(const char *key, char **value);

bool set_display_config_value(const char *key, const char *value);
bool get_display_config_value(const char *key, char **value);

bool set_meteo_chart_config_value(const char *key, const char *value);
bool get_meteo_chart_config_value(const char *key, char **value);

bool set_bme280_config_value(const char *key, const char *value);
bool get_bme280_config_value(const char *key, char **value);

bool set_update_config_value(const char *key, const char *value);
bool get_update_config_value(const char *key, char **value);

#endif /* MAIN_JSON_JSON_CONFIG_H_ */
