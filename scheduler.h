/**
 * scheduler.h
 *
 * This file defines the basic functionality of the scheduler. The scheduler can start threads and handles various other tasks regarding threads.
 *
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "thread.h"

/**
 * Initializes the scheduler, which should be done before enabling global interrupts.
 */
void scheduler_init(void);

/**
 * Starts a new thread that executes the specified function and returns the assigned ThreadID_t.
 */
ThreadID_t scheduler_startThread(ThreadFunction_t tFunc);

/**
 * Returns the ThreadID_t of the currently running thread.
 */
ThreadID_t scheduler_getRunningThread(void);

/**
 * Saves the current thread state and runs a different thread according to the round robin principle.
 */
void scheduler_runNextThread(void);

/**
 * Puts a thread to sleep for the specified sleep time in milliseconds (approx).
 */
void scheduler_threadSleep(uint16_t sleepTime);

/**
 * Blocks the current thread and prevents it from being executed further until resumed.
 */
void scheduler_blockThread(ThreadID_t id);

/**
 * Resumes a thread with the specified ThreadID_t.
 */
void scheduler_resumeThread(ThreadID_t id);

#endif /* SCHEDULER_H_ */
