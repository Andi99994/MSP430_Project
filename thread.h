#ifndef THREAD_H_
#define THREAD_H_

#include <inttypes.h>
#include <setjmp.h>

#define THREAD_ID_INVALID   0xFFFF

typedef uint16_t ThreadID_t;
typedef void (*ThreadFunction_t)(void);

typedef enum {
    THREADSTATE_INVALID = -1,
    THREADSTATE_READY,
    THREADSTATE_RUNNING,
    THREADSTATE_BLOCKED,
    THREADSTATE_SLEEPING,
    THREADSTATE_DEAD
} ThreadState_t;

typedef struct Thread {
    ThreadFunction_t function;
    ThreadState_t state;
    uint16_t sleepTime;
    jmp_buf context;
} Thread_t;

#endif /* THREAD_H_ */
