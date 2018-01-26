#include "launchpad.h"

static uint32_t gSystemTicks = 0;

static void launchpad_initLED();
static void launchpad_initButton();
static void launchpad_initTimer();

extern void timerCallback();

void launchpad_init() {
    WDTCTL = WDTPW | WDTHOLD;  // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // power manager - turn on module

    launchpad_initLED();
    launchpad_initButton();
    launchpad_initTimer();
}

static void launchpad_initLED() {
    LED_RED_DIR |= LED_RED_SHIFT;
    LED_GREEN_DIR |= LED_GREEN_SHIFT;

    LED_RED_OUT &= ~LED_RED_SHIFT;      //Turn off red LED
    LED_GREEN_OUT &= ~LED_GREEN_SHIFT;  //Turn off green LED
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
