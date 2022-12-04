set(srcs "mpu6050.c" "inv_mpu_dmp_motion_driver.c" "inv_mpu.c")
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include" 
                       PRIV_REQUIRES driver esp_timer
                       )
