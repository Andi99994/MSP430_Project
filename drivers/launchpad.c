#include "launchpad.h"
#include "LEDDriver.h"
#include "DisplayDriver.h"

static uint32_t gSystemTicks = 0;

static void launchpad_initButton(void);
static void launchpad_initTimer(void);

extern void timerCallback();

void launchpad_init() {
    WDTCTL = WDTPW | WDTHOLD;  // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // power manager - turn on module

    led_init();
    display_init();
    launchpad_initButton();
    launchpad_initTimer();
    display_showTemperature(0, Celsius);
}

static void launchpad_initButton() {
    BTN_PORT_REN |= BTN_SHIFT;
    BTN_PORT_OUT |= BTN_SHIFT;
    BTN_PORT_DIR &= ~BTN_SHIFT;
}

static void launchpad_initTimer() {
    TA0CTL = TASSEL_2 + MC_1 + ID_0;     // SMCLK/8, upmode
    TA0CCR0 = 1000;                      // upper limit
    TA0CCTL0 = CCIE;                    // activate interrupt for Time A0
}

uint32_t launchpad_getSystemTicks() {
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

void launchpad_toggleGreenLED() {
    LED_GREEN_TOGGLE;
}

void launchpad_toggleRedLED() {
    LED_RED_TOGGLE;
}

void launchpad_toggleRedLEDEnable() {
    LED_RED_TOGGLE_ENABLE;
}

void launchpad_showTemperature(Temperature_t temperature, TemperatureUnit_t unit) {
    display_showTemperature(temperature, unit);
}
