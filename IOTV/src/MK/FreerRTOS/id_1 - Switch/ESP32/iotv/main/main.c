#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "freertos/event_groups.h"

#include "iotv.h"

#define PORT                        CONFIG_EXAMPLE_PORT
#define KEEPALIVE_IDLE              CONFIG_EXAMPLE_KEEPALIVE_IDLE
#define KEEPALIVE_INTERVAL          CONFIG_EXAMPLE_KEEPALIVE_INTERVAL
#define KEEPALIVE_COUNT             CONFIG_EXAMPLE_KEEPALIVE_COUNT

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT		BIT0
#define WIFI_DISCONNECTED_BIT 	BIT1
#define WIFI_FAIL_BIT      		BIT2

extern uint64_t realBufSize, expextedDataSize;
static const char *TAG = "main";

QueueHandle_t xQueueInData, xQueueOutData;
static TaskHandle_t xHandleWriteData;

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

static void writeData(void *pvParameters)
{
	ESP_LOGW(TAG, "writeData task created");
	struct DataPkg pkg = { NULL, 0 };

	while (true)
	{
		if (xQueueReceive(xQueueOutData, (void *)&pkg, portMAX_DELAY) == pdPASS)
		{
			if (send(*(int *)pvParameters, pkg.data, pkg.size, 0) < 0)
			{
				ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
				break;
			}
			free(pkg.data);
		}
		else
		{
			ESP_LOGE(TAG, "xQueueReceive( xQueueOutData, pkg, ( TickType_t ) 10 ) != pdPASS");
			//			esp_restart();
		}
	}

	vTaskDelete(NULL);
	xHandleWriteData = NULL;
}

static void do_retransmit(const int sock)
{
	int len;
	uint8_t rx_buffer[BUFSIZE];
	struct DataPkg pkg = { NULL, 0 };

	if (xTaskCreate(writeData, "writeData_Task", 2048,  (void *)&sock, 2, &xHandleWriteData ) != pdPASS)
	{
		ESP_LOGE(TAG, "xTaskCreate(writeData, \"writeData_Task\", 4096,  NULL, 2, &xHandleWriteData ) != pdPASS");
		return;
	}

	do
	{
		len = recv(sock, rx_buffer, BUFSIZE, 0);
		if (len < 0)
		{
			ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
		}
		else if (len == 0)
		{
			ESP_LOGW(TAG, "Connection closed");
		}
		else
		{
			pkg.data = malloc(len);
			memcpy(pkg.data, rx_buffer, len);
			pkg.size = len;

			if( xQueueSend(xQueueInData, (void *)&pkg, (TickType_t)10) != pdPASS )
			{
				ESP_LOGE(TAG, "xQueueSend( xQueueOutData, (void *)&pkg, ( TickType_t ) 10 ) != pdPASS");
				esp_restart();
			}
			taskYIELD();
		}
	} while (len > 0);
}

static void tcp_server_task(void *pvParameters)
{
	char addr_str[128];
	int addr_family = (int)pvParameters;
	int ip_protocol = 0;
	int keepAlive = 1;
	int keepIdle = KEEPALIVE_IDLE;
	int keepInterval = KEEPALIVE_INTERVAL;
	int keepCount = KEEPALIVE_COUNT;
	struct sockaddr_storage dest_addr;

	if (addr_family == AF_INET) {
		struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
		dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
		dest_addr_ip4->sin_family = AF_INET;
		dest_addr_ip4->sin_port = htons(PORT);
		ip_protocol = IPPROTO_IP;
	}

	int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
	if (listen_sock < 0) {
		ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
		vTaskDelete(NULL);
		return;
	}
	int opt = 1;
	setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	ESP_LOGI(TAG, "Socket created");

	int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if (err != 0) {
		ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
		ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
		goto CLEAN_UP;
	}
	ESP_LOGI(TAG, "Socket bound, port %d", PORT);

	err = listen(listen_sock, 1);
	if (err != 0) {
		ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
		goto CLEAN_UP;
	}

	while (true)
	{
		ESP_LOGI(TAG, "Socket listening");

		struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
		socklen_t addr_len = sizeof(source_addr);
		int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
		if (sock < 0) {
			ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
			break;
		}

		// Set tcp keepalive option
		setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
		setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
		setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
		setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
		// Convert ip address to string
		if (source_addr.ss_family == PF_INET)
		{
			inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
		}
		ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);

		do_retransmit(sock);

		if (xHandleWriteData != NULL)
		{
			vTaskDelete(xHandleWriteData);
			ESP_LOGW(TAG, "writeData task delete");
		}

		shutdown(sock, SHUT_RDWR); //0
		close(sock);
	}

	CLEAN_UP:
	close(listen_sock);
	vTaskDelete(NULL);
}

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		esp_wifi_connect();
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		realBufSize = 0;
		expextedDataSize = 0;

		if (true) //!!!
		{
			esp_wifi_connect();
			//            s_retry_num++;
			ESP_LOGI(TAG, "retry to connect to the AP");
		}
		else
			xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);

		ESP_LOGI(TAG, "disconnect from AP");
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
		ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
		ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
	}
}

void wifi_init_sta(void)
{
	s_wifi_event_group = xEventGroupCreate();

	ESP_ERROR_CHECK(esp_netif_init());

	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
			ESP_EVENT_ANY_ID,
			&event_handler,
			NULL,
			&instance_any_id));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
			IP_EVENT_STA_GOT_IP,
			&event_handler,
			NULL,
			&instance_got_ip));

	wifi_config_t wifi_config = {
			.sta = {
					.ssid = CONFIG_EXAMPLE_WIFI_SSID,
					.password = CONFIG_EXAMPLE_WIFI_PASSWORD,
					 .threshold.authmode = WIFI_AUTH_WPA2_PSK,
					 .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
			},
	};
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
	ESP_ERROR_CHECK(esp_wifi_start() );

	ESP_LOGI(TAG, "wifi_init_sta finished.");

	/* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
	 * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
	EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
			WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
			pdFALSE,
			pdFALSE,
			portMAX_DELAY);

	/* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
	 * happened. */
	if (bits & WIFI_CONNECTED_BIT)
		ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD);
	else if (bits & WIFI_FAIL_BIT)
		ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD);
	else
		ESP_LOGE(TAG, "UNEXPECTED EVENT");
}

void app_main(void)
{
	//Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
	wifi_init_sta();

	xQueueInData = xQueueCreate(10, sizeof(struct DataPkg));
	xQueueOutData = xQueueCreate(10, sizeof(struct DataPkg));

	if (xQueueInData == NULL  || xQueueOutData == NULL)
	{
		ESP_LOGE(TAG, "Queue was not created and must not be used");
		esp_restart();
	}

	xTaskCreate(iotvTask, "iotvTask", 2048, NULL, 1, NULL);
	vTaskDelay(100 / portTICK_PERIOD_MS);

	xTaskCreate(Vl6180X_Task, "Vl6180X_Task", 2048, NULL, 2, NULL);
	xTaskCreate(tcp_server_task, "tcp_server", 4096, (void*)AF_INET, 1, NULL);

//	while(true)
//	{
//		printf("Heap - %d Bytes\n", (int)esp_get_free_heap_size());
//		vTaskDelay(1000 / portTICK_PERIOD_MS);
//	}
}


