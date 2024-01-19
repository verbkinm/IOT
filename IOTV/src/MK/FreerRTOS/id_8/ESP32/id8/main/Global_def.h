#ifndef MAIN_GLOBAL_DEF_H_
#define MAIN_GLOBAL_DEF_H_

#include <stdint.h>

// SPI
#define	SPI_NUM_MOSI			11
#define	SPI_NUM_MISO			13
#define	SPI_NUM_SCK				12
#define	SPI_NUM_CS				10

// TCP
#define TCP_PORT                        	8888
#define KEEPALIVE_IDLE              		5
#define KEEPALIVE_INTERVAL          		5
#define KEEPALIVE_COUNT             		3
#define BUFSIZE 							2048  // по умолчанию, после компиляции, BUFSIZ = 128

// WIFI
#define WIFI_SSID							"realme9Pro"
#define WIFI_PASSWORD						"01234567"
//#define WIFI_SSID							"TP-Link_A6BE"
//#define WIFI_PASSWORD						"41706831"

// IOTV
#define LED_PIN 							GPIO_NUM_32

#define CH_COUNT		5
#define CH_CAM_DATA		0
#define CH_MIC_DATA		1
#define CH_CAM_WIDTH	2
#define CH_CAM_HEIGHT	3
#define CH_CAM_QUALITY	4

// FS PATH
#define WIFI_CONF_PATH		"/sdcard/wifi.json"

// Глобалные флаги статусов
enum STATUS_REG {
	STATUS_WIFI_STA_START = 0x0001,
	STATUS_WIFI_STA_CONNECTING = 0x0002,
	STATUS_WIFI_STA_CONNECTED = 0x0004,
	STATUS_IP_GOT = 0x0008,
	STATUS_TCP_CONNECTED = 0x0010,
	STATUS_WIFI_AUTOCONNECT = 0x0020,
	STATUS_CAMERA_START = 0x0040,
};

// Глобалные флаги ошибок
enum STATUS_ERROR {
	STATUS_SD_ERROR = 0x0001,
	STATUS_CAMERA_ERROR = 0x0002
};

#endif /* MAIN_GLOBAL_DEF_H_ */
