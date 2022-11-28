
#include "nvs_flash.h"
#include "esp_app_smartconfig.h"


void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    esp_smartconfig_wifi_init();
}

