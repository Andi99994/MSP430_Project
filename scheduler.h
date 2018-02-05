#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "thread.h"

void scheduler_init(void);
ThreadID_t scheduler_startThread(ThreadFunction_t tFunc);
ThreadID_t scheduler_getRunningThread(void);
void scheduler_runNextThread(void);
void scheduler_threadSleep(uint16_t sleepTime);
void scheduler_blockThread(ThreadID_t id);
void scheduler_resumeThread(ThreadID_t id);

#endif /* SCHEDULER_H_ */
