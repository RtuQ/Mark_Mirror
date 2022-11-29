
#include "nvs_flash.h"
#include "lvgl_app.h"
#include <time.h>
#include <sys/time.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_app_smartconfig.h"
#include "esp_weather_time.h"


static const char *TAG = "app main";


static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "waitting to get internet time!!");
        esp_internet_time_init();
        esp_start_sync_weather();
    } else if (event_base == NTP_TIME_EVENT) {
        // wait for time to be set
        time_t now = 0;
        struct tm timeinfo = { 0 };
        time(&now);
        localtime_r(&now, &timeinfo);
        ESP_LOGI(TAG,"Now time:%d:%d:%d",timeinfo.tm_hour+8,timeinfo.tm_min,timeinfo.tm_sec);        
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );

    esp_lvgl_device_app_init();

    // wifi初始化
    esp_smartconfig_wifi_init();

    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(NTP_TIME_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
}

