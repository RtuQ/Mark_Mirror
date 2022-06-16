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
#include "lvgl/lvgl.h"
#include <stdbool.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

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


#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
//#define PIN_NUM_CS   22
#define PIN_NUM_DC   2
#define PIN_NUM_RST  4
#define PIN_NUM_BCKL 5

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
spi_device_handle_t spi;
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
typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t data_bytes; // Length of data in above data array; 0xFF = end of cmds.
} lcd_init_cmd_t;

//Place data into DRAM. Constant data gets placed into DROM by default, which is not accessible by DMA.
DRAM_ATTR static const lcd_init_cmd_t st_init_cmds[]={
    /* Sleep Out */
    {0x11, {0}, 0x80},
    /* Memory Data Access Control, MX=MV=0, MY=ML=MH=0, RGB=0 */
    {0x36, {0}, 1},
    /* Interface Pixel Format, 18bits/pixel for RGB/MCU interface */
    {0x3A, {0x66}, 1},
    /* Porch Setting */
    {0xB2, {0x0c, 0x0c, 0x00, 0x33, 0x33}, 5},
    /* Gate Control, Vgh=13.65V, Vgl=-10.43V */
    {0xB7, {0x35}, 1},
    /* VCOM Setting, VCOM=1.175V */
    {0xBB, {0x32}, 1},
    /* VDV and VRH Command Enable, enable=1 */
    {0xC2, {0x01}, 1},
    /* VRH Set, Vap=4.4+... */
    {0xC3, {0x19}, 1},
    /* VDV Set, VDV=0 */
    {0xC4, {0x20}, 1},
    /* Frame Rate Control, 60Hz, inversion=0 */
    {0xC6, {0x0f}, 1},
    /* Power Control 1, AVDD=6.8V, AVCL=-4.8V, VDDS=2.3V */
    {0xD0, {0xA4, 0xA1}, 1},
    /* Positive Voltage Gamma Control */
    {0xE0, {0xD0, 0x08, 0x0E, 0x09, 0x09, 0x05, 0x31, 0x33, 0x48, 0x17, 0x14, 0x15, 0x31, 0x34}, 14},
    /* Negative Voltage Gamma Control */
    {0xE1, {0xD0, 0x08, 0x0E, 0x09, 0x09, 0x05, 0x31, 0x33, 0x48, 0x17, 0x14, 0x15, 0x31, 0x34}, 14},
    /* Display Inversion OFF */
 //   {0x20, {0}, 0x80},
    /* Display Inversion ON */
    {0x21, {0}, 0x80},
    /* Display On */
    {0x29, {0}, 0x80},
    {0, {0}, 0xff}
};

void lcd_cmd(spi_device_handle_t spi, const uint8_t cmd, bool keep_cs_active)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    if (keep_cs_active) {
      t.flags = SPI_TRANS_CS_KEEP_ACTIVE;   //Keep CS active after data transfer
    }
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

void lcd_data(spi_device_handle_t spi, const uint8_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (len==0) return;             //no need to send anything
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=data;               //Data
    t.user=(void*)1;                //D/C needs to be set to 1
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

//Initialize the display
void lcd_init(spi_device_handle_t spi)
{
    int cmd=0;
    const lcd_init_cmd_t* lcd_init_cmds;

    //Initialize non-SPI GPIOs
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);

    //Reset the display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    printf("ST7789V detected.\n");
    printf("LCD ST7789V initialization.\n");
    lcd_init_cmds = st_init_cmds;

    //Send all the commands
    while (lcd_init_cmds[cmd].data_bytes!=0xff) {
        lcd_cmd(spi, lcd_init_cmds[cmd].cmd,0);
        if (lcd_init_cmds[cmd].data_bytes&0x80) {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        } else {
            lcd_data(spi, lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].data_bytes&0x1F);
        }

        cmd++;
    }
    

}

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc=(int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

static void _lcd_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd){
	uint8_t setxcmd=0x2A;
	uint8_t setycmd=0x2B;
	uint8_t wramcmd=0x2C;
    lcd_cmd(spi,setxcmd,0);
    lcd_data(spi,(uint8_t[]){xStar>>8, 0x00FF&xStar, xEnd>>8, 0x00FF&xEnd}, 4);
    
    lcd_cmd(spi,setycmd,0);	
	lcd_data(spi,(uint8_t[]){yStar>>8, 0x00FF&yStar, yEnd>>8, 0x00FF&yEnd}, 4);

	lcd_cmd(spi,wramcmd,0);   //start writing GRAM			
}

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
    esp_err_t ret;
    uint16_t color[2];
    spi_bus_config_t buscfg={
        .miso_io_num=-1,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=240*240*2+8
    };
    spi_device_interface_config_t devcfg={
        .clock_speed_hz=10*1000*1000,           //Clock out at 10 MHz
        .mode=3,                                //SPI mode 3
        .spics_io_num=-1,                       //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        .pre_cb=lcd_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
    };
    //Initialize the SPI bus
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    ESP_ERROR_CHECK(ret);
    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);
    //Initialize the LCD
    lcd_init(spi);
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

