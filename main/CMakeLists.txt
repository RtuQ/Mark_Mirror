idf_component_register(SRCS "app_main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES lcd_tft mpu6050 driver esp_timer spi_flash)
