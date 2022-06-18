/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lv_port_disp.h"
#include "lvgl.h"
#include <stdbool.h>

#include "st7789.h"

/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
    #define MY_DISP_HOR_RES    240
#endif

#ifndef MY_DISP_VER_RES
    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen height, default value 240 is used for now.
    #define MY_DISP_VER_RES    240
#endif




#define PARALLEL_LINES 16

#define USER_BUFF_LEN       720  

#define RGB_R(n)	(n>>8)&0xF8
#define RGB_G(n)	(n>>3)&0xFC
#define RGB_B(n)	n<<3

uint8_t GRAM_BUFF[USER_BUFF_LEN] = {0};
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    /* Example for 1) */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[MY_DISP_HOR_RES * 10];                          /*A buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

    // /* Example for 2) */
    // static lv_disp_draw_buf_t draw_buf_dsc_2;
    // static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                        /*A buffer for 10 rows*/
    // static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];                        /*An other buffer for 10 rows*/
    // lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

    // /* Example for 3) also set disp_drv.full_refresh = 1 below*/
    // static lv_disp_draw_buf_t draw_buf_dsc_3;
    // static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
    // static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*Another screen sized buffer*/
    // lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
    //                       MY_DISP_VER_RES * LV_VER_RES);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_1;

    /*Required for Example 3)*/
    //disp_drv.full_refresh = 1

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

// static void _lcd_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd){
// 	uint8_t setxcmd=0x2A;
// 	uint8_t setycmd=0x2B;
// 	uint8_t wramcmd=0x2C;
//     lcd_cmd(spi,setxcmd,0);
//     lcd_data(spi,(uint8_t[]){xStar>>8, 0x00FF&xStar, xEnd>>8, 0x00FF&xEnd}, 4);
    
//     lcd_cmd(spi,setycmd,0);	
// 	lcd_data(spi,(uint8_t[]){yStar>>8, 0x00FF&yStar, yEnd>>8, 0x00FF&yEnd}, 4);

// 	lcd_cmd(spi,wramcmd,0);   //start writing GRAM			
// }

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    //Initialize the LCD

}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

// void LCD_Disp_Flush(const lv_area_t * area, lv_color_t * color_p)
// {
//     int32_t cnt = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 3;
//     uint16_t i = 0;
//     uint16_t index = 0;
//     uint16_t numTime = 0;   //多少组GRAM_BUFF
//     uint16_t numSurplus = 0;//最后剩下的

//     if(cnt < 0){
//         printf("parameter setting error!!!\n");
//         return;
//     }

//     _lcd_SetWindows(area->x1, area->y1, area->x2, area->y2);

//     // printf("CNT  %d\n\n\n",cnt);
//     if(cnt <= USER_BUFF_LEN){
//         memset(GRAM_BUFF, 0, USER_BUFF_LEN);
//         for(i=0;i<cnt;){
//             GRAM_BUFF[i] = RGB_R(color_p->full);
//             GRAM_BUFF[i+1] = RGB_G(color_p->full);
//             GRAM_BUFF[i+2] = RGB_B(color_p->full);
//             i += 3;
//             color_p++;
//         }
//         lcd_data(spi,GRAM_BUFF, cnt);
//     }else{
//         numTime = cnt/USER_BUFF_LEN;
//         numSurplus = cnt%USER_BUFF_LEN;
//         // printf("NumTime:%d  NumSurplus:%d\n\n", numTime, numSurplus);
//         for(uint16_t j=0;j<numTime;j++){
//             // index = j*USER_BUFF_LEN;
//             memset(GRAM_BUFF, 0, USER_BUFF_LEN);
//             for(i=0;i<USER_BUFF_LEN;){
//                 GRAM_BUFF[i] = RGB_R(color_p->full);
//                 GRAM_BUFF[i+1] = RGB_G(color_p->full);
//                 GRAM_BUFF[i+2] = RGB_B(color_p->full);
//                 i += 3;
//                 color_p++;
//             }
//             lcd_data(spi,GRAM_BUFF, USER_BUFF_LEN);
//         }
//         if(numSurplus > 0){
//             // index = cnt-numSurplus;
//             memset(GRAM_BUFF, 0, USER_BUFF_LEN);
//             for(i=0;i<=numSurplus;){
//                 GRAM_BUFF[i] = RGB_R(color_p->full);
//                 GRAM_BUFF[i+1] = RGB_G(color_p->full);
//                 GRAM_BUFF[i+2] = RGB_B(color_p->full);
//                 i += 3;
//                 color_p++;
//             }
//             lcd_data(spi,GRAM_BUFF, numSurplus);
//         }
//     }
// }

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    // if(disp_flush_enabled) {
    //     /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

    //     int32_t x;
    //     int32_t y;
    //     for(y = area->y1; y <= area->y2; y++) {
    //         for(x = area->x1; x <= area->x2; x++) {
    //             /*Put a pixel to the display. For example:*/
    //             /*put_px(x, y, *color_p)*/
    //             st7789_fill_react_data(area->x1,area->y1,area->x2,area->y2,(uint16_t *)color_p);
                
    //             color_p++;
    //         }
    //     }
    // }
    // st7789_fill_react_data(area->x1,area->y1,area->x2,area->y2,color_p->full);
    // LCD_Disp_Flush(area,color_p);

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