void st7789_fill_react_data(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t*color)
{          
    esp_err_t ret;
    int x;
    uint16_t width, height;
    //Transaction descriptors. Declared static so they're not allocated on the stack; we need this memory even when this
    //function is finished because the SPI driver needs access to it even while we're already calculating the next line.
    static spi_transaction_t trans[6];
    width = xend - xsta + 1;
    height = yend - ysta + 1;

    //In theory, it's better to initialize trans and data only once and hang on to the initialized
    //variables. We allocate them on the stack, so we need to re-init them each call.
    for (x=0; x<6; x++) {
        memset(&trans[x], 0, sizeof(spi_transaction_t));
        if ((x&1)==0) {
            //Even transfers are commands
            trans[x].length=8;
            trans[x].user=(void*)0;
        } else {
            //Odd transfers are data
            trans[x].length=8*4;
            trans[x].user=(void*)1;
        }
        trans[x].flags=SPI_TRANS_USE_TXDATA;
    }
    printf("=========> %d %d %d\r\n", xend,yend,strlen(color));
    trans[0].tx_data[0]=0x2A;           //Column Address Set
    trans[1].tx_data[0]=xsta>>8;              //Start Col High
    trans[1].tx_data[1]=xsta&0xff;              //Start Col Low
    trans[1].tx_data[2]=xend>>8;       //End Col High
    trans[1].tx_data[3]=xend&0xff;     //End Col Low
    trans[2].tx_data[0]=0x2B;           //Page address set
    trans[3].tx_data[0]=ysta>>8;        //Start page high
    trans[3].tx_data[1]=ysta&0xff;      //start page low
    trans[3].tx_data[2]=yend>>8;    //end page high
    trans[3].tx_data[3]=yend&0xff;  //end page low
    trans[4].tx_data[0]=0x2C;           //memory write
    trans[5].tx_buffer=color;        //finally send the line data
    trans[5].length=width*height*2;          //Data length, in bits
    trans[5].flags=0; //undo SPI_TRANS_USE_TXDATA flag

    //Queue all transactions.
    for (x=0; x<6; x++) {
        ret=spi_device_queue_trans(spi, &trans[x], portMAX_DELAY);
        assert(ret==ESP_OK);
    } 					  	    
}




void LCD_Disp_Flush(const lv_area_t * area, lv_color_t * color_p){
    int32_t cnt = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 3;
    uint16_t i = 0;
    uint16_t index = 0;
    uint16_t numTime = 0;   //多少组GRAM_BUFF
    uint16_t numSurplus = 0;//最后剩下的

    if(cnt < 0){
        printf("parameter setting error!!!\n");
        return;
    }

    _lcd_SetWindows(area->x1, area->y1, area->x2, area->y2);

    // printf("CNT  %d\n\n\n",cnt);
    if(cnt <= USER_BUFF_LEN){
        memset(GRAM_BUFF, 0, USER_BUFF_LEN);
        for(i=0;i<cnt;){
            GRAM_BUFF[i] = RGB_R(color_p->full);
            GRAM_BUFF[i+1] = RGB_G(color_p->full);
            GRAM_BUFF[i+2] = RGB_B(color_p->full);
            i += 3;
            color_p++;
        }
        lcd_data(spi,GRAM_BUFF, cnt);
    }else{
        numTime = cnt/USER_BUFF_LEN;
        numSurplus = cnt%USER_BUFF_LEN;
        // printf("NumTime:%d  NumSurplus:%d\n\n", numTime, numSurplus);
        for(uint16_t j=0;j<numTime;j++){
            // index = j*USER_BUFF_LEN;
            memset(GRAM_BUFF, 0, USER_BUFF_LEN);
            for(i=0;i<USER_BUFF_LEN;){
                GRAM_BUFF[i] = RGB_R(color_p->full);
                GRAM_BUFF[i+1] = RGB_G(color_p->full);
                GRAM_BUFF[i+2] = RGB_B(color_p->full);
                i += 3;
                color_p++;
            }
            lcd_data(spi,GRAM_BUFF, USER_BUFF_LEN);
        }
        if(numSurplus > 0){
            // index = cnt-numSurplus;
            memset(GRAM_BUFF, 0, USER_BUFF_LEN);
            for(i=0;i<=numSurplus;){
                GRAM_BUFF[i] = RGB_R(color_p->full);
                GRAM_BUFF[i+1] = RGB_G(color_p->full);
                GRAM_BUFF[i+2] = RGB_B(color_p->full);
                i += 3;
                color_p++;
            }
            lcd_data(spi,GRAM_BUFF, numSurplus);
        }
    }
}

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
    LCD_Disp_Flush(area,color_p);

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
