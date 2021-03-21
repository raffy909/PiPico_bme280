#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"

#include "bme280.h"

/*i2c0 or i2c1*/
#define I2C_INST i2c0

#define I2C_SDA_PIN 16
#define I2C_SCL_PIN 17

#define DELAY 1000

int8_t i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *ptr)
{
    i2c_write_blocking(I2C_INST, BME280_I2C_ADDR_PRIM, &reg_addr, 1, false);
    i2c_read_blocking(I2C_INST, BME280_I2C_ADDR_PRIM, data, len, false);

    return BME280_OK;
}

int8_t i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *ptr)
{
    uint32_t full_len = len + 1;
    uint8_t *tmp_buff = malloc((full_len) * sizeof(uint8_t));

    tmp_buff[0] = reg_addr;
    memcpy(tmp_buff + 1, data, len);

    i2c_write_blocking(I2C_INST, BME280_I2C_ADDR_PRIM, tmp_buff, full_len, false);
    free(tmp_buff);

    return BME280_OK;
}

void delay(uint32_t period, void *ptr)
{
    sleep_us(period);
}

int8_t bme280_configure(struct bme280_dev *dev)
{
    int8_t res;
    uint8_t settings_sel = 0;

    dev->settings.osr_h = BME280_OVERSAMPLING_1X;
    dev->settings.osr_p = BME280_OVERSAMPLING_1X;
    dev->settings.osr_t = BME280_OVERSAMPLING_1X;
    dev->settings.filter = BME280_FILTER_COEFF_OFF;

    settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    res = bme280_set_sensor_settings(settings_sel, dev);
    if (res != BME280_OK)
    {
        printf("Failed to set sensor settings (code %+d).", res);
        return res;
    }

    return BME280_OK;
}

int main()
{
    struct bme280_dev dev;
    struct bme280_data comp_data;
    int8_t res;
    uint32_t mes_delay;

    float temp, press, hum;

    stdio_init_all();

    i2c_init(i2c_default, 400 * 1000);

    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    dev.intf = BME280_I2C_INTF;
    dev.read = i2c_read;
    dev.write = i2c_write;
    dev.delay_us = delay;
    dev.intf_ptr = NULL;

    res = bme280_init(&dev);
    if (res != BME280_OK)
    {
        printf("Failed to initialize the device (code %+d).\n", res);
    }

    res = bme280_configure(&dev);
    if (res != BME280_OK)
    {
        printf("Failed to configure the sensor (code %+d).\n", res);
    }

    mes_delay = bme280_cal_meas_delay(&dev.settings);

    while (1)
    {
        res = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
        if (res != BME280_OK)
        {
            printf("Failed to set sensor mode (code %+d).", res);
            break;
        }

        dev.delay_us(mes_delay, dev.intf_ptr);

        res = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
        if (res != BME280_OK)
        {
            printf("Failed to get sensor data (code %+d).", res);
            break;
        }

        temp = comp_data.temperature;
        press = 0.01 * comp_data.pressure;
        hum = comp_data.humidity;

        printf("%0.2lf %0.2lf %0.2lf\n", temp, press, hum);

        sleep_ms(DELAY);
    }
}
