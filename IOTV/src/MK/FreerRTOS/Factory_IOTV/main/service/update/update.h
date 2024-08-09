/*
 * factory_reset.h
 *
 *  Created on: 23 янв. 2024 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_UPDATE_UPDATE_H_
#define MAIN_SERVICE_UPDATE_UPDATE_H_

#include "esp_err.h"

void update_service_task(void *pvParameters);
const char *update_service_task_name(void);
char *update_service_get_available_version(void);

esp_err_t update_service_write_firmware(const char *data, size_t dataSize, size_t pkgNumber, size_t pkgTotal);

#endif /* MAIN_SERVICE_UPDATE_UPDATE_H_ */
