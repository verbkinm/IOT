#include "BME280.h"

static const char *TAG = "BME280";

static int32_t _t_fine;
static bool BME280_state = false;

static void BME280_writeReg(uint8_t reg, uint8_t value);
static uint8_t BME280_readReg(uint8_t reg);

static double BME280_temperature(void);
static double BME280_pressure(void);
static double BME280_humidity(void);

void BME280_init(void)
{
	vTaskDelay(500 / portTICK_PERIOD_MS);
	// 1
	int counter = 0;
	while (BME280_readReg(ID) != 0x60) // default value
	{
		vTaskDelay(500 / portTICK_PERIOD_MS);
		if (++counter >= 3)
		{
			ESP_LOGE(TAG, "initialized fail");
			return;
		}
	}

	BME280_state = true;
	ESP_LOGI(TAG, "initialized successfully");

	// 2
	BME280_writeReg(CTRL_HUM, 0b101);
	BME280_writeReg(CTRL_MEAS, 0b10110111); // 101 101 11
	BME280_writeReg(CONFIG, 0b11110000); // 111 100 00

	return;
}

void BME280_deinit(void)
{
	if (i2c_driver_delete(I2C_MASTER_NUM) != ESP_OK)
	{
		ESP_LOGI(TAG, "I2C de-initialized fail");
		return;
	}

	ESP_LOGI(TAG, "I2C de-initialized successfully");
}

// Writes an 8-bit register
static void BME280_writeReg(uint8_t reg, uint8_t value)
{
	uint8_t data_write[2] = { reg, value };

	if (i2c_master_write_to_device(I2C_MASTER_NUM, BME280_ADDR, data_write, 2, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS) != ESP_OK)
	{
		ESP_LOGE(TAG, "write error");
		BME280_state = false;
	}
	else
		BME280_state = true;
}

// Reads an 8-bit register
static uint8_t BME280_readReg(uint8_t reg)
{
	uint8_t data_read;

	if (i2c_master_write_read_device(I2C_MASTER_NUM, BME280_ADDR, &reg, 1, &data_read, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS) != ESP_OK)
	{
		ESP_LOGE(TAG, "read error");
		BME280_state = false;
	}
	else
		BME280_state = true;

	return data_read;
}

struct THP BME280_readValues()
{
	struct THP thp = {.err = false};

	if (BME280_state == false)
		BME280_init();

	if (BME280_state == false)
	{
		thp.temperature = INFINITY;
		thp.humidity = INFINITY;
		thp.pressure = INFINITY;
		thp.err = true;
		return thp;
	}

	thp.temperature = BME280_temperature();
	thp.humidity = BME280_humidity();
	thp.pressure = BME280_pressure();
	thp.pressure *= 0.0075; // паскали в мм рт.ст.

	return thp;
}

