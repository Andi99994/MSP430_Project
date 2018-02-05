#include "launchpad.h"
#include "LEDDriver.h"
#include "DisplayDriver.h"
#include "sensorDriver.h"

static uint32_t gSystemTicks = 0;

static void launchpad_initButton(void);
static void launchpad_initTimer(void);
static void launchpad_init_I2C(void);

extern void timerCallback();

void launchpad_init() {
    WDTCTL = WDTPW | WDTHOLD;  // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // power manager - turn on module

    led_init();
    display_init();
    launchpad_initButton();
    launchpad_initTimer();
    launchpad_init_I2C();
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

void launchpad_showTemperature(uint16_t sensorValue, TemperatureUnit_t unit) {
    display_showTemperature(sensorValue, unit);
}

void launchpad_clearDisplay(void) {
    DISPLAY_CLEAR;
}

static void launchpad_init_I2C(void) {
    P1SEL0 |= I2C_SDA_PIN + I2C_SCL_PIN;      // route i2c pins
    UCB0CTLW0 |= UCSWRST;                     // enter sw reset mode (holds i2c module)
    UCB0CTLW0 |= UCMST | UCMODE_3 | UCSYNC;   // master mode, i2c mode, synchronous mode
    UCB0CTLW0 |= UCSSEL_2;                    // use SMCLK
    UCB0CTLW1 |= UCASTP_2;                    // generate stop condition if byte counter UCB0TBCNT reached
    UCB0BRW = 4;                              // fSCL = SMCLK/4 = 250kHz
    UCB0TBCNT = 0x03;                         // generate stop condition after 3 bytes
    UCB0I2CSA = 0x40;                         // slave Address (temp+humidity sensor SHT21)
    UCB0CTLW0 &= ~UCSWRST;                    // clear SW reset (i2c module resumes operation)
    UCB0IE |= UCRXIE | UCNACKIE | UCBCNTIFG;  // enable relevant interrupts
}

