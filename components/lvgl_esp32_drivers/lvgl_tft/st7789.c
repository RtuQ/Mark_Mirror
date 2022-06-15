/**
 * @file st7789.c
 *
 * Mostly taken from lbthomsen/esp-idf-littlevgl github.
 */

#include "st7789.h"

#include "disp_spi.h"
#include "display_port.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct. */
typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void st7789_set_orientation(lv_disp_drv_t *drv, uint8_t orientation);
static void st7789_send_cmd(lv_disp_drv_t * drv, uint8_t cmd);
static void st7789_send_data(lv_disp_drv_t * drv, void *data, uint16_t length);
static void st7789_send_color(lv_disp_drv_t * drv, void *data, uint16_t length);
static void st7789_reset(lv_disp_drv_t * drv);

static void setup_initial_offsets(lv_disp_drv_t * drv);
static lv_coord_t get_display_hor_res(lv_disp_drv_t * drv);
static lv_coord_t get_display_ver_res(lv_disp_drv_t * drv);
/**********************
 *  STATIC VARIABLES
 **********************/
static uint16_t user_x_offset = 0u;
static uint16_t user_y_offset = 0u;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void st7789_init(lv_disp_drv_t *drv)
{
    setup_initial_offsets(drv);

    lcd_init_cmd_t st7789_init_cmds[] = {
        /* Memory Data Access Control, MX=MV=0, MY=ML=MH=0, RGB=0 */
        {0x36, {0}, 1},
        /* Interface Pixel Format, 18bits/pixel for RGB/MCU interface */
        {0x3A, {0x66}, 1},
        /* Porch Setting */
        {0xB2, {0x0c, 0x0c, 0x00, 0x33, 0x33}, 5},
        /* Gate Control, Vgh=13.65V, Vgl=-10.43V */
        {0xB7, {0x45}, 1},
        /* VCOM Setting, VCOM=1.175V */
        {0xBB, {0x2B}, 1},
        /* LCM Control, XOR: BGR, MX, MH */
        {0xC0, {0x2C}, 1},
        /* VDV and VRH Command Enable, enable=1 */
        {0xC2, {0x01, 0xff}, 2},
        /* VRH Set, Vap=4.4+... */
        {0xC3, {0x11}, 1},
        /* VDV Set, VDV=0 */
        {0xC4, {0x20}, 1},
        /* Frame Rate Control, 60Hz, inversion=0 */
        {0xC6, {0x0f}, 1},
        /* Power Control 1, AVDD=6.8V, AVCL=-4.8V, VDDS=2.3V */
        {0xD0, {0xA4, 0xA1}, 1},
        /* Positive Voltage Gamma Control */
        {0xE0, {0xD0, 0x00, 0x05, 0x0E, 0x15, 0x0D, 0x37, 0x43, 0x47, 0x09, 0x15, 0x12, 0x16, 0x19}, 14},
        /* Negative Voltage Gamma Control */
        {0xE1, {0xD0, 0x00, 0x05, 0x0D, 0x0C, 0x06, 0x2D, 0x44, 0x40, 0x0E, 0x1C, 0x18, 0x16, 0x19}, 14},
        /* Sleep Out */
        {0x11, {0}, 0x80},
        /* Display Inversion OFF */
        //   {0x20, {0}, 0x80},
        /* Display Inversion ON */
        {0x21, {0}, 0x80},
        /* Display On */
        {0x29, {0}, 0x80},
        {0, {0}, 0xff}
    };

    st7789_reset(drv);

    //Send all the commands
    uint16_t cmd = 0;
    while (st7789_init_cmds[cmd].databytes!=0xff) {
        st7789_send_cmd(drv, st7789_init_cmds[cmd].cmd);
        st7789_send_data(drv, st7789_init_cmds[cmd].data, st7789_init_cmds[cmd].databytes&0x1F);
        if (st7789_init_cmds[cmd].databytes & 0x80) {
            display_port_delay(drv, 100);
        }
        cmd++;
    }

    /* NOTE: Setting rotation from lv_disp_drv_t instead of menuconfig */
    lv_disp_rot_t rotation;
#if (LVGL_VERSION_MAJOR >= 8)
    rotation = lv_disp_get_rotation((lv_disp_t *) &drv);
#else
    rotation = lv_disp_get_rotation((lv_disp_t *) drv);
#endif

    st7789_set_orientation(drv, rotation);
}

