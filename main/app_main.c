/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_spi_flash.h"
#include "inv_mpu.h"
#include "mpu6050.h"
#include "esp_system.h"
#include "esp_timer.h"


/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "lv_port_disp.h"
/*********************
 *      DEFINES
 *********************/
#define LV_TICK_PERIOD_MS 1

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lvgl_tick_inc(void *arg);
static void guiTask(void *pvParameter);

void app_main(void)
{
    printf("Hello world!\n");
    int ret = 0;
    float pitch,roll,yaw;

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
    

    ESP_ERROR_CHECK(i2c_master_init());
    vTaskDelay(500 / portTICK_PERIOD_MS);
    // ret = mpu_dmp_init();
    // printf("mpu_dmp:%d\r\n",ret);


    xTaskCreatePinnedToCore(guiTask, "gui", 4096*2, NULL, 0, NULL, 1);


    while(1) {
        // while (mpu_dmp_get_data(&pitch,&roll,&yaw)) {
        //     printf("%f,%f,%f\r\n",pitch,roll,yaw);
        // }
        vTaskDelay(200/portTICK_PERIOD_MS);
    }
    

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}


static void guiTask(void *pvParameter)
{
    (void) pvParameter;
    lv_init();

    lv_port_disp_init();

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lvgl_tick_inc,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    /* Create a Hellow World label on the currently active screen */
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);

    /* LVGL v8 lv_label_create no longer takes 2 parameters */
    lv_obj_t *label1 =  lv_label_create(scr);
    // lv_obj_set_style_bg_color(label1,lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(label1, "Hello\nworld");

    /* Align the Label to the center
     * NULL means align on parent (which is the screen now)
     * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));
        lv_task_handler();
    }
}

static void lvgl_tick_inc(void *arg) {
    (void) arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}
