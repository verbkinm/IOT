#include "i2c.h"

extern QueueHandle_t xQueueLedSignals;

static const char *TAG = "i2c";

void i2c_init(void)
{
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
	{
		struct LedSignalPkg pkg = {TAG, I2C_INIT_FAIL};
		xQueueSend(xQueueLedSignals, (void *)&pkg, 10 / portTICK_PERIOD_MS);
	}
}

