/*
 * LEDDriver.h
 *
 *  This file defines the various properties of the LEDs and also some functions via a define to reduce function calls.
 *
 */

#ifndef LEDDRIVER_H_
#define LEDDRIVER_H_

#include <msp430.h>

#define LED_RED                     0                                       //Red LED pin
#define LED_RED_DIR                 P1DIR                                   //Red LED direction Input/Output
#define LED_RED_OUT                 P1OUT                                   //Red LED out
#define LED_RED_SHIFT               (1 << LED_RED)                          //Red LED Shift onto the Pin Bit

#define LED_GREEN                   7                                       //Green LED pin
#define LED_GREEN_DIR               P9DIR                                   //Green LED direction Input/Output
#define LED_GREEN_OUT               P9OUT                                   //Green LED out
#define LED_GREEN_SHIFT             (1 << LED_GREEN)                        //Green LED Shift onto the Pin Bit

#define LED_GREEN_ENABLE            LED_GREEN_DIR |= LED_GREEN_SHIFT        //Enables the green LED
#define LED_GREEN_OFF               LED_GREEN_OUT &= ~LED_GREEN_SHIFT       //Turns off the green LED
#define LED_GREEN_TOGGLE            LED_GREEN_OUT ^= LED_GREEN_SHIFT        //Toggles the green LED
#define LED_GREEN_DISABLE           LED_GREEN_DIR &= ~LED_GREEN_SHIFT       //Disables the green LED
#define LED_GREEN_TOGGLE_ENABLE     LED_GREEN_DIR ^= LED_GREEN_SHIFT        //Toggles between enabling and disabling the green LED

#define LED_RED_ENABLE              LED_RED_DIR |= LED_RED_SHIFT            //Enables the red LED
#define LED_RED_OFF                 LED_RED_OUT &= ~LED_RED_SHIFT           //Turns off the red LED
#define LED_RED_TOGGLE              LED_RED_OUT ^= LED_RED_SHIFT            //Toggles the red LED
#define LED_RED_DISABLE             LED_RED_DIR &= ~LED_RED_SHIFT           //Disables the red LED
#define LED_RED_TOGGLE_ENABLE       LED_RED_DIR ^= LED_RED_SHIFT            //Toggles between enabling and disabling the red LED

/**
 * Initializes the red and green LED and turns them off initially.
 */
void ledDriver_init(void);

#endif /* LEDDRIVER_H_ */
