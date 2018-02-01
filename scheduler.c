#include "scheduler.h"
#include "drivers/launchpad.h"

static Thread_t gThreads[THREADPOOL_SIZE];
static ThreadID_t gRunningThread = 0;

static ThreadID_t scheduler_getNextOpenSlot();
static ThreadID_t scheduler_getPendingThread();
static void scheduler_killThread();

void timerCallback(uint16_t time);

void scheduler_init() {
    unsigned int i;
    for(i = 0; i < THREADPOOL_SIZE; i++) {
        gThreads[i].state = THREADSTATE_INVALID;
    }
    gThreads[gRunningThread].state = THREADSTATE_RUNNING;
}

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

ThreadID_t scheduler_getRunningThread() {
    return gRunningThread;
}

void scheduler_runNextThread() {
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

void scheduler_threadSleep(uint16_t sleepTime) {
    gThreads[gRunningThread].sleepTime = sleepTime;
    gThreads[gRunningThread].state = THREADSTATE_SLEEPING;
    scheduler_runNextThread();
}

static ThreadID_t scheduler_getPendingThread() {
    unsigned int i = gRunningThread;
    do {
        i = (i + 1) % THREADPOOL_SIZE;
        if(gThreads[i].state == THREADSTATE_READY) {
            return i;
        }
    } while (i != gRunningThread);

    return gRunningThread;
}

static ThreadID_t scheduler_getNextOpenSlot() {
    unsigned int i = 0;

    for (i = 0; i < THREADPOOL_SIZE; i++) {
        if(gThreads[i].state == THREADSTATE_INVALID) {
            return i;
        }
    }

    return THREAD_ID_INVALID;
}

static void scheduler_killThread() {
    unsigned short s;
    ATOMIC_START(s);
    gThreads[gRunningThread].state = THREADSTATE_INVALID;
    scheduler_runNextThread();
    ATOMIC_END(s);
}

void scheduler_blockThread(ThreadID_t id) {
    gThreads[id].state = THREADSTATE_BLOCKED;
    scheduler_runNextThread();
}

void scheduler_resumeThread(ThreadID_t id) {
    gThreads[id].state = THREADSTATE_READY;
}

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
