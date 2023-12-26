/*
 * service.h
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_SERVICE_H_
#define MAIN_SERVICE_SERVICE_H_

#include "wifi/wifi.h"
#include "sntp/sntp.h"
#include "weather/weather.h"
#include "display/display.h"

void rgb_panel_start_services(void);

#endif /* MAIN_SERVICE_SERVICE_H_ */
