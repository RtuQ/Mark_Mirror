set(USER_LIBS_PATH)
file(GLOB_RECURSE USER_LIBS_PATH ./src/*.c)
idf_component_register(SRCS ${USER_LIBS_PATH}
                       INCLUDE_DIRS "." "include/smartconfig")
                    # PRIV_REQUIRES lcd_tft mpu6050 driver esp_timer spi_flash nvs_flash get_weather_time
                    #               gui json esp_http_client esp-tls sd_card lv_fs_if)
