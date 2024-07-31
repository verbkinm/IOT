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
#define WIFI_SSID							"realme9Pro"
#define WIFI_PASSWORD						"01234567"
//#define WIFI_SSID							"TP-Link_A6BE"
//#define WIFI_PASSWORD						"41706831"

// I2C
#define I2C_MASTER_SCL_IO           		GPIO_NUM_22				   /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           		GPIO_NUM_21				   /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              		0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          		400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   		0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   		0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS      			1000

// Адреса i2c устройств

// IOTV
#define IOTV_DEVICE_NAME					"ID_1"

#define RELE_0_PIN 							GPIO_NUM_2 //s25
#define RELE_1_PIN 							GPIO_NUM_32
#define RELE_2_PIN 							GPIO_NUM_33

#define BUTTON_0_PIN 						GPIO_NUM_12
#define BUTTON_1_PIN 						GPIO_NUM_14
#define BUTTON_2_PIN 						GPIO_NUM_27

// Глобалные флаги статусов
enum STATUS_REG {
	STATUS_WIFI_STA_START 		= 0x0001,
	STATUS_WIFI_STA_CONNECTING 	= 0x0002,
	STATUS_WIFI_STA_CONNECTED 	= 0x0004,
	STATUS_IP_GOT 				= 0x0008,
	STATUS_WIFI_AUTOCONNECT 	= 0x0010,
	STATUS_TCP_CONNECTED 		= 0x0020,
};

// Глобалные флаги ошибок
enum STATUS_ERROR {
	STATUS_UPDATE_ERROR 		= 0x0002,
};

// Глобалные флаги обновления
enum UPDATE_FLAGS {
	UPDATE_NOW 					= 0x01,
	UPDATE_CHECK 				= 0x02,
	UPDATE_AVAILABLE 			= 0x04,
	UPDATE_DONE 				= 0x08,
};

#endif /* MAIN_GLOBAL_DEF_H_ */
