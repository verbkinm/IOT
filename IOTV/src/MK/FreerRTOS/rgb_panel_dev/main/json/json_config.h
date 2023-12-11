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

#include "../Local_Lib/cJSON.h"

bool set_wifi_config_value(const char *key, const char *value);
char *get_wifi_config_value(const char *monitor, const char *key);

#endif /* MAIN_JSON_JSON_CONFIG_H_ */
