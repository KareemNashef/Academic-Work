#include <stdbool.h>
#include "segel.h"

// Node structure
typedef struct Node *Node;
Node CreateNode(int inValue, struct timeval inArrivalTime);
int NodeValue(Node inNode);
struct timeval NodeTime(Node inNode);

// Queue structure
typedef struct Queue *Queue;
Queue CreateQueue(int inSize);

void PushQueueNode(Queue inQueue, int inValue, struct timeval inArrivalTime);
void PopQueueNode(Queue inQueue);
void RemoveQueueNode(Queue inQueue, int inValue);
int FreeRandom(Queue inQueue);

bool QueueEmpty(Queue inQueue);
bool QueueFull(Queue inQueue);

bool QueuesFull(Queue inQueue1, Queue inQueue2);

Node GetQueueHead(Queue inQueue);
int GetQueueSize(Queue inQueue);