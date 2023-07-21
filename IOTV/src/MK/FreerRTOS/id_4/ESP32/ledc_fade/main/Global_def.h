#ifndef MAIN_GLOBAL_DEF_H_
#define MAIN_GLOBAL_DEF_H_

#include "driver/gpio.h"

// Приоритеты заданий
#define NET_PRIORITY						1
#define IOTV_PRIORITY						1
#define UART_PRIORITY						4
#define Led_RGB_PRIORITY					5
#define DF_PLAYER_PRIORITY					5
//#define LED_SIGNALS_PRIORITY				6
#define LOW_PROIRITY						10

// TCP
#define PORT                        		8888
#define KEEPALIVE_IDLE              		5
#define KEEPALIVE_INTERVAL          		5
#define KEEPALIVE_COUNT             		3
#define BUFSIZE 							2048  // по умолчанию, после компиляции, BUFSIZ = 128

// WIFI
#define WIFI_SSID							"TP-Link_A6BE"
#define WIFI_PASSWORD						"41706831"

// WIFI event
#define WIFI_CONNECTED_BIT					BIT0
#define WIFI_DISCONNECTED_BIT 				BIT1
#define WIFI_FAIL_BIT      					BIT2

// LED
#define LED_MAX_DUTY_VAL					8192
#define LED_MIN_DUTY_VAL					0

#define LED_0_RED							GPIO_NUM_15
#define LED_0_GREEN							GPIO_NUM_2
#define LED_0_BLUE							GPIO_NUM_4

#define LED_1_RED							GPIO_NUM_33
#define LED_1_GREEN							GPIO_NUM_32
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

#define LED_MODE_MIN						1
#define LED_MODE_MAX						6
// Led Signals
//#define LED_WIFI							GPIO_NUM_26
//#define LED_TCP								GPIO_NUM_25

// UART
#define TXD_PIN								GPIO_NUM_17
#define RXD_PIN								GPIO_NUM_16
#define UART_BUF_SIZE						256

// DF_PLAYER
#define DF_BUSY								GPIO_NUM_36
/*
  	DF_CMD
	0 - Выключить / включить воспроизведение.
	1 - Режим воспроизведения.
	2 - Выключить / включить повтор текущего режима воспроизведения.
	3 - Громкость.
	4 - Эквалайзер.
*/
#define DF_CMD_PS							0
#define DF_CMD_MODE							1
#define DF_CMD_REPEATE						2
#define DF_CMD_VOLUME						3
#define DF_CMD_EQ							4

#define DF_MODE_SIZE						3
#define DF_MODE_1							1
#define DF_MODE_2							2
#define DF_MODE_3							3

// Структуры
struct DataPkg
{
	uint8_t *data;
	uint64_t size;
};

#endif /* MAIN_GLOBAL_DEF_H_ */
