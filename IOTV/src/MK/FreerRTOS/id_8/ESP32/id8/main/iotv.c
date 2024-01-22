#include "iotv.h"

#define STREAM_BUF_SIZE 8192

static const char *TAG = "iotv";

static uint64_t realBufSize = 0;
static uint64_t expextedDataSize = 20;

static char recivedBuffer[BUFSIZE];
static char transmitBuffer[BUFSIZE];

static uint64_t cutDataSize = 0;
static bool error = false;

static int last_client_socket = 0;

static uint8_t readType[CH_COUNT] = {
		DATA_TYPE_RAW, 		// Камера — сырые данные
		DATA_TYPE_NONE,		// Микрофон — сырые данные
		DATA_TYPE_INT_16,	// Камера — ширина
		DATA_TYPE_INT_16,	// Камера — высота
		DATA_TYPE_INT_8		// Камера — качество
};

static uint8_t writeType[CH_COUNT] = {
		DATA_TYPE_NONE,
		DATA_TYPE_NONE,
		DATA_TYPE_INT_16,
		DATA_TYPE_INT_16,
		DATA_TYPE_INT_8
};

static struct IOTV_Server_embedded iot = {
		.id = 8,
		.numberReadChannel = CH_COUNT,
		.numberWriteChannel = CH_COUNT,
		.state = 0,
		.nameSize = 9,
		.descriptionSize = 9,

		.readChannel = NULL,
		.readChannelType = readType,
		.writeChannelType = writeType,
		.name = "ESP32-CAM",
		.description = "ESP32-CAM",
};

static uint64_t iotv_write_func(char *data, uint64_t size, void *obj);
static void iotv_init(void);

static uint64_t iotv_write_func(char *data, uint64_t size, void *obj)
{
	if (obj == NULL || data == NULL)
		return 0;

	int socket = *(int *)obj;

//	ESP_LOGI(TAG, "Data send");
//	for (int i = 0; i < size; ++i)
//	{
//		printf("%02x:", data[i]);
//	}
//	printf("\n");

	return send(socket, data, size, 0);
}

void iotv_clear_buf_data(void)
{
	realBufSize = 0;
	expextedDataSize = 0;
	cutDataSize = 0;
	last_client_socket = 0;
	camera_stop();
}

static void iotv_init(void)
{
	ESP_LOGI(TAG, "iotv_init start");

	iot.readChannel = (struct RawEmbedded*)malloc(sizeof(struct RawEmbedded) * 5);

	iot.readChannel[CH_CAM_DATA].dataSize = 0;
	iot.readChannel[CH_MIC_DATA].dataSize = 0;
	iot.readChannel[CH_CAM_WIDTH].dataSize = dataSizeonDataType(readType[CH_CAM_WIDTH]);
	iot.readChannel[CH_CAM_HEIGHT].dataSize = dataSizeonDataType(readType[CH_CAM_HEIGHT]);
	iot.readChannel[CH_CAM_QUALITY].dataSize = dataSizeonDataType(readType[CH_CAM_QUALITY]);

	iot.readChannel[CH_CAM_DATA].data = NULL;
	iot.readChannel[CH_MIC_DATA].data = NULL;
	iot.readChannel[CH_CAM_WIDTH].data = (char *)malloc(iot.readChannel[CH_CAM_WIDTH].dataSize);
	iot.readChannel[CH_CAM_HEIGHT].data = (char *)malloc(iot.readChannel[CH_CAM_HEIGHT].dataSize);
	iot.readChannel[CH_CAM_QUALITY].data = (char *)malloc(iot.readChannel[4].dataSize);

	int16_t value = 1600;
	memcpy(iot.readChannel[CH_CAM_WIDTH].data, &value, iot.readChannel[CH_CAM_WIDTH].dataSize);
	value = 1200;
	memcpy(iot.readChannel[CH_CAM_HEIGHT].data, &value, iot.readChannel[CH_CAM_HEIGHT].dataSize);
	value = 50;
	memcpy(iot.readChannel[CH_CAM_QUALITY].data, &value, iot.readChannel[CH_CAM_QUALITY].dataSize);

	iot.state = 1;

	ESP_LOGI(TAG, "iotv_init complete");
}

