/**
 * thread.h
 *
 * This Headerfile defines the basic properties of a thread structure to be used in the scheduler.
 * Also there are some definitions for constants and helpful typedefs.
 *
 */


#ifndef THREAD_H_
#define THREAD_H_

#include <inttypes.h>
#include <setjmp.h>

#define THREAD_ID_INVALID   0xFFFF              //Defines an invalid thread ID

typedef uint16_t ThreadID_t;                    //Defines the type and range of ThreadIDs
typedef void (*ThreadFunction_t)(void);         //Defines the function pointers to a function that will be executed in a thread

typedef enum {                                  //Defines which states a thread can have
    THREADSTATE_INVALID = -1,
    THREADSTATE_READY,
    THREADSTATE_RUNNING,
    THREADSTATE_BLOCKED,
    THREADSTATE_SLEEPING,
    THREADSTATE_DEAD
} ThreadState_t;

typedef struct Thread {                         //Defines the control block of a thread
    ThreadFunction_t function;
    ThreadState_t state;
    uint16_t sleepTime;
    jmp_buf context;
} Thread_t;

#endif /* THREAD_H_ */
