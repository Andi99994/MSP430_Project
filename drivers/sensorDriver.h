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

#define I2C_SDA_PIN     (1 << 6)
#define I2C_SCL_PIN     (1 << 7)
#define NULL 0

struct i2c_data {
    const void *tx_buf;
    size_t tx_len;
    void *rx_buf;
    size_t rx_len;
};

void sensorDriver_initI2C(void);
int sensorDriver_measureTemperature(void);
int sensorDriver_checkI2CAck(const uint8_t deviceAddress);
int sensorDriver_receiveI2C(const uint8_t deviceAddress, uint8_t *buf, size_t nbytes);
int sensorDriver_transmitI2C(const uint8_t deviceAdress, const uint8_t *buf, size_t nbytes);
int sensorDriver_transferI2C(const uint8_t deviceAddress, struct i2c_data *data);
int sensorDriver_readTemperatureValue(uint8_t rx_data[]);
int sensorDriver_resetTemperatureSensor(void);


#endif /* DRIVERS_SENSORDRIVER_H_ */
