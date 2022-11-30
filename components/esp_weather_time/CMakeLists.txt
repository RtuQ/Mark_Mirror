set(srcs "esp_weather_time.c")
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include" 
                       PRIV_REQUIRES driver esp_timer esp_wifi wpa_supplicant nvs_flash
                                     json esp_http_client esp-tls lvgl lvgl_gui
                       )
