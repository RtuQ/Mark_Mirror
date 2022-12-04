/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#include "lvgl/lvgl.h"
#include <stdio.h>
#include "gui_guider.h"


void setup_scr_table1(lv_ui *ui){

	//Write codes table1
	ui->table1 = lv_obj_create(NULL);

	//Write style state: LV_STATE_DEFAULT for style_table1_main_main_default
	static lv_style_t style_table1_main_main_default;
	lv_style_reset(&style_table1_main_main_default);
	lv_style_set_bg_color(&style_table1_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_opa(&style_table1_main_main_default, 0);
	lv_obj_add_style(ui->table1, &style_table1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes table1_list_1
	ui->table1_list_1 = lv_list_create(ui->table1);
	lv_obj_set_pos(ui->table1_list_1, 0, 0);
	lv_obj_set_size(ui->table1_list_1, 240, 240);

	//Write style state: LV_STATE_DEFAULT for style_table1_list_1_main_main_default
	static lv_style_t style_table1_list_1_main_main_default;
	lv_style_reset(&style_table1_list_1_main_main_default);
	lv_style_set_radius(&style_table1_list_1_main_main_default, 3);
	lv_style_set_bg_color(&style_table1_list_1_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_table1_list_1_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_table1_list_1_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_table1_list_1_main_main_default, 255);
	lv_style_set_border_color(&style_table1_list_1_main_main_default, lv_color_make(0x24, 0xab, 0xff));
	lv_style_set_border_width(&style_table1_list_1_main_main_default, 1);
	lv_style_set_pad_left(&style_table1_list_1_main_main_default, 5);
	lv_style_set_pad_right(&style_table1_list_1_main_main_default, 5);
	lv_style_set_pad_top(&style_table1_list_1_main_main_default, 5);
	lv_obj_add_style(ui->table1_list_1, &style_table1_list_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for style_table1_list_1_main_scrollbar_default
	static lv_style_t style_table1_list_1_main_scrollbar_default;
	lv_style_reset(&style_table1_list_1_main_scrollbar_default);
	lv_style_set_radius(&style_table1_list_1_main_scrollbar_default, 3);
	lv_style_set_bg_color(&style_table1_list_1_main_scrollbar_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_table1_list_1_main_scrollbar_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_table1_list_1_main_scrollbar_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_table1_list_1_main_scrollbar_default, 255);
	lv_obj_add_style(ui->table1_list_1, &style_table1_list_1_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for style_table1_list_1_extra_btns_main_default
	static lv_style_t style_table1_list_1_extra_btns_main_default;
	lv_style_reset(&style_table1_list_1_extra_btns_main_default);
	lv_style_set_radius(&style_table1_list_1_extra_btns_main_default, 3);
	lv_style_set_bg_color(&style_table1_list_1_extra_btns_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_table1_list_1_extra_btns_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_table1_list_1_extra_btns_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_table1_list_1_extra_btns_main_default, 255);
	lv_style_set_text_color(&style_table1_list_1_extra_btns_main_default, lv_color_make(0x0D, 0x30, 0x55));
	lv_style_set_text_font(&style_table1_list_1_extra_btns_main_default, &lv_font_simsun_12);
	lv_obj_t *table1_list_1_btn;
	table1_list_1_btn = lv_list_add_btn(ui->table1_list_1, LV_SYMBOL_SETTINGS, "reset system");
	ui->table1_list_1_item0 = table1_list_1_btn;
	lv_obj_add_style(table1_list_1_btn, &style_table1_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	table1_list_1_btn = lv_list_add_btn(ui->table1_list_1, LV_SYMBOL_POWER, "reboot");
	ui->table1_list_1_item1 = table1_list_1_btn;
	lv_obj_add_style(table1_list_1_btn, &style_table1_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
}