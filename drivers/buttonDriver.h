/*
 * buttonDriver.h
 *
 *  This file defines the various properties of a button and also some functions via a define to reduce function calls.
 *
 */

#ifndef DRIVERS_BUTTONDRIVER_H_
#define DRIVERS_BUTTONDRIVER_H_

#include <msp430.h>

#define BTN                         1                           //Button 1 Pin
#define BTN_PORT_DIR                P1DIR                       //Button 1 Direction Input/Output
#define BTN_PORT_IN                 P1IN                        //Button 1 In
#define BTN_PORT_REN                P1REN                       //Button 1 Pullup/Pulldown
#define BTN_PORT_OUT                P1OUT                       //Button 1 Out
#define BTN_SHIFT                   (1 << BTN)                  //Button 1 Shift onto the Pin Bit

#define BTN_DEBOUNCE_TIME           50                          //Time in milliseconds to sleep after a button press to reduce bounce

#define BTN_STATE                   BTN_PORT_IN & BTN_SHIFT     //Macro to check the current state of the button

/**
 * Initializes the button 1
 */
void buttonDriver_init(void);

#endif /* DRIVERS_BUTTONDRIVER_H_ */
