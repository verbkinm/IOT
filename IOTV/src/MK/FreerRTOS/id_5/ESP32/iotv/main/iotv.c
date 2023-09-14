#include "iotv.h"

//extern QueueHandle_t xQueueInData, xQueueOutData;

extern uint64_t realBufSize;
extern uint64_t expextedDataSize;
extern uint8_t glob_status;

static const char *TAG = "iotv";

static uint8_t recivedBuffer[BUFSIZE];
uint8_t transmitBuffer[BUFSIZE];
int transmitBufferSize = 0;

static uint64_t cutDataSize = 0;
static bool error = false;

static uint8_t readType[15] = {
		DATA_TYPE_BOOL,			// состояние реле
		DATA_TYPE_INT_16, 		// порог срабатывания реле

		DATA_TYPE_INT_8, 		// секунды
		DATA_TYPE_INT_8,		// минуты
		DATA_TYPE_INT_8,		// часы
		DATA_TYPE_INT_8,		// день недели
		DATA_TYPE_INT_8,		// день месяца
		DATA_TYPE_INT_8,		// месяц
		DATA_TYPE_INT_8,		// год

		DATA_TYPE_INT_8,		// ориентация дисплея

		DATA_TYPE_INT_16,		// текущее расстояние
		DATA_TYPE_DOUBLE_64,	// освещённость
		DATA_TYPE_DOUBLE_64,	// температура
		DATA_TYPE_DOUBLE_64,	// влажность
		DATA_TYPE_DOUBLE_64		// давление
};

static uint8_t writeType[10] = {
		DATA_TYPE_BOOL,			// состояние реле
		DATA_TYPE_INT_16, 		// порог срабатывания реле

		DATA_TYPE_INT_8, 		// секунды
		DATA_TYPE_INT_8,		// минуты
		DATA_TYPE_INT_8,		// часы
		DATA_TYPE_INT_8,		// день недели
		DATA_TYPE_INT_8,		// день месяца
		DATA_TYPE_INT_8,		// месяц
		DATA_TYPE_INT_8,		// год

		DATA_TYPE_INT_8			// ориентация дисплея
};

static struct IOTV_Server_embedded iot = {
		.id = 5,
		.name = "vl6180x+bme280+relay",
		.description = "ESP-32 id-5",
		.numberReadChannel = 15,
		.readChannel = NULL,
		.readChannelType = readType,
		.numberWriteChannel = 10,
		.writeChannelType = writeType,
		.state = 0,
		.nameSize = 20,
		.descriptionSize = 11,
};

static int16_t readBorderDistanceFromNVS(void);
static void writeBorderDistanceToNVS(int16_t value);

static uint8_t readDisplayOrientationFromNVS(void);
static void writeDisplayOrientationToNVS(uint8_t value);

void iotvInit(void)
{
	// Выделения памяти для iot структуры
	iot.readChannel = (struct RawEmbedded *)malloc(sizeof(struct RawEmbedded) * iot.numberReadChannel);

	for (uint8_t i = 0; i < iot.numberReadChannel; ++i)
	{
		iot.readChannel[i].dataSize = dataSizeonDataType(readType[i]);
		iot.readChannel[i].data = (char *)calloc(1, iot.readChannel[i].dataSize);
	}

	iot.state = 1;
}

