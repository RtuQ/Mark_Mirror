/* i2c - Simple example

   Simple I2C example that shows how to initialize I2C
   as well as reading and writing from and to registers for a sensor connected over I2C.

   The sensor used in this example is a MPU6050 inertial measurement unit.

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "mpu6050.h"
#include "driver/i2c.h"
#include "esp_timer.h"

#define I2C_MASTER_SCL_IO           33      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           32      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define MPU6050_SENSOR_ADDR                 0x68        /*!< Slave address of the MPU6050 sensor */
#define MPU6050_WHO_AM_I_REG_ADDR           0x75        /*!< Register addresses of the "who am I" register */

#define MPU6050_PWR_MGMT_1_REG_ADDR         0x6B        /*!< Register addresses of the power managment register */
#define MPU6050_RESET_BIT                   7

/**
 * @brief Read a sequence of bytes from a MPU6050 sensor registers
 */
static esp_err_t mpu6050_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_SENSOR_ADDR, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

/**
 * @brief Write a byte to a MPU6050 sensor register
 */
static esp_err_t mpu6050_register_write_byte(uint8_t reg_addr, uint8_t data)
{
    int ret;
    uint8_t write_buf[2] = {reg_addr, data};

    ret = i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_SENSOR_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    return ret;
}

/**
 * @brief i2c master initialization
 */
esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}



/**
  * @brief   设置MPU6050的数字低通滤波器
  * @param   lpf:数字低通滤波频率(Hz)
  * @retval  正常返回0，异常返回1
  */
uint8_t mpu6050_set_lpf(uint16_t lpf)
{
	uint8_t data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return mpu6050_register_write_byte(MPU6050_RA_CONFIG,data);//设置数字低通滤波器  
}

/**
  * @brief   设置MPU6050的采样率(假定Fs=1KHz)
  * @param   rate:4~1000(Hz)
  * @retval  正常返回0，异常返回1
  */
esp_err_t mpu6050_set_rate(uint16_t rate)
{
	uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	mpu6050_register_read(MPU6050_RA_SMPLRT_DIV,&data,1);	//设置数字低通滤波器
 	return mpu6050_set_lpf(rate/2);	//自动设置LPF为采样率的一半
}



/**
  * @brief   设置MPU6050陀螺仪传感器满量程范围
  * @param   fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
  * @retval  正常返回0，异常返回1
  */
esp_err_t mpu6050_set_gyro_fsr(uint8_t fsr)
{
	return mpu6050_register_write_byte(MPU6050_RA_GYRO_CONFIG,fsr<<3);//设置陀螺仪满量程范围  
}

/**
  * @brief   设置MPU6050加速度传感器满量程范围
  * @param   fsr:0,±2g;1,±4g;2,±8g;3,±16g
  * @retval  正常返回0，异常返回1
  */
esp_err_t mpu6050_set_accel_fsr(uint8_t fsr)
{
	return mpu6050_register_write_byte(MPU6050_RA_ACCEL_CONFIG,fsr<<3);//设置加速度传感器满量程范围  
}


/**
  * @brief   读取MPU6050的加速度数据
  * @param   
  * @retval  
  */
void mpu6050_read_acc(short *accData)
{
    uint8_t buf[6];

    ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_RA_ACCEL_XOUT_H,&buf[0],1));
    ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_RA_ACCEL_XOUT_L,&buf[1],1));
    ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_RA_ACCEL_YOUT_H,&buf[2],1));
    ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_RA_ACCEL_YOUT_L,&buf[3],1));
    ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_RA_ACCEL_ZOUT_H,&buf[4],1));
    ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_RA_ACCEL_ZOUT_L,&buf[5],1));
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
}

/**
  * @brief   读取MPU6050的角加速度数据
  * @param   
  * @retval  
  */
void mpu6050_read_gyro(short *gyroData)
{
    uint8_t buf[6];
    ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_RA_GYRO_XOUT_H,&buf[0],1));
    ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_RA_GYRO_XOUT_L,&buf[1],1));
    ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_RA_GYRO_YOUT_H,&buf[2],1));
    ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_RA_GYRO_YOUT_L,&buf[3],1));
    ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_RA_GYRO_ZOUT_H,&buf[4],1));
    ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_RA_GYRO_ZOUT_L,&buf[5],1));
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5];
}

