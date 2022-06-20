set(srcs "sd_card_main.c" "lv_port_fs.c")
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES fatfs lvgl
                       )
