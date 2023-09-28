/*
 * global_def.h
 *
 *  Created on: 27 сент. 2023 г.
 *      Author: user
 */

#ifndef MAIN_GLOBAL_DEF_H_
#define MAIN_GLOBAL_DEF_H_

#define WIFI_SSID "Factory_IOTV"
#define WIFI_PASS "12345678"
#define MAX_STA_CONN 4

#define WIFI_SSID_MAX_LENGH  32
#define WIFI_PASS_MAX_LENGH  64

typedef enum {
	IOTV_WIFI_STA_NULL = 0,
	IOTV_WIFI_STA_START_CONNECTING = 1,
	IOTV_WIFI_STA_CONNECTED = 2,
	IOTV_WIFI_FAIL_CONNECT = 4
} iotv_wifi_status_t;

#endif /* MAIN_GLOBAL_DEF_H_ */
