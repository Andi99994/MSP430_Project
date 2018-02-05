/**
 * scheduler.c
 *
 * This file implements the functionality of scheduler.h. This requires a "launchpad.h" file to be present which should contain
 * the various hardware related parameters.
 *
 */

#include "scheduler.h"
#include "drivers/launchpad.h"

static Thread_t gThreads[THREADPOOL_SIZE];                          //The current threadpool that contains all active threads. THREADPOOL_SIZE is a hardware related parameter
static ThreadID_t gRunningThread = 0;                               //The currently running ThreadID_t

/**
 * Searches for a free thread slot by checking if the status is invalid. Returns an index.
 */
static ThreadID_t scheduler_getNextOpenSlot(void);

/**
 * Searches for the next ready thread to be continued with the round robin principle.
 */
static ThreadID_t scheduler_getPendingThread(void);

/**
 * Invalidates the status of the current thread to release its resources.
 */
static void scheduler_killThread(void);

/**
 * Implementation of the function that is being executed every "system tick" by the timer module.
 */
void timerCallback(uint16_t time);

/**
 * Initializes the scheduler by invalidating every slot of the threadpool except the currently running one,
 * which is the main thread.
 */
void scheduler_init(void) {
    unsigned int i;
    for(i = 0; i < THREADPOOL_SIZE; i++) {
        gThreads[i].state = THREADSTATE_INVALID;
    }
    gThreads[gRunningThread].state = THREADSTATE_RUNNING;
}

/**
 * Starts a new thread, if possible and returns the assigned id. This function takes a function pointer as a parameter,
 * which is being executed by the thread. This is an atomic function, that cannot be interrupted. A new thread is being initialized
 * and assigned an index in the threadpool. Each new thread receives its own share of the stack, which is being defined in "launchpad.h".
 * If it is possible to start a new thread its function is executed and after it finished, the resources are being released.
 */
ThreadID_t scheduler_startThread(ThreadFunction_t function) {
    unsigned short s;
    ATOMIC_START(s);
    ThreadID_t newThread = THREAD_ID_INVALID;
    newThread = scheduler_getNextOpenSlot();
    if(newThread == THREAD_ID_INVALID) {
        return newThread;
    }

    gThreads[newThread].state = THREADSTATE_READY;
    gThreads[newThread].function = function;

    if(setjmp(gThreads[newThread].context) == 0) {
        ATOMIC_END(s);
        return newThread;
    } else {
        _set_SP_register(STACK_UPPER_EDGE_ADDRESS - (gRunningThread * STACKSIZE_PER_THREAD));
        ATOMIC_END(s);
        gThreads[gRunningThread].function();
        scheduler_killThread();
        return THREAD_ID_INVALID;
    }
}

/**
 * Returns the id of the currently running thread.
 */
ThreadID_t scheduler_getRunningThread(void) {
    return gRunningThread;
}

/**
 * Interrupts the execution of the currently running thread, saves its registers and switches to the next pending thread.
 * This uses the round robin principle. If there is no other pending thread, the current thread is not being switched.
 * This is an atomic function.
 */
void scheduler_runNextThread(void) {
    unsigned short s;
    ATOMIC_START(s);
    ThreadID_t nextThread = scheduler_getPendingThread();
    switch (gThreads[nextThread].state) {
        case THREADSTATE_RUNNING:
            break;
        case THREADSTATE_READY:
            if(nextThread == gRunningThread) {
                break;
            }
            if (setjmp(gThreads[gRunningThread].context) == 0) {
                if (gThreads[gRunningThread].state == THREADSTATE_RUNNING) {
                    gThreads[gRunningThread].state = THREADSTATE_READY;
                }
                gRunningThread = nextThread;
                gThreads[gRunningThread].state = THREADSTATE_RUNNING;
                longjmp(gThreads[gRunningThread].context,1);
            }
            break;
        default:
            break;
    }
    ATOMIC_END(s);
}

/**
 * Puts a thread to sleep by changing its state and sets the sleep time.
 * The current thread is being switched to a pending thread.
 */
void scheduler_threadSleep(uint16_t sleepTime) {
    gThreads[gRunningThread].sleepTime = sleepTime;
    gThreads[gRunningThread].state = THREADSTATE_SLEEPING;
    scheduler_runNextThread();
}

/**
 * Searches for a pending thread to be continued.
 */
static ThreadID_t scheduler_getPendingThread(void) {
    unsigned int i = gRunningThread;
    do {
        i = (i + 1) % THREADPOOL_SIZE;
        if(gThreads[i].state == THREADSTATE_READY) {
            return i;
        }
    } while (i != gRunningThread);

    return gRunningThread;
}

/**
 * Searches for an open slot in the threadpool for a new thread.
 */
static ThreadID_t scheduler_getNextOpenSlot() {
    unsigned int i = 0;

    for (i = 0; i < THREADPOOL_SIZE; i++) {
        if(gThreads[i].state == THREADSTATE_INVALID) {
            return i;
        }
    }

    return THREAD_ID_INVALID;
}

/**
 * Invalidates the slot in the threadpool of the current thread to release its resources.
 */
static void scheduler_killThread(void) {
    unsigned short s;
    ATOMIC_START(s);
    gThreads[gRunningThread].state = THREADSTATE_INVALID;
    scheduler_runNextThread();
    ATOMIC_END(s);
}

/**
 * Blocks a thread by setting its state to blocked and run the next thread.
 */
void scheduler_blockThread(ThreadID_t id) {
    gThreads[id].state = THREADSTATE_BLOCKED;
    scheduler_runNextThread();
}

/**
 * Mark a blocked thread with the specified id as ready to be continued.
 */
void scheduler_resumeThread(ThreadID_t id) {
    gThreads[id].state = THREADSTATE_READY;
}

/**
 * Implementation of the callback function for every "system tick". This function decrements the sleep time of all sleeping threads
 * and calls the runNextThread function. The callback is called from the launchpad timer interrupt.
 */
void timerCallback(uint16_t time) {
    unsigned int i;
    for (i = 0; i < THREADPOOL_SIZE; i++) {
        if(gThreads[i].state == THREADSTATE_SLEEPING) {
            if(gThreads[i].sleepTime > time) {
                gThreads[i].sleepTime -= time;
            } else {
                scheduler_resumeThread(i);
            }
        }
    }
    scheduler_runNextThread();
}
