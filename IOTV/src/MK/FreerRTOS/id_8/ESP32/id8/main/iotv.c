#include "iotv.h"

#define STREAM_BUF_SIZE 8192

static const char *TAG = "iotv";
static const char *task_name = "iotv_service_task";

static uint64_t realBufSize = 0;

static char recivedBuffer[BUFSIZE];
static char transmitBuffer[BUFSIZE];

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

	return send(socket, data, size, 0);
}

void iotv_clear_buf_data(void)
{
	realBufSize = 0;
	last_client_socket = 0;
	camera_stop();
}

static void iotv_init(void)
{
	printf("%s iotv_init start\n", TAG);

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

	printf("%s iotv_init complete\n", TAG);
}

void iotv_data_recived(const char *data, int size, int sock)
{
    bool error;
    uint64_t cutDataSize, expectedDataSize;

	last_client_socket = sock;

	if (data == NULL)
		return;

	//страховка
	if ((realBufSize + size) >= BUFSIZE)
	{
		iotv_clear_buf_data();
		printf("%s Buffer overflow\n", TAG);
		return;
	}

	memcpy(&recivedBuffer[realBufSize], data, size);
	realBufSize += size;

	while (realBufSize > 0)
	{
		uint64_t size = 0;

		struct Header* header = createPkgs((uint8_t *)recivedBuffer, realBufSize, &error, &expectedDataSize, &cutDataSize);

		if (header == NULL)
			printf("%s header == NULL\n", TAG);

		if (error == true)
		{
			iotv_clear_buf_data();
			printf("%s Data error\n", TAG);
			break;
		}

		if (expectedDataSize > 0)
		{
			printf("%s expextedDataSize %d\n", TAG, (int)expectedDataSize);
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
				size = responseStateData(transmitBuffer, BUFSIZE, &iot);
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
	printf("%s %s start\n", TAG, task_name);

	iotv_init();

	if (camera_init() != ESP_OK)
	{
		glob_set_bits_status_err(STATUS_CAMERA_ERROR);
		printf("%s camera_init error\n", TAG);
		esp_restart();
	}

	char *buf = calloc(1, STREAM_BUF_SIZE);

	for( ;; )
	{
//		if (glob_get_status_err())
//			break;

		if ( !(glob_get_status_reg() & STATUS_WIFI_STA_CONNECTED) )
			goto for_end;

		if ( !(glob_get_status_reg() & STATUS_CAMERA_START) )
			goto for_end;

		camera_fb_t *pic = esp_camera_fb_get();

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

		esp_camera_fb_return(pic);

		for_end:

		iot.readChannel[CH_CAM_DATA].dataSize = 0;
		iot.readChannel[CH_CAM_DATA].data = NULL;
		vTaskDelay(30 / portTICK_PERIOD_MS);
	}

	printf("%s %s stop\n", TAG, task_name);
	vTaskDelete(NULL);
	free(buf);
}
