set(srcs "esp32_wifi_main.c")
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "." 
                       PRIV_REQUIRES driver esp_timer
                       )
