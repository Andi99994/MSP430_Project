#include "launchpad.h"
#include "LEDDriver.h"
#include "DisplayDriver.h"
#include "sensorDriver.h"

static uint32_t gSystemTicks = 0;

static void launchpad_initButton(void);
static void launchpad_initTimer(void);

extern void timerCallback();

void launchpad_init(void) {
    WDTCTL = WDTPW | WDTHOLD;  // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // power manager - turn on module

    ledDriver_init();
    displayDriver_init();
    launchpad_initButton();
    launchpad_initTimer();
    sensorDriver_initI2C();
}

static void launchpad_initButton(void) {
    BTN_PORT_REN |= BTN_SHIFT;
    BTN_PORT_OUT |= BTN_SHIFT;
    BTN_PORT_DIR &= ~BTN_SHIFT;
}

static void launchpad_initTimer(void) {
    TA0CTL = TASSEL_2 + MC_1 + ID_0;     // SMCLK/8, upmode
    TA0CCR0 = 1000;                      // upper limit
    TA0CCTL0 = CCIE;                    // activate interrupt for Time A0
}

uint32_t launchpad_getSystemTicks(void) {
    return gSystemTicks;
}

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

void launchpad_toggleGreenLED(void) {
    LED_GREEN_TOGGLE;
}

void launchpad_toggleRedLED(void) {
    LED_RED_TOGGLE;
}

void launchpad_toggleRedLEDEnable(void) {
    LED_RED_TOGGLE_ENABLE;
}

void launchpad_showTemperature(uint16_t sensorValue, TemperatureUnit_t unit) {
    displayDriver_showTemperature(sensorValue, unit);
}

void launchpad_clearDisplay(void) {
    DISPLAY_CLEAR;
}

void launchpad_measureTemperature(void) {
    sensorDriver_measureTemperature();
}

