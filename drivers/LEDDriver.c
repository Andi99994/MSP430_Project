/*
 * LEDDriver.c
 *
 *  Created on: 28.01.2018
 *      Author: Andi
 */

#include "LEDDriver.h"

void ledDriver_init(void) {
    LED_RED_DIR |= LED_RED_SHIFT;
    LED_GREEN_DIR |= LED_GREEN_SHIFT;

    LED_RED_OFF;
    LED_GREEN_OFF;
}
