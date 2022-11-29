
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "lvgl_app.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_fs_fatfs.h"
#include "gui_guider.h"
#include "esp_wifi.h"
#include "esp_weather_time.h"

#include "esp_log.h"
#include "esp_timer.h"


#define LV_TICK_PERIOD_MS 1

static const char *TAG = "lvgl app";

lv_ui guider_ui;
lv_timer_t* disp_tim_task = NULL;

static void lvgl_tick_inc(void* arg)
{
    (void)arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

// bar对象动画回调函数
static void __bar_anim_exec_callback(void* bar, int32_t value)
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
    char buf[40];
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
    sprintf(buf, "%04d/%02d/%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
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

static void __esp_lvgl_gui_task(void* pvParameter)
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
    // lv_scr_load(guider_ui.screen);

    // lv_anim_t anim;
    // lv_anim_init(&anim); // 初始化动画
    // lv_anim_set_exec_cb(&anim, __bar_anim_exec_callback); // 添加回调函数
    // lv_anim_set_time(&anim, 20000); // 设置动画时长
    // lv_anim_set_var(&anim, guider_ui.screen_bar_1); // 动画绑定对象
    // lv_anim_set_values(&anim, 0, 100); // 设置开始值和结束值
    // lv_anim_set_repeat_count(&anim, 1); // 重复次数，默认值为1 LV_ANIM_REPEAT_INFINIT用于无限重复
    // lv_anim_start(&anim); // 应用动画效果

    // // lv_scr_load_anim(guider_ui.main, LV_SCR_LOAD_ANIM_FADE_ON, 500, 5000, true);
    disp_tim_task = lv_timer_create(__disp_time_task_proc, 1000, NULL);
    // // disp_weather_task = lv_timer_create(__disp_weather_task_proc, 100000, NULL); //且用且珍惜
    lv_timer_enable(true);

    // lv_obj_t * img;
    // img = lv_gif_create(lv_scr_act());
    // /* Assuming a File system is attached to letter 'A'
    //  * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    // lv_gif_set_src(img, "S:login.bin");
    // lv_obj_align(img, LV_ALIGN_RIGHT_MID, -20, 0);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));
        lv_task_handler();
    }
}


static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "waitting to get internet time!!");
        esp_internet_time_init();
        lv_bar_set_value(guider_ui.screen_bar_1, 50, LV_ANIM_ON); // 设置进度条对象的值
        lv_label_set_text(guider_ui.screen_label_1, "sync time");
    } else if (event_base == NTP_TIME_EVENT) {
        // wait for time to be set
        time_t now = 0;
        struct tm timeinfo = { 0 };
        time(&now);
        localtime_r(&now, &timeinfo);
        ESP_LOGI(TAG,"Now time:%d:%d:%d",timeinfo.tm_hour+8,timeinfo.tm_min,timeinfo.tm_sec);     
        //获取到时间之后再同步天气
        esp_start_sync_weather(); 
        
        lv_bar_set_value(guider_ui.screen_bar_1, 100, LV_ANIM_ON); // 设置进度条对象的值  

        lv_scr_load_anim(guider_ui.main, LV_SCR_LOAD_ANIM_FADE_ON, 500, 5000, true);
    }
}

/**
* @brief lvgl 应用初始化
*
* @param[in] 
* @return 
*/
void esp_lvgl_device_app_init(void)
{
    //初始化lvgl
    lv_init();  
    
    //初始化液晶屏
    lv_port_disp_init();

    //初始化SD卡
    lv_fs_if_fatfs_init();

    
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(NTP_TIME_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );

    xTaskCreatePinnedToCore(__esp_lvgl_gui_task, "__esp_lvgl_gui_task", 4096 * 2, NULL, 5, NULL, 1);
}