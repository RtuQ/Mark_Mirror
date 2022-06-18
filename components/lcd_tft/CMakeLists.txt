set(srcs "lv_port_disp.c" "st7789.c")
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include" 
                       PRIV_REQUIRES lvgl driver
                       )
