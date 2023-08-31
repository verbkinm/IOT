#include "iotv.h"

extern QueueHandle_t xQueueInData, xQueueOutData, xQueueDF;

extern uint64_t realBufSize;
extern uint64_t expextedDataSize;

static const char *TAG = "iotv";

static uint8_t recivedBuffer[BUFSIZE], transmitBuffer[BUFSIZE];
static uint64_t cutDataSize = 0;
static bool error = false;

static void readNVSData();
static void writeNVSData(int8_t ch);

static uint8_t readType[CH_NUMBER] = {
		DATA_TYPE_BOOL,			// Выключить / включить воспроизведение.
		DATA_TYPE_INT_8, 		// Режим воспроизведения.
		DATA_TYPE_INT_8, 		// Режимы света.
		DATA_TYPE_BOOL,			// Выключить / включить повтор текущего режима воспроизведения.
		DATA_TYPE_INT_16,		// Максимальная яркость светодиодов.
		DATA_TYPE_INT_16,		// Минимальная яркость светодиодов.
		DATA_TYPE_INT_8,		// Громкость
		DATA_TYPE_INT_8,		// Цвет светодиода
		DATA_TYPE_INT_8			// Эквалайзер
};

struct IOTV_Server_embedded iot = {
		.id = 4,
		.name = "Облачко",
		.description = "Облачко - ночник, для Евы - королевы и остальных жителей её королевства!",
		.numberReadChannel = CH_NUMBER,
		.readChannel = NULL,
		.readChannelType = readType,
		.numberWriteChannel = CH_NUMBER,
		.writeChannelType = readType,
		.state = 0,
		.nameSize = 14,
		.descriptionSize = 129,
};

static void dataRecived(const struct DataPkg *pkg);
static void df_dataSend(uint8_t chNumm, uint8_t val);

void iotvTask(void *pvParameters)
{
	ESP_LOGW(TAG, "iotv task created");

	// Выделения памяти для iot структуры
	iot.readChannel = (struct RawEmbedded *)malloc(sizeof(struct RawEmbedded) * iot.numberReadChannel);

	for (uint8_t i = 0; i < CH_NUMBER; ++i)
	{
		iot.readChannel[i].dataSize = dataSizeonDataType(readType[i]);
		iot.readChannel[i].data = (char *)calloc(1, iot.readChannel[i].dataSize);
	}

	// Настройки по умолчанию
	*(int16_t *)iot.readChannel[CH_MAX_DUTY].data = 8192;
	*(int16_t *)iot.readChannel[CH_MIN_DUTY].data = 100;
	*(int8_t *)iot.readChannel[CH_LED_COLOR].data = -1;
	*(bool *)iot.readChannel[CH_REPEATE].data = true;

	readNVSData();

	iot.state = 1;

	struct DataPkg pkg = { NULL, 0 };

	while (true)
	{
		if (xQueueReceive(xQueueInData, (void *)&pkg, portMAX_DELAY) == pdPASS)
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

				if (pkg.size > 0)
				{
					uint8_t channelNumber = ((struct Read_Write *)header->pkg)->channelNumber;
					void *val = (void *)iot.readChannel[channelNumber].data;
					switch (channelNumber)
					{
					case CH_PLAY:
					case CH_PLAY_MODE:
					case CH_REPEATE:
					case CH_VOLUME:
					case CH_EQ:
						df_dataSend(channelNumber, *(uint8_t *)val);
						writeNVSData(channelNumber);
						break;
					case CH_MAX_DUTY:
					case CH_MIN_DUTY:
						writeNVSData(channelNumber);
						break;
					case CH_LED_COLOR:
						*(int8_t *)iot.readChannel[CH_LED_MODE].data = -2; // выключенный 1-й светодиод
						break;
					default:
						break;
					}
				}

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
			else if(header->assignment == HEADER_ASSIGNMENT_TECH)
			{
				// МК не отвечает HEADER_ASSIGNMENT_TECH
			}
		}

		memcpy(recivedBuffer, &recivedBuffer[cutDataSize], BUFSIZE - cutDataSize);
		realBufSize -= cutDataSize;

		clearHeader(header);
	}
}

