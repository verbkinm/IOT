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

// LED
#define LED_0_RED							GPIO_NUM_15
#define LED_0_GREEN							GPIO_NUM_2
#define LED_0_BLUE							GPIO_NUM_4

#define LED_1_RED							GPIO_NUM_16
#define LED_1_GREEN							GPIO_NUM_17
#define LED_1_BLUE							GPIO_NUM_5

#define LED_2_RED							GPIO_NUM_18
#define LED_2_GREEN							GPIO_NUM_19
#define LED_2_BLUE							GPIO_NUM_21

#define LED_3_RED							GPIO_NUM_22
#define LED_3_GREEN							GPIO_NUM_23
#define LED_3_BLUE							GPIO_NUM_13

#define LED_4_RED							GPIO_NUM_12
#define LED_4_GREEN							GPIO_NUM_14
#define LED_4_BLUE							GPIO_NUM_27

// Led Signals
#define LED_WIFI							GPIO_NUM_26
#define LED_TCP								GPIO_NUM_25
#define LED_ERROR							GPIO_NUM_33

// IOTV
#define RELE_PIN 							GPIO_NUM_2
#define BORDER_DISTANCE_DEFAULT 			150

// Номер ошибки
#define I2C_INIT_FAIL						1
#define I2C_DEINIT_FAIL						2
#define I2C_WRITE_FAIL						3
#define I2C_READ_FAIL						4
#define RESULT__INTERRUPT_STATUS_GPIO_LOOP 	5
#define RESULT__RANGE_STATUS_LOOP			6
#define SYSTEM__FRESH_OUT_OF_RESET_LOOP		7
#define OLED_INIT_FAIL						8

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
