
#include "nvs_flash.h"
#include "lvgl_app.h"
#include <time.h>
#include <sys/time.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_timer.h"
#include "esp_app_smartconfig.h"
#include "mpu6050.h"


static const char *TAG = "app main";

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    esp_lvgl_device_app_init();
    // wifi初始化
    esp_smartconfig_wifi_init(); 

    mpu6050_init();
    
}