void iotv_data_recived(const char *data, int size, int sock)
{
	last_client_socket = sock;

	if (data == NULL)
		return;

	//страховка
	if ((realBufSize + size) >= BUFSIZE)
	{
		iotv_clear_buf_data();
		ESP_LOGE(TAG, "Buffer overflow");
		return;
	}

	memcpy(&recivedBuffer[realBufSize], data, size);
	realBufSize += size;

	if (realBufSize < expextedDataSize)
	{
		ESP_LOGE(TAG, "realBufSize < expextedDataSize");
		return;
	}

	while (realBufSize > 0)
	{
		uint64_t size = 0;

		struct Header* header = createPkgs((uint8_t *)recivedBuffer, realBufSize, &error, &expextedDataSize, &cutDataSize);

		if (header == NULL)
			ESP_LOGE(TAG, "header == NULL");

		if (error == true)
		{
			iotv_clear_buf_data();
			ESP_LOGE(TAG, "Data error");
			break;
		}

		if (expextedDataSize > 0)
		{
			ESP_LOGE(TAG, "expextedDataSize %d", (int)expextedDataSize);
			return;
		}

		if (header->type == HEADER_TYPE_REQUEST)
		{
			if (header->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
				size = responseIdentificationData(transmitBuffer, BUFSIZE, &iot);
			else if (header->assignment == HEADER_ASSIGNMENT_READ)
			{
				struct Read_Write *rwPkg = ((struct Read_Write *)header->pkg);
				if ((rwPkg->channelNumber == 0 || rwPkg->channelNumber == 1) && rwPkg->flags == ReadWrite_FLAGS_OPEN_STREAM)
					camera_start();
				else if ((rwPkg->channelNumber == 0 || rwPkg->channelNumber == 1) && rwPkg->flags == ReadWrite_FLAGS_CLOSE_STREAM)
					camera_stop();

				responseReadData(transmitBuffer, BUFSIZE, &iot, header, iotv_write_func, (void *)&sock);
				size = 0;
			}
			else if (header->assignment == HEADER_ASSIGNMENT_WRITE)
				size = responseWriteData((char *)transmitBuffer, BUFSIZE, &iot, header);
			else if (header->assignment == HEADER_ASSIGNMENT_PING_PONG)
				size = responsePingData(transmitBuffer, BUFSIZE);
			else if (header->assignment == HEADER_ASSIGNMENT_STATE)
			{
				size = responseStateData(transmitBuffer, BUFSIZE, &iot);
//				ESP_LOGI(TAG, "responseStateData. size = %d, state = %d", (int)size, (int)iot.state);
			}
		}

		if (size)
			iotv_write_func((char *)transmitBuffer, size, (void *)&sock);

		memcpy(recivedBuffer, &recivedBuffer[cutDataSize], BUFSIZE - cutDataSize);
		realBufSize -= cutDataSize;

		clearHeader(header);
	}
}

void iotv_service_task(void *pvParameters)
{
	ESP_LOGI(TAG, "iotv_service_task start");

	iotv_init();

	if (camera_init() != ESP_OK)
	{
		glob_set_bits_status_err(STATUS_CAMERA_ERROR);
		esp_restart();
	}

	char *buf = calloc(1, STREAM_BUF_SIZE);

	for( ;; )
	{
		if (glob_get_status_err())
			break;

		if ( !(glob_get_status_reg() & STATUS_WIFI_STA_CONNECTED) )
			goto for_end;

		if ( !(glob_get_status_reg() & STATUS_CAMERA_START) )
			goto for_end;

		camera_fb_t *pic = esp_camera_fb_get();
		ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes", pic->len);

		int16_t width = pic->width;
		int16_t height = pic->height;

		iot.readChannel[CH_CAM_DATA].dataSize = pic->len;
		iot.readChannel[CH_CAM_DATA].data = (char *)pic->buf;

		*(int16_t *)iot.readChannel[CH_CAM_WIDTH].data = width;
		*(int16_t *)iot.readChannel[CH_CAM_HEIGHT].data = height;
		*(int8_t *)iot.readChannel[CH_CAM_QUALITY].data = 100;

		struct Read_Write readWrite = {
				.nameSize = iot.nameSize,
				.channelNumber = 0,
				.flags = ReadWrite_FLAGS_OPEN_STREAM,
				.dataSize = iot.readChannel[CH_CAM_DATA].dataSize,
				.name = iot.name,
				.data = iot.readChannel[CH_CAM_DATA].data
		};

		struct Header header = {
				.version = 2,
				.type = HEADER_TYPE_RESPONSE,
				.assignment = HEADER_ASSIGNMENT_READ,
				.flags = HEADER_FLAGS_NONE,
				.fragment = 1,
				.fragments = 1,
				.dataSize = readWriteSize(&readWrite),
				.pkg = &readWrite
		};
		responseReadData((char *)buf, STREAM_BUF_SIZE, &iot, &header, iotv_write_func, (void *)&last_client_socket);
//		if (responseReadData((char *)transmitBuffer, BUFSIZE, &iot, &header, iotv_write_func, (void *)&last_client_socket) < 1)
//			camera_stop();

		esp_camera_fb_return(pic);

		for_end:

		iot.readChannel[CH_CAM_DATA].dataSize = 0;
		iot.readChannel[CH_CAM_DATA].data = NULL;
		vTaskDelay(20 / portTICK_PERIOD_MS);
	}

	ESP_LOGI(TAG, "iotv_service_task stop");
	vTaskDelete(NULL);
	free(buf);
}
