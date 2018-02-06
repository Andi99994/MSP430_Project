/*
 * launchpad.h
 *
 * This file defines the various properties of the launchpad itself also some functions via a define to reduce function calls.
 * This is also the interface between the application and the launchpad. It delegates any function calls to the responsible drivers.
 *
 */

#ifndef LAUNCHPAD_H_
#define LAUNCHPAD_H_

#include <inttypes.h>
#include <msp430.h>
#include <intrinsics.h>
#include "displayDriver.h"
#include "sensorDriver.h"
#include "buttonDriver.h"

#define LAUNCHPAD_TIMER_INTERVAL    50                                                      //Defines the duration of a time slice for a thread. After this number of system ticks the timerCallback is executed, which is to be implemented by the OS

#define THREADPOOL_SIZE             5                                                       //Defines the size of the threadpool, which limits how many concurrent threads can run
#define STACKSIZE_PER_THREAD        256                                                     //Defines the stack size that each thread can be assigned
#define STACK_UPPER_EDGE_ADDRESS    0x0023FF                                                //Defines the upper edge address of the stack to correctly divide the stack to each thread

#define ATOMIC_START(x)             x = _get_interrupt_state(); _disable_interrupts();      //Disables global interrupts and saves the interrupt state to a variable
#define ATOMIC_END(x)               _set_interrupt_state(x);                                //Enables global interrupts and restores their interrupt state

/**
 * Initializes the launchpad and any dependant components via their respective drivers.
 */
void launchpad_init(void);

/**
 * Returns the current system ticks. A system tick depends on how the timer is initialized.
 */
uint32_t launchpad_getSystemTicks(void);

/**
 * Toggles the green LED.
 */
void launchpad_toggleGreenLED(void);

/**
 * Toggles the red LED.
 */
void launchpad_toggleRedLED(void);

/**
 * Enables/Disables the red LED.
 */
void launchpad_toggleRedLEDEnable(void);

/**
 * Clears the LCD display.
 */
void launchpad_clearDisplay(void);

/**
 * Display a temperature of specified unit on the LCD display. The temperature has one decimal place multiplied by 10 (20,1°C = 201 here) and can range
 * from -9999 to +9999 (= -999.9° to +999.9°). Any overflowing places will be truncated and not displayed.
 */
void launchpad_showTemperature(uint16_t sensorValue, TemperatureUnit_t unit);

/**
 * Triggers a temperature measurement of the SHT21 via I2C. A temperature measurement can take up to 100ms to return a result. For this reason the result needs to be
 * requested seperately after at least 100ms. Otherwise the result may be outdated.
 */
void launchpad_measureTemperature(void);

/**
 * Requests the result of a previously triggered temperature measurement. Returns the sensor value, which needs to be converted to the respective unit.
 */
int16_t launchpad_readTemperature(void);

/**
 * Returns the current state of the button 1.
 */
unsigned char launchpad_getButtonState(void);

#endif /* LAUNCHPAD_H_ */