/**
  * @brief   读取MPU6050的温度数据，转化成摄氏度
  * @param   
  * @retval  
  */
void mpu6050_read_temperature(float *Temperature)
{
	short temp3;
	uint8_t buf[2];
	ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_RA_TEMP_OUT_H,&buf[0],1));
    ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_RA_TEMP_OUT_L,&buf[1],1));
    temp3= (buf[0] << 8) | buf[1];
	*Temperature=((double) temp3/340.0)+36.53;
}

/**
* @brief i2c 连续写
*
* @param[in] 
* @return 
*/
esp_err_t mpu6050_i2c_write(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char const *data)
{
	int ret;
	uint8_t write_buf[1 + length];
	write_buf[0] = reg_addr;
	memcpy(&write_buf[1], data, length);

	ret = i2c_master_write_to_device(0, slave_addr, write_buf, 1 + length, 100 / portTICK_PERIOD_MS);

	return ret;
}

/**
* @brief i2c 连续读
*
* @param[in] 
* @return 
*/
esp_err_t mpu6050_i2c_read(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data)
{
    return i2c_master_write_read_device(0, slave_addr, &reg_addr, 1, data, length, 100 / portTICK_PERIOD_MS);
}

/**
* @brief 延迟
*
* @param[in] 
* @return 
*/
int mpu6050_delay_ms(unsigned long num_ms)
{
  vTaskDelay(num_ms / portTICK_PERIOD_MS);
  return 0;
}

/**
* @brief 获取时间
*
* @param[in] 
* @return 
*/
int mpu6050_get_clock_ms(unsigned long *count)
{
  *count = (unsigned long)esp_timer_get_time();
  return 0;
}


/**
* @brief 初始化mpu6050
*
* @param[in] 
* @return 
*/
esp_err_t mpu6050_init(void) 
{
    uint8_t data = 0;
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_ERROR_CHECK(mpu6050_register_write_byte(MPU6050_RA_PWR_MGMT_1, 0x80));
    vTaskDelay(200 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(mpu6050_register_write_byte(MPU6050_RA_PWR_MGMT_1, 0x00));
    mpu6050_set_gyro_fsr(3);
    mpu6050_set_accel_fsr(0);
    mpu6050_set_rate(50);
    ESP_ERROR_CHECK(mpu6050_register_write_byte(MPU6050_RA_INT_ENABLE, 0x00));//关闭所有中断
    ESP_ERROR_CHECK(mpu6050_register_write_byte(MPU6050_RA_USER_CTRL, 0x00));//I2C主模式关闭
    ESP_ERROR_CHECK(mpu6050_register_write_byte(MPU6050_RA_FIFO_EN, 0xff));//关闭FIFO
    ESP_ERROR_CHECK(mpu6050_register_write_byte(MPU6050_RA_INT_PIN_CFG, 0x80));//INT引脚低电平有效

    ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_WHO_AM_I,&data,1));
    printf("[MPU6050]:who am I> %x is 0x68?\n",data);

    ESP_ERROR_CHECK(mpu6050_register_write_byte(MPU6050_RA_PWR_MGMT_1, 0x01));    
    ESP_ERROR_CHECK(mpu6050_register_write_byte(MPU6050_RA_PWR_MGMT_2, 0x00));       
    mpu6050_set_rate(50);    
    return ESP_OK;
}


// void app_main(void)
// {
//     uint8_t data[2];
//     ESP_ERROR_CHECK(i2c_master_init());
//     ESP_LOGI(TAG, "I2C initialized successfully");

//     /* Read the MPU6050 WHO_AM_I register, on power up the register should have the value 0x71 */
//     ESP_ERROR_CHECK(mpu6050_register_read(MPU6050_WHO_AM_I_REG_ADDR, data, 1));
//     ESP_LOGI(TAG, "WHO_AM_I = %X", data[0]);

//     /* Demonstrate writing by reseting the MPU6050 */
//     ESP_ERROR_CHECK(mpu6050_register_write_byte(MPU6050_PWR_MGMT_1_REG_ADDR, 1 << MPU6050_RESET_BIT));

//     ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
//     ESP_LOGI(TAG, "I2C de-initialized successfully");
// }