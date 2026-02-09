idf_component_register(SRCS "get_weather.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_http_client json main esp_timer
                    )
