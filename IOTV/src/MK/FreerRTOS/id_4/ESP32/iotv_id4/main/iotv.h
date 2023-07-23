#ifndef MAIN_IOTV_H_
#define MAIN_IOTV_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "nvs.h"

#include "Protocols/embedded/creatorpkgs.h"
#include "Protocols/embedded/IOTV_SH.h"
#include "Protocols/embedded/iotv_server_embedded.h"

#include "Global_def.h"
#include "Local_Lib/local_lib.h"

#define CH_NUMBER			9

#define CH_PLAY		 		0
#define CH_PLAY_MODE		1
#define CH_LED_MODE			2
#define CH_REPEATE			3
#define	CH_MAX_DUTY 		4
#define	CH_MIN_DUTY 		5
#define CH_VOLUME			6
#define CH_LED_COLOR		7
#define CH_EQ				8

void iotvTask(void *pvParameters);

#endif /* MAIN_IOTV_H_ */
