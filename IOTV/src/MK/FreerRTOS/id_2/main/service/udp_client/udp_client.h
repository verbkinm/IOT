/*
 * udp_client.h
 *
 *  Created on: 8 мая 2024 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_UDP_CLIENT_UDP_CLIENT_H_
#define MAIN_SERVICE_UDP_CLIENT_UDP_CLIENT_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Global_def.h"
#include "Local_Lib/local_lib.h"

void udp_client_service_task(void *pvParameters);

#endif /* MAIN_SERVICE_UDP_CLIENT_UDP_CLIENT_H_ */
