#include "sensorDriver.h"

uint8_t cnt = 0;
uint8_t gTemperature[3];

void sensorDriver_initI2C(void) {
    P1SEL0 |= I2C_SDA_PIN + I2C_SCL_PIN;      // route i2c pins
    UCB0CTLW0 |= UCSWRST;                     // enter sw reset mode (holds i2c module)
    UCB0CTLW0 |= UCMST | UCMODE_3 | UCSYNC;   // master mode, i2c mode, synchronous mode
    UCB0CTLW0 |= UCSSEL_2;                    // use SMCLK
    UCB0CTLW1 |= UCASTP_2;                    // generate stop condition if byte counter UCB0TBCNT reached
    UCB0BRW = 4;                              // fSCL = SMCLK/4 = 250kHz
    UCB0TBCNT = 0x03;                         // generate stop condition after 3 bytes
    UCB0I2CSA = 0x40;                         // slave Address (temp+humidity sensor SHT21)
    UCB0CTLW0 &= ~UCSWRST;                    // clear SW reset (i2c module resumes operation)
    UCB0IE |= UCRXIE | UCNACKIE | UCBCNTIFG;  // enable relevant interrupts
}

int sensorDriver_resetTemperatureSensor(void) {
    int err;
    static const uint8_t deviceAddress = 0x40;
    struct i2c_data data;
    uint8_t write_cmd[1];

    write_cmd[0] = 0xFE;

    data.tx_buf = &write_cmd;
    data.tx_len = 1;
    data.rx_buf = NULL;
    data.rx_len = 0;

    err = sensorDriver_transferI2C(deviceAddress, &data);
    UCB0CTLW0 |= UCTXSTP;

    return err;
}

int sensorDriver_readTemperatureValue(uint8_t rx_data[]) {
    int err;
    uint8_t deviceAddress = 0x40;
    struct i2c_data data;

    data.tx_buf = NULL;
    data.tx_len = 0;
    data.rx_buf = (uint8_t *) rx_data;
    data.rx_len = 3;

    err = sensorDriver_transferI2C(deviceAddress, &data);

    return err;
}

int sensorDriver_measureTemperature(void) {
    int err;
    static const uint8_t deviceAddress = 0x40;
    struct i2c_data data;
    uint8_t write_cmd[1];
    write_cmd[0] = 0xF3;

    data.tx_buf = &write_cmd;
    data.tx_len = 1;
    data.rx_buf = NULL;
    data.rx_len = 0;

    err = sensorDriver_transferI2C(deviceAddress, &data);
    UCB0CTLW0 &= ~UCTR;
    UCB0CTLW0 |= UCTXSTT;
    return err;
}

//TODO is deviceAddress needed here?
int sensorDriver_checkI2CAck(const uint8_t deviceAddress) {
    int err = 0;

    /* Check for ACK */
    if (UCB0IFG & UCNACKIFG) {
        /* Stop the I2C transmission */
        UCB0CTLW0 |= UCTXSTP;

        /* Clear the interrupt flag */
        UCB0IFG &= ~UCNACKIFG;

        /* Set the error code */
        err = -1;
    }

    return err;
}

int sensorDriver_receiveI2C(const uint8_t deviceAddress, uint8_t *buf, size_t nbytes) {
    int err = 0;

    /* Send the start and wait */
    UCB0CTLW0 &= ~UCTR;
    UCB0CTLW0 |= UCTXSTT;

    /* Wait for the start condition to be sent */
    while (UCB0CTLW0 & UCTXSTT);

    /*
     * If there is only one byte to receive, then set the stop
     * bit as soon as start condition has been sent
     */
    if (nbytes == 1) {
        UCB0CTLW0 |= UCTXSTP;
    }

    /* Check for ACK */
    err = sensorDriver_checkI2CAck(deviceAddress);

    /* If no error and bytes left to receive, receive the data */
    while ((err == 0) && (nbytes > 0)) {
        /* Wait for the data */
        while ((UCB0IFG & UCRXIFG0) == 0){
            if( (UCB0IFG & UCNACKIFG) ){
                //UCB0CTLW0 |= UCTXSTT;
            }
        }

        *buf = UCB0RXBUF;
        buf++;
        nbytes--;

        /*
         * If there is only one byte left to receive
         * send the stop condition
         */
        if (nbytes == 1) {
            UCB0CTLW0 |= UCTXSTP;
        }
    }

    return err;
}

int sensorDriver_transmitI2C(const uint8_t deviceAddress, const uint8_t *buf, size_t nbytes) {
    int err = 0;

    /* Send the start condition */
    UCB0CTLW0 |= UCTR | UCTXSTT;

    /* Wait for the start condition to be sent and ready to transmit interrupt */
    while ( (UCB0CTLW0 & UCTXSTT) && ( (UCB0IFG & UCTXIFG0) == 0) );

    /* Check for ACK */
    err = sensorDriver_checkI2CAck(deviceAddress);

    /* If no error and bytes left to send, transmit the data */
    while ((err == 0) && (nbytes > 0)) {
        UCB0TXBUF = *buf;
        while ((UCB0IFG & UCTXIFG0) == 0) {

            //UCB0CTLW0 |= UCTR | UCTXSTT;
            //UCB0TXBUF = *buf;

            err = sensorDriver_checkI2CAck(deviceAddress);
            if (err < 0) {
                break;
            }
        }

        buf++;
        nbytes--;
    }

    return err;
}

int sensorDriver_transferI2C(const uint8_t deviceAddress, struct i2c_data *data) {
    int err = 0;

    /* Set the slave device address */
    UCB0I2CSA = deviceAddress;

    /* Transmit data if there is any */
    if (data->tx_len > 0) {
        err = sensorDriver_transmitI2C(deviceAddress, (const uint8_t *) data->tx_buf, data->tx_len);
    }

    /* Receive data if there is any */
    if ((err == 0) && (data->rx_len > 0)) {
        err = sensorDriver_receiveI2C(deviceAddress, (uint8_t *) data->rx_buf, data->rx_len);
    } else {
        /* No bytes to receive send the stop condition */
       //UCB0CTLW0 |= UCTXSTP;
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
       if(UCB0I2CSA == 0x40){
           gTemperature[cnt] = UCB0RXBUF;                   // Get RX data
       }
       cnt++;

       if(cnt == 3){
           cnt = 0;
       }
      break;

    case USCI_I2C_UCTXIFG0:  break;         // Vector 24: TXIFG0
    case USCI_I2C_UCBCNTIFG:
        //lockI2C = 0;
        break;         // Vector 26: BCNTIFG
    case USCI_I2C_UCCLTOIFG: break;         // Vector 28: clock low timeout
    case USCI_I2C_UCBIT9IFG: break;         // Vector 30: 9th bit
    default: break;
  }
}
