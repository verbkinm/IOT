#include "tcp.h"

const static char *TAG = "TCP";
const static char *task_name = "tcp_server_task";

static void do_retransmit(const int sock)
{
	int len;
	char rx_buffer[BUFSIZE];

	do
	{
		len = recv(sock, rx_buffer, BUFSIZE, 0);

		if (len < 0)
			printf("%s Error occurred during receiving: errno %d\n", TAG, errno);
		else if (len == 0)
			printf("%s Connection closed\n", TAG);
		else
			iotv_data_recived(rx_buffer, len, sock);

		taskYIELD();
	} while (len > 0);
}

void tcp_server_task(void *pvParameters)
{
	printf("%s %s start\n", TAG, task_name);

	while (true)
	{
//		if (glob_get_status_err())
//			break;

		if (!(glob_get_status_reg() & STATUS_IP_GOT))
		{
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			continue;
		}

		char addr_str[128];
		int addr_family = AF_INET;//(int)pvParameters;
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
			dest_addr_ip4->sin_port = htons(TCP_PORT);
			ip_protocol = IPPROTO_IP;
		}

		int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
		if (listen_sock < 0)
		{
			printf("%s Unable to create socket: errno %d\n", TAG, errno);
			vTaskDelete(NULL);
			return;
		}

		int opt = 1;
		setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		printf("%s Socket created\n", TAG);

		int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
		if (err != 0)
		{
			printf("%s Socket unable to bind: errno %d\n", TAG, errno);
			printf("%s IPPROTO: %d\n", TAG, addr_family);
			goto CLEAN_UP;
		}
		printf("%s Socket bound, port %d\n", TAG, TCP_PORT);

		err = listen(listen_sock, 1);
		if (err != 0)
		{
			printf("%s Error occurred during listen: errno %d\n", TAG, errno);
			goto CLEAN_UP;
		}

		printf("%s Socket listening\n", TAG);

		struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
		socklen_t addr_len = sizeof(source_addr);
		int client_socket = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
		if (client_socket < 0)
		{
			printf("%s Unable to accept connection: errno %d\n", TAG, errno);
			break;
		}

		// Set tcp keepalive option
		setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
		setsockopt(client_socket, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
		setsockopt(client_socket, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
		setsockopt(client_socket, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));

		// Convert ip address to string
		if (source_addr.ss_family == PF_INET)
			inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);

		printf("%s Socket accepted ip address: %s\n", TAG, addr_str);
		glob_set_bits_status_reg(STATUS_TCP_CONNECTED);

		do_retransmit(client_socket);

		shutdown(client_socket, SHUT_RDWR); //0
		close(client_socket);
		glob_clear_bits_status_reg(STATUS_TCP_CONNECTED);

		iotv_clear_buf_data();

		CLEAN_UP:
		close(listen_sock);
	}

	printf("%s %s stop\n", TAG, task_name);
	vTaskDelete(NULL);
}
