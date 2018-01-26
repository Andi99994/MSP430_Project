#include "launchpad.h"
#include "scheduler.h"
#include "semaphor.h"

static void switchRedLEDThread(void);
static void aliveThread();
static Semaphor_t prodConsSemaphor;

static void switchRedLEDThread() {
    while(1) {
        LED_RED_OUT ^= LED_RED_SHIFT;
        scheduler_threadSleep(500);
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
        LED_RED_DIR ^= LED_RED_SHIFT;
    }
}

int main(void) {
    launchpad_init();
    scheduler_init();
    __enable_interrupt();
    scheduler_startThread(&switchRedLEDThread);

    semaphor_init(&prodConsSemaphor);
    scheduler_startThread(&producerThread);
    scheduler_startThread(&consumerThread);

    aliveThread();
    /*uint32_t oldTime = 0;
    while(1) {
        uint32_t time = launchpad_getSystemTicks();
        if(time - oldTime >= 250) {
            LED_GREEN_OUT ^= (LED_GREEN_SHIFT);
            oldTime = time;
        }
    }*/
}

static void aliveThread() {
    while(1) {
        LED_GREEN_OUT ^= (LED_GREEN_SHIFT);
        scheduler_threadSleep(500);
    }
}
