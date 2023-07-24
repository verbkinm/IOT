/*
 * button.h
 *
 *  Created on: 24 июл. 2023 г.
 *      Author: user
 */

#ifndef MAIN_BUTTON_H_
#define MAIN_BUTTON_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "Global_def.h"
#include "Local_Lib/local_lib.h"
#include "iotv.h"

void Button_Task(void *pvParameters);

#endif /* MAIN_BUTTON_H_ */
