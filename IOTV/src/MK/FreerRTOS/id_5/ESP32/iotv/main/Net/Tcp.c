#include "Tcp.h"

extern uint64_t realBufSize, expextedDataSize;
extern QueueHandle_t xQueueInData, xQueueOutData;

static const char *TAG = "TCP";
static TaskHandle_t xHandleWriteData;

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
			ESP_LOGE(TAG, "xQueueReceive( xQueueOutData, pkg, ( TickType_t ) 10 ) != pdPASS");
	}

//	realBufSize = 0;
//	expextedDataSize = 0;

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
				ESP_LOGE(TAG, "xQueueSend( xQueueOutData, (void *)&pkg, ( TickType_t ) 10 ) != pdPASS");

			taskYIELD();
		}
	} while (len > 0);
}

void tcp_server_task(void *pvParameters)
{
	char addr_str[128];
	int addr_family = (int)pvParameters;
	int ip_protocol = 0;
	int keepAlive = 1;
	int keepIdle = KEEPALIVE_IDLE;
	int keepInterval = KEEPALIVE_INTERVAL;
	int keepCount = KEEPALIVE_COUNT;
	struct sockaddr_storage dest_addr;

	if (addr_family == AF_INET)
	{
		struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
		dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
		dest_addr_ip4->sin_family = AF_INET;
		dest_addr_ip4->sin_port = htons(PORT);
		ip_protocol = IPPROTO_IP;
	}

	int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
	if (listen_sock < 0)
	{
		ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
		vTaskDelete(NULL);
		return;
	}

	int opt = 1;
	setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	ESP_LOGI(TAG, "Socket created");

	int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if (err != 0)
	{
		ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
		ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
		goto CLEAN_UP;
	}
	ESP_LOGI(TAG, "Socket bound, port %d", PORT);

	err = listen(listen_sock, 1);
	if (err != 0)
	{
		ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
		goto CLEAN_UP;
	}

	while (true)
	{
		ESP_LOGI(TAG, "Socket listening");

		struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
		socklen_t addr_len = sizeof(source_addr);
		int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
		if (sock < 0)
		{
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
		LedSignals_setTCPState(true);

		do_retransmit(sock);

		if (xHandleWriteData != NULL)
		{
			vTaskDelete(xHandleWriteData);
			ESP_LOGW(TAG, "writeData task delete");
		}

		shutdown(sock, SHUT_RDWR); //0
		close(sock);
		LedSignals_setTCPState(false);

		realBufSize = 0;
		expextedDataSize = 0;
	}

	CLEAN_UP:
	close(listen_sock);
	vTaskDelete(NULL);
}
