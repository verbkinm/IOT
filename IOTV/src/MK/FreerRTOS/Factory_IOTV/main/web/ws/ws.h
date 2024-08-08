/*
 * ws.h
 *
 *  Created on: 7 авг. 2024 г.
 *      Author: user
 */

#ifndef MAIN_WEB_WS_WS_H_
#define MAIN_WEB_WS_WS_H_

#include "esp_http_server.h"
#include "esp_err.h"

struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
    char *data;
    size_t dataSize;
};

esp_err_t ws_handler(httpd_req_t *req);

#endif /* MAIN_WEB_WS_WS_H_ */
