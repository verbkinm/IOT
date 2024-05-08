/*
 * local_lib.c
 *
 *  Created on: 10 мая 2023 г.
 *      Author: user
 */

#include "local_lib.h"

static uint32_t glob_status_reg = 0;
static uint32_t glob_status_err = 0;
static uint32_t glob_update_reg = 0;

uint8_t bcdToDec(uint8_t val)
{
	return( (val / 16 * 10) + (val % 16) );
}

uint8_t decToBcd(uint8_t val)
{
	return( (val / 10 * 16) + (val % 10) );
}

void setBitInByte(uint8_t *byte, bool bitValue, uint8_t bitNumber)
{
	if (byte == NULL)
		return;

	if (bitValue)
		*byte |= (1 << bitNumber);
	else
		*byte &= ~(1 << bitNumber);
}

double inRange(double val, double min, double max)
{
	if (val < min)
		return min;
	else if (val > max)
		return max;

	return val;
}

void strmac_to_arr(const char *str, uint8_t mac[6])
{
    int arr[6];
    sscanf(str, "%x:%x:%x:%x:%x:%x*c",
           &arr[0], &arr[1], &arr[2],
           &arr[3], &arr[4], &arr[5]);

    for (uint8_t i = 0; i < 6; ++i)
        mac[i] = arr[i];
}

char *url_encode(const char *s)
{
    //char rfc3986[256] = {0};
    //char html5[256] = {0};

    unsigned char *html5 = calloc(256, 1);
    char *enc = calloc(strlen(s) * 3 + 1, 1);
    char *enc_begin = enc;

    for (int i = 0; i < 256; i++)
    {
        //        rfc3986[i] = isalnum( i) || i == '~' || i == '-' || i == '.' || i == '_' ? i : 0;
        html5[i] = isalnum( i) || i == '*' || i == '-' || i == '.' || i == '_' ? i : (i == ' ') ? '+' : 0;
    }

    for (; *s; s++)
    {
        if (html5[(int)*s])
            *enc = html5[(int)*s];
        else
            sprintf(enc, "%%%02X", (unsigned char)*s);
        while (*++enc);
    }

    free(html5);
    return(enc_begin);
}

char *url_decode(const char *str)
{
	char *dStr = (char *) malloc(strlen(str) + 1);
	memset(dStr, 0, strlen(str) + 1);
	char eStr[] = "00"; /* for a hex code */

	strcpy(dStr, str);

	int i;

	for(i=0;i<strlen(dStr);++i)
	{
		if(dStr[i] == '%')
		{
			if(dStr[i+1] == 0)
				return dStr;

			if(isxdigit((int)dStr[i+1]) && isxdigit((int)dStr[i+2]))
			{
				eStr[0] = dStr[i+1];
				eStr[1] = dStr[i+2];

				long int x = strtol(eStr, NULL, 16);

				memmove(&dStr[i+1], &dStr[i+3], strlen(&dStr[i+3])+1);

				dStr[i] = x;
			}
		}
		else if(dStr[i] == '+') { dStr[i] = ' '; }
	}

	return dStr;
}

double convert_range(double value, double From1, double From2, double To1, double To2)
{
      return (value-From1)/(From2-From1)*(To2-To1)+To1;
}


uint32_t glob_get_status_reg(void)
{
	return glob_status_reg;
}

void glob_set_status_reg(uint32_t reg)
{
	glob_status_reg = reg;
}

void glob_set_bits_status_reg(uint32_t bits)
{
	glob_status_reg |= bits;
}

void glob_clear_bits_status_reg(uint32_t bits)
{
	glob_status_reg &= ~bits;
}

uint32_t glob_get_status_err(void)
{
	return glob_status_err;
}

void glob_set_status_err(uint32_t reg)
{
	glob_status_err = reg;
}

void glob_set_bits_status_err(uint32_t bits)
{
	glob_status_err |= bits;
}

void glob_clear_bits_status_err(uint32_t bits)
{
	glob_status_err &= ~bits;
}

uint32_t glob_get_update_reg(void)
{
	return glob_update_reg;
}

void glob_set_update_reg(uint32_t reg)
{
	glob_update_reg = reg;
}

void glob_set_bits_update_reg(uint32_t bits)
{
	glob_update_reg |= bits;
}

void glob_clear_bits_update_reg(uint32_t bits)
{
	glob_update_reg &= ~bits;
}
