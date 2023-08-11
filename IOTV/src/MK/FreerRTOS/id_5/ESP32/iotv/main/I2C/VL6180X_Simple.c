#include "VL6180X_Simple.h"

extern QueueHandle_t xQueueLedSignals;

static const char *TAG = "VL6180X_Simple";
static bool VL6180X_state = false;

static void VL6180X_writeReg(uint16_t reg, uint8_t value);
static uint8_t VL6180X_readReg(uint16_t reg);
static uint16_t VL6180X_readReg16(uint16_t reg);

void VL6180X_init(void)
{
	// 1
	uint8_t counter = 0;
	while (VL6180X_readReg(IDENTIFICATION__MODEL_ID) != 0xB4) // default value
	{
		vTaskDelay(500 / portTICK_PERIOD_MS);
		if (++counter >= 3)
		{
			struct LedSignalPkg pkg = {TAG, I2C_INIT_FAIL};
			xQueueSend(xQueueLedSignals, (void *)&pkg, 10 / I2C_DEINIT_FAIL);
			return;
		}
	}

	counter = 0;
	while (VL6180X_readReg(SYSTEM__FRESH_OUT_OF_RESET) != 0) //!! != 1
	{
		ESP_LOGI(TAG, "\t SYSTEM__FRESH_OUT_OF_RESET: %d", VL6180X_readReg(SYSTEM__FRESH_OUT_OF_RESET ));
		VL6180X_writeReg(SYSTEM__FRESH_OUT_OF_RESET, 0x00);
		vTaskDelay(500 / portTICK_PERIOD_MS);
		if (++counter >= 3)
		{
			struct LedSignalPkg pkg = {TAG, SYSTEM__FRESH_OUT_OF_RESET_LOOP};
			xQueueSend(xQueueLedSignals, (void *)&pkg, 10 / I2C_DEINIT_FAIL);
			return;
		}
	}

	VL6180X_state = true;
	ESP_LOGI(TAG, "I2C initialized successfully");

	// 2
	VL6180X_writeReg(0x017, 0x01);

	VL6180X_writeReg(0x207, 0x01);
	VL6180X_writeReg(0x208, 0x01);
	VL6180X_writeReg(0x096, 0x00);
	VL6180X_writeReg(0x097, 0xFD); // RANGE_SCALER = 253
	VL6180X_writeReg(0x0E3, 0x01);
	VL6180X_writeReg(0x0E4, 0x03);
	VL6180X_writeReg(0x0E5, 0x02);
	VL6180X_writeReg(0x0E6, 0x01);
	VL6180X_writeReg(0x0E7, 0x03);
	VL6180X_writeReg(0x0F5, 0x02);
	VL6180X_writeReg(0x0D9, 0x05);
	VL6180X_writeReg(0x0DB, 0xCE);
	VL6180X_writeReg(0x0DC, 0x03);
	VL6180X_writeReg(0x0DD, 0xF8);
	VL6180X_writeReg(0x09F, 0x00);
	VL6180X_writeReg(0x0A3, 0x3C);
	VL6180X_writeReg(0x0B7, 0x00);
	VL6180X_writeReg(0x0BB, 0x3C);
	VL6180X_writeReg(0x0B2, 0x09);
	VL6180X_writeReg(0x0CA, 0x09);
	VL6180X_writeReg(0x198, 0x01);
	VL6180X_writeReg(0x1B0, 0x17);
	VL6180X_writeReg(0x1AD, 0x00);
	VL6180X_writeReg(0x0FF, 0x05);
	VL6180X_writeReg(0x100, 0x05);
	VL6180X_writeReg(0x199, 0x05);
	VL6180X_writeReg(0x1A6, 0x1B);
	VL6180X_writeReg(0x1AC, 0x3E);
	VL6180X_writeReg(0x1A7, 0x1F);
	VL6180X_writeReg(0x030, 0x00);

	// Recommended : Public registers - See data sheet for more detail
	VL6180X_writeReg(0x0011, 0x10); // Enables polling for ‘New Sample ready’
	// when measurement completes
	VL6180X_writeReg(0x010a, 0x30); // Set the averaging sample period
	// (compromise between lower noise and
	// increased execution time)

	VL6180X_writeReg(0x003f, 0x46); // Sets the light and dark gain (upper
	// nibble). Dark gain should not be
	// changed.
	VL6180X_writeReg(0x0031, 0xFF); // sets the # of range measurements after
	// which auto calibration of system is
	// performed
	VL6180X_writeReg(0x0040, 0x63); // Set ALS integration time to 100ms
	VL6180X_writeReg(0x002e, 0x01); // perform a single temperature calibration
	// of the ranging sensor optional: Public registers - See data sheet for more detail
	VL6180X_writeReg(0x001b, 0x09); // Set default ranging inter-measurement
	// period to 100ms
	VL6180X_writeReg(0x003e, 0x31); // Set default ALS inter-measurement period
	// to 500ms
	VL6180X_writeReg(0x0014, 0x24); // Configures interrupt on ‘New Sample
	// Ready threshold event
	// 3
	//	Apply other specific settings e.g. cross talk, GPIO, max convergence time etc.
	//	(Optional)

	// 4
	VL6180X_writeReg(SYSTEM__FRESH_OUT_OF_RESET, 0x00);

	VL6180X_writeReg(0x017, 0x00);

	return;
}

