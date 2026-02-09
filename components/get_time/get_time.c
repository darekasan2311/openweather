#include "esp_sntp.h"
#include "esp_log.h"
#include "get_time.h"
#include "openweather.h"

static const char *TAG = "get_time";

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Time synchronized!");
}

void time_task(void *pvParameters)
{
    setenv("TZ", "JST-9", 1);  // Japan Standard Time (UTC+9)
    tzset();
    // Initialize SNTP
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_init();
    
    int retry = 0;
    const int retry_count = 10;
    while (esp_sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && retry < retry_count) {
        retry++;
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    
    while (1) {
        time_t now;
        struct tm timeinfo;
        time(&now);
        localtime_r(&now, &timeinfo);
        
        if (xSemaphoreTake(time_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            g_time_data.current_time = now;
            g_time_data.timeinfo = timeinfo;
            g_time_data.synced = (esp_sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED);
            xSemaphoreGive(time_mutex);
            
            xEventGroupSetBits(data_events, TIME_DATA_READY);
            
            // ESP_LOGI(TAG, "Current time: %04d-%02d-%02d %02d:%02d:%02d",
            //          timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
            //          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
