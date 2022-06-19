set(srcs "esp_get_weather_time.c")
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include" 
                       PRIV_REQUIRES driver esp_timer
                       )