static void df_dataSend(uint8_t chNumm, uint8_t val)
{
	int8_t cmd = -1;
	switch (chNumm)
	{
	case CH_PLAY:
		cmd = DF_CMD_PS;
		break;
	case CH_PLAY_MODE:
		cmd = DF_CMD_MODE;
		break;
	case CH_REPEATE:
		cmd = DF_CMD_REPEATE;
		break;
	case CH_VOLUME:
		cmd = DF_CMD_VOLUME;
		break;
	case CH_EQ:
		cmd = DF_CMD_EQ;
		break;
	default:
		break;
	}

	if (cmd == CH_PLAY || cmd == CH_PLAY_MODE || cmd == CH_REPEATE || cmd == CH_VOLUME || cmd == CH_EQ)
		xQueueSend(xQueueDF, (void *)&cmd, portMAX_DELAY);
}

static void readNVSData()
{
	nvs_handle_t my_handle;
	esp_err_t err = nvs_open(TAG, NVS_READONLY, &my_handle);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
		return;
	}

//	nvs_iterator_t it = NULL;
//	nvs_entry_find("nvs", NULL, NVS_TYPE_ANY, &it);
//	while (it != NULL) {
//		nvs_entry_info_t info;
//		nvs_entry_info(it, &info);
//		nvs_entry_next(&it);
//		printf("key '%s', type '%02x' \n", info.key, info.type);
//	};
//	nvs_release_iterator(it);

	nvs_get_i8(my_handle, "CH_PLAY_MODE", (int8_t *)iot.readChannel[CH_PLAY_MODE].data);
	nvs_get_i8(my_handle, "CH_REPEATE", (int8_t *)iot.readChannel[CH_REPEATE].data);
	nvs_get_i16(my_handle, "CH_MAX_DUTY", (int16_t *)iot.readChannel[CH_MAX_DUTY].data);
	nvs_get_i16(my_handle, "CH_MIN_DUTY", (int16_t *)iot.readChannel[CH_MIN_DUTY].data);
	nvs_get_i8(my_handle, "CH_VOLUME", (int8_t *)iot.readChannel[CH_VOLUME].data);
	nvs_get_i8(my_handle, "CH_EQ", (int8_t *)iot.readChannel[CH_EQ].data);

	printf("CH_PLAY_MODE = %d\n"
			"CH_REPEATE = %d\n"
			"CH_MAX_DUTY = %d\n"
			"CH_MIN_DUTY = %d\n"
			"CH_VOLUME = %d\n"
			"CH_EQ = %d\n",
			*(int8_t *)iot.readChannel[CH_PLAY_MODE].data,
			*(int8_t *)iot.readChannel[CH_REPEATE].data,
			*(int16_t *)iot.readChannel[CH_MAX_DUTY].data,
			*(int16_t *)iot.readChannel[CH_MIN_DUTY].data,
			*(int8_t *)iot.readChannel[CH_VOLUME].data,
			*(int8_t *)iot.readChannel[CH_EQ].data);

	nvs_close(my_handle);
}

static void writeNVSData(int8_t ch)
{
	nvs_handle_t my_handle;
	esp_err_t err = nvs_open(TAG, NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
		return;
	}

	switch (ch)
	{
	case CH_PLAY:
		if (*(int8_t *)iot.readChannel[CH_PLAY].data == 0)
			nvs_set_i8(my_handle, "CH_REPEATE", 0);
		break;
	case CH_PLAY_MODE:
		nvs_set_i8(my_handle, "CH_PLAY_MODE", *(int8_t *)iot.readChannel[CH_PLAY_MODE].data);
		break;
	case CH_REPEATE:
		nvs_set_i8(my_handle, "CH_REPEATE", *(int8_t *)iot.readChannel[CH_REPEATE].data);
		break;
	case CH_MAX_DUTY:
		nvs_set_i16(my_handle, "CH_MAX_DUTY", *(int16_t *)iot.readChannel[CH_MAX_DUTY].data);
		break;
	case CH_MIN_DUTY:
		nvs_set_i16(my_handle, "CH_MIN_DUTY", *(int16_t *)iot.readChannel[CH_MIN_DUTY].data);
		break;
	case CH_VOLUME:
		nvs_set_i8(my_handle, "CH_VOLUME", *(int8_t *)iot.readChannel[CH_VOLUME].data);
		break;
	case CH_EQ:
		nvs_set_i8(my_handle, "CH_EQ", *(int8_t *)iot.readChannel[CH_EQ].data);
		break;
	default:
		break;
	}

	// Commit written value.
	if (nvs_commit(my_handle) != ESP_OK)
		ESP_LOGE(TAG, "writeBorderDistanceToNVS commit failed");

	// Close
	nvs_close(my_handle);
}
