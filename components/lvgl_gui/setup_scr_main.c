/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#include "lvgl/lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"


void setup_scr_main(lv_ui *ui){

	//Write codes main
	ui->main = lv_obj_create(NULL);

	//Write style state: LV_STATE_DEFAULT for style_main_main_main_default
	static lv_style_t style_main_main_main_default;
	lv_style_reset(&style_main_main_main_default);
	lv_style_set_bg_color(&style_main_main_main_default, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_bg_opa(&style_main_main_main_default, 255);
	lv_obj_add_style(ui->main, &style_main_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes main_label_1
	ui->main_label_1 = lv_label_create(ui->main);
	lv_obj_set_pos(ui->main_label_1, 7, 66);
	lv_obj_set_size(ui->main_label_1, 95, 71);
	lv_label_set_text(ui->main_label_1, "18");
	lv_label_set_long_mode(ui->main_label_1, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(ui->main_label_1, LV_TEXT_ALIGN_LEFT, 0);

	//Write style state: LV_STATE_DEFAULT for style_main_label_1_main_main_default
	static lv_style_t style_main_label_1_main_main_default;
	lv_style_reset(&style_main_label_1_main_main_default);
	lv_style_set_radius(&style_main_label_1_main_main_default, 0);
	lv_style_set_bg_color(&style_main_label_1_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_main_label_1_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_main_label_1_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_main_label_1_main_main_default, 0);
	lv_style_set_text_color(&style_main_label_1_main_main_default, lv_color_make(0x87, 0xc4, 0x54));
	lv_style_set_text_font(&style_main_label_1_main_main_default, &lv_font_MiSans_Normal_75);
	lv_style_set_text_letter_space(&style_main_label_1_main_main_default, 0);
	lv_style_set_pad_left(&style_main_label_1_main_main_default, 0);
	lv_style_set_pad_right(&style_main_label_1_main_main_default, 0);
	lv_style_set_pad_top(&style_main_label_1_main_main_default, 0);
	lv_style_set_pad_bottom(&style_main_label_1_main_main_default, 0);
	lv_obj_add_style(ui->main_label_1, &style_main_label_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes main_label_2
	ui->main_label_2 = lv_label_create(ui->main);
	lv_obj_set_pos(ui->main_label_2, 94, 66);
	lv_obj_set_size(ui->main_label_2, 95, 72);
	lv_label_set_text(ui->main_label_2, "32");
	lv_label_set_long_mode(ui->main_label_2, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(ui->main_label_2, LV_TEXT_ALIGN_LEFT, 0);

	//Write style state: LV_STATE_DEFAULT for style_main_label_2_main_main_default
	static lv_style_t style_main_label_2_main_main_default;
	lv_style_reset(&style_main_label_2_main_main_default);
	lv_style_set_radius(&style_main_label_2_main_main_default, 0);
	lv_style_set_bg_color(&style_main_label_2_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_main_label_2_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_main_label_2_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_main_label_2_main_main_default, 0);
	lv_style_set_text_color(&style_main_label_2_main_main_default, lv_color_make(0x4a, 0x86, 0x19));
	lv_style_set_text_font(&style_main_label_2_main_main_default, &lv_font_MiSans_Normal_75);
	lv_style_set_text_letter_space(&style_main_label_2_main_main_default, 0);
	lv_style_set_pad_left(&style_main_label_2_main_main_default, 0);
	lv_style_set_pad_right(&style_main_label_2_main_main_default, 0);
	lv_style_set_pad_top(&style_main_label_2_main_main_default, 0);
	lv_style_set_pad_bottom(&style_main_label_2_main_main_default, 0);
	lv_obj_add_style(ui->main_label_2, &style_main_label_2_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes main_label_3
	ui->main_label_3 = lv_label_create(ui->main);
	lv_obj_set_pos(ui->main_label_3, 185, 97);
	lv_obj_set_size(ui->main_label_3, 51, 40);
	lv_label_set_text(ui->main_label_3, "10");
	lv_label_set_long_mode(ui->main_label_3, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(ui->main_label_3, LV_TEXT_ALIGN_LEFT, 0);

	//Write style state: LV_STATE_DEFAULT for style_main_label_3_main_main_default
	static lv_style_t style_main_label_3_main_main_default;
	lv_style_reset(&style_main_label_3_main_main_default);
	lv_style_set_radius(&style_main_label_3_main_main_default, 0);
	lv_style_set_bg_color(&style_main_label_3_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_main_label_3_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_main_label_3_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_main_label_3_main_main_default, 0);
	lv_style_set_text_color(&style_main_label_3_main_main_default, lv_color_make(0xf9, 0xfb, 0xf9));
	lv_style_set_text_font(&style_main_label_3_main_main_default, &lv_font_MiSans_Normal_40);
	lv_style_set_text_letter_space(&style_main_label_3_main_main_default, 0);
	lv_style_set_pad_left(&style_main_label_3_main_main_default, 0);
	lv_style_set_pad_right(&style_main_label_3_main_main_default, 0);
	lv_style_set_pad_top(&style_main_label_3_main_main_default, 0);
	lv_style_set_pad_bottom(&style_main_label_3_main_main_default, 0);
	lv_obj_add_style(ui->main_label_3, &style_main_label_3_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes main_label_4
	ui->main_label_4 = lv_label_create(ui->main);
	lv_obj_set_pos(ui->main_label_4, 17, 148);
	lv_obj_set_size(ui->main_label_4, 109, 21);
	lv_label_set_text(ui->main_label_4, "2022/06/18 ");
	lv_label_set_long_mode(ui->main_label_4, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(ui->main_label_4, LV_TEXT_ALIGN_LEFT, 0);

	//Write style state: LV_STATE_DEFAULT for style_main_label_4_main_main_default
	static lv_style_t style_main_label_4_main_main_default;
	lv_style_reset(&style_main_label_4_main_main_default);
	lv_style_set_radius(&style_main_label_4_main_main_default, 0);
	lv_style_set_bg_color(&style_main_label_4_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_main_label_4_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_main_label_4_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_main_label_4_main_main_default, 0);
	lv_style_set_text_color(&style_main_label_4_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_main_label_4_main_main_default, &lv_font_MiSans_Normal_20);
	lv_style_set_text_letter_space(&style_main_label_4_main_main_default, 0);
	lv_style_set_pad_left(&style_main_label_4_main_main_default, 0);
	lv_style_set_pad_right(&style_main_label_4_main_main_default, 0);
	lv_style_set_pad_top(&style_main_label_4_main_main_default, 0);
	lv_style_set_pad_bottom(&style_main_label_4_main_main_default, 0);
	lv_obj_add_style(ui->main_label_4, &style_main_label_4_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes main_label_5
	ui->main_label_5 = lv_label_create(ui->main);
	lv_obj_set_pos(ui->main_label_5, 136, 148);
	lv_obj_set_size(ui->main_label_5, 43, 21);
	lv_label_set_text(ui->main_label_5, "Thur");
	lv_label_set_long_mode(ui->main_label_5, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(ui->main_label_5, LV_TEXT_ALIGN_LEFT, 0);

	//Write style state: LV_STATE_DEFAULT for style_main_label_5_main_main_default
	static lv_style_t style_main_label_5_main_main_default;
	lv_style_reset(&style_main_label_5_main_main_default);
	lv_style_set_radius(&style_main_label_5_main_main_default, 0);
	lv_style_set_bg_color(&style_main_label_5_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_main_label_5_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_main_label_5_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_main_label_5_main_main_default, 0);
	lv_style_set_text_color(&style_main_label_5_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_main_label_5_main_main_default, &lv_font_MiSans_Normal_20);
	lv_style_set_text_letter_space(&style_main_label_5_main_main_default, 0);
	lv_style_set_pad_left(&style_main_label_5_main_main_default, 0);
	lv_style_set_pad_right(&style_main_label_5_main_main_default, 0);
	lv_style_set_pad_top(&style_main_label_5_main_main_default, 0);
	lv_style_set_pad_bottom(&style_main_label_5_main_main_default, 0);
	lv_obj_add_style(ui->main_label_5, &style_main_label_5_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes main_label_6
	ui->main_label_6 = lv_label_create(ui->main);
	lv_obj_set_pos(ui->main_label_6, 15, 48);
	lv_obj_set_size(ui->main_label_6, 155, 19);
	lv_label_set_text(ui->main_label_6, "Hello world!");
	lv_label_set_long_mode(ui->main_label_6, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_style_text_align(ui->main_label_6, LV_TEXT_ALIGN_CENTER, 0);

	//Write style state: LV_STATE_DEFAULT for style_main_label_6_main_main_default
	static lv_style_t style_main_label_6_main_main_default;
	lv_style_reset(&style_main_label_6_main_main_default);
	lv_style_set_radius(&style_main_label_6_main_main_default, 0);
	lv_style_set_bg_color(&style_main_label_6_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_main_label_6_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_main_label_6_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_main_label_6_main_main_default, 0);
	lv_style_set_text_color(&style_main_label_6_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_main_label_6_main_main_default, &lv_font_MiSans_Normal_20);
	lv_style_set_text_letter_space(&style_main_label_6_main_main_default, 2);
	lv_style_set_pad_left(&style_main_label_6_main_main_default, 0);
	lv_style_set_pad_right(&style_main_label_6_main_main_default, 0);
	lv_style_set_pad_top(&style_main_label_6_main_main_default, 0);
	lv_style_set_pad_bottom(&style_main_label_6_main_main_default, 0);
	lv_obj_add_style(ui->main_label_6, &style_main_label_6_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes main_label_7
	ui->main_label_7 = lv_label_create(ui->main);
	lv_obj_set_pos(ui->main_label_7, 113, 181);
	lv_obj_set_size(ui->main_label_7, 35, 19);
	lv_label_set_text(ui->main_label_7, "20");
	lv_label_set_long_mode(ui->main_label_7, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(ui->main_label_7, LV_TEXT_ALIGN_RIGHT, 0);

	//Write style state: LV_STATE_DEFAULT for style_main_label_7_main_main_default
	static lv_style_t style_main_label_7_main_main_default;
	lv_style_reset(&style_main_label_7_main_main_default);
	lv_style_set_radius(&style_main_label_7_main_main_default, 0);
	lv_style_set_bg_color(&style_main_label_7_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_main_label_7_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_main_label_7_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_main_label_7_main_main_default, 0);
	lv_style_set_text_color(&style_main_label_7_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_main_label_7_main_main_default, &lv_font_MiSans_Normal_20);
	lv_style_set_text_letter_space(&style_main_label_7_main_main_default, 2);
	lv_style_set_pad_left(&style_main_label_7_main_main_default, 0);
	lv_style_set_pad_right(&style_main_label_7_main_main_default, 0);
	lv_style_set_pad_top(&style_main_label_7_main_main_default, 0);
	lv_style_set_pad_bottom(&style_main_label_7_main_main_default, 0);
	lv_obj_add_style(ui->main_label_7, &style_main_label_7_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes main_label_8
	ui->main_label_8 = lv_label_create(ui->main);
	lv_obj_set_pos(ui->main_label_8, 119, 207);
	lv_obj_set_size(ui->main_label_8, 31, 19);
	lv_label_set_text(ui->main_label_8, "80");
	lv_label_set_long_mode(ui->main_label_8, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(ui->main_label_8, LV_TEXT_ALIGN_RIGHT, 0);

	//Write style state: LV_STATE_DEFAULT for style_main_label_8_main_main_default
	static lv_style_t style_main_label_8_main_main_default;
	lv_style_reset(&style_main_label_8_main_main_default);
	lv_style_set_radius(&style_main_label_8_main_main_default, 0);
	lv_style_set_bg_color(&style_main_label_8_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_main_label_8_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_main_label_8_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_main_label_8_main_main_default, 0);
	lv_style_set_text_color(&style_main_label_8_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_main_label_8_main_main_default, &lv_font_MiSans_Normal_20);
	lv_style_set_text_letter_space(&style_main_label_8_main_main_default, 2);
	lv_style_set_pad_left(&style_main_label_8_main_main_default, 0);
	lv_style_set_pad_right(&style_main_label_8_main_main_default, 0);
	lv_style_set_pad_top(&style_main_label_8_main_main_default, 0);
	lv_style_set_pad_bottom(&style_main_label_8_main_main_default, 0);
	lv_obj_add_style(ui->main_label_8, &style_main_label_8_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes main_bar_1
	ui->main_bar_1 = lv_bar_create(ui->main);
	lv_obj_set_pos(ui->main_bar_1, 36, 185);
	lv_obj_set_size(ui->main_bar_1, 80, 10);

	//Write style state: LV_STATE_DEFAULT for style_main_bar_1_main_main_default
	static lv_style_t style_main_bar_1_main_main_default;
	lv_style_reset(&style_main_bar_1_main_main_default);
	lv_style_set_radius(&style_main_bar_1_main_main_default, 10);
	lv_style_set_bg_color(&style_main_bar_1_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_main_bar_1_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_main_bar_1_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_main_bar_1_main_main_default, 60);
	lv_style_set_pad_left(&style_main_bar_1_main_main_default, 0);
	lv_style_set_pad_right(&style_main_bar_1_main_main_default, 0);
	lv_style_set_pad_top(&style_main_bar_1_main_main_default, 0);
	lv_style_set_pad_bottom(&style_main_bar_1_main_main_default, 0);
	lv_obj_add_style(ui->main_bar_1, &style_main_bar_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for style_main_bar_1_main_indicator_default
	static lv_style_t style_main_bar_1_main_indicator_default;
	lv_style_reset(&style_main_bar_1_main_indicator_default);
	lv_style_set_radius(&style_main_bar_1_main_indicator_default, 10);
	lv_style_set_bg_color(&style_main_bar_1_main_indicator_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_main_bar_1_main_indicator_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_main_bar_1_main_indicator_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_main_bar_1_main_indicator_default, 255);
	lv_obj_add_style(ui->main_bar_1, &style_main_bar_1_main_indicator_default, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_anim_time(ui->main_bar_1, 1000, 0);
	lv_bar_set_mode(ui->main_bar_1, LV_BAR_MODE_NORMAL);
	lv_bar_set_value(ui->main_bar_1, 50, LV_ANIM_OFF);

	//Write codes main_bar_2
	ui->main_bar_2 = lv_bar_create(ui->main);
	lv_obj_set_pos(ui->main_bar_2, 36, 212);
	lv_obj_set_size(ui->main_bar_2, 80, 10);

	//Write style state: LV_STATE_DEFAULT for style_main_bar_2_main_main_default
	static lv_style_t style_main_bar_2_main_main_default;
	lv_style_reset(&style_main_bar_2_main_main_default);
	lv_style_set_radius(&style_main_bar_2_main_main_default, 10);
	lv_style_set_bg_color(&style_main_bar_2_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_main_bar_2_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_main_bar_2_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_main_bar_2_main_main_default, 60);
	lv_style_set_pad_left(&style_main_bar_2_main_main_default, 0);
	lv_style_set_pad_right(&style_main_bar_2_main_main_default, 0);
	lv_style_set_pad_top(&style_main_bar_2_main_main_default, 0);
	lv_style_set_pad_bottom(&style_main_bar_2_main_main_default, 0);
	lv_obj_add_style(ui->main_bar_2, &style_main_bar_2_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for style_main_bar_2_main_indicator_default
	static lv_style_t style_main_bar_2_main_indicator_default;
	lv_style_reset(&style_main_bar_2_main_indicator_default);
	lv_style_set_radius(&style_main_bar_2_main_indicator_default, 10);
	lv_style_set_bg_color(&style_main_bar_2_main_indicator_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_main_bar_2_main_indicator_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_main_bar_2_main_indicator_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_main_bar_2_main_indicator_default, 255);
	lv_obj_add_style(ui->main_bar_2, &style_main_bar_2_main_indicator_default, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_anim_time(ui->main_bar_2, 1000, 0);
	lv_bar_set_mode(ui->main_bar_2, LV_BAR_MODE_NORMAL);
	lv_bar_set_value(ui->main_bar_2, 50, LV_ANIM_OFF);

	//Write codes main_img_1
	ui->main_img_1 = lv_img_create(ui->main);
	lv_obj_set_pos(ui->main_img_1, 7, 177);
	lv_obj_set_size(ui->main_img_1, 30, 25);

	//Write style state: LV_STATE_DEFAULT for style_main_img_1_main_main_default
	static lv_style_t style_main_img_1_main_main_default;
	lv_style_reset(&style_main_img_1_main_main_default);
	lv_style_set_img_recolor(&style_main_img_1_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_main_img_1_main_main_default, 0);
	lv_style_set_img_opa(&style_main_img_1_main_main_default, 255);
	lv_obj_add_style(ui->main_img_1, &style_main_img_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->main_img_1, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->main_img_1,&_temp_30x25);
	lv_img_set_pivot(ui->main_img_1, 0,0);
	lv_img_set_angle(ui->main_img_1, 0);

	//Write codes main_img_2
	ui->main_img_2 = lv_img_create(ui->main);
	lv_obj_set_pos(ui->main_img_2, 11, 206);
	lv_obj_set_size(ui->main_img_2, 21, 22);

	//Write style state: LV_STATE_DEFAULT for style_main_img_2_main_main_default
	static lv_style_t style_main_img_2_main_main_default;
	lv_style_reset(&style_main_img_2_main_main_default);
	lv_style_set_img_recolor(&style_main_img_2_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_main_img_2_main_main_default, 0);
	lv_style_set_img_opa(&style_main_img_2_main_main_default, 255);
	lv_obj_add_style(ui->main_img_2, &style_main_img_2_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->main_img_2, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->main_img_2,&_hum_21x22);
	lv_img_set_pivot(ui->main_img_2, 0,0);
	lv_img_set_angle(ui->main_img_2, 0);

	//Write codes main_img_3
	ui->main_img_3 = lv_img_create(ui->main);
	lv_obj_set_pos(ui->main_img_3, 171, 3);
	lv_obj_set_size(ui->main_img_3, 64, 64);

	static lv_style_t style_main_img_3_main_main_default;
	lv_style_reset(&style_main_img_3_main_main_default);
	lv_style_set_img_recolor(&style_main_img_3_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_main_img_3_main_main_default, 0);
	lv_style_set_img_opa(&style_main_img_3_main_main_default, 255);
	lv_obj_add_style(ui->main_img_3, &style_main_img_3_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->main_img_3, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->main_img_3,"S:/weather/103.bin");
	lv_img_set_pivot(ui->main_img_3, 0,0);
	lv_img_set_angle(ui->main_img_3, 0);

	//Write codes main_label_10
	ui->main_label_10 = lv_label_create(ui->main);
	lv_obj_set_pos(ui->main_label_10, 116, 8);
	lv_obj_set_size(ui->main_label_10, 48, 20);
	lv_label_set_text(ui->main_label_10, "1");
	lv_label_set_long_mode(ui->main_label_10, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(ui->main_label_10, LV_TEXT_ALIGN_CENTER, 0);

	//Write style state: LV_STATE_DEFAULT for style_main_label_10_main_main_default
	static lv_style_t style_main_label_10_main_main_default;
	lv_style_reset(&style_main_label_10_main_main_default);
	lv_style_set_radius(&style_main_label_10_main_main_default, 0);
	lv_style_set_bg_color(&style_main_label_10_main_main_default, lv_color_make(0x00, 0xff, 0x00));
	lv_style_set_bg_grad_color(&style_main_label_10_main_main_default, lv_color_make(0x00, 0xff, 0x00));
	lv_style_set_bg_grad_dir(&style_main_label_10_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_main_label_10_main_main_default, 255);
	lv_style_set_text_color(&style_main_label_10_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_main_label_10_main_main_default, &lv_font_MiSans_Normal_20);
	lv_style_set_text_letter_space(&style_main_label_10_main_main_default, 2);
	lv_style_set_pad_left(&style_main_label_10_main_main_default, 0);
	lv_style_set_pad_right(&style_main_label_10_main_main_default, 0);
	lv_style_set_pad_top(&style_main_label_10_main_main_default, 0);
	lv_style_set_pad_bottom(&style_main_label_10_main_main_default, 0);
	lv_obj_add_style(ui->main_label_10, &style_main_label_10_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes main_label_11
	ui->main_label_11 = lv_label_create(ui->main);
	lv_obj_set_pos(ui->main_label_11, 18, 8);
	lv_obj_set_size(ui->main_label_11, 48, 20);
	lv_label_set_text(ui->main_label_11, "HZ");
	lv_label_set_long_mode(ui->main_label_11, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(ui->main_label_11, LV_TEXT_ALIGN_CENTER, 0);

	//Write style state: LV_STATE_DEFAULT for style_main_label_11_main_main_default
	static lv_style_t style_main_label_11_main_main_default;
	lv_style_reset(&style_main_label_11_main_main_default);
	lv_style_set_radius(&style_main_label_11_main_main_default, 0);
	lv_style_set_bg_color(&style_main_label_11_main_main_default, lv_color_make(0x3d, 0x62, 0xe6));
	lv_style_set_bg_grad_color(&style_main_label_11_main_main_default, lv_color_make(0x3d, 0x62, 0xe6));
	lv_style_set_bg_grad_dir(&style_main_label_11_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_main_label_11_main_main_default, 255);
	lv_style_set_text_color(&style_main_label_11_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_main_label_11_main_main_default, &lv_font_MiSans_Normal_20);
	lv_style_set_text_letter_space(&style_main_label_11_main_main_default, 2);
	lv_style_set_pad_left(&style_main_label_11_main_main_default, 0);
	lv_style_set_pad_right(&style_main_label_11_main_main_default, 0);
	lv_style_set_pad_top(&style_main_label_11_main_main_default, 0);
	lv_style_set_pad_bottom(&style_main_label_11_main_main_default, 0);
	lv_obj_add_style(ui->main_label_11, &style_main_label_11_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
}