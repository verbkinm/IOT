/*
 * urldecode.c
 *
 *  Created on: 28 сент. 2023 г.
 *      Author: user
 */


#include "urldecode.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char *urlDecode(const char *str)
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
