/*
 * ota.h
 *
 *  Created on: 28 сент. 2023 г.
 *      Author: user
 */

#ifndef MAIN_OTA_OTA_H_
#define MAIN_OTA_OTA_H_

#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_crt_bundle.h"

esp_err_t backtofactory(void);
esp_err_t ota_firmware(const char *url);

#endif /* MAIN_OTA_OTA_H_ */
