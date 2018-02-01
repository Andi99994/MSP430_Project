#ifndef LAUNCHPAD_H_
#define LAUNCHPAD_H_

#include <inttypes.h>
#include <msp430.h>
#include <intrinsics.h>

#define BTN                 1
#define BTN_PORT_DIR        P1DIR
#define BTN_PORT_IN         P1IN
#define BTN_PORT_REN        P1REN
#define BTN_PORT_OUT        P1OUT
#define BTN_SHIFT           (1 << BTN)

#define BTN_DEBOUNCE_TIME           50
#define LAUNCHPAD_TIMER_INTERVAL    50

#define THREADPOOL_SIZE            5
#define STACKSIZE_PER_THREAD        256
#define STACK_UPPER_EDGE_ADDRESS    0x0023FF

#define ATOMIC_START(x)     x = _get_interrupt_state(); _disable_interrupts();
#define ATOMIC_END(x)       _set_interrupt_state(x);

void launchpad_init();
uint32_t launchpad_getSystemTicks();
void launchpad_toggleGreenLED();
void launchpad_toggleRedLED();
void launchpad_toggleRedLEDEnable();
void launchpad_setLCD(uint16_t pos, uint16_t digit);
void launchpad_clearLCD();
void launchpad_setTemperature(uint16_t tempSensorValue);

#endif /* LAUNCHPAD_H_ */