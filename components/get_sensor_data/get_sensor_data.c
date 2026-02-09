#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "get_sensor_data.h"
#include "scd41.h"
#include "openweather.h"

// SCD41 I2C config
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_FREQ_HZ 100000

static const char *TAG = "get_sensor_data";

void sensor_task(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0));

    // Initialize SCD41
    scd41_config_t config = SCD41_CONFIG_DEFAULT();
    config.i2c_port = I2C_NUM_0;
    config.timeout_ms = 1000;

    ESP_ERROR_CHECK(scd41_init(&config));
    ESP_ERROR_CHECK(scd41_start_measurement());

    // Wait for first measurement (5 seconds)
    vTaskDelay(pdMS_TO_TICKS(5000));

    while (1) {
        scd41_data_t data;
        esp_err_t ret = scd41_read_measurement(&data);

        if (ret == ESP_OK && data.data_ready) {
            if (xSemaphoreTake(sensor_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                g_sensor_data.co2_ppm = data.co2_ppm;
                g_sensor_data.temperature = data.temperature;
                g_sensor_data.humidity = data.humidity;
                g_sensor_data.timestamp = esp_timer_get_time() / 1000; // ms
                xSemaphoreGive(sensor_mutex);
                
                // Signal new data is ready
                xEventGroupSetBits(data_events, SENSOR_DATA_READY);
                // ESP_LOGI(TAG, "CO2: %d ppm, Temperature: %.1f°C, Humidity: %.1f%%",
                //         data.co2_ppm, data.temperature, data.humidity);
            }
        } else {
            ESP_LOGW(TAG, "Failed to read sensor data");
        }
        // SCD41 provides new data every 5 seconds
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
