/*
 * registers.c
 *
 *  Created on: 2 авг. 2024 г.
 *      Author: user
 */

#include "registers.h"

static uint32_t glob_status_reg = 0;
static uint32_t glob_status_err = 0;
static uint32_t glob_update_reg = 0;
static uint32_t glob_service_reg = 0;

uint32_t glob_get_status_reg(void)
{
	return glob_status_reg;
}

void glob_set_status_reg(uint32_t reg)
{
	glob_status_reg = reg;
}

void glob_set_bits_status_reg(status_reg_flag_t bit)
{
	glob_status_reg |= bit;
}

void glob_clear_bits_status_reg(status_reg_flag_t bit)
{
	glob_status_reg &= ~bit;
}

uint32_t glob_get_status_err(void)
{
	return glob_status_err;
}

void glob_set_status_err(uint32_t reg)
{
	glob_status_err = reg;
}

void glob_set_bits_status_err(status_error_flag_t bit)
{
	glob_status_err |= bit;
}

void glob_clear_bits_status_err(status_error_flag_t bit)
{
	glob_status_err &= ~bit;
}

uint32_t glob_get_update_reg(void)
{
	return glob_update_reg;
}

void glob_set_update_reg(uint32_t reg)
{
	glob_update_reg = reg;
}

void glob_set_bits_update_reg(update_flag_t bit)
{
	glob_update_reg |= bit;
}

void glob_clear_bits_update_reg(update_flag_t bit)
{
	glob_update_reg &= ~bit;
}

uint32_t glob_get_service_reg(void)
{
	return glob_service_reg;
}

void glob_set_service_reg(uint32_t reg)
{
	glob_service_reg = reg;
}

void glob_set_bits_service_reg(service_flag_t bit)
{
	glob_service_reg |= bit;
}
void glob_clear_bits_service_reg(service_flag_t bit)
{
	glob_service_reg &= ~bit;
}
