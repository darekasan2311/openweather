#include <stdio.h>
#include <string.h>
#include "wifi_connect.h"
#include "get_weather.h"
#include "st7789.h"

static const char *TAG = "openweather";

void app_main(void)
{
    init_lcd(0);
    wifi_connection_task();

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("WIFI was initiated ...........\n\n");
 
    get_weather_data();
}
