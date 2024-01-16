#ifndef MAIN_GLOBAL_DEF_H_
#define MAIN_GLOBAL_DEF_H_

// TCP
#define PORT                        		8888
#define KEEPALIVE_IDLE              		5
#define KEEPALIVE_INTERVAL          		5
#define KEEPALIVE_COUNT             		3
#define BUFSIZE 							2048  // по умолчанию, после компиляции, BUFSIZ = 128

// WIFI event
//#define WIFI_CONNECTED_BIT					BIT0
//#define WIFI_DISCONNECTED_BIT 				BIT1
//#define WIFI_FAIL_BIT      					BIT2

// WIFI
//#define WIFI_SSID							"realme 9 Pro+"
//#define WIFI_PASSWORD						"y3z4pfrm"
//#define WIFI_SSID							"TP-Link_A6BE"
//#define WIFI_PASSWORD						"41706831"

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

// Для регулировки яркости дисплея
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          PIN_NUM_BK_LIGHT
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_8_BIT
#define LEDC_DUTY               (64) // Set duty to 50%. ((2 ** 8) - 1) * 50%
#define LEDC_FREQUENCY          (120) // Frequency in Hertz.

// FS PATH
#define	SD_ON				"A:/sdcard/panel/sd_on.png"
#define	SD_OFF				"A:/sdcard/panel/sd_off.png"
#define	WIFI_CONNECTED		"A:/sdcard/panel/wifi_con.png"
#define	WIFI_CONNECTING		"A:/sdcard/panel/wifi_ing.png"
#define	WIFI_DISABLE		"A:/sdcard/panel/wifi_off.png"
#define	WIFI_ENABLE			"A:/sdcard/panel/wifi_on.png"

#define TEMPERATURE			"A:/sdcard/img/t.png"
#define HUMIDITY			"A:/sdcard/img/h.png"
#define PRESSURE			"A:/sdcard/img/p.png"
#define TEMPERATURE32		"A:/sdcard/img/t32.png"
#define HUMIDITY32			"A:/sdcard/img/h32.png"
#define PRESSURE32			"A:/sdcard/img/p32.png"

#define CLOUD_COVER_1		"A:/sdcard/img/c_1.png"
#define CLOUD_COVER_2		"A:/sdcard/img/c_2.png"
#define CLOUD_COVER_3		"A:/sdcard/img/c_3.png"
#define CLOUD_COVER_4		"A:/sdcard/img/c_4.png"

#define RAIN_1				"A:/sdcard/img/r_1.png"
#define RAIN_2				"A:/sdcard/img/r_2.png"
#define RAIN_3				"A:/sdcard/img/r_3.png"
#define RAIN_4				"A:/sdcard/img/r_4.png"

#define SNOW_1				"A:/sdcard/img/s_1.png"
#define SNOW_2				"A:/sdcard/img/s_2.png"
#define SNOW_3				"A:/sdcard/img/s_3.png"
#define SNOW_4				"A:/sdcard/img/s_4.png"

#define SNOW_AND_RAIN		"A:/sdcard/img/sr.png"
#define PRECIPITATION0		"A:/sdcard/img/sr0.png"

#define WIND				"A:/sdcard/img/w.png"
#define WIND_NO				"A:/sdcard/img/w_no.png"
#define WIND_0				"A:/sdcard/img/w0.png"
#define WIND_45				"A:/sdcard/img/w45.png"
#define WIND_90				"A:/sdcard/img/w90.png"
#define WIND_135			"A:/sdcard/img/w135.png"
#define WIND_180			"A:/sdcard/img/w180.png"
#define WIND_225			"A:/sdcard/img/w225.png"
#define WIND_270			"A:/sdcard/img/w270.png"
#define WIND_315			"A:/sdcard/img/w315.png"

#define WIFI_CONF_PATH		"/sdcard/wifi.json"
#define SNTP_CONF_PATH		"/sdcard/sntp.json"
#define DISPLAY_PATH		"/sdcard/disp.json"
#define METEO_CONF_PATH		"/sdcard/meteo.json"
#define METEO_CHART_PATH	"/sdcard/meteo_chart.json"
#define METEO_CITY_PATH		"/sdcard/city.json"
#define METEO_WEEK_PATH		"/sdcard/week.json"

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
    PAGE_METEO_CHARTS,
    PAGE_SETTINGS,
	PAGE_DATETIME_1
};

// Глобалные флаги статусов
enum STATUS_REG {
	STATUS_WIFI_STA_START = 0x0001,
	STATUS_WIFI_STA_CONNECTING = 0x0002,
	STATUS_WIFI_STA_CONNECTED = 0x0004,
	STATUS_IP_GOT = 0x0008,
	STATUS_WIFI_SCANNING = 0x0010,
	STATUS_WIFI_SCAN_DONE = 0x0020,
	STATUS_WIFI_AUTOCONNECT = 0x0040,
	STATUS_SNTP_ON = 0x0080,
	STATUS_METEO_ON = 0x0100,
	STATUS_METEO_CITY_SEARCH = 0x0200,
	STATUS_METEO_CITY_SEARCH_DONE = 0x0400,
	STATUS_DISPLAY_NIGHT_MODE_ON = 0x0800,
	STATUS_TIME_SYNC = 0x1000
};

// Глобалные флаги ошибок
enum STATUS_ERROR {
	STATUS_SD_ERROR = 0x0001,
};

// json config word
#define DISPLAY_STR				"display"
#define ROTATE_STR 				"rotate"
#define BRIGHTNESS_STR			"brightness"
#define NIGHT_MODE_STR			"night_mode"
#define BRIGHTNESS_DAY_STR		"b_day"
#define BRIGHTNESS_NIGHT_STR	"b_night"
#define DAY_BEGIN_STR			"d_begin"
#define NIGHT_BEGIN_STR			"n_begin"

#endif /* MAIN_GLOBAL_DEF_H_ */
