/**
 * @file imu.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-10-10
 *
 * @copyright Copyright (c) 2024
 *
 */
// https://github.com/boschsensortec/BMI270_SensorAPI/blob/master/bmi270_examples/accel_gyro/accel_gyro.c
// https://github.com/arduino-libraries/Arduino_BMI270_BMM150/blob/master/src/BMI270.cpp
#include "imu.h"
#include "../../hal_config.h"
#include "../utils/BMI270-Sensor-API/bmi270.h"
#include <esp_timer.h>
#include <driver/i2c.h>
#include <mooncake_log.h>

/*! Earth's gravity in m/s^2 */
#define GRAVITY_EARTH (9.80665f)

/*! Macros to select the sensors */
#define ACCEL UINT8_C(0x00)
#define GYRO  UINT8_C(0x01)

static const char* _tag = "imu";

void ImuBmi270::init()
{
    mclog::tagInfo(_tag, "bmi270 init");

    /* Status of api are returned to this variable. */
    int8_t rslt;

    _bmi.chip_id = BMI2_I2C_PRIM_ADDR;
    _bmi.read = bmi2_i2c_read;
    _bmi.write = bmi2_i2c_write;
    _bmi.delay_us = bmi2_delay_us;
    _bmi.intf = BMI2_I2C_INTF;
    _bmi.intf_ptr = NULL;
    _bmi.read_write_len = 30;    // Limitation of the Wire library
    _bmi.config_file_ptr = NULL; // Use the default BMI270 config file

    rslt = bmi270_init(&_bmi);
    bmi2_error_codes_print_result(rslt);

    /* Accel and gyro configuration settings. */
    rslt = set_accel_gyro_config(&_bmi);
    bmi2_error_codes_print_result(rslt);

    /* Assign accel and gyro sensor to variable. */
    uint8_t sensor_list[2] = {BMI2_ACCEL, BMI2_GYRO};

    /* NOTE:
     * Accel and Gyro enable must be done after setting configurations
     */
    rslt = bmi2_sensor_enable(sensor_list, 2, &_bmi);
    bmi2_error_codes_print_result(rslt);
}

void ImuBmi270::update()
{
    /* Structure to define type of sensor and their respective data. */
    static struct bmi2_sens_data sensor_data = {{0}};
    int8_t rslt = bmi2_get_sensor_data(&sensor_data, &_bmi);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK) {
        /* Converting lsb to meter per second squared for 16 bit accelerometer at 2G range. */
        _imu_data.accelY = lsb_to_mps2(sensor_data.acc.x, (float)2, _bmi.resolution);
        _imu_data.accelX = -lsb_to_mps2(sensor_data.acc.y, (float)2, _bmi.resolution);
        _imu_data.accelZ = lsb_to_mps2(sensor_data.acc.z, (float)2, _bmi.resolution);

        /* Converting lsb to degree per second for 16 bit gyro at 2000dps range. */
        _imu_data.gyroY = lsb_to_dps(sensor_data.gyr.x, (float)2000, _bmi.resolution);
        _imu_data.gyroX = -lsb_to_dps(sensor_data.gyr.y, (float)2000, _bmi.resolution);
        _imu_data.gyroZ = lsb_to_dps(sensor_data.gyr.z, (float)2000, _bmi.resolution);
    }
}

void ImuBmi270::bmi2_error_codes_print_result(int8_t rslt)
{
    // 没必要吧兄弟
    if (rslt != BMI2_OK) {
        mclog::tagError(_tag, "shit happened bro");
    }
}

/*!
 * @brief This internal API is used to set configurations for accel and gyro.
 */
