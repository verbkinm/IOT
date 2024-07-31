/*
 * nvs.c
 *
 *  Created on: 7 мар. 2024 г.
 *      Author: user
 */

#include "local_nvs.h"

static const char *TAG = "local_nvs";

//int16_t readBorderDistanceFromNVS(void)
//{
//	int16_t borderDistance = BORDER_DISTANCE_DEFAULT; // значение по умолчанию
//
//	// Open
//	nvs_handle_t my_handle;
//	if (nvs_open("VL6180X", NVS_READONLY, &my_handle) != ESP_OK)
//	{
//		ESP_LOGE(TAG, "Error nvs_open");
//		return borderDistance;
//	}
//
//	// Read
//	if (nvs_get_i16(my_handle, "borderDistance", &borderDistance) != ESP_OK)
//		ESP_LOGE(TAG, "Error nvs_get_i16");
//
//	nvs_close(my_handle);
//	return borderDistance;
//}
//
//void writeBorderDistanceToNVS(int16_t value)
//{
//	// Open
//	nvs_handle_t my_handle;
//	if (nvs_open("VL6180X", NVS_READWRITE, &my_handle) != ESP_OK)
//	{
//		ESP_LOGE(TAG, "BorderDistanceToNVS open failed");
//		return;
//	}
//
//	// Write
//	if (nvs_set_i16(my_handle, "borderDistance", value) != ESP_OK)
//		ESP_LOGE(TAG, "writeBorderDistanceToNVS write failed");
//
//	// Commit written value.
//	if (nvs_commit(my_handle) != ESP_OK)
//		ESP_LOGE(TAG, "writeBorderDistanceToNVS commit failed");
//
//	// Close
//	nvs_close(my_handle);
//}
//
//uint8_t readDisplayOrientationFromNVS(void)
//{
//	uint8_t displayOrientation = DISPLAY_ORIENTATION_DEFAULT; // значение по умолчанию
//
//	// Open
//	nvs_handle_t my_handle;
//	if (nvs_open("DISP", NVS_READONLY, &my_handle) != ESP_OK)
//	{
//		ESP_LOGE(TAG, "Error nvs_open");
//		return displayOrientation;
//	}
//
//	// Read
//	if (nvs_get_u8(my_handle, "dispOrientation", &displayOrientation) != ESP_OK)
//		ESP_LOGE(TAG, "Error nvs_get_u8");
//
//	nvs_close(my_handle);
//	return displayOrientation;
//}
//
//void writeDisplayOrientationToNVS(uint8_t value)
//{
//	// Open
//	nvs_handle_t my_handle;
//	if (nvs_open("DISP", NVS_READWRITE, &my_handle) != ESP_OK)
//	{
//		ESP_LOGE(TAG, "displayOrientation open failed");
//		return;
//	}
//
//	// Write
//	if (nvs_set_u8(my_handle, "dispOrientation", value) != ESP_OK)
//		ESP_LOGE(TAG, "displayOrientation write failed");
//
//	// Commit written value.
//	if (nvs_commit(my_handle) != ESP_OK)
//		ESP_LOGE(TAG, "displayOrientation commit failed");
//
//	// Close
//	nvs_close(my_handle);
//}
