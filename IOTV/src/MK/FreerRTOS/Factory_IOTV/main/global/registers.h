/*
 * registers.h
 *
 *  Created on: 2 авг. 2024 г.
 *      Author: user
 */

#ifndef MAIN_GLOBAL_REGISTERS_H_
#define MAIN_GLOBAL_REGISTERS_H_

#include "global_def.h"

uint32_t glob_get_status_reg(void);
void glob_set_status_reg(uint32_t reg);
void glob_set_bits_status_reg(status_reg_flag_t bits);
void glob_clear_bits_status_reg(status_reg_flag_t bits);

uint32_t glob_get_status_err(void);
void glob_set_status_err(uint32_t reg);
void glob_set_bits_status_err(status_error_flag_t bits);
void glob_clear_bits_status_err(status_error_flag_t bits);

uint32_t glob_get_update_reg(void);
void glob_set_update_reg(uint32_t reg);
void glob_set_bits_update_reg(update_flag_t bits);
void glob_clear_bits_update_reg(update_flag_t bits);

uint32_t glob_get_service_reg(void);
void glob_set_service_reg(uint32_t reg);
void glob_set_bits_service_reg(service_flag_t bits);
void glob_clear_bits_service_reg(service_flag_t bits);

#endif /* MAIN_GLOBAL_REGISTERS_H_ */