int8_t ImuBmi270::set_accel_gyro_config(struct bmi2_dev* bmi)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define accelerometer and gyro configuration. */
    struct bmi2_sens_config config[2];

    /* Configure the type of feature. */
    config[ACCEL].type = BMI2_ACCEL;
    config[GYRO].type = BMI2_GYRO;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi2_get_sensor_config(config, 2, bmi);
    bmi2_error_codes_print_result(rslt);

    /* Map data ready interrupt to interrupt pin. */
    rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT1, bmi);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK) {
        /* NOTE: The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[ACCEL].cfg.acc.odr = BMI2_ACC_ODR_200HZ;

        /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G). */
        config[ACCEL].cfg.acc.range = BMI2_ACC_RANGE_2G;

        /* The bandwidth parameter is used to configure the number of sensor samples that are averaged
         * if it is set to 2, then 2^(bandwidth parameter) samples
         * are averaged, resulting in 4 averaged samples.
         * Note1 : For more information, refer the datasheet.
         * Note2 : A higher number of averaged samples will result in a lower noise level of the signal, but
         * this has an adverse effect on the power consumed.
         */
        config[ACCEL].cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;

        /* Enable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         * For more info refer datasheet.
         */
        config[ACCEL].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;

        /* The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[GYRO].cfg.gyr.odr = BMI2_GYR_ODR_200HZ;

        /* Gyroscope Angular Rate Measurement Range.By default the range is 2000dps. */
        config[GYRO].cfg.gyr.range = BMI2_GYR_RANGE_2000;

        /* Gyroscope bandwidth parameters. By default the gyro bandwidth is in normal mode. */
        config[GYRO].cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;

        /* Enable/Disable the noise performance mode for precision yaw rate sensing
         * There are two modes
         *  0 -> Ultra low power mode(Default)
         *  1 -> High performance mode
         */
        config[GYRO].cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;

        /* Enable/Disable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         */
        config[GYRO].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;

        /* Set the accel and gyro configurations. */
        rslt = bmi2_set_sensor_config(config, 2, bmi);
        bmi2_error_codes_print_result(rslt);
    }

    return rslt;
}

/*!
 * @brief This function converts lsb to meter per second squared for 16 bit accelerometer at
 * range 2G, 4G, 8G or 16G.
 */
float ImuBmi270::lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width)
{
    double power = 2;

    float half_scale = (float)((pow((double)power, (double)bit_width) / 2.0f));

    return (GRAVITY_EARTH * val * g_range) / half_scale;
}

/*!
 * @brief This function converts lsb to degree per second for 16 bit gyro at
 * range 125, 250, 500, 1000 or 2000dps.
 */
float ImuBmi270::lsb_to_dps(int16_t val, float dps, uint8_t bit_width)
{
    double power = 2;

    float half_scale = (float)((pow((double)power, (double)bit_width) / 2.0f));

    return (dps / (half_scale)) * (val);
}

/* -------------------------------------------------------------------------- */
/*                                    Port                                    */
/* -------------------------------------------------------------------------- */

int8_t ImuBmi270::bmi2_i2c_read(uint8_t reg_addr, uint8_t* reg_data, uint32_t len, void* intf_ptr)
{
    if ((reg_data == NULL) || (len == 0) || (len > 32)) {
        return -1;
    }

    auto ret = i2c_master_write_read_device(HAL_I2C_BUS_PORT_NUM, HAL_IMU_DEVICE_ADDR, &reg_addr, 1, reg_data, len,
                                            portMAX_DELAY);
    if (ret == ESP_OK) {
        return 0;
    }
    return -1;
}

int8_t ImuBmi270::bmi2_i2c_write(uint8_t reg_addr, const uint8_t* reg_data, uint32_t len, void* intf_ptr)
{
    if ((reg_data == NULL) || (len == 0) || (len > 32)) {
        return -1;
    }

    uint8_t* write_buffer = new uint8_t[len + 1];
    write_buffer[0] = reg_addr;
    for (int i = 0; i < len; i++) {
        write_buffer[i + 1] = reg_data[i];
    }

    auto ret =
        i2c_master_write_to_device(HAL_I2C_BUS_PORT_NUM, HAL_IMU_DEVICE_ADDR, write_buffer, len + 1, portMAX_DELAY);
    if (ret == ESP_OK) {
        return 0;
    }
    return -1;
}

#define NOP() asm volatile("nop")

void ImuBmi270::bmi2_delay_us(uint32_t period, void* intf_ptr)
{
    uint64_t m = (uint64_t)esp_timer_get_time();
    if (period) {
        uint64_t e = (m + period);
        if (m > e) { // overflow
            while ((uint64_t)esp_timer_get_time() > e) {
                NOP();
            }
        }
        while ((uint64_t)esp_timer_get_time() < e) {
            NOP();
        }
    }
}
