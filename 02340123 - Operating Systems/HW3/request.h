#ifndef __REQUEST_H__
#include "Queue.h"

typedef struct {
    int ID, Total, Static, Dynamic;
} Thread;

int requestHandle(int inValue, struct timeval inArrivalTime, struct timeval inHandleTime, Thread* Current);

#endif
