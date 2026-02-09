// openweather.h
#ifndef OPENWEATHER_H
#define OPENWEATHER_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include <time.h>

// Event bits
#define SENSOR_DATA_READY   BIT0
#define TIME_DATA_READY     BIT1
#define WEATHER_DATA_READY  BIT2
#define WIFI_READY  BIT3

extern EventGroupHandle_t data_events;
extern SemaphoreHandle_t sensor_mutex;
extern SemaphoreHandle_t time_mutex;
extern SemaphoreHandle_t weather_mutex;

typedef struct {
    uint16_t co2_ppm;
    float temperature;
    float humidity;
    uint64_t timestamp;
} sensor_data_t;

typedef struct {
    float temperature;
    float feels_like;
    int humidity;
    char condition[64];
    float wind_speed;
    uint64_t updated_at;
} weather_data_t;

typedef struct {
    time_t current_time;
    struct tm timeinfo;
    bool synced;
} time_data_t;

extern sensor_data_t g_sensor_data;
extern weather_data_t g_weather_data;
extern time_data_t g_time_data;

#endif // OPENWEATHER_H
