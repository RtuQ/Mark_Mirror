/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "esp_chip_info.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "esp_http_client.h"
#include "cJSON.h"
#include "esp_tls.h"

#include "esp_get_weather_time.h"
#include "inv_mpu.h"
#include "mpu6050.h"

#include "lv_fs_fatfs.h"

#include "lv_port_disp.h"
#include "lvgl.h"

#include "gui_guider.h"


static const char *TAG = "app main";

/*********************
 *      DEFINES
 *********************/
#define LV_TICK_PERIOD_MS 1

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lvgl_tick_inc(void* arg);
static void gui_tim_task(void* pvParameter);
static void guiTask(void* pvParameter);
static void __disp_weather_task(void* pvParameters);


static lv_style_t style_btn;
static lv_style_t style_btn_pressed;
static lv_style_t style_btn_red;

lv_ui guider_ui;
lv_timer_t* disp_tim_task = NULL;
lv_timer_t* disp_weather_task = NULL;

static lv_color_t darken(const lv_color_filter_dsc_t* dsc, lv_color_t color, lv_opa_t opa)
{
    LV_UNUSED(dsc);
    return lv_color_darken(color, opa);
}

static void style_init(void)
{
    /*Create a simple button style*/
    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, 10);
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_bg_color(&style_btn, lv_palette_lighten(LV_PALETTE_GREY, 3));
    lv_style_set_bg_grad_color(&style_btn, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);

    lv_style_set_border_color(&style_btn, lv_color_black());
    lv_style_set_border_opa(&style_btn, LV_OPA_20);
    lv_style_set_border_width(&style_btn, 2);

    lv_style_set_text_color(&style_btn, lv_color_black());

    /*Create a style for the pressed state.
     *Use a color filter to simply modify all colors in this state*/
    static lv_color_filter_dsc_t color_filter;
    lv_color_filter_dsc_init(&color_filter, darken);
    lv_style_init(&style_btn_pressed);
    lv_style_set_color_filter_dsc(&style_btn_pressed, &color_filter);
    lv_style_set_color_filter_opa(&style_btn_pressed, LV_OPA_20);

    /*Create a red style. Change only some colors.*/
    lv_style_init(&style_btn_red);
    lv_style_set_bg_color(&style_btn_red, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_bg_grad_color(&style_btn_red, lv_palette_lighten(LV_PALETTE_GREEN, 3));
}

/**
 * Create styles from scratch for buttons.
 */
