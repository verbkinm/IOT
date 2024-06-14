#ifndef MAIN_GLOBAL_DEF_H_
#define MAIN_GLOBAL_DEF_H_

#include <stdint.h>

// Отложенный запуск сервисов

#define DELAYED_LAUNCH						0

// UDP
#define UDP_SERVER_ADDR						"192.168.40.216"
#define UDP_SERVER_PORT                     8888
#define BUFSIZE 							2048  // по умолчанию, после компиляции, BUFSIZ = 128

// WIFI
#define WIFI_SSID							"realme9Pro"
#define WIFI_PASSWORD						"01234567"
//#define WIFI_SSID							"TP-Link_A6BE"
//#define WIFI_PASSWORD						"41706831"

// IOTV
#define CH_TEMPERATURE						0
#define CH_HUMIDITY							1
#define CH_PRESSURE							2
#define CH_ACCUM							3

// Глобалные флаги статусов
enum STATUS_REG {
	STATUS_WIFI_STA_START 		= 0x0001,
	STATUS_WIFI_STA_CONNECTING 	= 0x0002,
	STATUS_WIFI_STA_CONNECTED 	= 0x0004,
	STATUS_IP_GOT 				= 0x0008,
	STATUS_WIFI_AUTOCONNECT 	= 0x0010,
};

#endif /* MAIN_GLOBAL_DEF_H_ */
