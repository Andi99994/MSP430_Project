/*
 * LEDDriver.c
 *
 *  This file implements all functionality of the LEDs required. Currently only the initializer function is neccessary.
 *  Most of the other functionality can be implemented via macros.
 *
 */

#include "LEDDriver.h"

/**
 * Initializes the red and green LED and turns them off initially.
 */
void ledDriver_init(void) {
    LED_RED_DIR |= LED_RED_SHIFT;
    LED_GREEN_DIR |= LED_GREEN_SHIFT;

    LED_RED_OFF;
    LED_GREEN_OFF;
}
