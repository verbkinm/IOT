/*
 * ota.h
 *
 *  Created on: 28 сент. 2023 г.
 *      Author: user
 */

#ifndef MAIN_OTA_OTA_H_
#define MAIN_OTA_OTA_H_

#include "esp_err.h"

esp_err_t ota_firmware(const char *url);

#endif /* MAIN_OTA_OTA_H_ */
