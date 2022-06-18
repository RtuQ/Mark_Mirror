#include "st7789.h"
#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MISO_PIN_NUM -1
#define MOSI_PIN_NUM 23
#define CLK_PIN_NUM 18
//#define PIN_NUM_CS   22
#define DC_PIN_NUM 2
#define RST_PIN_NUM 4
#define BCKL_PIN_NUM 5

typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t data_bytes; // Length of data in above data array; 0xFF = end of cmds.
} lcd_init_cmd_t;

spi_device_handle_t spi;

// Place data into DRAM. Constant data gets placed into DROM by default, which is not accessible by DMA.
DRAM_ATTR static const lcd_init_cmd_t st_init_cmds[] = {
    /* Sleep Out */
    { 0x11, { 0 }, 0x80 },
    /* Memory Data Access Control, MX=MV=0, MY=ML=MH=0, RGB=0 */
    { 0x36, { 0 }, 1 },
    /* Interface Pixel Format, 18bits/pixel for RGB/MCU interface */
    { 0x3A, { 0x55 }, 1 },
    /* Porch Setting */
    { 0xB2, { 0x0c, 0x0c, 0x00, 0x33, 0x33 }, 5 },
    /* Gate Control, Vgh=13.65V, Vgl=-10.43V */
    { 0xB7, { 0x35 }, 1 },
    /* VCOM Setting, VCOM=1.175V */
    { 0xBB, { 0x32 }, 1 },
    /* VDV and VRH Command Enable, enable=1 */
    { 0xC2, { 0x01 }, 1 },
    /* VRH Set, Vap=4.4+... */
    { 0xC3, { 0x19 }, 1 },
    /* VDV Set, VDV=0 */
    { 0xC4, { 0x20 }, 1 },
    /* Frame Rate Control, 60Hz, inversion=0 */
    { 0xC6, { 0x0f }, 1 },
    /* Power Control 1, AVDD=6.8V, AVCL=-4.8V, VDDS=2.3V */
    { 0xD0, { 0xA4, 0xA1 }, 1 },
    /* Positive Voltage Gamma Control */
    { 0xE0, { 0xD0, 0x08, 0x0E, 0x09, 0x09, 0x05, 0x31, 0x33, 0x48, 0x17, 0x14, 0x15, 0x31, 0x34 }, 14 },
    /* Negative Voltage Gamma Control */
    { 0xE1, { 0xD0, 0x08, 0x0E, 0x09, 0x09, 0x05, 0x31, 0x33, 0x48, 0x17, 0x14, 0x15, 0x31, 0x34 }, 14 },
    /* Display Inversion OFF */
    //   {0x20, {0}, 0x80},
    /* Display Inversion ON */
    { 0x21, { 0 }, 0x80 },
    /* Display On */
    { 0x29, { 0 }, 0x80 },
    { 0, { 0 }, 0xff }
};



void lcd_cmd(const uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

void lcd_data(const uint8_t *data, int len)
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

void lcd_spi_pre_transfer_callback(spi_transaction_t* t)
{
    int dc = (int)t->user;
    gpio_set_level(DC_PIN_NUM, dc);
}

esp_err_t __esp_spi_init(void)
{
    esp_err_t ret;
    spi_bus_config_t bus_cfg = {
        .miso_io_num = -1,
        .mosi_io_num = MOSI_PIN_NUM,
        .sclk_io_num = CLK_PIN_NUM,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 240 * 240 * 3 + 8
    };
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 10 * 1000 * 1000, // Clock out at 10 MHz
        .mode = 0, // SPI mode 3
        .spics_io_num = -1, // CS pin
        .queue_size = 7, // We want to be able to queue 7 transactions at a time
        .pre_cb = lcd_spi_pre_transfer_callback, // Specify pre-transfer callback to handle D/C line
    };
    // Initialize the SPI bus
    ret = spi_bus_initialize(HSPI_HOST, &bus_cfg, 1);
    ESP_ERROR_CHECK(ret);
    // Attach the LCD to the SPI bus
    ret = spi_bus_add_device(HSPI_HOST, &dev_cfg, &spi);
    ESP_ERROR_CHECK(ret);
    return ret;
}


void st7789_fill_react_data(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t*buf)
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

    trans[0].tx_data[0]=0x2A;           //Column Address Set
    trans[1].tx_data[0]=(xsta>>8)&0xff;              //Start Col High
    trans[1].tx_data[1]=xsta&0xff;              //Start Col Low
    trans[1].tx_data[2]=(xend>>8)&0xff;       //End Col High
    trans[1].tx_data[3]=xend&0xff;     //End Col Low
    trans[2].tx_data[0]=0x2B;           //Page address set
    trans[3].tx_data[0]=(ysta>>8)&0xff;        //Start page high
    trans[3].tx_data[1]=ysta&0xff;      //start page low
    trans[3].tx_data[2]=(yend>>8)&0xff;    //end page high
    trans[3].tx_data[3]=yend&0xff;  //end page low
    trans[4].tx_data[0]=0x2C;           //memory write
    trans[5].tx_buffer=buf;        //finally send the line data
    trans[5].length=width*height*2*8;          //Data length, in bits
    trans[5].flags=0; //undo SPI_TRANS_USE_TXDATA flag

    //Queue all transactions.
    for (x=0; x<6; x++) {
        ret=spi_device_queue_trans(spi, &trans[x], portMAX_DELAY);
        assert(ret==ESP_OK);
    } 					  	    
}


/**
* @brief st7789 tft 初始化
*
* @param[in] 
* @return 
*/
esp_err_t st7789_tft_init(void)
{
    int cmd = 0;
    uint16_t buf[10*240];
    const lcd_init_cmd_t* lcd_init_cmds;

    __esp_spi_init();

    // Initialize non-SPI GPIOs
    gpio_set_direction(DC_PIN_NUM, GPIO_MODE_OUTPUT);
    gpio_set_direction(RST_PIN_NUM, GPIO_MODE_OUTPUT);
    gpio_set_direction(BCKL_PIN_NUM, GPIO_MODE_OUTPUT);

    // Reset the display
    gpio_set_level(RST_PIN_NUM, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(RST_PIN_NUM, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    printf("ST7789V detected.\n");
    printf("LCD ST7789V initialization.\n");
    lcd_init_cmds = st_init_cmds;

    // Send all the commands
    while (lcd_init_cmds[cmd].data_bytes != 0xff) {
        lcd_cmd(lcd_init_cmds[cmd].cmd);
        if (lcd_init_cmds[cmd].data_bytes & 0x80) {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        } else {
            lcd_data(lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].data_bytes & 0x1F);
        }

        cmd++;
    }

    for (uint16_t i = 0; i < 10*240;i++) {
        buf[i] = 0xf800;
    }

    st7789_fill_react_data(0,0,10,239,buf);

    return ESP_OK;
}


