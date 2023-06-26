#ifndef MAIN_GLOBAL_DEF_H_
#define MAIN_GLOBAL_DEF_H_

#include <stdint.h>
//
//// TCP
//#define PORT                        		8888
//#define KEEPALIVE_IDLE              		CONFIG_EXAMPLE_KEEPALIVE_IDLE
//#define KEEPALIVE_INTERVAL          		CONFIG_EXAMPLE_KEEPALIVE_INTERVAL
//#define KEEPALIVE_COUNT             		CONFIG_EXAMPLE_KEEPALIVE_COUNT
//#define BUFSIZE 							2048  // по умолчанию, после компиляции, BUFSIZ = 128
//
//// WIFI event
//#define WIFI_CONNECTED_BIT					BIT0
//#define WIFI_DISCONNECTED_BIT 				BIT1
//#define WIFI_FAIL_BIT      					BIT2

// LED
#define LED_0_RED			GPIO_NUM_15
#define LED_0_GREEN			GPIO_NUM_2
#define LED_0_BLUE			GPIO_NUM_4

#define LED_1_RED			GPIO_NUM_16
#define LED_1_GREEN			GPIO_NUM_17
#define LED_1_BLUE			GPIO_NUM_5

#define LED_2_RED			GPIO_NUM_18
#define LED_2_GREEN			GPIO_NUM_19
#define LED_2_BLUE			GPIO_NUM_21

