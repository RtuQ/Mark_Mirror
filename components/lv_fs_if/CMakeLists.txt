set(srcs "lv_fs_fatfs.c")
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES fatfs lvgl sd_card
                       )
