#ifndef SEMAPHOR_H_
#define SEMAPHOR_H_

typedef struct {
    int counter;
    unsigned char queueCount;
    unsigned short queue;
} Semaphor_t;

void semaphor_init(Semaphor_t* semaphor);
void semaphor_P(Semaphor_t* semaphor);
void semaphor_V(Semaphor_t* semaphor);

#endif /* SEMAPHOR_H_ */
