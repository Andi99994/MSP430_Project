// http://www.simplyembedded.org/tutorials/msp430-i2c-basics/



#include "sensorDriver.h"

uint8_t byteIndex = 0;
uint8_t gTemperature[3];

typedef const uint8_t DeviceAddress_t;

typedef struct i2c_data {
    const void *tx_buf;
    size_t tx_len;
} I2CData_t;

static int sensorDriver_checkI2CAck(void);
static int sensorDriver_transmitI2C(DeviceAddress_t deviceAdress, const uint8_t *buf, size_t nbytes);
static int sensorDriver_transferI2C(DeviceAddress_t deviceAddress, I2CData_t *data);

void sensorDriver_initI2C(void) {
    P1SEL0 |= I2C_SDA_PIN + I2C_SCL_PIN;      // route i2c pins
    UCB0CTLW0 |= UCSWRST;                     // enter sw reset mode (holds i2c module)
    UCB0CTLW0 |= UCMST | UCMODE_3 | UCSYNC;   // master mode, i2c mode, synchronous mode
    UCB0CTLW0 |= UCSSEL_2;                    // use SMCLK
    UCB0CTLW1 |= UCASTP_2;                    // generate stop condition if byte counter UCB0TBCNT reached
    UCB0BRW = 4;                              // fSCL = SMCLK/4 = 250kHz
    UCB0TBCNT = 0x03;                         // generate stop condition after 3 bytes
    UCB0I2CSA = TEMPERATURE_SENSOR_ADDRESS;   // slave Address (temp+humidity sensor SHT21)
    UCB0CTLW0 &= ~UCSWRST;                    // clear SW reset (i2c module resumes operation)
    UCB0IE |= UCRXIE | UCNACKIE | UCBCNTIFG;  // enable relevant interrupts
}

int sensorDriver_measureTemperature(void) {
    uint8_t write_cmd[1] = {0xF3};
    I2CData_t data = { .tx_buf = &write_cmd, .tx_len = 1 };

    int err = sensorDriver_transferI2C(TEMPERATURE_SENSOR_ADDRESS, &data);
    UCB0CTLW0 &= ~UCTR;
    UCB0CTLW0 |= UCTXSTT;
    return err;
}

int sensorDriver_checkI2CAck(void) {
    int err = 0;

    if (UCB0IFG & UCNACKIFG) {        /* Check for ACK */
        UCB0CTLW0 |= UCTXSTP;         /* Stop the I2C transmission */
        UCB0IFG &= ~UCNACKIFG;        /* Clear the interrupt flag */
        err = -1;                     /* Set the error code */
    }

    return err;
}

int sensorDriver_transmitI2C(DeviceAddress_t deviceAddress, const uint8_t *buf, size_t nbytes) {
    int err = 0;

    /* Send the start condition */
    UCB0CTLW0 |= UCTR | UCTXSTT;

    /* Wait for the start condition to be sent and ready to transmit interrupt */
    while ( (UCB0CTLW0 & UCTXSTT) && ( (UCB0IFG & UCTXIFG0) == 0) );

    /* Check for ACK */
    err = sensorDriver_checkI2CAck();

    /* If no error and bytes left to send, transmit the data */
    while ((err == 0) && (nbytes > 0)) {
        UCB0TXBUF = *buf;
        while ((UCB0IFG & UCTXIFG0) == 0) {

            err = sensorDriver_checkI2CAck();
            if (err < 0) {
                break;
            }
        }

        buf++;
        nbytes--;
    }

    return err;
}

int sensorDriver_transferI2C(DeviceAddress_t deviceAddress, I2CData_t *data) {
    int err = 0;
    UCB0I2CSA = deviceAddress;      /* Set the slave device address */

    if (data->tx_len > 0) {         /* Transmit data if there is any */
        err = sensorDriver_transmitI2C(deviceAddress, (const uint8_t *) data->tx_buf, data->tx_len);
    }

    return err;
}

#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
  switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
  {
    case USCI_NONE:          break;         // Vector 0: No interrupts
    case USCI_I2C_UCALIFG:   break;         // Vector 2: ALIFG
    case USCI_I2C_UCNACKIFG:                // Vector 4: NACKIFG
      UCB0CTLW0 |= UCTXSTT;                  // I2C start condition
      break;

    case USCI_I2C_UCSTTIFG:  break;         // Vector 6: STTIFG
    case USCI_I2C_UCSTPIFG:  break;         // Vector 8: STPIFG
    case USCI_I2C_UCRXIFG3:  break;         // Vector 10: RXIFG3
    case USCI_I2C_UCTXIFG3:  break;         // Vector 12: TXIFG3
    case USCI_I2C_UCRXIFG2:  break;         // Vector 14: RXIFG2
    case USCI_I2C_UCTXIFG2:  break;         // Vector 16: TXIFG2
    case USCI_I2C_UCRXIFG1:  break;         // Vector 18: RXIFG1
    case USCI_I2C_UCTXIFG1:  break;         // Vector 20: TXIFG1
    case USCI_I2C_UCRXIFG0:                 // Vector 22: RXIFG0
       if(UCB0I2CSA == TEMPERATURE_SENSOR_ADDRESS){
           gTemperature[byteIndex] = UCB0RXBUF;                   // Get RX data
       }

       if(++byteIndex == 3){
           byteIndex = 0;
       }
      break;

    case USCI_I2C_UCTXIFG0:  break;         // Vector 24: TXIFG0
    case USCI_I2C_UCBCNTIFG: break;         // Vector 26: BCNTIFG
    case USCI_I2C_UCCLTOIFG: break;         // Vector 28: clock low timeout
    case USCI_I2C_UCBIT9IFG: break;         // Vector 30: 9th bit
    default: break;
  }
}
