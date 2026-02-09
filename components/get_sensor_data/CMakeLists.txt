idf_component_register(SRCS "get_sensor_data.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver chiehmin__scd41 main esp_timer)
