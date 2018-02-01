/*
 * DisplayDriver.h
 *
 *  Created on: 01.02.2018
 *      Author: andre
 */

#ifndef DISPLAYDRIVER_H_
#define DISPLAYDRIVER_H_

#include <msp430.h>
#include <inttypes.h>

#define DISPLAY_CLEAR       LCDCMEMCTL = LCDCLRM

typedef int32_t Temperature_t;

typedef enum {
    Kelvin,
    Celsius,
    Fahrenheit
} TemperatureUnit_t;

void display_init(void);
void display_showTemperature(Temperature_t temperature, TemperatureUnit_t unit);

#endif /* DISPLAYDRIVER_H_ */