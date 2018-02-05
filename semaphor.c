/**
 * semaphor.c
 *
 * This file contains the implementation of the functionality declared in semaphor.h.
 *
 */

#include "scheduler.h"
#include "semaphor.h"
#include "drivers/launchpad.h"

//Helper functions to use the bitwise queue
static inline void semaphor_enqueue(Semaphor_t* semaphor, ThreadID_t id);
static inline ThreadID_t semaphor_dequeue(Semaphor_t* semaphor);

/**
 * Initializes a semaphor by initializing the struct variables with 0.
 */
void semaphor_init(Semaphor_t* semaphor) {
    semaphor->counter = 0;
    semaphor->queueCount = 0;
    semaphor->queue = 0;
}

/**
 * Blocking function for a semaphor. This is an atomic function which blocks the current thread until semaphor_V is called.
 */
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

/**
 * Releasing function for a semaphor. This is an atomic function which releases the block from blocked threads.
 */
void semaphor_V(Semaphor_t* semaphor) {
    unsigned short s;
    ATOMIC_START(s);
    semaphor->counter++;
    if (semaphor->counter <= 0) {
        scheduler_resumeThread(semaphor_dequeue(semaphor));
    }
    ATOMIC_END(s);
}

/**
 * Helper function to enqueue a ThreadID_t into the bitwise queue of a semaphor.
 */
static inline void semaphor_enqueue(Semaphor_t* semaphor, ThreadID_t id) {
    semaphor->queue |= id << (semaphor->queueCount++ * 4);                  //shift the id into the queue by queue count times 4bit and then increment the counter
}

/**
 * Helper function to dequeue a ThreadID_t from a bitwise queue of a semaphor.
 *
 */
static inline ThreadID_t semaphor_dequeue(Semaphor_t* semaphor) {
    ThreadID_t id = semaphor->queue & 0x000F;                               //extract the last 4 bit of the queue
    semaphor->queue >>= 4;                                                  //shift the whole queue right by 4 bit
    semaphor->queueCount--;                                                 //decrement the counter
    return id;
}
