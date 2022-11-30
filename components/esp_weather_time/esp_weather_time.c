#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_timer.h"
#include "esp_event.h"
#include "esp_weather_time.h"

#include "lvgl.h"
#include "gui_guider.h"

#include "esp_http_client.h"
#include "cJSON.h"
#include "esp_tls.h"


#define SNTP_MAX_SERVERS     1
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif


ESP_EVENT_DEFINE_BASE(NTP_TIME_EVENT);

esp_timer_handle_t check_time_timer;
static const char *TAG = "weather time";

static void initialize_sntp(void);


void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}


void esp32_get_time(void)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        // update 'now' variable with current time
        time(&now);
    }

}

static void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);

/*
 * If 'NTP over DHCP' is enabled, we set dynamic pool address
 * as a 'secondary' server. It will act as a fallback server in case that address
 * provided via NTP over DHCP is not accessible
 */
    sntp_setservername(0, "ntp1.aliyun.com");

#if LWIP_IPV6 && SNTP_MAX_SERVERS > 2          // statically assigned IPv6 address is also possible
    ip_addr_t ip6;
    if (ipaddr_aton("2a01:3f7::1", &ip6)) {    // ipv6 ntp source "ntp.netnod.se"
        sntp_setserver(2, &ip6);
    }
#endif  /* LWIP_IPV6 */

    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();

    ESP_LOGI(TAG, "List of configured NTP servers:");

    for (uint8_t i = 0; i < SNTP_MAX_SERVERS; ++i){
        if (sntp_getservername(i)){
            ESP_LOGI(TAG, "server %d: %s", i, sntp_getservername(i));
        } else {
            // we have either IPv4 or IPv6 address, let's print it
            char buff[INET6_ADDRSTRLEN];
            ip_addr_t const *ip = sntp_getserver(i);
            if (ipaddr_ntoa_r(ip, buff, INET6_ADDRSTRLEN) != NULL)
                ESP_LOGI(TAG, "server %d: %s", i, buff);
        }
    }
}

static void __check_time_timer_callback(void* arg)
{
    ESP_LOGI(TAG, "check time is geted?");
    if (sntp_get_sync_status() >= SNTP_SYNC_STATUS_COMPLETED) {
        ESP_LOGI(TAG, "time get!!!!!");
        ESP_ERROR_CHECK(esp_event_post( NTP_TIME_EVENT, 0, NULL, 0, 0));
        return;
    }
    ESP_ERROR_CHECK(esp_timer_start_once(check_time_timer, 2000000));
}


void esp_internet_time_init(void)
{
    /**
     * NTP server address could be aquired via DHCP,
     * see following menuconfig options:
     * 'LWIP_DHCP_GET_NTP_SRV' - enable STNP over DHCP
     * 'LWIP_SNTP_DEBUG' - enable debugging messages
     *
     * NOTE: This call should be made BEFORE esp aquires IP address from DHCP,
     * otherwise NTP option would be rejected by default.
     */
#ifdef LWIP_DHCP_GET_NTP_SRV
    sntp_servermode_dhcp(1);      // accept NTP offers from DHCP server, if any
#endif

    initialize_sntp();

    
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &__check_time_timer_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "esp_check_time"
    };

    
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &check_time_timer));

    ESP_ERROR_CHECK(esp_timer_start_once(check_time_timer, 2000000));
}


