#include "scheduler.h"
#include "semaphor.h"
#include "launchpad.h"

static inline void semaphor_enqueue(Semaphor_t* semaphor, ThreadID_t id);
static inline ThreadID_t semaphor_dequeue(Semaphor_t* semaphor);


void semaphor_init(Semaphor_t* semaphor) {
    semaphor->counter = 0;
    semaphor->queueCount = 0;
    semaphor->queue = 0;
}

void semaphor_P(Semaphor_t* semaphor) {
    unsigned short s;
    ATOMIC_START(s);
    semaphor->counter--;
    if(semaphor->counter < 0) {
        ThreadID_t id = scheduler_getRunningThread();
        semaphor_enqueue(semaphor, id);
        scheduler_blockThread(id);
    }
    ATOMIC_END(s);
}

void semaphor_V(Semaphor_t* semaphor) {
    unsigned short s;
    ATOMIC_START(s);
    semaphor->counter++;
    if (semaphor->counter <= 0) {
        scheduler_resumeThread(semaphor_dequeue(semaphor));
    }
    ATOMIC_END(s);
}

static inline void semaphor_enqueue(Semaphor_t* semaphor, ThreadID_t id) {
    semaphor->queue |= id << (semaphor->queueCount++ * 4);
}

static inline ThreadID_t semaphor_dequeue(Semaphor_t* semaphor) {
    ThreadID_t id = semaphor->queue & 0x000F;
    semaphor->queue >>= 4;
    semaphor->queueCount--;
    return id;
}
