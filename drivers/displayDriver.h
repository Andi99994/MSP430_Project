/*
 * DisplayDriver.h
 *
 * This file defines the various properties of the LCD display and also some functions via a define to reduce function calls.
 */

#ifndef DISPLAYDRIVER_H_
#define DISPLAYDRIVER_H_

#include <msp430.h>
#include <inttypes.h>

#define DISPLAY_CLEAR               LCDCMEMCTL = LCDCLRM                //Clear the LCD display

typedef int16_t Temperature_t;                                          //Defines the type of a temperature value

typedef enum {                                                          //Defines the different measurement units to possibly be displayed
    CELSIUS,
    FAHRENHEIT
} TemperatureUnit_t;

/**
 * Initializes the various LCD segments required
 */
void displayDriver_init(void);

/**
 * Display a temperature of specified unit on the LCD display. The temperature has one decimal place multiplied by 10 (20,1°C = 201 here) and can range
 * from -9999 to +9999 (= -999.9° to +999.9°). Any overflowing places will be truncated and not displayed.
 */
void displayDriver_showTemperature(Temperature_t temperature, TemperatureUnit_t unit);

#endif /* DISPLAYDRIVER_H_ */
