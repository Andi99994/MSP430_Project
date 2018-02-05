/*
 * buttonDriver.h
 *
 *  Created on: 05.02.2018
 *      Author: Andi
 */

#ifndef DRIVERS_BUTTONDRIVER_H_
#define DRIVERS_BUTTONDRIVER_H_

#include <msp430.h>

#define BTN                         1
#define BTN_PORT_DIR                P1DIR
#define BTN_PORT_IN                 P1IN
#define BTN_PORT_REN                P1REN
#define BTN_PORT_OUT                P1OUT
#define BTN_SHIFT                   (1 << BTN)

#define BTN_DEBOUNCE_TIME           50

#define BTN_STATE                   BTN_PORT_IN & BTN_SHIFT

void buttonDriver_init(void);

#endif /* DRIVERS_BUTTONDRIVER_H_ */
