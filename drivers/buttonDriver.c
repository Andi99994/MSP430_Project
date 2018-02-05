/*
 * buttonDriver.c
 *
 *  Created on: 05.02.2018
 *      Author: Andi
 */

#include "buttonDriver.h"

void buttonDriver_init(void) {
    BTN_PORT_REN |= BTN_SHIFT;
    BTN_PORT_OUT |= BTN_SHIFT;
    BTN_PORT_DIR &= ~BTN_SHIFT;
}
