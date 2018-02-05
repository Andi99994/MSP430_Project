#include "drivers/launchpad.h"
#include "scheduler.h"
#include "semaphor.h"

typedef enum {
    DISPLAYMODE_CELSIUS,
    DISPLAYMODE_FAHRENHEIT
} DisplayMode_t;

static DisplayMode_t displayMode;
static Semaphor_t btnSemaphor;
static Semaphor_t tempSemaphor;
extern uint8_t gTemperature[3];

static void readTempThread(void);
static void showTempThread(void);
static void buttonProducerThread(void);
static void buttonConsumerThread(void);
static void aliveThread(void);

int main(void) {
    displayMode = DISPLAYMODE_CELSIUS;
    launchpad_init();
    scheduler_init();
    __enable_interrupt();

    semaphor_init(&tempSemaphor);
    semaphor_init(&btnSemaphor);
    scheduler_startThread(&readTempThread);
    scheduler_startThread(&showTempThread);
    scheduler_startThread(&buttonProducerThread);
    scheduler_startThread(&buttonConsumerThread);

    aliveThread();
}

static void readTempThread(void) {
    while(1) {
        launchpad_measureTemperature();
        scheduler_threadSleep(100);
        semaphor_V(&tempSemaphor);
    }
}

static void showTempThread(void) {
    while(1) {
        semaphor_P(&tempSemaphor);
        int32_t sensorValue = gTemperature[0]*256 + gTemperature[1];
        sensorValue *= 17572;
        sensorValue /= 65536;
        sensorValue -= 4685;
        sensorValue /= 10;

        switch (displayMode) {
        case DISPLAYMODE_CELSIUS:
            launchpad_showTemperature(sensorValue, CELSIUS);
            break;
        case DISPLAYMODE_FAHRENHEIT:
            sensorValue *= 18;
            sensorValue /= 10;
            sensorValue += 320;
            launchpad_showTemperature(sensorValue, FAHRENHEIT);
            break;
        default:
            launchpad_clearDisplay();
            break;
        }
    }
}

static void buttonProducerThread(void) {
    static unsigned char oldBtnState = BTN_SHIFT;

    while(1) {
        unsigned char btnState = (BTN_PORT_IN & BTN_SHIFT);
        if(btnState != oldBtnState) {
            if(btnState == 0) {
                semaphor_V(&btnSemaphor);
            }
            oldBtnState = btnState;
        }
        scheduler_threadSleep(BTN_DEBOUNCE_TIME);
    }
}

static void buttonConsumerThread(void) {
    while(1) {
        semaphor_P(&btnSemaphor);
        displayMode = displayMode == DISPLAYMODE_CELSIUS ? DISPLAYMODE_FAHRENHEIT : DISPLAYMODE_CELSIUS;
    }
}

static void aliveThread(void) {
    while(1) {
        launchpad_toggleGreenLED();
        scheduler_threadSleep(500);
    }
}