void VL6180X_deinit(void)
{
	if (i2c_driver_delete(I2C_MASTER_NUM) != ESP_OK)
	{
		struct LedSignalPkg pkg = {TAG, I2C_READ_FAIL};
		xQueueSend(xQueueLedSignals, (void *)&pkg, 10 / I2C_DEINIT_FAIL);
	}
	ESP_LOGI(TAG, "I2C de-initialized successfully");
}

// Writes an 8-bit register
static void VL6180X_writeReg(uint16_t reg, uint8_t value)
{
	uint8_t data_write[3];
	data_write[0] = (reg >> 8); // MSB of register address
	data_write[1] = reg; // LSB of register address
	data_write[2] = value;

	if (i2c_master_write_to_device(I2C_MASTER_NUM, VL6180X_ADDR, data_write, 3, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS) != ESP_OK)
	{
//		ESP_LOGE(TAG, "write error");
		struct LedSignalPkg pkg = {TAG, I2C_READ_FAIL};
		xQueueSend(xQueueLedSignals, (void *)&pkg, 10 / I2C_WRITE_FAIL);
	}
}

// Reads an 8-bit register
static uint8_t VL6180X_readReg(uint16_t reg)
{
	uint8_t data_write[2];
	uint8_t data_read[1];

	data_write[0] = reg >> 8; // MSB of register address
	data_write[1] = reg; // LSB of register address

	if (i2c_master_write_read_device(I2C_MASTER_NUM, VL6180X_ADDR, data_write, 2, data_read, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS) != ESP_OK)
	{
//		ESP_LOGE(TAG, "read error");
		VL6180X_state = false;
		struct LedSignalPkg pkg = {TAG, I2C_READ_FAIL};
		xQueueSend(xQueueLedSignals, (void *)&pkg, 10 / I2C_READ_FAIL);
	}
	else
		VL6180X_state = true;

	return data_read[0];
}

// Reads an 16-bit register
static uint16_t VL6180X_readReg16(uint16_t reg)
{
	uint8_t data_write[2];
	uint8_t data_read[2];

	data_write[0] = reg >> 8; // MSB of register address
	data_write[1] = reg; // LSB of register address

	if (i2c_master_write_read_device(I2C_MASTER_NUM, VL6180X_ADDR, data_write, 2, data_read, 2, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS) != ESP_OK)
	{
//		ESP_LOGE(TAG, "read error");
		VL6180X_state = false;
		struct LedSignalPkg pkg = {TAG, I2C_READ_FAIL};
		xQueueSend(xQueueLedSignals, (void *)&pkg, 10 / I2C_READ_FAIL);
	}
	else
		VL6180X_state = true;

	return ((uint16_t)data_read[0] << 8) | data_read[1];
}

uint8_t VL6180X_simpleRange(void)
{
	if (VL6180X_state == false)
		VL6180X_init();

	if (VL6180X_state == false)
		return 255;

	uint8_t counter = 0;
	// 1
	while ((VL6180X_readReg(RESULT__RANGE_STATUS) & 0x01) != 1)
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
		if (++counter >= 25)
		{
			struct LedSignalPkg pkg = {TAG, I2C_READ_FAIL};
			xQueueSend(xQueueLedSignals, (void *)&pkg, 10 / RESULT__RANGE_STATUS_LOOP);
			break;
		}
	}

	// 2
	VL6180X_writeReg(SYSRANGE__START, 0x01);

	// 3
	counter = 0;
	while ((VL6180X_readReg(RESULT__INTERRUPT_STATUS_GPIO) & 0x04) == 0)
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
		if (++counter >= 25)
		{
			struct LedSignalPkg pkg = {TAG, I2C_READ_FAIL};
			xQueueSend(xQueueLedSignals, (void *)&pkg, 10 / RESULT__INTERRUPT_STATUS_GPIO_LOOP);
			break;
		}
	}

	// 4
	uint8_t range = VL6180X_readReg(RESULT__RANGE_VAL);

	// 5
	VL6180X_writeReg(SYSTEM__INTERRUPT_CLEAR, 0x05);

	return range;
}

