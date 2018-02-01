#include "launchpad.h"
#include "scheduler.h"
#include "semaphor.h"

#define NUMBER_OF_DISPLAY_MODES         3

static void switchRedLEDThread(void);
static void aliveThread();
static Semaphor_t prodConsSemaphor;
static int16_t temperature;
static uint8_t displayMode;

static void switchRedLEDThread() {
    while(1) {
        switch (displayMode) {
        case 0:
            break;
        case 1:
            launchpad_toggleRedLED();
            scheduler_threadSleep(500);
            break;
        case 2:
            launchpad_toggleRedLED();
            scheduler_threadSleep(1000);
            break;
        default:
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
        //displayMode %= NUMBER_OF_DISPLAY_MODES;
        //launchpad_toggleRedLEDEnable();
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
