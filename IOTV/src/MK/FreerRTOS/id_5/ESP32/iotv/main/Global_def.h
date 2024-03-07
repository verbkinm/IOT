#ifndef MAIN_GLOBAL_DEF_H_
#define MAIN_GLOBAL_DEF_H_

#include <stdint.h>

// Отложенный запуск сервисов

#define DELAYED_LAUNCH						1000

// TCP
#define TCP_PORT                       		8888
#define KEEPALIVE_IDLE              		5
#define KEEPALIVE_INTERVAL          		5
#define KEEPALIVE_COUNT             		3
#define BUFSIZE 							2048  // по умолчанию, после компиляции, BUFSIZ = 128

// WIFI
//#define WIFI_SSID							"realme 9 Pro+"
//#define WIFI_PASSWORD						"y3z4pfrm"
#define WIFI_SSID							"TP-Link_A6BE"
#define WIFI_PASSWORD						"41706831"

// I2C
#define I2C_MASTER_SCL_IO           		GPIO_NUM_22				   /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           		GPIO_NUM_21				   /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              		0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          		400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   		0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   		0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS      			1000

// Адреса i2c устройств
#define BME280_ADDR                			0x76
#define VL6180X_ADDR                 		0x29
#define OLED_ADDR							0x3C
#define DS3231_ADDR							0x68

// IOTV
#define RELE_PIN 							GPIO_NUM_32
#define BORDER_DISTANCE_DEFAULT 			150
#define DISPLAY_ORIENTATION_DEFAULT			0

#define CH_RELAY_STATE 	0
#define CH_BORDER		1
#define CH_SEC			2
#define CH_MIN			3
#define	CH_HOUR			4
#define	CH_DAY			5
#define CH_DATE			6
#define CH_MONTH		7
#define CH_YEAR			8
#define CH_DISP_ORNT	9
#define CH_RANGE		10
#define CH_LUX			11
#define CH_TEMP			12
#define CH_HUM			13
#define CH_PRES			14

// OLED
#define OLED_PIN_NUM_RST           			-1
#define OLED_LCD_H_RES              		128
#define OLED_LCD_V_RES              		64
#define OLED_LCD_CMD_BITS           		8
#define OLED_LCD_PARAM_BITS         		8

#define OLED_LVGL_TICK_PERIOD_MS    		1
#define OLED_BOOT_SCREEN_TIME				5000

// Глобалные флаги статусов
enum STATUS_REG {
	STATUS_WIFI_STA_START 		= 0x0001,
	STATUS_WIFI_STA_CONNECTING 	= 0x0002,
	STATUS_WIFI_STA_CONNECTED 	= 0x0004,
	STATUS_IP_GOT 				= 0x0008,
	STATUS_WIFI_AUTOCONNECT 	= 0x0010,
	STATUS_SNTP_ON 				= 0x0020,
	STATUS_TIME_SYNC 			= 0x0040,
	STATUS_RELE 				= 0x0080,
	STATUS_TCP_CONNECTED 		= 0x0100,
};

// Глобалные флаги ошибок
enum STATUS_ERROR {
	STATUS_UPDATE_ERROR = 0x0002,
};

// Глобалные флаги обновления
enum UPDATE_FLAGS {
	UPDATE_NOW = 0x01,
	UPDATE_CHECK = 0x02,
	UPDATE_AVAILABLE = 0x04,
	UPDATE_DONE = 0x08,
};

#endif /* MAIN_GLOBAL_DEF_H_ */
