# Bosch BME280 on Raspberry Pi Pico with C sdk

This is an exercise i made for myself to get familiar with the i2c interface provided with the Pi Pico C SDK using the official bme280 driver library from the Bosch github repository.

# Build
Using cmake via terminal (Assuming the SDK is installed and configured and the PICO_SDK_PATH is defined)

```
mkdir build
cd build
cmake ..
make
```

# Links
BME280 driver (https://github.com/BoschSensortec/BME280_driver)