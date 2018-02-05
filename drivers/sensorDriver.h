/*
 * sensorDriver.h
 *
 *  Created on: 02.02.2018
 *      Author: Andi
 */

#ifndef DRIVERS_SENSORDRIVER_H_
#define DRIVERS_SENSORDRIVER_H_

#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>

#define I2C_SDA_PIN                     (1 << 6)
#define I2C_SCL_PIN                     (1 << 7)
#define TEMPERATURE_SENSOR_ADDRESS      0x40

void sensorDriver_initI2C(void);
int sensorDriver_measureTemperature(void);

#endif /* DRIVERS_SENSORDRIVER_H_ */
