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
#define OTA_URI_MAX_LENGH  128

typedef enum {
	IOTV_WIFI_STA_NULL,
	IOTV_WIFI_STA_START_CONNECTING,
	IOTV_WIFI_STA_CONNECTED,
	IOTV_WIFI_FAIL_CONNECT
} iotv_wifi_status_t;

typedef enum {
	IOTV_OTA_NULL,
	IOTV_OTA_DOWNLOADING,
	IOTV_OTA_DOWNLOADED,
	IOTV_OTA_FAIL,
	IOTV_OTA_REBOOTING
} iotv_ota_status_t;

#endif /* MAIN_GLOBAL_DEF_H_ */
