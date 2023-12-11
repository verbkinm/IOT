/*
 * json_config.c
 *
 *  Created on: 11 дек. 2023 г.
 *      Author: user
 */

#include "json_config.h"

bool set_wifi_config_value(const char *key, const char *value)
{
    FILE *file = fopen("/sdcard/conf.jsn", "r");
    if (!file)
    {
        fprintf(stderr, "File not open\n");
        return false;
    }

    char *content[BUFSIZ] = {0};
    fread(content, BUFSIZ, 1, file);
    fclose(file);

    cJSON *monitor = cJSON_Parse(content);

    cJSON *jwifi = cJSON_GetObjectItemCaseSensitive(monitor, "wifi");
    if (jwifi == NULL)
        goto bad_end;

    cJSON *jkey = cJSON_GetObjectItemCaseSensitive(jwifi, key);
    if (jkey == NULL)
        goto bad_end;

    cJSON_SetValuestring(jkey, value);

    size_t len = strlen(cJSON_GetStringValue(jkey));
    if (len == 0)
        goto bad_end;

    char *print = NULL;
    print = cJSON_Print(monitor);

    if (print == NULL)
        goto bad_end;

    file = fopen("/sdcard/conf.jsn", "w");
    if (file == NULL)
        goto bad_end;

    fprintf(file, "%s", print);
    fclose(file);

    cJSON_Delete(monitor);
    free(print);
    return true;

bad_end:
    cJSON_Delete(monitor);
    return false;
}


char *get_wifi_config_value(const char *monitor, const char *key)
{
    cJSON *obj = NULL;
    cJSON *results = NULL;
    cJSON *monitor_json = cJSON_Parse(monitor);

    if (monitor_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
            fprintf(stdout, "Error before: %s\n", error_ptr);
    }

    results = cJSON_GetObjectItemCaseSensitive(monitor_json, "wifi");
    if (results == NULL)
    {
        cJSON_Delete(monitor_json);
        return NULL;
    }

    obj = cJSON_GetObjectItemCaseSensitive(results, key);
    if (obj == NULL)
    {
        cJSON_Delete(monitor_json);
        return NULL;
    }

    size_t len = strlen(cJSON_GetStringValue(obj));
    if (len == 0)
        return 0;

    char *val = calloc(1, len + 1);
    if (val == NULL)
        return NULL;

    memcpy(val, cJSON_GetStringValue(obj), len);

    cJSON_Delete(monitor_json);
    return val;
}
