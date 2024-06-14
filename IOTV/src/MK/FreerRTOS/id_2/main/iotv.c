#include "iotv.h"
#include "Protocols/embedded/iotv_server_embedded.h"

//static const char *TAG = "IOTV";

static uint8_t readType[] = {
		DATA_TYPE_FLOAT_32,		// температура
		DATA_TYPE_FLOAT_32,		// влажность
		DATA_TYPE_FLOAT_32,		// давление
		DATA_TYPE_INT_8			// заряд аккумулятора
};

static struct IOTV_Server_embedded iot = {
		.id = 2,
		.name = "ESP-32 id-2",
		.description = "Автономная станция",
		.numberReadChannel = 4,
		.readChannel = NULL,
		.readChannelType = readType,
		.numberWriteChannel = 0,
		.writeChannelType = NULL,
		.state = 0,
		.nameSize = 11,
		.descriptionSize = 35,
};

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

	iot.state = 1;
}
