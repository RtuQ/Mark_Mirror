set(srcs "events_init.c" "gui_guider.c" "setup_scr_main.c"
         "setup_scr_screen.c"
         "guider_fonts/lv_font_MiSans_Normal_20.c"
         "guider_fonts/lv_font_MiSans_Normal_40.c"
         "guider_fonts/lv_font_MiSans_Normal_75.c"
         "guider_fonts/lv_font_simsun_12.c"
         "images/_hum_21x22.c"
         "images/_temp_30x25.c")
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "guider_customer_fonts" "."
                       "guider_fonts" 
                       PRIV_REQUIRES lvgl
                       )
