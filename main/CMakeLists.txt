set(USER_LIBS_PATH)
file(GLOB_RECURSE USER_LIBS_PATH ./src/*.c)
idf_component_register(SRCS ${USER_LIBS_PATH}
                       INCLUDE_DIRS "." "include"
                    PRIV_REQUIRES driver esp_timer nvs_flash esp_app_smartconfig esp_wifi 
                                  lvgl lvgl_fs lvgl_gui lvgl_port_driver esp_weather_time lv_lib_gif
                                  mpu6050)