//// I2C
//#define I2C_MASTER_SCL_IO           		CONFIG_I2C_MASTER_SCL      /*!< GPIO number used for I2C master clock */
//#define I2C_MASTER_SDA_IO           		CONFIG_I2C_MASTER_SDA      /*!< GPIO number used for I2C master data  */
//#define I2C_MASTER_NUM              		0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
//#define I2C_MASTER_FREQ_HZ          		400000                     /*!< I2C master clock frequency */
//#define I2C_MASTER_TX_BUF_DISABLE   		0                          /*!< I2C master doesn't need buffer */
//#define I2C_MASTER_RX_BUF_DISABLE   		0                          /*!< I2C master doesn't need buffer */
//#define I2C_MASTER_TIMEOUT_MS      			1000
//
//// Адреса i2c устройств
//#define BME280_ADDR                			0x76
//#define VL6180X_ADDR                 		0x29
//#define OLED_ADDR							0x3C
//#define DS3231_ADDR							0x68
//
//// Led Signals
//#define LED_WIFI							GPIO_NUM_5
//#define LED_TCP								GPIO_NUM_4
//#define LED_ERROR							GPIO_NUM_15
//
//// IOTV
//#define RELE_PIN 							GPIO_NUM_2
//#define BORDER_DISTANCE_DEFAULT 			150
//
//// Номер ошибки
//#define I2C_INIT_FAIL						1
//#define I2C_DEINIT_FAIL						2
//#define I2C_WRITE_FAIL						3
//#define I2C_READ_FAIL						4
//#define RESULT__INTERRUPT_STATUS_GPIO_LOOP 	5
//#define RESULT__RANGE_STATUS_LOOP			6
//#define SYSTEM__FRESH_OUT_OF_RESET_LOOP		7
//#define OLED_INIT_FAIL						8
//
//// VL6180X
//#define VL6180X_ALS_GAIN_1 					0x06    ///< 1x gain
//#define VL6180X_ALS_GAIN_1_25 				0x05 	///< 1.25x gain
//#define VL6180X_ALS_GAIN_1_67 				0x04 	///< 1.67x gain
//#define VL6180X_ALS_GAIN_2_5 				0x03  	///< 2.5x gain
//#define VL6180X_ALS_GAIN_5 					0x02    ///< 5x gain
//#define VL6180X_ALS_GAIN_10 				0x01   	///< 10x gain
//#define VL6180X_ALS_GAIN_20 				0x00   	///< 20x gain
//#define VL6180X_ALS_GAIN_40 				0x07   	///< 40x gain
//
//// OLED
//#define OLED_PIN_NUM_RST           			-1
//#define OLED_LCD_H_RES              		128
//#define OLED_LCD_V_RES              		64
//#define OLED_LCD_CMD_BITS           		8
//#define OLED_LCD_PARAM_BITS         		8
//
//#define OLED_LVGL_TICK_PERIOD_MS    		2
//
//// Регистры BME280
//enum
//{
//	HUM_LSB			= 0xFE,
//	HUM_MSB			= 0xFD,
//
//	TEMP_XLSB		= 0xFC,
//	TEMP_LSB		= 0xFB,
//	TEMP_MSB		= 0xFA,
//
//	PRESS_XLSB		= 0xF9,
//	PRESS_LSB_		= 0xF8,
//	PRESS_MSB		= 0xF7,
//
//	CONFIG			= 0xF5,
//	CTRL_MEAS		= 0xF4,
//	_STATUS			= 0xF3,
//	CTRL_HUM		= 0xF2,
//
//	RESET			= 0xE0,
//	ID				= 0xD0,
//
//	/* CALIB26..CALIB41 = 0xE1..0xF0 */
//	/* CALIB00..CALIB25 = 0x88..0xA1 */
//
//	dig_T1_LSB 		= 0x88,
//	dig_T1_MSB 		= 0x89,
//	dig_T2_LSB 		= 0x8A,
//	dig_T2_MSB 		= 0x8B,
//	dig_T3_LSB 		= 0x8C,
//	dig_T3_MSB 		= 0x8D,
//
//	dig_P1_LSB 		= 0x8E,
//	dig_P1_MSB 		= 0x8F,
//	dig_P2_LSB 		= 0x90,
//	dig_P2_MSB 		= 0x91,
//	dig_P3_LSB 		= 0x92,
//	dig_P3_MSB 		= 0x93,
//	dig_P4_LSB 		= 0x94,
//	dig_P4_MSB 		= 0x95,
//	dig_P5_LSB 		= 0x96,
//	dig_P5_MSB 		= 0x97,
//	dig_P6_LSB 		= 0x98,
//	dig_P6_MSB 		= 0x99,
//	dig_P7_LSB 		= 0x9A,
//	dig_P7_MSB 		= 0x9B,
//	dig_P8_LSB 		= 0x9C,
//	dig_P8_MSB 		= 0x9D,
//	dig_P9_LSB 		= 0x9E,
//	dig_P9_MSB 		= 0x9F,
//
//	_dig_H1	 		= 0xA1,
//	dig_H2_LSB 		= 0xE1,
//	dig_H2_MSB 		= 0xE2,
//	_dig_H3	 		= 0xE3,
//
//	//	0xE4 / 0xE5[3:0] 	dig_H4 [11:4] / [3:0]
//	//	0xE5[7:4] / 0xE6	dig_H5 [3:0] / [11:4]
//	dig_H4_LSB 		= 0xE4,
//	dig_H4_MSB 		= 0xE5,
//	dig_H5_LSB 		= 0xE5,
//	dig_H5_MSB 		= 0xE6,
//	_dig_H6	 		= 0xE7,
//
//};
//
//// Регистры VL6180X
//enum
//{
//	IDENTIFICATION__MODEL_ID              = 0x000,
//	IDENTIFICATION__MODEL_REV_MAJOR       = 0x001,
//	IDENTIFICATION__MODEL_REV_MINOR       = 0x002,
//	IDENTIFICATION__MODULE_REV_MAJOR      = 0x003,
//	IDENTIFICATION__MODULE_REV_MINOR      = 0x004,
//	IDENTIFICATION__DATE_HI               = 0x006,
//	IDENTIFICATION__DATE_LO               = 0x007,
//	IDENTIFICATION__TIME                  = 0x008, // 16-bit
//
//	SYSTEM__MODE_GPIO0                    = 0x010,
//	SYSTEM__MODE_GPIO1                    = 0x011,
//	SYSTEM__HISTORY_CTRL                  = 0x012,
//	SYSTEM__INTERRUPT_CONFIG_GPIO         = 0x014,
//	SYSTEM__INTERRUPT_CLEAR               = 0x015,
//	SYSTEM__FRESH_OUT_OF_RESET            = 0x016,
//	SYSTEM__GROUPED_PARAMETER_HOLD        = 0x017,
//
//	SYSRANGE__START                       = 0x018,
//	SYSRANGE__THRESH_HIGH                 = 0x019,
//	SYSRANGE__THRESH_LOW                  = 0x01A,
//	SYSRANGE__INTERMEASUREMENT_PERIOD     = 0x01B,
//	SYSRANGE__MAX_CONVERGENCE_TIME        = 0x01C,
//	SYSRANGE__CROSSTALK_COMPENSATION_RATE = 0x01E, // 16-bit
//	SYSRANGE__CROSSTALK_VALID_HEIGHT      = 0x021,
//	SYSRANGE__EARLY_CONVERGENCE_ESTIMATE  = 0x022, // 16-bit
//	SYSRANGE__PART_TO_PART_RANGE_OFFSET   = 0x024,
//	SYSRANGE__RANGE_IGNORE_VALID_HEIGHT   = 0x025,
//	SYSRANGE__RANGE_IGNORE_THRESHOLD      = 0x026, // 16-bit
//	SYSRANGE__MAX_AMBIENT_LEVEL_MULT      = 0x02C,
//	SYSRANGE__RANGE_CHECK_ENABLES         = 0x02D,
//	SYSRANGE__VHV_RECALIBRATE             = 0x02E,
//	SYSRANGE__VHV_REPEAT_RATE             = 0x031,
//
//	SYSALS__START                         = 0x038,
//	SYSALS__THRESH_HIGH                   = 0x03A,
//	SYSALS__THRESH_LOW                    = 0x03C,
//	SYSALS__INTERMEASUREMENT_PERIOD       = 0x03E,
//	SYSALS__ANALOGUE_GAIN                 = 0x03F,
//
//	///! Integration period for ALS mode, high byte
//	SYSALS__INTEGRATION_PERIOD_H          = 0x040,
//	///! Integration period for ALS mode, low byte
//	SYSALS__INTEGRATION_PERIOD_L          = 0x041,
//
//	RESULT__RANGE_STATUS                  = 0x04D,
//	RESULT__ALS_STATUS                    = 0x04E,
//	RESULT__INTERRUPT_STATUS_GPIO         = 0x04F,
//	RESULT__ALS_VAL                       = 0x050, // 16-bit
//	RESULT__HISTORY_BUFFER_0              = 0x052, // 16-bit
//	RESULT__HISTORY_BUFFER_1              = 0x054, // 16-bit
//	RESULT__HISTORY_BUFFER_2              = 0x056, // 16-bit
//	RESULT__HISTORY_BUFFER_3              = 0x058, // 16-bit
//	RESULT__HISTORY_BUFFER_4              = 0x05A, // 16-bit
//	RESULT__HISTORY_BUFFER_5              = 0x05C, // 16-bit
//	RESULT__HISTORY_BUFFER_6              = 0x05E, // 16-bit
//	RESULT__HISTORY_BUFFER_7              = 0x060, // 16-bit
//	RESULT__RANGE_VAL                     = 0x062,
//	RESULT__RANGE_RAW                     = 0x064,
//	RESULT__RANGE_RETURN_RATE             = 0x066, // 16-bit
//	RESULT__RANGE_REFERENCE_RATE          = 0x068, // 16-bit
//	RESULT__RANGE_RETURN_SIGNAL_COUNT     = 0x06C, // 32-bit
//	RESULT__RANGE_REFERENCE_SIGNAL_COUNT  = 0x070, // 32-bit
//	RESULT__RANGE_RETURN_AMB_COUNT        = 0x074, // 32-bit
//	RESULT__RANGE_REFERENCE_AMB_COUNT     = 0x078, // 32-bit
//	RESULT__RANGE_RETURN_CONV_TIME        = 0x07C, // 32-bit
//	RESULT__RANGE_REFERENCE_CONV_TIME     = 0x080, // 32-bit
//
//	RANGE_SCALER                          = 0x096, // 16-bit - see STSW-IMG003 core/inc/vl6180x_def.h
//
//	READOUT__AVERAGING_SAMPLE_PERIOD      = 0x10A,
//	FIRMWARE__BOOTUP                      = 0x119,
//	FIRMWARE__RESULT_SCALER               = 0x120,
//	I2C_SLAVE__DEVICE_ADDRESS             = 0x212,
//	INTERLEAVED_MODE__ENABLE              = 0x2A3,
//};

// Структуры
struct DataPkg
{
	uint8_t *data;
	uint64_t size;
};

struct LedSignalPkg
{
	const char *TAG;
	int errorNumber;
};

#endif /* MAIN_GLOBAL_DEF_H_ */
