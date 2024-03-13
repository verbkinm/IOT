/*
 * Докуменация по разработке ESP32-CAM:
 * https://docs.espressif.com/projects/esp-idf/en/release-v5.2/esp32/api-reference/peripherals/sdmmc_host.html
 * https://randomnerdtutorials.com/esp32-cam-ai-thinker-pinout/
 */

#include "Global_def.h"
#include "sd/sd_mmc.h"
#include "service/service.h"

void app_main(void)
{
	sd_mmc_init();
	start_services();
}
