/*
 * LEDDriver.h
 *
 *  Created on: 28.01.2018
 *      Author: Andi
 */

#ifndef LEDDRIVER_H_
#define LEDDRIVER_H_

#include <msp430.h>

#define LED_RED                     0
#define LED_RED_DIR                 P1DIR
#define LED_RED_OUT                 P1OUT
#define LED_RED_SHIFT               (1 << LED_RED)

#define LED_GREEN                   7
#define LED_GREEN_DIR               P9DIR
#define LED_GREEN_OUT               P9OUT
#define LED_GREEN_SHIFT             (1 << LED_GREEN)

#define LED_GREEN_ENABLE            LED_GREEN_DIR |= LED_GREEN_SHIFT
#define LED_GREEN_OFF               LED_GREEN_OUT &= ~LED_GREEN_SHIFT
#define LED_GREEN_TOGGLE            LED_GREEN_OUT ^= LED_GREEN_SHIFT
#define LED_GREEN_DISABLE           LED_GREEN_DIR &= ~LED_GREEN_SHIFT
#define LED_GREEN_TOGGLE_ENABLE     LED_GREEN_DIR ^= LED_GREEN_SHIFT

#define LED_RED_ENABLE              LED_RED_DIR |= LED_RED_SHIFT
#define LED_RED_OFF                 LED_RED_OUT &= ~LED_RED_SHIFT
#define LED_RED_TOGGLE              LED_RED_OUT ^= LED_RED_SHIFT
#define LED_RED_DISABLE             LED_RED_DIR &= ~LED_RED_SHIFT
#define LED_RED_TOGGLE_ENABLE       LED_RED_DIR ^= LED_RED_SHIFT


void ledDriver_init(void);

#endif /* LEDDRIVER_H_ */