/* The ST7789 display controller can drive up to 320*240 displays, when using a 240*240 or 240*135
 * displays there's a gap of 80px or 40/52/53px respectively. 52px or 53x offset depends on display orientation.
 * We need to edit the coordinates to take into account those gaps, this is not necessary in all orientations. */
void st7789_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map)
{
    uint8_t data[4] = {0};

    uint16_t offsetx1 = area->x1;
    uint16_t offsetx2 = area->x2;
    uint16_t offsety1 = area->y1;
    uint16_t offsety2 = area->y2;
    uint32_t size = lv_area_get_width(area) * lv_area_get_height(area);

    /* On LVGLv7 we have to manually update the driver orientation,
     * in LVGLv8 we use the driver update callback. */
#if (LVGL_VERSION_MAJOR < 8)
    static lv_disp_rot_t cached_rotation = LV_DISP_ROT_NONE;
    lv_disp_rot_t rotation = lv_disp_get_rotation((lv_disp_t *) drv);
    if (cached_rotation != rotation) {
        st7789_set_orientation(drv, (uint8_t) rotation);
        /* Update offset values */
        setup_initial_offsets(drv);
        cached_rotation = rotation;
    }
#endif

    offsetx1 += st7789_x_offset();
    offsetx2 += st7789_x_offset();
    offsety1 += st7789_y_offset();
    offsety2 += st7789_y_offset();

    /*Column addresses*/
    st7789_send_cmd(drv, ST7789_CASET);
    data[0] = (offsetx1 >> 8) & 0xFF;
    data[1] = offsetx1 & 0xFF;
    data[2] = (offsetx2 >> 8) & 0xFF;
    data[3] = offsetx2 & 0xFF;
    st7789_send_data(drv, data, 4);

    /*Page addresses*/
    st7789_send_cmd(drv, ST7789_RASET);
    data[0] = (offsety1 >> 8) & 0xFF;
    data[1] = offsety1 & 0xFF;
    data[2] = (offsety2 >> 8) & 0xFF;
    data[3] = offsety2 & 0xFF;
    st7789_send_data(drv, data, 4);

    /*Memory write*/
    st7789_send_cmd(drv, ST7789_RAMWR);
    st7789_send_color(drv, (void*) color_map, size * 2);
}

void st7789_set_x_offset(const uint16_t offset)
{
    user_x_offset = offset;
}

void st7789_set_y_offset(const uint16_t offset)
{
    user_y_offset = offset;
}

uint16_t st7789_x_offset(void)
{
    return user_x_offset;
}

