#include "iotv.h"

extern QueueHandle_t xQueueInData, xQueueOutData, xQueueDF;

extern uint64_t realBufSize;
extern uint64_t expextedDataSize;

static const char *TAG = "iotv";

static uint8_t recivedBuffer[BUFSIZE], transmitBuffer[BUFSIZE];
static uint64_t cutDataSize = 0;
static bool error = false;

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

//static uint8_t writeType[CH_NUMBER] = {
//		DATA_TYPE_BOOL,			// Выключить / включить воспроизведение.
//		DATA_TYPE_INT_8, 		// Режим воспроизведения.
//		DATA_TYPE_INT_8, 		// Режимы света.
//		DATA_TYPE_BOOL,			// Выключить / включить повтор текущего режима воспроизведения.
//		DATA_TYPE_INT_16,		// Максимальная яркость светодиодов.
//		DATA_TYPE_INT_16,		// Минимальная яркость светодиодов.
//		DATA_TYPE_INT_8,		// Громкость
//		DATA_TYPE_INT_8,		// Цвет светодиода
//		DATA_TYPE_INT_8			// Эквалайзер
//};

struct IOTV_Server_embedded iot = {
		.id = 4,
		.name = "Облачко",
		.description = "Облачко - ночник, для Евы - королевы!",
		.numberReadChannel = CH_NUMBER,
		.readChannel = NULL,
		.readChannelType = readType,
		.numberWriteChannel = CH_NUMBER,
		.writeChannelType = readType,
		.state = 0,
		.nameSize = 14,
		.descriptionSize = 64,
};

static void dataRecived(const struct DataPkg *pkg);
static void df_dataSend(uint8_t chNumm, uint8_t val);
//static int16_t readBorderDistanceFromNVS();
//static void writeBorderDistanceToNVS(int16_t value);

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
	*(int16_t *)iot.readChannel[CH_MAX_BRIGHTNESS].data = 8192;
	*(int16_t *)iot.readChannel[CH_MIN_BRIGHTNESS].data = 100;
	*(int8_t *)iot.readChannel[CH_LED_COLOR].data = -1;
	*(bool *)iot.readChannel[CH_REPEATE].data = true;

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
						break;
//					case CH_LED_MODE:
//						Led_RGB_scriptNumber = *(uint8_t *)val;
//						break;
//
//					case CH_MAX_BRIGHTNESS:
//						LED_maxDuty = inRange(*(uint16_t *)val, LED_MIN_DUTY_VAL, LED_MAX_DUTY_VAL);
//						break;
//					case CH_MIN_BRIGHTNESS:
//						LED_minDuty = inRange(*(uint16_t *)val, LED_MIN_DUTY_VAL, LED_MAX_DUTY_VAL);
//						break;
					case CH_LED_COLOR:
//						Led_Manual = decToBcd(*(uint8_t *)val);
//						*iot.readChannel[CH_LED_MODE].data = 254;
//						Led_RGB_scriptNumber = 254;
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

	xQueueSend(xQueueDF, (void *)&cmd, portMAX_DELAY);
}
//static int16_t readBorderDistanceFromNVS()
//{
//	int16_t borderDistance = BORDER_DISTANCE_DEFAULT; // значение по умолчанию
//
//	// Open
//	nvs_handle_t my_handle;
//	if (nvs_open("VL6180X", NVS_READONLY, &my_handle) != ESP_OK)
//	{
//		ESP_LOGE(TAG, "Error nvs_open");
//		return borderDistance;
//	}
//
//	// Read
//	if (nvs_get_i16(my_handle, "borderDistance", &borderDistance) != ESP_OK)
//		ESP_LOGE(TAG, "Error nvs_get_i16");
//
//	nvs_close(my_handle);
//	return borderDistance;
//}

//static void writeBorderDistanceToNVS(int16_t value)
//{
//	// Open
//	nvs_handle_t my_handle;
//	if (nvs_open("VL6180X", NVS_READWRITE, &my_handle) != ESP_OK)
//	{
//		ESP_LOGE(TAG, "writeBorderDistanceToNVS open failed");
//		return;
//	}
//
//	// Write
//	if (nvs_set_i16(my_handle, "borderDistance", value) != ESP_OK)
//		ESP_LOGE(TAG, "writeBorderDistanceToNVS write failed");
//
//	// Commit written value.
//	if (nvs_commit(my_handle) != ESP_OK)
//		ESP_LOGE(TAG, "writeBorderDistanceToNVS commit failed");
//
//	// Close
//	nvs_close(my_handle);
//}




