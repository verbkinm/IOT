#include "iotv.h"

extern QueueHandle_t xQueueInData, xQueueOutData;

extern uint64_t realBufSize;
extern uint64_t expextedDataSize;

static const char *TAG = "iotv";
static bool oled_page_lock = false;

static uint8_t recivedBuffer[BUFSIZE], transmitBuffer[BUFSIZE];
static uint64_t cutDataSize = 0;
static bool error = false;

static uint8_t readType[7] = {
		DATA_TYPE_BOOL,			// состояние реле
		DATA_TYPE_INT_16, 		// порог срабатывания реле
		DATA_TYPE_INT_16,		// расстояние
		DATA_TYPE_DOUBLE_64,	// освещённость
		DATA_TYPE_DOUBLE_64,	// температура
		DATA_TYPE_DOUBLE_64,	// влажность
		DATA_TYPE_DOUBLE_64		// давление
};

static uint8_t writeType[2] = {
		DATA_TYPE_BOOL,			// состояние реле
		DATA_TYPE_INT_16 		// порог срабатывания реле
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
				{
					gpio_set_level(RELE_PIN, *iot.readChannel[0].data);
					if (*iot.readChannel[0].data != gpio_get_level(RELE_PIN))
						ESP_LOGE(TAG, "Can't switch relay");
					else
						printf("Rele state: %s, remote switch\n", (*iot.readChannel[0].data ? "ON" : "OFF"));
				}
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
		int16_t *range = (int16_t *)iot.readChannel[2].data;
		*range = VL6180X_simpleRange();
		//		*(double *)iot.readChannel[3].data = VL6180X_simpleALS(VL6180X_ALS_GAIN_5);

		//		printf("ALS: %d\n", VL6180X_simpleALS(VL6180X_ALS_GAIN_5));

		if (*range < *DISTANCE)
		{
			oled_page_lock = true;

			gpio_set_level(RELE_PIN, !(*releState));
			if (*releState != gpio_get_level(RELE_PIN))
			{
				*releState ^= 1;
				printf("Rele state: %s, StateRange: %d\n", (*releState ? "ON" : "OFF"), *range);
			}
			else
				ESP_LOGE(TAG, "Can't switch relay");

			uint16_t counter = 0;
			while ((*range = VL6180X_simpleRange()) < *DISTANCE)
			{
				vTaskDelay(70 / portTICK_PERIOD_MS);
				if (++counter >= 14)
					break;
			}
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}
		else
			oled_page_lock = false;

		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void BME280_Task(void *pvParameters)
{
	ESP_LOGW(TAG, "BME280 task created");

	BME280_init();

	while(true)
	{
		BME280_readValues((double *)iot.readChannel[4].data, (double *)iot.readChannel[6].data, (double *)iot.readChannel[5].data);
		//		ESP_LOGI(TAG, "BME280_readValues. %f, %f, %f", *(double *)iot.readChannel[4].data, *(double *)iot.readChannel[6].data, *(double *)iot.readChannel[5].data);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void OLED_Task(void *pvParameters)
{
	ESP_LOGW(TAG, "OLED task created");

	OLED_init();

	int counter = 0;

	while(true)
	{
		if (oled_page_lock)
		{
			counter = 300;
			OLED_Draw_Distance(*(int16_t *)iot.readChannel[2].data);
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
		else
		{
			if (++counter > 300)
			{
				OLED_Draw_Page(*(double *)iot.readChannel[4].data, *(double *)iot.readChannel[5].data, *(double *)iot.readChannel[6].data);
				counter = 0;
			}
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
	}
}


