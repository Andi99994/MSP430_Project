/*
 * buttonDriver.c
 *
 *  This file implements all functionality of a button required. Currently only the initializer function is neccessary.
 *
 */

#include "buttonDriver.h"

/**
 * Initializes the button 1.
 */
void buttonDriver_init(void) {
    BTN_PORT_REN |= BTN_SHIFT;              //Enable internal pull-up/down resistors
    BTN_PORT_OUT |= BTN_SHIFT;              //Select pull-up mode
    BTN_PORT_DIR &= ~BTN_SHIFT;             //Button pin to input
}
