/*
 * factory_reset.h
 *
 *  Created on: 23 янв. 2024 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_UPDATE_UPDATE_H_
#define MAIN_SERVICE_UPDATE_UPDATE_H_

void update_service_task(void *pvParameters);
const char *update_service_task_name(void);
char *update_service_get_available_version(void);

#endif /* MAIN_SERVICE_UPDATE_UPDATE_H_ */
