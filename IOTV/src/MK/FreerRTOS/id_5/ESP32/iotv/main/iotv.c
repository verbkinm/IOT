#include "iotv.h"

//extern QueueHandle_t xQueueInData, xQueueOutData;

static uint64_t realBufSize = 0;

static const char *TAG = "iotv";

static char recivedBuffer[BUFSIZE];
static char transmitBuffer[BUFSIZE];

static int last_client_socket = 0;

static uint8_t readType[] = {
		DATA_TYPE_BOOL,			// состояние реле
		DATA_TYPE_INT_16, 		// порог срабатывания реле

		DATA_TYPE_INT_8,		// ориентация дисплея

		DATA_TYPE_INT_16,		// текущее расстояние
		DATA_TYPE_FLOAT_32,		// освещённость
		DATA_TYPE_FLOAT_32,		// температура
		DATA_TYPE_FLOAT_32,		// влажность
		DATA_TYPE_FLOAT_32		// давление
};

static uint8_t writeType[] = {
		DATA_TYPE_BOOL,			// состояние реле
		DATA_TYPE_INT_16, 		// порог срабатывания реле

		DATA_TYPE_INT_8			// ориентация дисплея
};

static struct IOTV_Server_embedded iot = {
		.id = 5,
		.name = "vl6180x+bme280+relay",
		.description = "ESP-32 id-5",
		.numberReadChannel = 8,
		.readChannel = NULL,
		.readChannelType = readType,
		.numberWriteChannel = 3,
		.writeChannelType = writeType,
		.state = 0,
		.nameSize = 20,
		.descriptionSize = 11,
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

struct IOTV_Server_embedded *iotv_get(void)
{
	return &iot;
}

void iotv_init(void)
{
	// Выделения памяти для iot структуры
	iot.readChannel = (struct RawEmbedded *)malloc(sizeof(struct RawEmbedded) * iot.numberReadChannel);

	for (uint8_t i = 0; i < iot.numberReadChannel; ++i)
	{
		iot.readChannel[i].dataSize = dataSizeonDataType(readType[i]);
		iot.readChannel[i].data = (char *)calloc(1, iot.readChannel[i].dataSize);
	}

	*(int16_t *)iot.readChannel[CH_BORDER].data = readBorderDistanceFromNVS();
	*(int8_t *)iot.readChannel[CH_DISP_ORNT].data = readDisplayOrientationFromNVS();

	iot.state = 1;
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
			else if(header->assignment == HEADER_ASSIGNMENT_READ)
			{
				responseReadData(transmitBuffer, BUFSIZE, &iot, header, iotv_write_func, (void *)&sock);
				size = 0;
			}
			else if (header->assignment == HEADER_ASSIGNMENT_WRITE)
			{
				size = responseWriteData((char *)transmitBuffer, BUFSIZE, &iot, header);

				if (size > 0)
				{
					uint8_t channelNumber = ((struct Read_Write *)header->pkg)->channelNumber;
					int8_t *val = (int8_t *)iot.readChannel[channelNumber].data;
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
							if (*rele_state)
								glob_set_bits_status_reg(STATUS_RELE);
							else
								glob_clear_bits_status_reg(STATUS_RELE);
							printf("Rele state: %s, remote switch\n", (*rele_state ? "ON" : "OFF"));
						}

						break;
					case CH_BORDER:
						int16_t *val16 = (int16_t *)iot.readChannel[channelNumber].data;
						*val16 = inRange(*val16, 0, 255);
						printf("Data in border: %d\n", *val16);
						writeBorderDistanceToNVS(*val16);
						break;
					case CH_DISP_ORNT:
						*val = inRange(*val, 0, 3);
						writeDisplayOrientationToNVS(*val);
						OLED_set_disp_rotation(*val);
						break;
					default:
						break;
					}
				}
			}
			else if(header->assignment == HEADER_ASSIGNMENT_PING_PONG)
				size = responsePingData(transmitBuffer, BUFSIZE);
			else if(header->assignment == HEADER_ASSIGNMENT_STATE)
				size = responseStateData(transmitBuffer, BUFSIZE, &iot);
		}

		if (size)
			iotv_write_func((char *)transmitBuffer, size, (void *)&sock);

		memcpy(recivedBuffer, &recivedBuffer[cutDataSize], BUFSIZE - cutDataSize);
		realBufSize -= cutDataSize;

		clearHeader(header);
	}
}
