/*
 * launchpad.c
 *
 * This file implements all functionality of the launchpad. Most of the functionality delegates to the respective drivers.
 *
 */

#include "launchpad.h"
#include "LEDDriver.h"
#include "DisplayDriver.h"
#include "sensorDriver.h"

static uint32_t gSystemTicks = 0;                                                   //Current system ticks running

/**
 * Initializes the timer module.
 */
static void launchpad_initTimer(void);

/**
 * The timer callback is to be implemented by the OS and is being called every time the relative system ticks since the last execution exceed the LAUNCHPAD_TIMER_PERIOD
 */
extern void timerCallback();

/**
 * Initializes the launchpad and any dependant components via their respective drivers.
 */
void launchpad_init(void) {
    WDTCTL = WDTPW | WDTHOLD;                                                       //Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                                                           //Power manager - Turn on module

    ledDriver_init();                                                               //Initialize green and red LED
    displayDriver_init();                                                           //Initialize required display segments
    buttonDriver_init();                                                            //Initialize button 1
    launchpad_initTimer();                                                          //Initialize timer
    sensorDriver_initI2C();                                                         //Initialize the I2C module
}

/**
 * Initializes the timer module.
 */
static void launchpad_initTimer(void) {
    TA0CTL = TASSEL_2 + MC_1 + ID_0;                                                //Configure TimerA0 to use SMCLK/8, upmode,
    TA0CCR0 = 1000;                                                                 //Configure TimerA0 to count to this limit
    TA0CCTL0 = CCIE;                                                                //Configure interrupt for TimerA0
}

/**
 * Returns the current system ticks. A system tick depends on how the timer is initialized.
 */
uint32_t launchpad_getSystemTicks(void) {
    return gSystemTicks;
}

/**
 * Code that is executed every timer interrupt. This increments the system ticks and checks if LAUNCHPAD_TIMER_PERIOD has been exceeded.
 * If so, the timerCallback is executed.
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR_HOOK(void) {
    static uint16_t count = 0;

    gSystemTicks++;
    if(count++ >= LAUNCHPAD_TIMER_INTERVAL) {
        uint16_t c = count;
        count = 0;
        timerCallback(c);
    }
}

/**
 * Toggles the green LED by using the macro defined in the LEDDriver
 */
void launchpad_toggleGreenLED(void) {
    LED_GREEN_TOGGLE;
}

/**
 * Toggles the red LED by using the macro defined in the LEDDriver
 */
void launchpad_toggleRedLED(void) {
    LED_RED_TOGGLE;
}

/**
 * Enables/Disables the red LED by using the macro defined in the LEDDriver
 */
void launchpad_toggleRedLEDEnable(void) {
    LED_RED_TOGGLE_ENABLE;
}

/**
 * Display a temperature of specified unit on the LCD display. The temperature has one decimal place multiplied by 10 (20,1°C = 201 here) and can range
 * from -9999 to +9999 (= -999.9° to +999.9°). Any overflowing places will be truncated and not displayed.
 * This delegates to the displayDriver.
 */
void launchpad_showTemperature(uint16_t sensorValue, TemperatureUnit_t unit) {
    displayDriver_showTemperature(sensorValue, unit);
}

/**
 * Clears the LCD display by using the macro defined in the displayDriver.
 */
void launchpad_clearDisplay(void) {
    DISPLAY_CLEAR;
}

/**
 * Triggers a temperature measurement of the SHT21 via I2C by delegating to the sensorDriver.
 * A temperature measurement can take up to 100ms to return a result. For this reason the result needs to be
 * requested seperately after at least 100ms. Otherwise the result may be outdated.
 */
void launchpad_measureTemperature(void) {
    sensorDriver_measureTemperature();
}

/**
 * Requests the result of a previously triggered temperature measurement by the sensorDriver.
 * Returns the sensor value, which needs to be converted to the respective unit.
 */
int16_t launchpad_readTemperature(void) {
    return sensorDriver_readTemperature();
}

/**
 * Returns the current state of the button 1 by using the macro defined in the buttonDriver.
 */
unsigned char launchpad_getButtonState(void) {
    return BTN_STATE;
}

