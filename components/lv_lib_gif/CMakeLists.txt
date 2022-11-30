set(srcs "gifdec.c" "lv_gif.c")
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "." 
                       PRIV_REQUIRES lvgl lvgl_fs
                       )
