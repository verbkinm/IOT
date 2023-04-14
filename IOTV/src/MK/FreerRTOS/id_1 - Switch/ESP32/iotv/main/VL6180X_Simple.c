/*
 * VL6180X_Simple.c
 *
 *  Created on: 11 апр. 2023 г.
 *      Author: user
 */

#include "VL6180X_Simple.h"

#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define VL6180X_ADDR                 0x29

static const char *failStr[] = {"I2C_INIT_FAIL", "I2C_DEINIT_FAIL", "I2C_WRITE_FAIL", "I2C_READ_FAIL", "RESULT__INTERRUPT_STATUS_GPIO_LOOP"};

#define I2C_INIT_FAIL						1
#define I2C_DEINIT_FAIL						2
#define I2C_WRITE_FAIL						3
#define I2C_READ_FAIL						4
#define RESULT__INTERRUPT_STATUS_GPIO_LOOP 	5

static const char *TAG = "VL6180X_Simple";

static void errorLoopBlink(uint8_t value);
static void VL6180X_writeReg(uint16_t reg, uint8_t value);
static uint8_t VL6180X_readReg(uint16_t reg);

void VL6180X_init(void)
{
	vTaskDelay(100 / portTICK_PERIOD_MS);

	int i2c_master_port = I2C_MASTER_NUM;

	i2c_config_t conf = {
			.mode = I2C_MODE_MASTER,
			.sda_io_num = I2C_MASTER_SDA_IO,
			.scl_io_num = I2C_MASTER_SCL_IO,
			.sda_pullup_en = GPIO_PULLUP_ENABLE,
			.scl_pullup_en = GPIO_PULLUP_ENABLE,
			.master.clk_speed = I2C_MASTER_FREQ_HZ,
	};

	i2c_param_config(i2c_master_port, &conf);

	if (i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0) != ESP_OK)
		errorLoopBlink(I2C_INIT_FAIL);

	ESP_LOGI(TAG, "I2C initialized successfully");

	vTaskDelay(500 / portTICK_PERIOD_MS);

	// 1
	while (VL6180X_readReg(SYSTEM__FRESH_OUT_OF_RESET) != 0) //!! != 1
	{
		ESP_LOGI(TAG, "\t SYSTEM__FRESH_OUT_OF_RESET: %d", VL6180X_readReg(SYSTEM__FRESH_OUT_OF_RESET ));
		VL6180X_writeReg(SYSTEM__FRESH_OUT_OF_RESET, 0x00);
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}

	// 2
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

	return;
}

void VL6180X_deinit(void)
{
	if (i2c_driver_delete(I2C_MASTER_NUM) != ESP_OK)
		errorLoopBlink(I2C_DEINIT_FAIL);
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
		errorLoopBlink(I2C_WRITE_FAIL);
}

// Reads an 8-bit register
static uint8_t VL6180X_readReg(uint16_t reg)
{
	uint8_t data_write[2];
	uint8_t data_read[1];

	data_write[0] = reg >> 8; // MSB of register address
	data_write[1] = reg; // LSB of register address

	if (i2c_master_write_read_device(I2C_MASTER_NUM, VL6180X_ADDR, data_write, 2, data_read, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS) != ESP_OK)
		errorLoopBlink(I2C_READ_FAIL);

	return data_read[0];
}

uint8_t VL6180X_simpleRange(void)
{
	// 1
	while ((VL6180X_readReg(RESULT__RANGE_STATUS) & 0x01) != 1)
		vTaskDelay(10 / portTICK_PERIOD_MS);

	// 2
	VL6180X_writeReg(SYSRANGE__START, 0x01);

	// 3
	static uint8_t counter = 0;
	while ((VL6180X_readReg(RESULT__INTERRUPT_STATUS_GPIO) & 0x04) == 0)
	{
//		ESP_LOGE(TAG, "\t Wait RESULT__INTERRUPT_STATUS_GPIO: %d", VL6180X_readReg(RESULT__INTERRUPT_STATUS_GPIO ));
		vTaskDelay(10 / portTICK_PERIOD_MS);
		if (++counter > 25)
			errorLoopBlink(RESULT__INTERRUPT_STATUS_GPIO_LOOP);
	}
	counter = 0;

	// 4
	uint8_t range = VL6180X_readReg(RESULT__RANGE_VAL);

	// 5
	VL6180X_writeReg(SYSTEM__INTERRUPT_CLEAR, 0x07);

	return range;
}

static void errorLoopBlink(uint8_t value)
{
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

	for (uint8_t counter = 0; counter < value; ++counter)
	{
		gpio_set_level(GPIO_NUM_2, 1);
		vTaskDelay(200 / portTICK_PERIOD_MS);
		gpio_set_level(GPIO_NUM_2, 0);
		vTaskDelay(200 / portTICK_PERIOD_MS);
	}

	ESP_LOGE(TAG, "FAIL: %s", failStr[value - 1]);
	ESP_LOGE(TAG, "Rebooting...");
	esp_restart();
}
