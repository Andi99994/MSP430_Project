/**
 * main.c
 *
 * This file implements the main entry point of the program and some simple application logic.
 * All threads are implemented here.
 *
 */

#include "drivers/launchpad.h"
#include "scheduler.h"
#include "semaphor.h"

typedef enum {                                              //Defines the different display modes to be shown on the display
    DISPLAYMODE_CELSIUS,
    DISPLAYMODE_FAHRENHEIT
} DisplayMode_t;

static DisplayMode_t displayMode;                           //Defines the currently active display mode
static Semaphor_t btnSemaphor;                              //Defines the producer/consumer semaphor used for the button and switching of display modes
static Semaphor_t tempSemaphor;                             //Defines the producer/consumer semaphor used for reading and displaying temperature

/**
 * This thread triggers a temperature measurement and releases the semaphor of the display thread afterwards.
 */
static void readTempThread(void);

/**
 * This thread blocks until there is a result of a temperature measurement. Afterwards it reads the current value,
 * converts it into the correct unit, depending on the display mode and shows it on the LCD screen.
 */
static void showTempThread(void);

/**
 * This thread listens for a button press and releases the semaphor of the button consumer thread if it detects one.
 */
static void buttonProducerThread(void);

/**
 * This thread blocks until a button press is detected and if so switches the display mode.
 */
static void buttonConsumerThread(void);

/**
 * This thread periodically blinks the green LED to keep the application alive and show this as visual feedback.
 */
static void aliveThread(void);

/**
 * Main entry point for the application and the main thread. Any module initializations are done here and also every thread is
 * started here.
 */
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

/**
 * This thread triggers a temperature measurement and releases the semaphor of the display thread afterwards.
 */
static void readTempThread(void) {
    while(1) {
        launchpad_measureTemperature();
        scheduler_threadSleep(100);                     //Sleep to ensure the measurement is complete
        semaphor_V(&tempSemaphor);                      //Produce for the display thread
    }
}

/**
 * This thread blocks until there is a result of a temperature measurement. Afterwards it reads the current value,
 * converts it into the correct unit, depending on the display mode and shows it on the LCD screen.
 */
static void showTempThread(void) {
    while(1) {
        semaphor_P(&tempSemaphor);                        //Block until a measurement happens.
        int32_t sensorValue = launchpad_readTemperature();//Read the current temperature which should have been updated in a different thread.
        sensorValue *= 17572;                             //Convert to °C with one digit after comma multiplied by 10 (20,1 °C = 201 here)
        sensorValue /= 65536;
        sensorValue -= 4685;
        sensorValue /= 10;

        switch (displayMode) {                          //Display °C with the correct unit
        case DISPLAYMODE_CELSIUS:
            launchpad_showTemperature(sensorValue, CELSIUS);
            break;
        case DISPLAYMODE_FAHRENHEIT:                    //Convert to °F and display with the correct unit
            sensorValue *= 18;
            sensorValue /= 10;
            sensorValue += 320;
            launchpad_showTemperature(sensorValue, FAHRENHEIT);
            break;
        default:                                        //Default do not display anything
            launchpad_clearDisplay();
            break;
        }
    }
}

/**
 * This thread listens for a button press and releases the semaphor of the button consumer thread if it detects one.
 */
static void buttonProducerThread(void) {
    static unsigned char oldBtnState = BTN_SHIFT;

    while(1) {
        unsigned char btnState = launchpad_getButtonState();
        if(btnState != oldBtnState) {
            if(btnState == 0) {
                semaphor_V(&btnSemaphor);
            }
            oldBtnState = btnState;
        }
        scheduler_threadSleep(BTN_DEBOUNCE_TIME);
    }
}

/**
 * This thread blocks until a button press is detected and if so switches the display mode.
 */
static void buttonConsumerThread(void) {
    while(1) {
        semaphor_P(&btnSemaphor);
        displayMode = displayMode == DISPLAYMODE_CELSIUS ? DISPLAYMODE_FAHRENHEIT : DISPLAYMODE_CELSIUS;
    }
}

/**
 * This thread periodically blinks the green LED to keep the application alive and show this as visual feedback.
 */
static void aliveThread(void) {
    while(1) {
        launchpad_toggleGreenLED();
        scheduler_threadSleep(500);
    }
}