//static void dataRecived(const struct DataPkg *pkg)
void dataRecived(const uint8_t *data, int size)
{
	if (data == NULL)
		return;

	//страховка
	if ((realBufSize + size) >= BUFSIZE)
	{
		realBufSize = 0;
		expextedDataSize = 0;
		cutDataSize = 0;
		ESP_LOGE(TAG, "Buffer overflow");
		return;
	}

	memcpy(&recivedBuffer[realBufSize], data, size);
	realBufSize += size;

	if (realBufSize < expextedDataSize)
		return;

	while (realBufSize > 0)
	{
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
				transmitBufferSize += responseIdentificationData((char *)(transmitBuffer + transmitBufferSize), BUFSIZE, &iot);
			else if(header->assignment == HEADER_ASSIGNMENT_READ)
				transmitBufferSize = responseReadData((char *)(transmitBuffer + transmitBufferSize), BUFSIZE, &iot, header);
			else if(header->assignment == HEADER_ASSIGNMENT_WRITE)
			{
				int len = responseWriteData((char *)(transmitBuffer + transmitBufferSize), BUFSIZE, &iot, header);
				transmitBufferSize += len;

				if (len > 0)
				{
					uint8_t channelNumber = ((struct Read_Write *)header->pkg)->channelNumber;
					int16_t *val = (int16_t *)iot.readChannel[channelNumber].data;
					switch (channelNumber)
					{
					case CH_RELAY_STATE:
						uint8_t *rele_state = (uint8_t *)iot.readChannel[CH_RELAY_STATE].data;
						gpio_set_level(RELE_PIN, *rele_state);
						if (*rele_state != gpio_get_level(RELE_PIN))
						{
							ESP_LOGE(TAG, "Can't switch relay");
							*rele_state ^= *rele_state;
						}
						else
						{
							setBitInByte(&glob_status, *rele_state, MY_STATUS_RELE);
							printf("Rele state: %s, remote switch\n", (*rele_state ? "ON" : "OFF"));
						}

						break;
					case CH_BORDER:
						*val = inRange(*val, 0, 255);
						printf("Data in border: %d\n", *val);
						writeBorderDistanceToNVS(*val);
						break;
					case CH_SEC:
					case CH_MIN:
						*val = inRange(*val, 0, 59);
						break;
					case CH_HOUR:
						*val = inRange(*val, 0, 23);
						break;
					case CH_DAY:
						*val = inRange(*val, 1, 7);
						break;
					case CH_DATE:
						*val = inRange(*val, 1, 31);
						break;
					case CH_MONTH:
						*val = inRange(*val, 1, 12);
						break;
					case CH_YEAR:
						*val = inRange(*val, 0, 99);
						break;
					case CH_DISP_ORNT:
						// проверка!!!
						writeDisplayOrientationToNVS(*(uint8_t *)iot.readChannel[channelNumber].data);
						break;
					default:
						break;
					}
				}

				if(((struct Read_Write *)header->pkg)->channelNumber >= CH_SEC && ((struct Read_Write *)header->pkg)->channelNumber <= CH_YEAR)
				{
					struct DateTime dt;
					for (uint8_t i = DS3231_REG_SEC, j = CH_SEC; i <= DS3231_REG_YEAR; ++i, ++j)
						((uint8_t *)&dt)[i] = *(uint8_t *)iot.readChannel[j].data;

					DS3231_SetDataTime(&dt);
				}
			}
			else if(header->assignment == HEADER_ASSIGNMENT_PING_PONG)
				transmitBufferSize += responsePingData((char *)(transmitBuffer + transmitBufferSize), BUFSIZE);
			else if(header->assignment == HEADER_ASSIGNMENT_STATE)
				transmitBufferSize += responseStateData((char *)(transmitBuffer + transmitBufferSize), BUFSIZE, &iot);
		}

		memcpy(recivedBuffer, &recivedBuffer[cutDataSize], BUFSIZE - cutDataSize);
		realBufSize -= cutDataSize;

		clearHeader(header);
	}
}

static int16_t readBorderDistanceFromNVS(void)
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
		ESP_LOGE(TAG, "BorderDistanceToNVS open failed");
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

static uint8_t readDisplayOrientationFromNVS(void)
{
	uint8_t displayOrientation = DISPLAY_ORIENTATION_DEFAULT; // значение по умолчанию

	// Open
	nvs_handle_t my_handle;
	if (nvs_open("DISP", NVS_READONLY, &my_handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "Error nvs_open");
		return displayOrientation;
	}

	// Read
	if (nvs_get_u8(my_handle, "dispOrientation", &displayOrientation) != ESP_OK)
		ESP_LOGE(TAG, "Error nvs_get_u8");

	nvs_close(my_handle);
	return displayOrientation;
}

static void writeDisplayOrientationToNVS(uint8_t value)
{
	// Open
	nvs_handle_t my_handle;
	if (nvs_open("DISP", NVS_READWRITE, &my_handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "displayOrientation open failed");
		return;
	}

	// Write
	if (nvs_set_u8(my_handle, "dispOrientation", value) != ESP_OK)
		ESP_LOGE(TAG, "displayOrientation write failed");

	// Commit written value.
	if (nvs_commit(my_handle) != ESP_OK)
		ESP_LOGE(TAG, "displayOrientation commit failed");

	// Close
	nvs_close(my_handle);
}