uint16_t VL6180X_simpleALS(uint8_t gain)
{
	if (VL6180X_state == false)
		VL6180X_init();

	if (VL6180X_state == false)
		return 255;

	uint8_t counter = 0;
	// 1
	while ((VL6180X_readReg(RESULT__ALS_STATUS) & 0x01) != 1)
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
		if (++counter >= 25)
		{
			struct LedSignalPkg pkg = {TAG, I2C_READ_FAIL};
			xQueueSend(xQueueLedSignals, (void *)&pkg, 10 / RESULT__RANGE_STATUS_LOOP);
			break;
		}
	}

	// 2
	VL6180X_writeReg(SYSALS__START, 0x01);

	// 3
	counter = 0;
	while ((VL6180X_readReg(RESULT__INTERRUPT_STATUS_GPIO) & 0x20) == 0)
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
		if (++counter >= 25)
		{
			struct LedSignalPkg pkg = {TAG, I2C_READ_FAIL};
			xQueueSend(xQueueLedSignals, (void *)&pkg, 10 / RESULT__INTERRUPT_STATUS_GPIO_LOOP);
			break;
		}
	}

	printf("RESULT__INTERRUPT_STATUS_GPIO: %d\n", VL6180X_readReg(RESULT__INTERRUPT_STATUS_GPIO));

	// 4
	uint16_t als = VL6180X_readReg16(RESULT__ALS_VAL);
	printf("RESULT__ALS_VAL_H: %d\n", VL6180X_readReg(RESULT__ALS_VAL));
	printf("RESULT__ALS_VAL_L: %d\n", VL6180X_readReg(RESULT__ALS_VAL + 1));

	// 5
	VL6180X_writeReg(SYSTEM__INTERRUPT_CLEAR, 0x07);


	return als;

//	uint8_t reg;
//
//	reg = VL6180X_readReg(SYSTEM__INTERRUPT_CONFIG_GPIO);
//	reg &= ~0x38;
//	reg |= (0x4 << 3); // IRQ on ALS ready
//	VL6180X_writeReg(SYSTEM__INTERRUPT_CONFIG_GPIO, reg);
//
//	// 100 ms integration period
//	VL6180X_writeReg(SYSALS__INTEGRATION_PERIOD_H, 0);
//	VL6180X_writeReg(SYSALS__INTEGRATION_PERIOD_L, 100);
//
//	// analog gain
//	if (gain > VL6180X_ALS_GAIN_40)
//		gain = VL6180X_ALS_GAIN_40;
//
//	VL6180X_writeReg(SYSALS__ANALOGUE_GAIN, 0x40 | gain);
//
//	// start ALS
//	VL6180X_writeReg(SYSALS__START, 0x1);
//
//	// Poll until "New Sample Ready threshold event" is set
//	while (4 != ((VL6180X_readReg(RESULT__INTERRUPT_STATUS_GPIO) >> 3) & 0x7))
//		;
//
//	// read lux!
//	float lux = VL6180X_readReg16(RESULT__ALS_VAL);
//
//	// clear interrupt
//	VL6180X_writeReg(SYSTEM__INTERRUPT_CLEAR, 0x07);
//
//	lux *= 0.32; // calibrated count/lux
//	switch (gain) {
//	case VL6180X_ALS_GAIN_1:
//		break;
//	case VL6180X_ALS_GAIN_1_25:
//		lux /= 1.25;
//		break;
//	case VL6180X_ALS_GAIN_1_67:
//		lux /= 1.67;
//		break;
//	case VL6180X_ALS_GAIN_2_5:
//		lux /= 2.5;
//		break;
//	case VL6180X_ALS_GAIN_5:
//		lux /= 5;
//		break;
//	case VL6180X_ALS_GAIN_10:
//		lux /= 10;
//		break;
//	case VL6180X_ALS_GAIN_20:
//		lux /= 20;
//		break;
//	case VL6180X_ALS_GAIN_40:
//		lux /= 40;
//		break;
//	}
//	lux *= 100;
//	lux /= 100; // integration time in ms
//
//	return lux;
}
