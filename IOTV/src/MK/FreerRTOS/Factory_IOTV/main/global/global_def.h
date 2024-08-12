#ifndef MAIN_GLOBAL_DEF_H_
#define MAIN_GLOBAL_DEF_H_

#include <stdint.h>

// Сборка
//#define DEBUG								1

// Отложенный запуск сервисов
#define DELAYED_LAUNCH						1000

// Глобальный размер буфера
#define BUFSIZE 							2048  // по умолчанию, после компиляции, BUFSIZ = 128

// WIFI
#define WIFI_SSID_AT						"ATS"

#define AP_INFO_ARR_SIZE 					10
#define WIFI_SSID_MAX_LENGH					32
#define WIFI_PASS_MAX_LENGH					32

// NVS namespace
#define NVS_NAMESPACE_WIFI					"WIFI"
#define NVS_NAMESPACE_UPDATE				"UPDATE"

// NVS keys
#define NVS_KEY_WIFI_STA_SSID				"sta_ssid"
#define NVS_KEY_WIFI_STA_PWD				"sta_pwd"
#define NVS_KEY_UPDATE_FLAG					"update_flag"
#define NVS_KEY_UPDATE_URL					"update_url"

// NVS value
#define NVS_VALUE_UPDATE_YES				1
#define NVS_VALUE_UPDATE_NO					0

// Глобалные флаги статусов
typedef enum STATUS_REG {
	STATUS_WIFI_STA_START 		= 0x0001,
	STATUS_WIFI_STA_CONNECTING 	= 0x0002,
	STATUS_WIFI_STA_CONNECTED 	= 0x0004,
	STATUS_WIFI_AP_START 		= 0x0008,				// была поднята wifi станция
	STATUS_WIFI_SCANNING 		= 0x0010,
	STATUS_WIFI_SCAN_DONE 		= 0x0020,
	STATUS_IP_GOT 				= 0x0040,
	STATUS_WIFI_AUTOCONNECT 	= 0x0080,
	STATUS_TCP_CONNECTED 		= 0x0100,
	STATUS_TIME_SYNC 			= 0x0400,
} status_reg_flag_t;

// Глобалные флаги ошибок
typedef enum STATUS_ERROR {
	STATUS_ERROR_SD				= 0x0001,
	STATUS_ERROR_UPDATE 		= 0x0002,
	STATUS_ERROR_BREAK_ALL_SERVICE	= 0x0004,
} status_error_flag_t;

// Глобалные флаги обновления
typedef enum UPDATE_FLAGS {
	UPDATE_NOW 					= 0x01,		// Происходит обновление по https
	UPDATE_CHECK 				= 0x02,
	UPDATE_AVAILABLE 			= 0x04,
	UPDATE_DONE 				= 0x08,
	UPDATE_MESSAGE 				= 0x10,
	UPDATE_MESSAGE_NOW 			= 0x20,
	UPDATE_SD_NOW 				= 0x40,		// Происходит обновление c sd
	UPDATE_CHECK_ERROR 			= 0x80,		// Ошибка файла с информацией об обновлениях
} update_flag_t;

// Глобалные флаги служб
typedef enum SERVICE_FLAG {
	SERVICE_SNTP_ON				= 0x0001,
	SERVICE_WIFI_ON				= 0x0002,
	SERVICE_UPDATE_ON			= 0x0004,
	SERVICE_TCP_SERVER_ON		= 0x0008,
	SERVICE_TCP_CLIENT_ON		= 0x0010,
	SERVICE_UDP_SERVER_ON		= 0x0020,
	SERVICE_UDP_CLIENT_ON		= 0x0040,
	SERVICE_DISPLAY_ON			= 0x0080,
	SERVICE_AUDIO_ON			= 0x0100,
	SERVICE_RELAY_ON			= 0x0200,
	SERVICE_BUTTON_ON			= 0x0400,
	SERVICE_I2C_ON				= 0x0800,
	SERVICE_SPI_ON				= 0x1000,
	SERVICE_FACTORY_MODE_ON		= 0x2000,
} service_flag_t;

#endif /* MAIN_GLOBAL_DEF_H_ */
