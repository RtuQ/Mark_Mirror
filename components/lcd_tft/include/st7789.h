#ifndef ST7789_H
#define ST7789_H

/*********************
 *      INCLUDES
 *********************/
#include "esp_system.h"
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**
* @brief st7789 tft 初始化
*
* @param[in] 
* @return 
*/
esp_err_t st7789_tft_init(void);


void st7789_tft_disp_flush(const lv_area_t * area, lv_color_t * color_p);

/**********************
 *      MACROS
 **********************/

#endif /*LV_PORT_DISP_TEMPL_H*/