uint16_t st7789_y_offset(void)
{
    return user_y_offset;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void st7789_send_cmd(lv_disp_drv_t *drv, uint8_t cmd)
{
    disp_wait_for_pending_transactions();
    display_port_gpio_dc(drv, 0);
    disp_spi_send_data(&cmd, 1);
}

static void st7789_send_data(lv_disp_drv_t *drv, void * data, uint16_t length)
{
    disp_wait_for_pending_transactions();
    display_port_gpio_dc(drv, 1);
    disp_spi_send_data(data, length);
}

static void st7789_send_color(lv_disp_drv_t *drv, void * data, uint16_t length)
{
    disp_wait_for_pending_transactions();
    display_port_gpio_dc(drv, 1);
    disp_spi_send_colors(data, length);
}

/* Reset the display, if we don't have a reset pin we use software reset */
static void st7789_reset(lv_disp_drv_t *drv)
{
#if !defined(ST7789_SOFT_RST)
    display_port_gpio_rst(drv, 0);
    display_port_delay(drv, 100);
    display_port_gpio_rst(drv, 1);
    display_port_delay(drv, 100);
#else
    st7789_send_cmd(drv, ST7789_SWRESET);
    display_port_delay(drv, 5);
#endif
}

static void st7789_set_orientation(lv_disp_drv_t *drv, uint8_t orientation)
{
    uint8_t data[] =
    {
#if CONFIG_LV_PREDEFINED_DISPLAY_TTGO
	0x60, 0xA0, 0x00, 0xC0
#else
	0xC0, 0x60, 0x00, 0xA0
#endif
    };

    st7789_send_cmd(drv, ST7789_MADCTL);
    st7789_send_data(drv, (void *) &data[orientation], 1);
}

static void setup_initial_offsets(lv_disp_drv_t * drv)
{
    lv_disp_rot_t rotation;
#if (LVGL_VERSION_MAJOR >= 8)
    rotation = lv_disp_get_rotation((lv_disp_t *) &drv);
#else
    rotation = lv_disp_get_rotation((lv_disp_t *) drv);
#endif

#if (CONFIG_LV_TFT_DISPLAY_OFFSETS)
    st7789_set_x_offset(CONFIG_LV_TFT_DISPLAY_X_OFFSET);
    st7789_set_y_offset(CONFIG_LV_TFT_DISPLAY_Y_OFFSET);
#else
    if (240U == get_display_hor_res(drv) && 135U == get_display_ver_res(drv))
    {
        if (LV_DISP_ROT_NONE == rotation || LV_DISP_ROT_180 == rotation)
        {
            st7789_set_x_offset(40);
            st7789_set_y_offset(53);
        }
        else
        {
            st7789_set_x_offset(0);
            st7789_set_y_offset(0);
        }
    }
    else if (135U == get_display_hor_res(drv) && 240U == get_display_ver_res(drv))
    {
        if (LV_DISP_ROT_90 == rotation || LV_DISP_ROT_270 == rotation)
        {
            st7789_set_x_offset(52);
            st7789_set_y_offset(40);
        }
        else
        {
            st7789_set_x_offset(0);
            st7789_set_y_offset(0);
        }
    }
    else if (240U == get_display_hor_res(drv) && 240U == get_display_ver_res(drv))
    {
        if (LV_DISP_ROT_NONE == rotation)
        {
            st7789_set_x_offset(80);
            st7789_set_y_offset(0);
        }
        else if (LV_DISP_ROT_90 == rotation || LV_DISP_ROT_180 == rotation)
        {
            st7789_set_x_offset(0);
            st7789_set_y_offset(0);
        }
        else if (LV_DISP_ROT_270 == rotation)
        {
            st7789_set_x_offset(0);
            st7789_set_y_offset(80);
        }
    }
#endif
}

/* Display update callback, we could update the orientation in here
 * NOTE Available only for LVGL v8 */
void st7789_update_cb(lv_disp_drv_t *drv)
{
    lv_disp_rot_t rotation;
#if (LVGL_VERSION_MAJOR >= 8)
    rotation = lv_disp_get_rotation((lv_disp_t *) &drv);
#else
    rotation = lv_disp_get_rotation((lv_disp_t *) drv);
#endif

    st7789_set_orientation(drv, (uint8_t) rotation);
    setup_initial_offsets(drv);
}

static lv_coord_t get_display_hor_res(lv_disp_drv_t * drv)
{
    lv_coord_t retval = 0;

#if (LVGL_VERSION_MAJOR >= 8)
    retval = drv->hor_res;
#else
    (void) drv;
    retval = LV_HOR_RES_MAX;
#endif

    return retval;
}

static lv_coord_t get_display_ver_res(lv_disp_drv_t * drv)
{
    lv_coord_t retval = 0;

#if (LVGL_VERSION_MAJOR >= 8)
    retval = drv->ver_res;
#else
    (void) drv;
    retval = LV_VER_RES_MAX;
#endif

    return retval;
}
