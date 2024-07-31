#include "iotv.h"

#include "esp_mac.h"

static uint64_t realBufSize = 0;

static const char *TAG = "iotv";

static char recivedBuffer[BUFSIZE];
static char transmitBuffer[BUFSIZE];

static int last_client_socket = 0;

static uint8_t readType[] = {
		DATA_TYPE_BOOL,			// состояние реле_1
		DATA_TYPE_BOOL,			// состояние реле_2
		DATA_TYPE_BOOL,			// состояние реле_3
};

//static uint8_t writeType[] = {
//		DATA_TYPE_BOOL,			// состояние реле_1
//		DATA_TYPE_BOOL,			// состояние реле_2
//		DATA_TYPE_BOOL,			// состояние реле_3
//};

static struct IOTV_Server_embedded iot = {
		.id = 1,
		.name = NULL,
		.description = "id-1(3x2A)",
		.numberReadChannel = 3,
		.readChannel = NULL,
		.readChannelType = readType,
		.numberWriteChannel = 3,
		.writeChannelType = readType,
		.state = 0,
		.nameSize = 0,
		.descriptionSize = 10,
};

static uint64_t iotv_write_func(char *data, uint64_t size, void *obj);

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
}

iotv_obj_t *iotv_get(void)
{
	return &iot;
}

esp_err_t iotv_init(void)
{
	// Выделения памяти для iot структуры
	iot.readChannel = (struct RawEmbedded *)malloc(sizeof(struct RawEmbedded) * iot.numberReadChannel);

	if (iot.readChannel == NULL)
	{
		ESP_LOGE(TAG, "iotv_init iot.readChannel - malloc error");
		return ESP_FAIL;
	}

	uint8_t mac[6] = {0};
	esp_efuse_mac_get_default(mac);

	char mac_str[18] = {0};
	sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	char name[256] = {0}; // длина имени в протоколе IOTV определяется одним байтом. Максимальное значение получается 255
	strcat(name, IOTV_DEVICE_NAME);
	strcat(name, " (");
	strcat(name, mac_str);
	strcat(name, ")");

	size_t nameSize = strlen(name);
	iot.name = malloc(nameSize);
	if (iot.name  == NULL)
	{
		ESP_LOGE(TAG, "iotv_init iot.name - malloc error");
		return ESP_FAIL;
	}
	iot.nameSize = nameSize;

	memcpy(iot.name, name, nameSize);

	for (uint8_t i = 0; i < iot.numberReadChannel; ++i)
	{
		iot.readChannel[i].dataSize = dataSizeonDataType(readType[i]);
		iot.readChannel[i].data = (char *)calloc(1, iot.readChannel[i].dataSize);
	}

	iot.state = 1;

	return ESP_OK;
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
		header_t *header = createPkgs((uint8_t *)recivedBuffer, realBufSize, &error, &expectedDataSize, &cutDataSize);

		if (error == true)
		{
			iotv_clear_buf_data();
			clearHeader(header);
			printf("%s Data error\n", TAG);
			return;
		}

		if (expectedDataSize > 0)
		{
			printf("%s expextedDataSize %d\n", TAG, (int)expectedDataSize);
			clearHeader(header);
			return;
		}

		if (header == NULL)
		{
			printf("%s header == NULL\n", TAG);
			return;
		}


		if (header->type == HEADER_TYPE_REQUEST)
		{
			if (header->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
				responseIdentificationData(transmitBuffer, BUFSIZE, &iot, iotv_write_func, (void *)&sock, Identification_FLAGS_NONE, HEADER_FLAGS_NONE);
			else if(header->assignment == HEADER_ASSIGNMENT_READ)
				responseReadData(transmitBuffer, BUFSIZE, &iot, header, iotv_write_func, (void *)&sock, ReadWrite_FLAGS_NONE, HEADER_FLAGS_NONE);
			else if (header->assignment == HEADER_ASSIGNMENT_WRITE)
				responseWriteData((char *)transmitBuffer, BUFSIZE, &iot, header, iotv_write_func, (void *)&sock, ReadWrite_FLAGS_NONE, HEADER_FLAGS_NONE);
			else if(header->assignment == HEADER_ASSIGNMENT_PING_PONG)
				responsePingData(transmitBuffer, BUFSIZE, iotv_write_func, (void *)&sock, HEADER_FLAGS_NONE);
			else if(header->assignment == HEADER_ASSIGNMENT_STATE)
				responseStateData(transmitBuffer, BUFSIZE, &iot, iotv_write_func, (void *)&sock, STATE_FLAGS_NONE, HEADER_FLAGS_NONE);
		}

		memcpy(recivedBuffer, &recivedBuffer[cutDataSize], BUFSIZE - cutDataSize);
		realBufSize -= cutDataSize;
		clearHeader(header);
	}
}