void lv_example_get_started_2(void)
{
    /*Initialize the style*/
    style_init();

    /*Create a button and use the new styles*/
    lv_obj_t* btn = lv_btn_create(lv_scr_act());
    /* Remove the styles coming from the theme
     * Note that size and position are also stored as style properties
     * so lv_obj_remove_style_all will remove the set size and position too */
    lv_obj_remove_style_all(btn);
    lv_obj_set_pos(btn, 10, 10);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_add_style(btn, &style_btn, 0);
    lv_obj_add_style(btn, &style_btn_pressed, LV_STATE_PRESSED);

    /*Add a label to the button*/
    lv_obj_t* label = lv_label_create(btn);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);

    /*Create another button and use the red style too*/
    lv_obj_t* btn2 = lv_btn_create(lv_scr_act());
    lv_obj_remove_style_all(btn2); /*Remove the styles coming from the theme*/
    lv_obj_set_pos(btn2, 10, 80);
    lv_obj_set_size(btn2, 120, 50);
    lv_obj_add_style(btn2, &style_btn, 0);
    lv_obj_add_style(btn2, &style_btn_red, 0);
    lv_obj_add_style(btn2, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_set_style_radius(btn2, LV_RADIUS_CIRCLE, 0); /*Add a local style too*/

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Button 2");
    lv_obj_center(label);
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    printf("Hello world!\n");

    lv_init();

    lv_port_disp_init();

    lv_fs_if_fatfs_init();

    float pitch, roll, yaw;

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
        CONFIG_IDF_TARGET,
        chip_info.cores,
        (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
        (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%uMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
        (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 2, NULL, 5, NULL, 1);

    extern void wifi_init_sta(void);
    wifi_init_sta();

    extern void esp32_get_time(void);
    esp32_get_time();

    ESP_ERROR_CHECK(i2c_master_init());
    vTaskDelay(500 / portTICK_PERIOD_MS);
    // ret = mpu_dmp_init();
    // printf("mpu_dmp:%d\r\n",ret);

    // xTaskCreatePinnedToCore(gui_tim_task, "gui_time", 4096, NULL, 0, NULL, 1);

    xTaskCreatePinnedToCore(__disp_weather_task, "disp_weather", 4096 * 2, NULL, 4, NULL, 1);

    while (1) {
        // while (mpu_dmp_get_data(&pitch,&roll,&yaw)) {
        //     printf("%f,%f,%f\r\n",pitch,roll,yaw);
        // }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}


//Http事件处理程序
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // Write out data
                printf("%.*s", evt->data_len, (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            break;
        case HTTP_EVENT_REDIRECT:

        break;
    }
    return ESP_OK;
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
    static const char* URL = "https://devapi.qweather.com/v7/weather/now?location=101210101&key=your_key&gzip=n&lang=en";
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

                    //打印响应内容，包括响应状态，响应体长度及其内容
                    ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                        esp_http_client_get_status_code(client), //获取响应状态信息
                        esp_http_client_get_content_length(client)); //获取响应信息长度

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

// bar对象动画回调函数
static void obj_bar_anim_exec_callback(void* bar, int32_t value)
{
    if (bar != NULL) {
        lv_bar_set_value((lv_obj_t*)bar, value, LV_ANIM_ON); // 设置进度条对象的值
        if (value == 100) {
            lv_scr_load_anim(guider_ui.main, LV_SCR_LOAD_ANIM_FADE_ON, 500, 5000, true);
        }
    }
}

static void __disp_time_task_proc(lv_timer_t* timer)
{
    char buf[20];
    time_t now = 0;
    struct tm timeinfo = { 0 };
    time(&now);
    localtime_r(&now, &timeinfo);
    memset(buf, 0, sizeof(buf));
    timeinfo.tm_hour += 8;
    if (timeinfo.tm_hour >= 24) {
        timeinfo.tm_hour = timeinfo.tm_hour % 24;
        timeinfo.tm_wday = (timeinfo.tm_wday + 1) % 7;
        timeinfo.tm_mday = timeinfo.tm_mday + 1;
        if (timeinfo.tm_mday > 31) {
            timeinfo.tm_mday = 1;
            timeinfo.tm_mon++;
            if (timeinfo.tm_mon > 12) {
                timeinfo.tm_mon = 1;
            }
        }
    }
    sprintf(buf, "%02d", timeinfo.tm_hour);
    lv_label_set_text(guider_ui.main_label_1, buf);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%02d", timeinfo.tm_min);
    lv_label_set_text(guider_ui.main_label_2, buf);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%02d", timeinfo.tm_sec);
    lv_label_set_text(guider_ui.main_label_3, buf);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%04d/%d/%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
    lv_label_set_text(guider_ui.main_label_4, buf);
    if (timeinfo.tm_wday == 0) {
        lv_label_set_text(guider_ui.main_label_5, "Sun");
    } else if (timeinfo.tm_wday == 1) {
        lv_label_set_text(guider_ui.main_label_5, "Mon");
    } else if (timeinfo.tm_wday == 2) {
        lv_label_set_text(guider_ui.main_label_5, "Tues");
    } else if (timeinfo.tm_wday == 3) {
        lv_label_set_text(guider_ui.main_label_5, "Wed");
    } else if (timeinfo.tm_wday == 4) {
        lv_label_set_text(guider_ui.main_label_5, "Thur");
    } else if (timeinfo.tm_wday == 5) {
        lv_label_set_text(guider_ui.main_label_5, "Fri");
    } else if (timeinfo.tm_wday == 6) {
        lv_label_set_text(guider_ui.main_label_5, "Sat");
    }
}

static void __disp_weather_task_proc(lv_timer_t* timer)
{
}

static void guiTask(void* pvParameter)
{
    (void)pvParameter;
    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lvgl_tick_inc,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    setup_scr_screen(&guider_ui);
    setup_scr_main(&guider_ui);
    lv_scr_load(guider_ui.screen);

    lv_anim_t anim;
    lv_anim_init(&anim); // 初始化动画
    lv_anim_set_exec_cb(&anim, obj_bar_anim_exec_callback); // 添加回调函数
    lv_anim_set_time(&anim, 20000); // 设置动画时长
    lv_anim_set_var(&anim, guider_ui.screen_bar_1); // 动画绑定对象
    lv_anim_set_values(&anim, 0, 100); // 设置开始值和结束值
    lv_anim_set_repeat_count(&anim, 1); // 重复次数，默认值为1 LV_ANIM_REPEAT_INFINIT用于无限重复
    lv_anim_start(&anim); // 应用动画效果

    // lv_scr_load_anim(guider_ui.main, LV_SCR_LOAD_ANIM_FADE_ON, 500, 5000, true);
    disp_tim_task = lv_timer_create(__disp_time_task_proc, 1000, NULL);
    // disp_weather_task = lv_timer_create(__disp_weather_task_proc, 100000, NULL); //且用且珍惜
    lv_timer_enable(true);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));
        lv_task_handler();
    }
}

static void lvgl_tick_inc(void* arg)
{
    (void)arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}
