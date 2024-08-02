/*
 * nvs.h
 *
 *  Created on: 7 мар. 2024 г.
 *      Author: user
 */

#ifndef MAIN_NVS_LOCAL_NVS_H_
#define MAIN_NVS_LOCAL_NVS_H_

#include <nvs.h>
#include <esp_log.h>
#include <esp_err.h>
#include <stdint.h>

#include "global/global_def.h"

esp_err_t nvs_read_update_flag(uint8_t *out);
esp_err_t nvs_read_update_url(char *out);

esp_err_t nvs_read_wifi_sta_ssid(char *out);
esp_err_t nvs_read_wifi_sta_pwd(char *out);

esp_err_t nvs_write_update_flag(uint8_t value);
esp_err_t nvs_write_update_url(const char *value);

esp_err_t nvs_write_wifi_sta_ssid(const char *value);
esp_err_t nvs_write_wifi_sta_pwd(const char *value);

#endif /* MAIN_NVS_LOCAL_NVS_H_ */