static void __disp_weather_task(void* pvParameters)
{
    char output_buffer[2048] = { 0 }; //用于接收通过http协议返回的数据
    int content_length = 0; // http协议头的长度
    char weather_buffer[100] = {0};
    char icon_buffer[20] = {0};

    //定义http配置结构体，并且进行清零
    esp_http_client_config_t config;
    memset(&config, 0, sizeof(config));

    //向配置结构体内部写入url
    static const char* URL = "https://devapi.qweather.com/v7/weather/now?location=101210101&key=0bbd0212a5bd4856bc7046fd6757dcce&gzip=n&lang=en";
    config.url = URL;
    // config.event_handler = _http_event_handler;

    //初始化结构体
    esp_http_client_handle_t client = esp_http_client_init(&config); //初始化http连接

    //设置发送请求
    esp_http_client_set_method(client, HTTP_METHOD_GET);

    while (1) {

        // 与目标主机创建连接，并且声明写入内容长度为0
        esp_err_t err = esp_http_client_open(client, 0);

        //如果连接失败
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        } else {

            //读取目标主机的返回内容的协议头
            content_length = esp_http_client_fetch_headers(client);

            //如果协议头长度小于0，说明没有成功读取到
            if (content_length < 0) {
                ESP_LOGE(TAG, "HTTP client fetch headers failed");
            }

            //如果成功读取到了协议头
            else {

                //读取目标主机通过http的响应内容
                int data_read = esp_http_client_read_response(client, output_buffer, 2048);
                if (data_read >= 0) {

                    ESP_LOGI(TAG,"%s",output_buffer);
                    // //对接收到的数据作相应的处理
                    cJSON* root = NULL;
                    root = cJSON_Parse(output_buffer);
                    if (root != NULL) {
                        cJSON* cjson_item =cJSON_GetObjectItem(root,"now");
                        // cJSON* cjson_results =  cJSON_GetArrayItem(cjson_item,0);
                        // cJSON* cjson_now = cJSON_GetObjectItem(cjson_results,"now");
                        cJSON* cjson_temperature = cJSON_GetObjectItem(cjson_item,"temp");
                        cJSON* cjson_hum = cJSON_GetObjectItem(cjson_item,"humidity");
                        cJSON* cjson_weather = cJSON_GetObjectItem(cjson_item,"text");
                        cJSON* cjson_windDir = cJSON_GetObjectItem(cjson_item,"windDir");
                        cJSON* cjson_windScale = cJSON_GetObjectItem(cjson_item,"windScale");
                        cJSON* cjson_windSpeed = cJSON_GetObjectItem(cjson_item,"windSpeed");
                        cJSON* cjson_icon = cJSON_GetObjectItem(cjson_item,"icon");
                        lv_label_set_text(guider_ui.main_label_7,cjson_temperature->valuestring);
                        lv_bar_set_value(guider_ui.main_bar_1,atoi(cjson_temperature->valuestring),LV_ANIM_ON);
                        lv_label_set_text(guider_ui.main_label_8,cjson_hum->valuestring);
                        lv_bar_set_value(guider_ui.main_bar_2,atoi(cjson_hum->valuestring),LV_ANIM_ON);

                        memset(weather_buffer,0,sizeof(weather_buffer));
                        sprintf(weather_buffer,"weather:%s windDir:%s windScale:%s windSpeed:%s",cjson_weather->valuestring,\
                                cjson_windDir->valuestring,cjson_windScale->valuestring,cjson_windSpeed->valuestring);
                        lv_label_set_text(guider_ui.main_label_6,weather_buffer);
                        
                        memset(icon_buffer,0,sizeof(icon_buffer));
                        sprintf(icon_buffer,"S:/weather/%s.bin",cjson_icon->valuestring);
                        ESP_LOGI(TAG,"%s",icon_buffer);
                        lv_img_set_src(guider_ui.main_img_3,icon_buffer);

                        cJSON_Delete(root);
                    } else {
                        ESP_LOGE(TAG,"root is null");
                    }
                } else {
                    ESP_LOGE(TAG, "Failed to read response");
                }
            }
        }
        //关闭连接
        esp_http_client_close(client);
        vTaskDelay(100000 / portTICK_PERIOD_MS);
    }
}

void esp_start_sync_weather(void)
{

    xTaskCreatePinnedToCore(__disp_weather_task, "disp_weather", 4096 * 2, NULL, 4, NULL, 1);
}
