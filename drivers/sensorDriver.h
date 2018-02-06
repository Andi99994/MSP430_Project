/*
 * sensorDriver.h
 *
 *  This file defines the various properties required for the sensorhub and the I2C module and also some functions.
 *
 */

#ifndef DRIVERS_SENSORDRIVER_H_
#define DRIVERS_SENSORDRIVER_H_

#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>

#define I2C_SDA_PIN                     (1 << 6)                //Defines the SDA (Signal Data) pin of the I2C module
#define I2C_SCL_PIN                     (1 << 7)                //Defines the SCL (Signal Clock) pin of the I2C module
#define TEMPERATURE_SENSOR_ADDRESS      0x40                    //Defines the slave address of the SHT21 temperature sensor
#define TEMPERATURE_SENSOR_COMMAND      0xF3                    //Defines the command to trigger a temperature measurement

/**
 * Initializes the I2C module to be able to communicate with the sensorhub via I2C.
 */
void sensorDriver_initI2C(void);

/**
 * Triggers a temperature measurement of the SHT21 via I2C. A temperature measurement can take up to 100ms to return a result. For this reason the result needs to be
 * requested seperately after at least 100ms. Otherwise the result may be outdated. Returns possible error codes.
 */
int sensorDriver_measureTemperature(void);

/**
 * Requests the result of a previously triggered temperature measurement. Returns the sensor value, which needs to be converted to the respective unit.
 */
int16_t sensorDriver_readTemperature(void);

#endif /* DRIVERS_SENSORDRIVER_H_ */
