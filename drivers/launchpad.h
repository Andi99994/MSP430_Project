#ifndef LAUNCHPAD_H_
#define LAUNCHPAD_H_

#include <inttypes.h>
#include <msp430.h>
#include <intrinsics.h>
#include "displayDriver.h"
#include "sensorDriver.h"
#include "buttonDriver.h"

#define LAUNCHPAD_TIMER_INTERVAL    50

#define THREADPOOL_SIZE             5
#define STACKSIZE_PER_THREAD        256
#define STACK_UPPER_EDGE_ADDRESS    0x0023FF

#define ATOMIC_START(x)             x = _get_interrupt_state(); _disable_interrupts();
#define ATOMIC_END(x)               _set_interrupt_state(x);

void launchpad_init(void);
uint32_t launchpad_getSystemTicks(void);
void launchpad_toggleGreenLED(void);
void launchpad_toggleRedLED(void);
void launchpad_toggleRedLEDEnable(void);
void launchpad_clearDisplay(void);
void launchpad_showTemperature(uint16_t sensorValue, TemperatureUnit_t unit);
void launchpad_measureTemperature(void);
int16_t launchpad_readTemperature(void);
unsigned char launchpad_getButtonState(void);

#endif /* LAUNCHPAD_H_ */
