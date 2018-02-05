/**
 * semaphor.h
 *
 * This Headerfile defines the basic structure and functions of a semaphor.
 *
 */


#ifndef SEMAPHOR_H_
#define SEMAPHOR_H_

typedef struct {                        //Defines the control block of a semaphor
    int counter;
    unsigned char queueCount;
    unsigned short queue;
} Semaphor_t;

/**
 * Initializer function for a semaphor.
 */
void semaphor_init(Semaphor_t* semaphor);

/**
 * Blocking function for a semaphor.
 */
void semaphor_P(Semaphor_t* semaphor);

/**
 * Releasing function for a semaphor.
 */
void semaphor_V(Semaphor_t* semaphor);

#endif /* SEMAPHOR_H_ */