void Vl6180X_Task(void *pvParameters)
{
	while(iot.state == 0)
		vTaskDelay(100 / portTICK_PERIOD_MS);

	ESP_LOGW(TAG, "Vl6180X task created");

	char *releState;
	releState = iot.readChannel[CH_RELAY_STATE].data;

	int16_t *border = (int16_t *)iot.readChannel[CH_BORDER].data;
	*border = readBorderDistanceFromNVS();

	int16_t *range = (int16_t *)iot.readChannel[CH_RANGE].data;

	gpio_set_direction(RELE_PIN, GPIO_MODE_INPUT_OUTPUT);

	VL6180X_init();

	while(true)
	{
		*range = VL6180X_simpleRange();
		//		*(double *)iot.readChannel[3].data = VL6180X_simpleALS(VL6180X_ALS_GAIN_5);
		//		printf("ALS: %d\n", VL6180X_simpleALS(VL6180X_ALS_GAIN_5));

		if (*range < *border)
		{
			vTaskDelay(10 / portTICK_PERIOD_MS);
			*range = VL6180X_simpleRange();
			if (*range < *border)
			{
				gpio_set_level(RELE_PIN, !(*releState));
				if (*releState != gpio_get_level(RELE_PIN))
				{
					*releState ^= 1;
					printf("Rele state: %s, StateRange: %d\n", (*releState ? "ON" : "OFF"), *range);
					setBitInByte(&glob_status, *releState, MY_STATUS_RELE);
				}
				else
					ESP_LOGE(TAG, "Can't switch relay");

				vTaskDelay(1000 / portTICK_PERIOD_MS);
			}
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void BME280_Task(void *pvParameters)
{
	while(iot.state == 0)
		vTaskDelay(100 / portTICK_PERIOD_MS);

	ESP_LOGW(TAG, "BME280 task created");

	BME280_init();

	while(true)
	{
		struct THP values = BME280_readValues();
		if (values.err)
		{
			*(double *)iot.readChannel[CH_TEMP].data = INFINITY;
			*(double *)iot.readChannel[CH_PRES].data = INFINITY;
			*(double *)iot.readChannel[CH_HUM].data = INFINITY;
		}
		else
		{
			*(double *)iot.readChannel[CH_TEMP].data = values.temperature;
			*(double *)iot.readChannel[CH_PRES].data = values.pressure;
			*(double *)iot.readChannel[CH_HUM].data = values.humidity;
		}
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}

void DS3231_Task(void *pvParameters)
{
	while(iot.state == 0)
		vTaskDelay(100 / portTICK_PERIOD_MS);

	ESP_LOGW(TAG, "DS3231 task created");

	struct DateTime dt;


	while(true)
	{
		dt = DS3231_DataTime();

		if (dt.err)
		{
			for (uint8_t i = CH_SEC, j = 0; i <= CH_YEAR; ++i, ++j)
				*(uint8_t *)iot.readChannel[i].data = 255;
		}
		else
		{
			for (uint8_t i = CH_SEC, j = 0; i <= CH_YEAR; ++i, ++j)
				*(uint8_t *)iot.readChannel[i].data = ((uint8_t *)&dt)[j];
		}

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void OLED_Task(void *pvParameters)
{
	while(iot.state == 0)
		vTaskDelay(100 / portTICK_PERIOD_MS);

	ESP_LOGW(TAG, "OLED task created");

	OLED_init2(readDisplayOrientationFromNVS());
	OLED_boot_screen();
	OLED_init_draw_element(); // после boot_screen

	struct DateTime dt;
	struct THP thp;

	static struct DateTime lastDT = {0};
	static struct THP lastTHP = {0};
	static uint8_t lastStatus = 0;

	while(true)
	{
		// на 32 байта меньше, чем поэлементное присваивание,
		// но при добавлении каналов вмежду CH_SEC и CH_YEAR нужно не забыть исправить.
		for (uint8_t i = CH_SEC, j = 0; i <= CH_YEAR; ++i, ++j)
			((uint8_t *)&dt)[j] = *(uint8_t *)iot.readChannel[i].data;

//		dt.seconds = *(uint8_t *)iot.readChannel[CH_SEC].data;
//		dt.minutes = *(uint8_t *)iot.readChannel[CH_MIN].data;
//		dt.hour = *(uint8_t *)iot.readChannel[CH_HOUR].data;
//		dt.day = *(uint8_t *)iot.readChannel[CH_DAY].data;
//		dt.date = *(uint8_t *)iot.readChannel[CH_DATE].data;
//		dt.month = *(uint8_t *)iot.readChannel[CH_MONTH].data;
//		dt.year = *(uint8_t *)iot.readChannel[CH_YEAR].data;
		dt.err = (dt.seconds == 255) ? true : false; // если данные не считались, то все значения для dt = 255. Смотри DS3231_Task if (dt.err)

		thp.temperature = *(double *)iot.readChannel[CH_TEMP].data;
		thp.humidity = *(double *)iot.readChannel[CH_HUM].data;
		thp.pressure = *(double *)iot.readChannel[CH_PRES].data;
		thp.err = thp.temperature == INFINITY ? true : false; // если данные не считались, то все значения для thp = INFINITY. Смотри BME280_Task if (values.err)

		if ( memcmp(&lastDT, &dt, sizeof(struct DateTime)) != 0
				|| memcmp(&lastTHP, &thp, sizeof(struct THP)) != 0
				|| (lastStatus != glob_status))
		{
			OLED_Draw_Page(&thp, &dt, glob_status);
			lastDT = dt;
			lastTHP = thp;
			lastStatus = glob_status;
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}


