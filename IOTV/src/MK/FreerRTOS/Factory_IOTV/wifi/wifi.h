/*
 * wifi.h
 *
 *  Created on: 27 сент. 2023 г.
 *      Author: user
 */

#ifndef MAIN_WIFI_WIFI_H_
#define MAIN_WIFI_WIFI_H_

#include "global_def.h"

void wifi_init(void);
//void wifi_init_sta(void);
void wifi_disconnect_sta(void);
void wifi_sta_connect(const char *ssid, const char *pass);

#endif /* MAIN_WIFI_WIFI_H_ */