static double BME280_temperature(void)
{
	uint8_t counter = 0;
	while ((BME280_readReg(_STATUS) & 0b1001) != 0)
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
		if (++counter >= 25)
			return infinity();
	}

	uint8_t msb = BME280_readReg(TEMP_MSB);
	uint8_t lsb = BME280_readReg(TEMP_LSB);
	uint8_t xlsb = BME280_readReg(TEMP_XLSB);

	uint32_t adc_T = (msb << 12) | (lsb << 4) | (xlsb >> 4);

	uint16_t dig_T1 = ((uint16_t)BME280_readReg(dig_T1_MSB) << 8 ) | BME280_readReg(dig_T1_LSB);
	int16_t dig_T2 = ((uint16_t)BME280_readReg(dig_T2_MSB) << 8 ) | BME280_readReg(dig_T2_LSB);
	int16_t dig_T3 = ((uint16_t)BME280_readReg(dig_T3_MSB) << 8 ) | BME280_readReg(dig_T3_LSB);

	double var1, var2, temperature;
	var1 = (((double)adc_T) / 16384.0 - ((double)dig_T1) / 1024.0);
	var1 = var1 *((double)dig_T2);
	var2 = (((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0);
	var2 = (var2 * var2) * ((double)dig_T3);

	_t_fine = (int32_t)(var1 + var2);
	temperature = (var1 + var2) / 5120.0;

	double temperature_min = -40;
	double temperature_max = 85;

	if (temperature < temperature_min)
		temperature = temperature_min;
	else if (temperature > temperature_max)
		temperature = temperature_max;

	return temperature;
}

static double BME280_pressure(void)
{
	uint8_t msb = BME280_readReg(PRESS_MSB);
	uint8_t lsb = BME280_readReg(PRESS_LSB_);
	uint8_t xlsb = BME280_readReg(PRESS_XLSB);

	uint16_t dig_P1 = ((uint16_t)BME280_readReg(dig_P1_MSB) << 8 ) | BME280_readReg(dig_P1_LSB);
	int16_t dig_P2 = ((uint16_t)BME280_readReg(dig_P2_MSB) << 8 ) | BME280_readReg(dig_P2_LSB);
	int16_t dig_P3 = ((uint16_t)BME280_readReg(dig_P3_MSB) << 8 ) | BME280_readReg(dig_P3_LSB);
	int16_t dig_P4 = ((uint16_t)BME280_readReg(dig_P4_MSB) << 8 ) | BME280_readReg(dig_P4_LSB);
	int16_t dig_P5 = ((uint16_t)BME280_readReg(dig_P5_MSB) << 8 ) | BME280_readReg(dig_P5_LSB);
	int16_t dig_P6 = ((uint16_t)BME280_readReg(dig_P6_MSB) << 8 ) | BME280_readReg(dig_P6_LSB);
	int16_t dig_P7 = ((uint16_t)BME280_readReg(dig_P7_MSB) << 8 ) | BME280_readReg(dig_P7_LSB);
	int16_t dig_P8 = ((uint16_t)BME280_readReg(dig_P8_MSB) << 8 ) | BME280_readReg(dig_P8_LSB);
	int16_t dig_P9 = ((uint16_t)BME280_readReg(dig_P9_MSB) << 8 ) | BME280_readReg(dig_P9_LSB);
	uint32_t adc_P = (msb << 12) | (lsb << 4) | (xlsb >> 4);

	double var1, var2, pressure;
	var1 = ((double)_t_fine/2.0) - 64000.0;
	var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
	var2 = var2 + var1 * ((double)dig_P5) * 2.0;
	var2 = (var2/4.0)+(((double)dig_P4) * 65536.0);
	var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0)*((double)dig_P1);

	if (var1 == 0.0)
		return 0;

	pressure = 1048576.0 - (double)adc_P;
	pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double)dig_P9) * pressure * pressure / 2147483648.0;
	var2 = pressure * ((double)dig_P8) / 32768.0;
	pressure = pressure + (var1 + var2 + ((double)dig_P7)) / 16.0;

	double pressure_min = 30000.0;
	double pressure_max = 110000.0;

	if (pressure < pressure_min)
		pressure = pressure_min;
	else if (pressure > pressure_max)
		pressure = pressure_max;

	return pressure;
}

static double BME280_humidity(void)
{
	uint8_t msb = BME280_readReg(HUM_MSB);
	uint8_t lsb = BME280_readReg(HUM_LSB);

	uint8_t dig_H1 = BME280_readReg(_dig_H1);
	int16_t dig_H2 = ((uint16_t)BME280_readReg(dig_H2_MSB) << 8 ) | BME280_readReg(dig_H2_LSB);
	uint8_t dig_H3 = BME280_readReg(_dig_H3);

	uint8_t dig_H4_msb = BME280_readReg(dig_H4_MSB) & 0x0F;
	uint8_t dig_H4_lsb = BME280_readReg(dig_H4_LSB);
	int16_t dig_H4 = dig_H4_lsb;
	dig_H4 = dig_H4 << 4;
	dig_H4 = dig_H4 | dig_H4_msb;

	uint8_t dig_H5_msb = BME280_readReg(dig_H5_MSB);
	uint8_t dig_H5_lsb = BME280_readReg(dig_H5_LSB) >> 4;
	int16_t dig_H5 = dig_H5_msb;
	dig_H5 <<= 4;
	dig_H5 = dig_H5 | dig_H5_lsb;

	int8_t dig_H6 = BME280_readReg(_dig_H6);

	uint32_t adc_H = (msb << 8) | lsb;

	double var_H = (((double)_t_fine) - 76800.0);
	var_H = (adc_H - (((double)dig_H4) * 64.0 + ((double)dig_H5) / 16384.0 * var_H)) *
			(((double)dig_H2) / 65536.0 * (1.0 + ((double)dig_H6) / 67108864.0 * var_H *
					(1.0 + ((double)dig_H3) / 67108864.0 * var_H)));
	var_H = var_H * (1.0 - ((double)dig_H1) * var_H / 524288.0);
	if (var_H > 100.0)
		var_H = 100.0;
	else if (var_H < 0.0)
		var_H = 0.0;

	return var_H;
}
