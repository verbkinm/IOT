/*
 * udp_client.c
 *
 *  Created on: 8 мая 2024 г.
 *      Author: user
 */

#include "udp_client.h"
#include "deep_sleep/deep_sleep.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include <esp_log.h>

static const char *TAG = "UDP_CLIENT";

static void udp_client_send_data(void)
{
	char *payload = "Data send -> ";

	int addr_family, ip_protocol;

	struct sockaddr_in dest_addr;
	dest_addr.sin_addr.s_addr = inet_addr(UDP_SERVER_ADDR);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(UDP_SERVER_PORT);
	addr_family = AF_INET;
	ip_protocol = IPPROTO_IP;

	int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
	if (sock < 0)
	{
		ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
		return;
	}
    // Set timeout
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

	ESP_LOGI(TAG, "Socket created, sending to %s:%d", UDP_SERVER_ADDR, UDP_SERVER_PORT);

	int err = sendto(sock, payload, strlen(payload), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if (err < 0)
	{
		ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
		return;
	}

	ESP_LOGI(TAG, "Message sent");


    if (sock != -1) {
        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
    }
}
void udp_client_service_task(void *pvParameters)
{
	vTaskDelay(DELAYED_LAUNCH / portTICK_PERIOD_MS);

	for( ;; )
	{
		if (glob_get_status_err())
			break;

		if (glob_get_status_reg() & STATUS_IP_GOT)
		{
			// читаем данные dht22
			// посылаем их на udp server
			udp_client_send_data();
			// спать...
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			deep_sleep();
		}

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);

}
