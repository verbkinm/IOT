#ifndef MAIN_GLOBAL_DEF_H_
#define MAIN_GLOBAL_DEF_H_

#include <stdint.h>

// TCP
#define PORT                        		8888
#define KEEPALIVE_IDLE              		5
#define KEEPALIVE_INTERVAL          		5
#define KEEPALIVE_COUNT             		3
#define BUFSIZE 							2048  // по умолчанию, после компиляции, BUFSIZ = 128

// WIFI event
#define WIFI_CONNECTED_BIT					BIT0
#define WIFI_DISCONNECTED_BIT 				BIT1
#define WIFI_FAIL_BIT      					BIT2

// WIFI
//#define WIFI_SSID							"realme 9 Pro+"
//#define WIFI_PASSWORD						"y3z4pfrm"
#define WIFI_SSID							"TP-Link_A6BE"
#define WIFI_PASSWORD						"41706831"

// I2C
#define I2C_MASTER_SCL_IO           		GPIO_NUM_20				   /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           		GPIO_NUM_19				   /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              		0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          		400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   		0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   		0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS      			1000

// Адреса i2c устройств
#define BME280_ADDR                			0x76
#define DS3231_ADDR							0x68

// SPI
#define	SPI_NUM_MOSI			11
#define	SPI_NUM_MISO			13
#define	SPI_NUM_SCK				12
#define	SPI_NUM_CS				10

// TFT
#define LCD_PIXEL_CLOCK_HZ     (18 * 1000 * 1000)
#define LCD_BK_LIGHT_ON_LEVEL  1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL
#define PIN_NUM_BK_LIGHT       2
#define PIN_NUM_HSYNC          39
#define PIN_NUM_VSYNC          41
#define PIN_NUM_DE             40
#define PIN_NUM_PCLK           42
#define PIN_NUM_DATA0          8  // B0
#define PIN_NUM_DATA1          3  // B1
#define PIN_NUM_DATA2          46 // B2
#define PIN_NUM_DATA3          9  // B3
#define PIN_NUM_DATA4          1  // B4
#define PIN_NUM_DATA5          5  // G0
#define PIN_NUM_DATA6          6  // G1
#define PIN_NUM_DATA7          7  // G2
#define PIN_NUM_DATA8          15 // G3
#define PIN_NUM_DATA9          16 // G4
#define PIN_NUM_DATA10         4  // G5
#define PIN_NUM_DATA11         45 // R0
#define PIN_NUM_DATA12         48 // R1
#define PIN_NUM_DATA13         47 // R2
#define PIN_NUM_DATA14         21 // R3
#define PIN_NUM_DATA15         14 // R4
#define PIN_NUM_DISP_EN        -1

// The pixel number in horizontal and vertical
#define LCD_H_RES              800
#define LCD_V_RES              480

#define LCD_NUM_FB             1

#define LVGL_TICK_PERIOD_MS    2
#define LCD_PANEL_STATUS_H	   30

// FS PATH
#define	SD_ON				"A:/sdcard/panel/sd_on.png"
#define	SD_OFF				"A:/sdcard/panel/sd_off.png"
#define	WIFI_CONNECTED		"A:/sdcard/panel/wifi_con.png"
#define	WIFI_CONNECTING		"A:/sdcard/panel/wifi_ing.png"
#define	WIFI_DISABLE		"A:/sdcard/panel/wifi_off.png"
#define	WIFI_ENABLE			"A:/sdcard/panel/wifi_on.png"

// Регистры BME280
enum
{
	HUM_LSB			= 0xFE,
	HUM_MSB			= 0xFD,

	TEMP_XLSB		= 0xFC,
	TEMP_LSB		= 0xFB,
	TEMP_MSB		= 0xFA,

	PRESS_XLSB		= 0xF9,
	PRESS_LSB_		= 0xF8,
	PRESS_MSB		= 0xF7,

	CONFIG			= 0xF5,
	CTRL_MEAS		= 0xF4,
	_STATUS			= 0xF3,
	CTRL_HUM		= 0xF2,

	RESET			= 0xE0,
	ID				= 0xD0,

	/* CALIB26..CALIB41 = 0xE1..0xF0 */
	/* CALIB00..CALIB25 = 0x88..0xA1 */

	dig_T1_LSB 		= 0x88,
	dig_T1_MSB 		= 0x89,
	dig_T2_LSB 		= 0x8A,
	dig_T2_MSB 		= 0x8B,
	dig_T3_LSB 		= 0x8C,
	dig_T3_MSB 		= 0x8D,

	dig_P1_LSB 		= 0x8E,
	dig_P1_MSB 		= 0x8F,
	dig_P2_LSB 		= 0x90,
	dig_P2_MSB 		= 0x91,
	dig_P3_LSB 		= 0x92,
	dig_P3_MSB 		= 0x93,
	dig_P4_LSB 		= 0x94,
	dig_P4_MSB 		= 0x95,
	dig_P5_LSB 		= 0x96,
	dig_P5_MSB 		= 0x97,
	dig_P6_LSB 		= 0x98,
	dig_P6_MSB 		= 0x99,
	dig_P7_LSB 		= 0x9A,
	dig_P7_MSB 		= 0x9B,
	dig_P8_LSB 		= 0x9C,
	dig_P8_MSB 		= 0x9D,
	dig_P9_LSB 		= 0x9E,
	dig_P9_MSB 		= 0x9F,

	_dig_H1	 		= 0xA1,
	dig_H2_LSB 		= 0xE1,
	dig_H2_MSB 		= 0xE2,
	_dig_H3	 		= 0xE3,

	//	0xE4 / 0xE5[3:0] 	dig_H4 [11:4] / [3:0]
	//	0xE5[7:4] / 0xE6	dig_H5 [3:0] / [11:4]
	dig_H4_LSB 		= 0xE4,
	dig_H4_MSB 		= 0xE5,
	dig_H5_LSB 		= 0xE5,
	dig_H5_MSB 		= 0xE6,
	_dig_H6	 		= 0xE7,
};

// Биты статусов
enum
{
	MY_STATUS_WIFI = 1,
	MY_STATUS_TCP = 2,
	MY_STATUS_RELE = 4
};

// Страницы дисплея
enum PAGE_NAME {
	PAGE_NONE,
    PAGE_HOME,
    PAGE_MENU,
    PAGE_CHARTS,
    PAGE_SETTINGS
};
// Глобалные флаги статусов
enum STATUS_REG {
	STATUS_WIFI_STA_START = 0x01,
	STATUS_WIFI_STA_CONNECTING = 0x02,
	STATUS_WIFI_STA_CONNECTED = 0x04,
	STATUS_IP_GOT = 0x08,
	STATUS_WIFI_SCANNING = 0x10,
	STATUS_WIFI_SCAN_DONE = 0x20,
	STATUS_WIFI_AUTOCONNECT = 0x40,
};

#endif /* MAIN_GLOBAL_DEF_H_ */
