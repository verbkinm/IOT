/*
 * camera.h
 *
 *  Created on: 17 янв. 2024 г.
 *      Author: user
 */

#ifndef MAIN_CAMERA_CAMERA_H_
#define MAIN_CAMERA_CAMERA_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_camera.h"

#include "Global_def.h"
#include "Local_Lib/local_lib.h"
#include "Protocols/embedded/IOTV_SH.h"

esp_err_t camera_init(void);
void camera_start(void);
void camera_stop(void);

#endif /* MAIN_CAMERA_CAMERA_H_ */
