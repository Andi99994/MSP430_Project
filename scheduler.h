#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "thread.h"

void scheduler_init();
ThreadID_t scheduler_startThread(ThreadFunction_t tFunc);
ThreadID_t scheduler_getRunningThread();
void scheduler_runNextThread();
void scheduler_threadSleep(uint16_t sleepTime);
void scheduler_blockThread(ThreadID_t id);
void scheduler_resumeThread(ThreadID_t id);

#endif /* SCHEDULER_H_ */
