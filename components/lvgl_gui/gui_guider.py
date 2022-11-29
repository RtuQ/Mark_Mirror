# Copyright 2021 NXP
# SPDX-License-Identifier: MIT

import SDL
import utime as time
import usys as sys
import lvgl as lv
import lodepng as png
import ustruct

lv.init()
SDL.init(w=240,h=240)

# Register SDL display driver.
disp_buf1 = lv.disp_draw_buf_t()
buf1_1 = bytearray(240*10)
disp_buf1.init(buf1_1, None, len(buf1_1)//4)
disp_drv = lv.disp_drv_t()
disp_drv.init()
disp_drv.draw_buf = disp_buf1
disp_drv.flush_cb = SDL.monitor_flush
disp_drv.hor_res = 240
disp_drv.ver_res = 240
disp_drv.register()

# Regsiter SDL mouse driver
indev_drv = lv.indev_drv_t()
indev_drv.init() 
indev_drv.type = lv.INDEV_TYPE.POINTER
indev_drv.read_cb = SDL.mouse_read
indev_drv.register()

# Below: Taken from https://github.com/lvgl/lv_binding_micropython/blob/master/driver/js/imagetools.py#L22-L94

COLOR_SIZE = lv.color_t.__SIZE__
COLOR_IS_SWAPPED = hasattr(lv.color_t().ch,'green_h')

class lodepng_error(RuntimeError):
    def __init__(self, err):
        if type(err) is int:
            super().__init__(png.error_text(err))
        else:
            super().__init__(err)

# Parse PNG file header
# Taken from https://github.com/shibukawa/imagesize_py/blob/ffef30c1a4715c5acf90e8945ceb77f4a2ed2d45/imagesize.py#L63-L85

def get_png_info(decoder, src, header):
    # Only handle variable image types

    if lv.img.src_get_type(src) != lv.img.SRC.VARIABLE:
        return lv.RES.INV

    data = lv.img_dsc_t.__cast__(src).data
    if data == None:
        return lv.RES.INV

    png_header = bytes(data.__dereference__(24))

    if png_header.startswith(b'\211PNG\r\n\032\n'):
        if png_header[12:16] == b'IHDR':
            start = 16
        # Maybe this is for an older PNG version.
        else:
            start = 8
        try:
            width, height = ustruct.unpack(">LL", png_header[start:start+8])
        except ustruct.error:
            return lv.RES.INV
    else:
        return lv.RES.INV

    header.always_zero = 0
    header.w = width
    header.h = height
    header.cf = lv.img.CF.TRUE_COLOR_ALPHA

    return lv.RES.OK

def convert_rgba8888_to_bgra8888(img_view):
    for i in range(0, len(img_view), lv.color_t.__SIZE__):
        ch = lv.color_t.__cast__(img_view[i:i]).ch
        ch.red, ch.blue = ch.blue, ch.red

# Read and parse PNG file

def open_png(decoder, dsc):
    img_dsc = lv.img_dsc_t.__cast__(dsc.src)
    png_data = img_dsc.data
    png_size = img_dsc.data_size
    png_decoded = png.C_Pointer()
    png_width = png.C_Pointer()
    png_height = png.C_Pointer()
    error = png.decode32(png_decoded, png_width, png_height, png_data, png_size)
    if error:
        raise lodepng_error(error)
    img_size = png_width.int_val * png_height.int_val * 4
    img_data = png_decoded.ptr_val
    img_view = img_data.__dereference__(img_size)

    if COLOR_SIZE == 4:
        convert_rgba8888_to_bgra8888(img_view)
    else:
        raise lodepng_error("Error: Color mode not supported yet!")

    dsc.img_data = img_data
    return lv.RES.OK

# Above: Taken from https://github.com/lvgl/lv_binding_micropython/blob/master/driver/js/imagetools.py#L22-L94

decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

def anim_x_cb(obj, v):
    obj.set_x(v)

def anim_y_cb(obj, v):
    obj.set_y(v)

def ta_event_cb(e,kb):
    code = e.get_code()
    ta = e.get_target()
    if code == lv.EVENT.FOCUSED:
        kb.set_textarea(ta)
        kb.clear_flag(lv.obj.FLAG.HIDDEN)

    if code == lv.EVENT.DEFOCUSED:
        kb.set_textarea(None)
        kb.add_flag(lv.obj.FLAG.HIDDEN)


main = lv.obj()
# create style style_main_main_main_default
style_main_main_main_default = lv.style_t()
style_main_main_main_default.init()
style_main_main_main_default.set_bg_color(lv.color_make(0x00,0x00,0x00))
style_main_main_main_default.set_bg_opa(255)

# add style for main
main.add_style(style_main_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

main_label_1 = lv.label(main)
main_label_1.set_pos(7,67)
main_label_1.set_size(77,78)
main_label_1.set_text("18")
main_label_1.set_long_mode(lv.label.LONG.WRAP)
main_label_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, 0)
# create style style_main_label_1_main_main_default
style_main_label_1_main_main_default = lv.style_t()
style_main_label_1_main_main_default.init()
style_main_label_1_main_main_default.set_radius(0)
style_main_label_1_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_1_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_main_label_1_main_main_default.set_bg_opa(0)
style_main_label_1_main_main_default.set_text_color(lv.color_make(0x87,0xc4,0x54))
try:
    style_main_label_1_main_main_default.set_text_font(lv.font_MiSans_Normal_75)
except AttributeError:
    try:
        style_main_label_1_main_main_default.set_text_font(lv.font_montserrat_75)
    except AttributeError:
        style_main_label_1_main_main_default.set_text_font(lv.font_montserrat_16)
style_main_label_1_main_main_default.set_text_letter_space(0)
style_main_label_1_main_main_default.set_pad_left(0)
style_main_label_1_main_main_default.set_pad_right(0)
style_main_label_1_main_main_default.set_pad_top(0)
style_main_label_1_main_main_default.set_pad_bottom(0)

# add style for main_label_1
main_label_1.add_style(style_main_label_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

main_label_2 = lv.label(main)
main_label_2.set_pos(85,67)
main_label_2.set_size(88,76)
main_label_2.set_text("32")
main_label_2.set_long_mode(lv.label.LONG.WRAP)
main_label_2.set_style_text_align(lv.TEXT_ALIGN.LEFT, 0)
# create style style_main_label_2_main_main_default
style_main_label_2_main_main_default = lv.style_t()
style_main_label_2_main_main_default.init()
style_main_label_2_main_main_default.set_radius(0)
style_main_label_2_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_2_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_2_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_main_label_2_main_main_default.set_bg_opa(0)
style_main_label_2_main_main_default.set_text_color(lv.color_make(0x4a,0x86,0x19))
try:
    style_main_label_2_main_main_default.set_text_font(lv.font_MiSans_Normal_75)
except AttributeError:
    try:
        style_main_label_2_main_main_default.set_text_font(lv.font_montserrat_75)
    except AttributeError:
        style_main_label_2_main_main_default.set_text_font(lv.font_montserrat_16)
style_main_label_2_main_main_default.set_text_letter_space(0)
style_main_label_2_main_main_default.set_pad_left(0)
style_main_label_2_main_main_default.set_pad_right(0)
style_main_label_2_main_main_default.set_pad_top(0)
style_main_label_2_main_main_default.set_pad_bottom(0)

# add style for main_label_2
main_label_2.add_style(style_main_label_2_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

main_label_3 = lv.label(main)
main_label_3.set_pos(185,97)
main_label_3.set_size(51,40)
main_label_3.set_text("10")
main_label_3.set_long_mode(lv.label.LONG.WRAP)
main_label_3.set_style_text_align(lv.TEXT_ALIGN.LEFT, 0)
# create style style_main_label_3_main_main_default
style_main_label_3_main_main_default = lv.style_t()
style_main_label_3_main_main_default.init()
style_main_label_3_main_main_default.set_radius(0)
style_main_label_3_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_3_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_3_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_main_label_3_main_main_default.set_bg_opa(0)
style_main_label_3_main_main_default.set_text_color(lv.color_make(0xf9,0xfb,0xf9))
try:
    style_main_label_3_main_main_default.set_text_font(lv.font_MiSans_Normal_40)
except AttributeError:
    try:
        style_main_label_3_main_main_default.set_text_font(lv.font_montserrat_40)
    except AttributeError:
        style_main_label_3_main_main_default.set_text_font(lv.font_montserrat_16)
style_main_label_3_main_main_default.set_text_letter_space(0)
style_main_label_3_main_main_default.set_pad_left(0)
style_main_label_3_main_main_default.set_pad_right(0)
style_main_label_3_main_main_default.set_pad_top(0)
style_main_label_3_main_main_default.set_pad_bottom(0)

# add style for main_label_3
main_label_3.add_style(style_main_label_3_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

main_label_4 = lv.label(main)
main_label_4.set_pos(17,152)
main_label_4.set_size(109,21)
main_label_4.set_text("2022/06/18 ")
main_label_4.set_long_mode(lv.label.LONG.WRAP)
main_label_4.set_style_text_align(lv.TEXT_ALIGN.LEFT, 0)
# create style style_main_label_4_main_main_default
style_main_label_4_main_main_default = lv.style_t()
style_main_label_4_main_main_default.init()
style_main_label_4_main_main_default.set_radius(0)
style_main_label_4_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_4_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_4_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_main_label_4_main_main_default.set_bg_opa(0)
style_main_label_4_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_main_label_4_main_main_default.set_text_font(lv.font_MiSans_Normal_20)
except AttributeError:
    try:
        style_main_label_4_main_main_default.set_text_font(lv.font_montserrat_20)
    except AttributeError:
        style_main_label_4_main_main_default.set_text_font(lv.font_montserrat_16)
style_main_label_4_main_main_default.set_text_letter_space(0)
style_main_label_4_main_main_default.set_pad_left(0)
style_main_label_4_main_main_default.set_pad_right(0)
style_main_label_4_main_main_default.set_pad_top(0)
style_main_label_4_main_main_default.set_pad_bottom(0)

# add style for main_label_4
main_label_4.add_style(style_main_label_4_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

main_label_5 = lv.label(main)
main_label_5.set_pos(136,152)
main_label_5.set_size(43,21)
main_label_5.set_text("Thur")
main_label_5.set_long_mode(lv.label.LONG.WRAP)
main_label_5.set_style_text_align(lv.TEXT_ALIGN.LEFT, 0)
# create style style_main_label_5_main_main_default
style_main_label_5_main_main_default = lv.style_t()
style_main_label_5_main_main_default.init()
style_main_label_5_main_main_default.set_radius(0)
style_main_label_5_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_5_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_5_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_main_label_5_main_main_default.set_bg_opa(0)
style_main_label_5_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_main_label_5_main_main_default.set_text_font(lv.font_MiSans_Normal_20)
except AttributeError:
    try:
        style_main_label_5_main_main_default.set_text_font(lv.font_montserrat_20)
    except AttributeError:
        style_main_label_5_main_main_default.set_text_font(lv.font_montserrat_16)
style_main_label_5_main_main_default.set_text_letter_space(0)
style_main_label_5_main_main_default.set_pad_left(0)
style_main_label_5_main_main_default.set_pad_right(0)
style_main_label_5_main_main_default.set_pad_top(0)
style_main_label_5_main_main_default.set_pad_bottom(0)

# add style for main_label_5
main_label_5.add_style(style_main_label_5_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

main_label_6 = lv.label(main)
main_label_6.set_pos(15,48)
main_label_6.set_size(101,19)
main_label_6.set_text("东北风")
main_label_6.set_long_mode(lv.label.LONG.WRAP)
main_label_6.set_style_text_align(lv.TEXT_ALIGN.CENTER, 0)
# create style style_main_label_6_main_main_default
style_main_label_6_main_main_default = lv.style_t()
style_main_label_6_main_main_default.init()
style_main_label_6_main_main_default.set_radius(0)
style_main_label_6_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_6_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_6_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_main_label_6_main_main_default.set_bg_opa(0)
style_main_label_6_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_main_label_6_main_main_default.set_text_font(lv.font_MiSans_Normal_20)
except AttributeError:
    try:
        style_main_label_6_main_main_default.set_text_font(lv.font_montserrat_20)
    except AttributeError:
        style_main_label_6_main_main_default.set_text_font(lv.font_montserrat_16)
style_main_label_6_main_main_default.set_text_letter_space(2)
style_main_label_6_main_main_default.set_pad_left(0)
style_main_label_6_main_main_default.set_pad_right(0)
style_main_label_6_main_main_default.set_pad_top(0)
style_main_label_6_main_main_default.set_pad_bottom(0)

# add style for main_label_6
main_label_6.add_style(style_main_label_6_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

main_label_7 = lv.label(main)
main_label_7.set_pos(113,181)
main_label_7.set_size(35,19)
main_label_7.set_text("20")
main_label_7.set_long_mode(lv.label.LONG.WRAP)
main_label_7.set_style_text_align(lv.TEXT_ALIGN.RIGHT, 0)
# create style style_main_label_7_main_main_default
style_main_label_7_main_main_default = lv.style_t()
style_main_label_7_main_main_default.init()
style_main_label_7_main_main_default.set_radius(0)
style_main_label_7_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_7_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_7_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_main_label_7_main_main_default.set_bg_opa(0)
style_main_label_7_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_main_label_7_main_main_default.set_text_font(lv.font_MiSans_Normal_20)
except AttributeError:
    try:
        style_main_label_7_main_main_default.set_text_font(lv.font_montserrat_20)
    except AttributeError:
        style_main_label_7_main_main_default.set_text_font(lv.font_montserrat_16)
style_main_label_7_main_main_default.set_text_letter_space(2)
style_main_label_7_main_main_default.set_pad_left(0)
style_main_label_7_main_main_default.set_pad_right(0)
style_main_label_7_main_main_default.set_pad_top(0)
style_main_label_7_main_main_default.set_pad_bottom(0)

# add style for main_label_7
main_label_7.add_style(style_main_label_7_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

main_label_8 = lv.label(main)
main_label_8.set_pos(119,207)
main_label_8.set_size(31,19)
main_label_8.set_text("80")
main_label_8.set_long_mode(lv.label.LONG.WRAP)
main_label_8.set_style_text_align(lv.TEXT_ALIGN.RIGHT, 0)
# create style style_main_label_8_main_main_default
style_main_label_8_main_main_default = lv.style_t()
style_main_label_8_main_main_default.init()
style_main_label_8_main_main_default.set_radius(0)
style_main_label_8_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_8_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_main_label_8_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_main_label_8_main_main_default.set_bg_opa(0)
style_main_label_8_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_main_label_8_main_main_default.set_text_font(lv.font_MiSans_Normal_20)
except AttributeError:
    try:
        style_main_label_8_main_main_default.set_text_font(lv.font_montserrat_20)
    except AttributeError:
        style_main_label_8_main_main_default.set_text_font(lv.font_montserrat_16)
style_main_label_8_main_main_default.set_text_letter_space(2)
style_main_label_8_main_main_default.set_pad_left(0)
style_main_label_8_main_main_default.set_pad_right(0)
style_main_label_8_main_main_default.set_pad_top(0)
style_main_label_8_main_main_default.set_pad_bottom(0)

# add style for main_label_8
main_label_8.add_style(style_main_label_8_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

main_bar_1 = lv.bar(main)
main_bar_1.set_pos(36,185)
main_bar_1.set_size(80,10)
main_bar_1.set_style_anim_time(1000, lv.PART.INDICATOR|lv.STATE.DEFAULT)
main_bar_1.set_mode(lv.bar.MODE.NORMAL)
main_bar_1.set_value(50, lv.ANIM.OFF)
# create style style_main_bar_1_main_main_default
style_main_bar_1_main_main_default = lv.style_t()
style_main_bar_1_main_main_default.init()
style_main_bar_1_main_main_default.set_radius(10)
style_main_bar_1_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_main_bar_1_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_main_bar_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_main_bar_1_main_main_default.set_bg_opa(60)
style_main_bar_1_main_main_default.set_pad_left(0)
style_main_bar_1_main_main_default.set_pad_right(0)
style_main_bar_1_main_main_default.set_pad_top(0)
style_main_bar_1_main_main_default.set_pad_bottom(0)

# add style for main_bar_1
main_bar_1.add_style(style_main_bar_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_main_bar_1_main_indicator_default
style_main_bar_1_main_indicator_default = lv.style_t()
style_main_bar_1_main_indicator_default.init()
style_main_bar_1_main_indicator_default.set_radius(10)
style_main_bar_1_main_indicator_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_main_bar_1_main_indicator_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_main_bar_1_main_indicator_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_main_bar_1_main_indicator_default.set_bg_opa(255)

# add style for main_bar_1
main_bar_1.add_style(style_main_bar_1_main_indicator_default, lv.PART.INDICATOR|lv.STATE.DEFAULT)

main_bar_2 = lv.bar(main)
main_bar_2.set_pos(36,212)
main_bar_2.set_size(80,10)
main_bar_2.set_style_anim_time(1000, lv.PART.INDICATOR|lv.STATE.DEFAULT)
main_bar_2.set_mode(lv.bar.MODE.NORMAL)
main_bar_2.set_value(50, lv.ANIM.OFF)
# create style style_main_bar_2_main_main_default
style_main_bar_2_main_main_default = lv.style_t()
style_main_bar_2_main_main_default.init()
style_main_bar_2_main_main_default.set_radius(10)
style_main_bar_2_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_main_bar_2_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_main_bar_2_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_main_bar_2_main_main_default.set_bg_opa(60)
style_main_bar_2_main_main_default.set_pad_left(0)
style_main_bar_2_main_main_default.set_pad_right(0)
style_main_bar_2_main_main_default.set_pad_top(0)
style_main_bar_2_main_main_default.set_pad_bottom(0)

# add style for main_bar_2
main_bar_2.add_style(style_main_bar_2_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_main_bar_2_main_indicator_default
style_main_bar_2_main_indicator_default = lv.style_t()
style_main_bar_2_main_indicator_default.init()
style_main_bar_2_main_indicator_default.set_radius(10)
style_main_bar_2_main_indicator_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_main_bar_2_main_indicator_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_main_bar_2_main_indicator_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_main_bar_2_main_indicator_default.set_bg_opa(255)

# add style for main_bar_2
main_bar_2.add_style(style_main_bar_2_main_indicator_default, lv.PART.INDICATOR|lv.STATE.DEFAULT)

main_img_1 = lv.img(main)
main_img_1.set_pos(7,177)
main_img_1.set_size(30,25)
main_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
try:
    with open('D:\\NXP\\GUI-Guider-Projects\\1\\dddd\\generated\\mp1640973859.png','rb') as f:
        main_img_1_img_data = f.read()
except:
    print('Could not open D:\\NXP\\GUI-Guider-Projects\\1\\dddd\\generated\\mp1640973859.png')
    sys.exit()

main_img_1_img = lv.img_dsc_t({
  'data_size': len(main_img_1_img_data),
  'header': {'always_zero': 0, 'w': 30, 'h': 25, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': main_img_1_img_data
})

main_img_1.set_src(main_img_1_img)
main_img_1.set_pivot(0,0)
main_img_1.set_angle(0)
# create style style_main_img_1_main_main_default
style_main_img_1_main_main_default = lv.style_t()
style_main_img_1_main_main_default.init()
style_main_img_1_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_main_img_1_main_main_default.set_img_recolor_opa(0)
style_main_img_1_main_main_default.set_img_opa(255)

# add style for main_img_1
main_img_1.add_style(style_main_img_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

main_img_2 = lv.img(main)
main_img_2.set_pos(11,206)
main_img_2.set_size(21,22)
main_img_2.add_flag(lv.obj.FLAG.CLICKABLE)
try:
    with open('D:\\NXP\\GUI-Guider-Projects\\1\\dddd\\generated\\mp-1366986800.png','rb') as f:
        main_img_2_img_data = f.read()
except:
    print('Could not open D:\\NXP\\GUI-Guider-Projects\\1\\dddd\\generated\\mp-1366986800.png')
    sys.exit()

main_img_2_img = lv.img_dsc_t({
  'data_size': len(main_img_2_img_data),
  'header': {'always_zero': 0, 'w': 21, 'h': 22, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': main_img_2_img_data
})

main_img_2.set_src(main_img_2_img)
main_img_2.set_pivot(0,0)
main_img_2.set_angle(0)
# create style style_main_img_2_main_main_default
style_main_img_2_main_main_default = lv.style_t()
style_main_img_2_main_main_default.init()
style_main_img_2_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_main_img_2_main_main_default.set_img_recolor_opa(0)
style_main_img_2_main_main_default.set_img_opa(255)

# add style for main_img_2
main_img_2.add_style(style_main_img_2_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

screen = lv.obj()
# create style style_screen_main_main_default
style_screen_main_main_default = lv.style_t()
style_screen_main_main_default.init()
style_screen_main_main_default.set_bg_color(lv.color_make(0x00,0x00,0x00))
style_screen_main_main_default.set_bg_opa(255)

# add style for screen
screen.add_style(style_screen_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_bar_1 = lv.bar(screen)
screen_bar_1.set_pos(22,95)
screen_bar_1.set_size(200,18)
screen_bar_1.set_style_anim_time(1000, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_bar_1.set_mode(lv.bar.MODE.NORMAL)
screen_bar_1.set_value(50, lv.ANIM.OFF)
# create style style_screen_bar_1_main_main_default
style_screen_bar_1_main_main_default = lv.style_t()
style_screen_bar_1_main_main_default.init()
style_screen_bar_1_main_main_default.set_radius(10)
style_screen_bar_1_main_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_bar_1_main_main_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_bar_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_bar_1_main_main_default.set_bg_opa(26)
style_screen_bar_1_main_main_default.set_pad_left(0)
style_screen_bar_1_main_main_default.set_pad_right(0)
style_screen_bar_1_main_main_default.set_pad_top(0)
style_screen_bar_1_main_main_default.set_pad_bottom(0)

# add style for screen_bar_1
screen_bar_1.add_style(style_screen_bar_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_bar_1_main_indicator_default
style_screen_bar_1_main_indicator_default = lv.style_t()
style_screen_bar_1_main_indicator_default.init()
style_screen_bar_1_main_indicator_default.set_radius(10)
style_screen_bar_1_main_indicator_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_bar_1_main_indicator_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_bar_1_main_indicator_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_bar_1_main_indicator_default.set_bg_opa(255)

# add style for screen_bar_1
screen_bar_1.add_style(style_screen_bar_1_main_indicator_default, lv.PART.INDICATOR|lv.STATE.DEFAULT)

screen_label_1 = lv.label(screen)
screen_label_1.set_pos(32,130)
screen_label_1.set_size(189,21)
screen_label_1.set_text("connect wifi...")
screen_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_label_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, 0)
# create style style_screen_label_1_main_main_default
style_screen_label_1_main_main_default = lv.style_t()
style_screen_label_1_main_main_default.init()
style_screen_label_1_main_main_default.set_radius(0)
style_screen_label_1_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_label_1_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_label_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_label_1_main_main_default.set_bg_opa(0)
style_screen_label_1_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_label_1_main_main_default.set_text_font(lv.font_MiSans_Normal_20)
except AttributeError:
    try:
        style_screen_label_1_main_main_default.set_text_font(lv.font_montserrat_20)
    except AttributeError:
        style_screen_label_1_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_label_1_main_main_default.set_text_letter_space(2)
style_screen_label_1_main_main_default.set_pad_left(0)
style_screen_label_1_main_main_default.set_pad_right(0)
style_screen_label_1_main_main_default.set_pad_top(0)
style_screen_label_1_main_main_default.set_pad_bottom(0)

# add style for screen_label_1
screen_label_1.add_style(style_screen_label_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)




# content from custom.py

# Load the default screen
lv.scr_load(main)

while SDL.check():
    time.sleep_ms(5)
