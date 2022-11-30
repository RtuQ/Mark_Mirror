/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include "guider_fonts.h"

typedef struct
{
	lv_obj_t *main;
	lv_obj_t *main_label_1;
	lv_obj_t *main_label_2;
	lv_obj_t *main_label_3;
	lv_obj_t *main_label_4;
	lv_obj_t *main_label_5;
	lv_obj_t *main_label_6;
	lv_obj_t *main_label_7;
	lv_obj_t *main_label_8;
	lv_obj_t *main_label_10;
	lv_obj_t *main_label_11;
	lv_obj_t *main_bar_1;
	lv_obj_t *main_bar_2;
	lv_obj_t *main_img_1;
	lv_obj_t *main_img_2;
	lv_obj_t *main_img_3;
	lv_obj_t *screen;
	lv_obj_t *screen_bar_1;
	lv_obj_t *screen_label_1;
}lv_ui;

void setup_ui(lv_ui *ui);
extern lv_ui guider_ui;
void setup_scr_main(lv_ui *ui);
void setup_scr_screen(lv_ui *ui);
LV_IMG_DECLARE(_temp_30x25);
LV_IMG_DECLARE(_hum_21x22);

#ifdef __cplusplus
}
#endif
#endif