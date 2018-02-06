/*
 * buttonDriver.c
 *
 *  Note: This driver has been implemented using this tutorial: http://www.simplyembedded.org/tutorials/msp430-i2c-basics/
 *  I am aware that this driver would need a lot of improvement to work correctly if more complicated usecases arise.
 *
 *  This file implements all functionality currently required for the SHT21 to return a result.
 *
 */

#include "sensorDriver.h"

static uint8_t gTemperature[3];                                     //Stores the individual bytes of a temperature measurement.

typedef const uint8_t DeviceAddress_t;                              //Defines the type of a device address (temperature sensor).

typedef struct i2c_data {                                           //Defines a struct that contains all transmission data. We receive data via an interrupt so it is not in here.
    const void *txBuf;
    size_t txLen;
} I2CData_t;

/**
 * Checks the responsible registers to see if a transmission was successful.
 */
static int sensorDriver_checkI2CAck(void);

/**
 * This function transfers the data given by the I2CData_t struct to the device specified by deviceAddress via I2C.
 */
static int sensorDriver_transferI2C(DeviceAddress_t deviceAddress, I2CData_t *data);

/**
 * Initializes the I2C module to be able to communicate with the sensorhub via I2C.
 */
void sensorDriver_initI2C(void) {
    P1SEL0 |= I2C_SDA_PIN + I2C_SCL_PIN;                            //Route the pins for the I2C module
    UCB0CTLW0 |= UCSWRST;                                           //Enter SW reset mode (holds i2c module)
    UCB0CTLW0 |= UCMST | UCMODE_3 | UCSYNC;                         //Master mode, i2c mode, synchronous mode
    UCB0CTLW0 |= UCSSEL_2;                                          //Use SMCLK
    UCB0CTLW1 |= UCASTP_2;                                          //Generate stop condition if byte counter UCB0TBCNT reached
    UCB0BRW = 4;                                                    //fSCL = SMCLK/4 = 250kHz
    UCB0TBCNT = 0x03;                                               //Generate stop condition after 3 bytes
    UCB0CTLW0 &= ~UCSWRST;                                          //Clear SW reset (i2c module resumes operation)
    UCB0IE |= UCRXIE | UCNACKIE | UCBCNTIFG;                        //Enable interrupts
}

/**
 * Requests the result of a previously triggered temperature measurement. This function concatenates and appends all individual bytes
 * and returns the sensor value, which needs to be converted to the respective unit.
 */
int16_t sensorDriver_readTemperature(void) {
    return gTemperature[0]*256 + gTemperature[1];
}

/**
 * Triggers a temperature measurement of the SHT21 via I2C. A temperature measurement can take up to 100ms to return a result. For this reason the result needs to be
 * requested seperately after at least 100ms. Otherwise the result may be outdated. Returns possible error codes.
 */
int sensorDriver_measureTemperature(void) {
    uint8_t write_cmd[1] = {TEMPERATURE_SENSOR_COMMAND};            //Initialize the command to trigger a temperature measurement
    I2CData_t data = { .txBuf = &write_cmd, .txLen = 1 };           //Initialize the struct that defines the transmission data

    int err = sensorDriver_transferI2C(TEMPERATURE_SENSOR_ADDRESS, &data);
    return err;
}

/**
 * Checks the responsible registers to see if a transmission was successful and has been acknowledged.
 */
int sensorDriver_checkI2CAck(void) {
    int err = 0;

    if (UCB0IFG & UCNACKIFG) {                                      //Check for ACK
        UCB0CTLW0 |= UCTXSTP;                                       //Stop the I2C transmission
        UCB0IFG &= ~UCNACKIFG;                                      //Clear the interrupt flag
        err = -1;
    }

    return err;
}

/**
 * This function transfers the data given by the I2CData_t struct to the device specified by deviceAddress via I2C.
 */
int sensorDriver_transferI2C(DeviceAddress_t deviceAddress, I2CData_t *data) {
    int err = 0;
    UCB0I2CSA = deviceAddress;                                      //Set the slave device address

    if (data->txLen > 0) {                                          //Transmit data if there is any
        const uint8_t * buf = data->txBuf;
        size_t txSize = data->txLen;
        UCB0CTLW0 |= UCTR | UCTXSTT;                                //Send the start condition

        while ( (UCB0CTLW0 & UCTXSTT) && ( (UCB0IFG & UCTXIFG0) == 0) );    //Wait for the start condition to be sent and ready to transmit interrupt

        err = sensorDriver_checkI2CAck();                           //Check for ACK
        while ((err == 0) && (txSize > 0)) {                        //If no error occurred and there are bytes left to send, transmit the data
            UCB0TXBUF = *buf;
            while ((UCB0IFG & UCTXIFG0) == 0) {                     //Wait for the data to be sent
                err = sensorDriver_checkI2CAck();                   //Check for ACK
                if (err < 0) {                                      //If there was an error, cancel transmission
                    break;
                }
            }

            buf++;
            txSize--;
        }
    }

    UCB0CTLW0 &= ~UCTR;                                             //Stop the transmission and start receiving
    UCB0CTLW0 |= UCTXSTT;
    return err;
}


/**
 * This interrupt is used to receive data.
 */
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
  static uint8_t byteIndex = 0;                                     //Save the current index, because data is received in individual bytes
  switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG)) {             //Tell the compiler that UCB0IV has to be an even value in range of USCI_I2C_UCBIT9IFG
    case USCI_I2C_UCNACKIFG:                                        //Vector 4: NACKIFG
      UCB0CTLW0 |= UCTXSTT;                                         //Send I2C start condition
      break;

    case USCI_I2C_UCRXIFG0:                                         //If this vector is received, data is going to be received
       if(UCB0I2CSA == TEMPERATURE_SENSOR_ADDRESS){                 //If the data is received from the temperature sensor
           gTemperature[byteIndex] = UCB0RXBUF;                     //Write the current byte into the "receive buffer"
       }

       if(++byteIndex == 3){                                        //After receiving 3 bytes reset the counter because we know we do not want more than this
           byteIndex = 0;
       }
      break;
    default: break;
  }
}
