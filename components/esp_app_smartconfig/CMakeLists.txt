set(srcs "esp_app_smartconfig.c")
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include" 
                       PRIV_REQUIRES driver esp_timer esp_wifi wpa_supplicant nvs_flash
                       )
