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
#include "nvs.h"

#include "Protocols/embedded/creatorpkgs.h"
#include "Protocols/embedded/IOTV_SH.h"
#include "Protocols/embedded/iotv_server_embedded.h"

#include "iotv.h"
#include "BME280.h"
#include "VL6180X_Simple.h"

#define RELE_PIN 					GPIO_NUM_2
#define BORDER_DISTANCE_DEFAULT 	150

extern QueueHandle_t xQueueInData, xQueueOutData;

uint64_t realBufSize = 0;
uint64_t expextedDataSize = 20;

static const char *TAG = "iotv";

static uint8_t recivedBuffer[BUFSIZE], transmitBuffer[BUFSIZE];
static uint64_t cutDataSize = 0;
static bool error = false;

static uint8_t readType[7] = {
		DATA_TYPE_BOOL,			// состояние реле
		DATA_TYPE_INT_16, 		// порог срабатывания реле
		DATA_TYPE_INT_16,		// расстояние
		DATA_TYPE_FLOAT_32,	// освещённость
		DATA_TYPE_FLOAT_32,	// температура
		DATA_TYPE_FLOAT_32,	// влажность
		DATA_TYPE_FLOAT_32		// давление
};

static uint8_t writeType[2] = {
		DATA_TYPE_BOOL,			// состояние реле
		DATA_TYPE_INT_16 		// порого срабатывания реле
};

static struct IOTV_Server_embedded iot = {
		.id = 5,
		.name = "vl6180x+bme280+relay",
		.description = "ESP-32 id-5",
		.numberReadChannel = 7,
		.readChannel = NULL,
		.readChannelType = readType,
		.numberWriteChannel = 2,
		.writeChannelType = writeType,
		.state = 1,
		.nameSize = 20,
		.descriptionSize = 11,
};

static void dataRecived(const struct DataPkg *pkg);
static int16_t readBorderDistanceFromNVS();
static void writeBorderDistanceToNVS(int16_t value);

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
		ESP_LOGE(TAG, "Buffer overlow");
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

				if (header->readWrite->channelNumber == 0)
					gpio_set_level(RELE_PIN, *iot.readChannel[0].data);
				else if (header->readWrite->channelNumber == 1)
					writeBorderDistanceToNVS(*(int16_t *)iot.readChannel[1].data);


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

static int16_t readBorderDistanceFromNVS()
{
	int16_t borderDistance = BORDER_DISTANCE_DEFAULT; // значение по умолчанию

	// Open
	nvs_handle_t my_handle;
	if (nvs_open("VL6180X", NVS_READONLY, &my_handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "Error nvs_open");
		return borderDistance;
	}

	// Read
	if (nvs_get_i16(my_handle, "borderDistance", &borderDistance) != ESP_OK)
		ESP_LOGE(TAG, "Error nvs_get_i16");

	nvs_close(my_handle);
	return borderDistance;
}

static void writeBorderDistanceToNVS(int16_t value)
{
	// Open
	nvs_handle_t my_handle;
	if (nvs_open("VL6180X", NVS_READWRITE, &my_handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "writeBorderDistanceToNVS open failed");
		return;
	}

	// Write
	if (nvs_set_i16(my_handle, "borderDistance", value) != ESP_OK)
		ESP_LOGE(TAG, "writeBorderDistanceToNVS write failed");

	// Commit written value.
	if (nvs_commit(my_handle) != ESP_OK)
		ESP_LOGE(TAG, "writeBorderDistanceToNVS commit failed");

	// Close
	nvs_close(my_handle);
}

void Vl6180X_Task(void *pvParameters)
{
	ESP_LOGW(TAG, "Vl6180X task created");

	char *releState;
	releState = iot.readChannel[0].data;

	int16_t *DISTANCE = (int16_t *)iot.readChannel[1].data;
	*DISTANCE = readBorderDistanceFromNVS();

	gpio_set_direction(RELE_PIN, GPIO_MODE_INPUT_OUTPUT);

	VL6180X_init();

	while(true)
	{
		if ((VL6180X_simpleRange() < *DISTANCE) && (VL6180X_simpleRange() != 0))
		{
			*releState ^= 1;
			gpio_set_level(RELE_PIN, *releState);
			printf("Rele state: %s, StateRange: %d\n", *releState ? "ON" : "OFF", VL6180X_simpleRange());

			uint16_t counter = 0;
			while (VL6180X_simpleRange() < *DISTANCE)
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

void BME280_Task(void *pvParameters)
{
	ESP_LOGW(TAG, "BME280 task created");

	BME280_init();

	while(true)
	{
		BME280_readValues((double *)iot.readChannel[4].data, (double *)iot.readChannel[6].data, (double *)iot.readChannel[5].data);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

