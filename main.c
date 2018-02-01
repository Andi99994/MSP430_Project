#include "drivers/launchpad.h"
#include "scheduler.h"
#include "semaphor.h"

#define NUMBER_OF_DISPLAY_MODES         3

static void switchRedLEDThread(void);
static void aliveThread();
static Semaphor_t prodConsSemaphor;
static Temperature_t temperature;
static uint8_t displayMode;


/*
int32_t temp;
uint32_t tempAbs;
uint16_t i;



temp = 17572 * (int32_t) tempSensorValue;     // fixed point -> all values multiplied by 100
temp = temp / 65536;
temp -= 4685;

tempAbs = (uint32_t) temp / 10;     // remove second digit after comma
*/

static void switchRedLEDThread() {
    while(1) {
        switch (displayMode) {
        case 0:
            launchpad_showTemperature(temperature, CELSIUS);
            temperature -= 10;
            scheduler_threadSleep(500);
            break;
        case 1:
            launchpad_showTemperature(temperature, CELSIUS);
            temperature += 10;
            launchpad_toggleRedLED();
            scheduler_threadSleep(500);
            break;
        case 2:
            launchpad_showTemperature(temperature, FAHRENHEIT);
            launchpad_toggleRedLED();
            scheduler_threadSleep(1000);
            break;
        default:
            launchpad_clearDisplay();
            launchpad_toggleRedLED();
            break;
        }
    }
}

static void producerThread() {
    static unsigned char oldBtnState = BTN_SHIFT;

    while(1) {
        unsigned char btnState = (BTN_PORT_IN & BTN_SHIFT);
        if(btnState != oldBtnState) {
            if(btnState == 0) {
                semaphor_V(&prodConsSemaphor);
            }
            oldBtnState = btnState;
        }
        scheduler_threadSleep(BTN_DEBOUNCE_TIME);
    }
}

static void consumerThread() {
    while(1) {
        semaphor_P(&prodConsSemaphor);
        displayMode = displayMode < NUMBER_OF_DISPLAY_MODES ? displayMode + 1 : 0;
    }
}

int main(void) {
    temperature = 0;
    displayMode = 0;
    launchpad_init();
    scheduler_init();
    __enable_interrupt();
    scheduler_startThread(&switchRedLEDThread);

    semaphor_init(&prodConsSemaphor);
    scheduler_startThread(&producerThread);
    scheduler_startThread(&consumerThread);

    aliveThread();
}

static void aliveThread() {
    while(1) {
        launchpad_toggleGreenLED();
        scheduler_threadSleep(500);
    }
}
