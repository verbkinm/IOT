/*
 * iotv.cpp
 *
 *  Created on: 13 апр. 2023 г.
 *      Author: user
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include <stdint.h>

#include "Protocols/embedded/creatorpkgs.h"
#include "Protocols/embedded/IOTV_SH.h"
#include "Protocols/embedded/iotv_server_embedded.h"

#include "iotv.h"
#include "VL6180X_Simple.h"

#define RELE_PIN GPIO_NUM_2

extern QueueHandle_t xQueueInData, xQueueOutData;

static const char *TAG = "iotv";

static uint8_t recivedBuffer[BUFSIZE], transmitBuffer[BUFSIZE];
static uint64_t realBufSize = 0;

static uint64_t expextedDataSize = 20;
static uint64_t cutDataSize = 0;
static bool error = false;

static uint8_t readType[1] = {DATA_TYPE_BOOL};

static struct IOTV_Server_embedded iot = {
		.id = 1,
		.name = "vl6180",
		.description = "ESP-01 + vl6180 + rele",
		.numberReadChannel = 1,
		.readChannel = NULL,
		.readChannelType = readType,
		.numberWriteChannel = 1,
		.writeChannelType = readType,
		.state = 1,
		.nameSize = 6,
		.descriptionSize = 22,
};

static void dataRecived(const struct DataPkg *pkg);

void iotvTask(void *pvParameters)
{
	ESP_LOGW(TAG, "iotv task created");

	// Выделения памяти для iot структуры
	iot.readChannel = (struct RawEmbedded *)malloc(sizeof(struct RawEmbedded) * iot.numberReadChannel);

	for (uint8_t i = 0; i < iot.numberReadChannel; ++i)
	{
		iot.readChannel[i].dataSize = dataSizeonDataType(readType[i]);
		iot.readChannel[i].data = (char *)calloc(1, iot.readChannel[i].dataSize);
	}

	struct DataPkg pkg = { NULL, 0 };

	while (true)
	{
		if (xQueueReceive( xQueueInData, (void *)&pkg, portMAX_DELAY) == pdPASS )
		{
			dataRecived(&pkg);
			free(pkg.data);
		}
	}
}

static void dataRecived(const struct DataPkg *pkg)
{
	//страховка
	if ((realBufSize + pkg->size) >= BUFSIZE)
	{
		realBufSize = 0;
		expextedDataSize = 0;
		cutDataSize = 0;
		return;
	}

	memcpy(&recivedBuffer[realBufSize], pkg->data, pkg->size);
	realBufSize += pkg->size;

	if (realBufSize < expextedDataSize)
		return;

	while (realBufSize > 0)
	{
		struct DataPkg pkg = { NULL, 0 };
		struct Header *header = createPkgs(recivedBuffer, realBufSize, &error, &expextedDataSize, &cutDataSize);

		if (error == true)
		{
			realBufSize = 0;
			expextedDataSize = 0;
			cutDataSize = 0;
			break;
		}

		if (expextedDataSize > 0)
			break;

		if (header->type == HEADER_TYPE_REQUEST)
		{
			if (header->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
			{
				pkg.size = responseIdentificationData((char *)transmitBuffer, BUFSIZE, &iot);
				pkg.data = malloc(pkg.size);
				memcpy(pkg.data, transmitBuffer, pkg.size);
				xQueueSend(xQueueOutData, (void *)&pkg, portMAX_DELAY);
			}
			else if(header->assignment == HEADER_ASSIGNMENT_READ)
			{
				pkg.size = responseReadData((char *)transmitBuffer, BUFSIZE, &iot, header);
				pkg.data = malloc(pkg.size);
				memcpy(pkg.data, transmitBuffer, pkg.size);
				xQueueSend(xQueueOutData, (void *)&pkg, portMAX_DELAY);
			}
			else if(header->assignment == HEADER_ASSIGNMENT_WRITE)
			{
				pkg.size = responseWriteData((char *)transmitBuffer, BUFSIZE, &iot, header);
				pkg.data = malloc(pkg.size);
				memcpy(pkg.data, transmitBuffer, pkg.size);
				xQueueSend(xQueueOutData, (void *)&pkg, portMAX_DELAY);
			}
			else if(header->assignment == HEADER_ASSIGNMENT_PING_PONG)
			{
				pkg.size = responsePingData((char *)transmitBuffer, BUFSIZE);
				pkg.data = malloc(pkg.size);
				memcpy(pkg.data, transmitBuffer, pkg.size);
				xQueueSend(xQueueOutData, (void *)&pkg, portMAX_DELAY);
			}
			else if(header->assignment == HEADER_ASSIGNMENT_STATE)
			{
				pkg.size = responseStateData((char *)transmitBuffer, BUFSIZE, &iot);
				pkg.data = malloc(pkg.size);
				memcpy(pkg.data, transmitBuffer, pkg.size);
				xQueueSend(xQueueOutData, (void *)&pkg, portMAX_DELAY);
			}
		}

		memcpy(recivedBuffer, &recivedBuffer[cutDataSize], BUFSIZE - cutDataSize);
		realBufSize -= cutDataSize;

		clearHeader(header);
	}
}

void Vl6180X_Task(void *pvParameters)
{
	ESP_LOGW(TAG, "Vl6180X task created");

	char *releState;
	releState = iot.readChannel[0].data;
	const uint8_t DISTANCE = 150;

	gpio_set_direction(RELE_PIN, GPIO_MODE_INPUT_OUTPUT);
//	gpio_set_level(RELE_PIN, *releState);

	VL6180X_init();

	while(true)
	{
		if (gpio_get_level(RELE_PIN) != *releState)
		{
			gpio_set_level(RELE_PIN, *releState);
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}

		if (VL6180X_simpleRange() < DISTANCE)
		{
			*releState ^= 1;
			gpio_set_level(RELE_PIN, *releState);
			printf("Rele state: %s, StateRange: %d\n", *releState ? "ON" : "OFF", VL6180X_simpleRange());

			uint16_t counter = 0;
			while (VL6180X_simpleRange() < DISTANCE)
			{
				vTaskDelay(70 / portTICK_PERIOD_MS);
				if (++counter >= 14)
					break;
			}
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}
		vTaskDelay(70 / portTICK_PERIOD_MS);
	}
}

