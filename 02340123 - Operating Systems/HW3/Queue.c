#include "Queue.h"

// Node implementation

struct Node {
    int Value;
    struct timeval ArrivalTime;
    Node Next;
};

Node CreateNode(int inValue, struct timeval inArrivalTime)
{
    // Allocating a new node with the given info
    Node NewNode = malloc(sizeof(*NewNode));
    NewNode->Value = inValue;
    NewNode->ArrivalTime = inArrivalTime;
    NewNode->Next = NULL;

    return NewNode;
}

int NodeValue(Node inNode)
{
    return inNode->Value;
}

struct timeval NodeTime(Node inNode)
{
    return inNode->ArrivalTime;
}

// Queue implementation

struct Queue {
    int CurrentSize;
    int MaxSize;
    Node Head;
    Node Tail;
};

Queue CreateQueue(int inSize)
{
    // Allocating a new queue with the given max size
    Queue NewQueue = malloc(sizeof(*NewQueue));
    NewQueue->CurrentSize = 0;
    NewQueue->MaxSize = inSize;
    NewQueue->Head = NULL;
    NewQueue->Tail = NULL;

    return NewQueue;
}

void PushQueueNode(Queue inQueue, int inValue, struct timeval inArrivalTime)
{
    // Checking if the queue is at max capacity
    if (inQueue->CurrentSize == inQueue->MaxSize)
        return;
    
    // Creating a new node and adding it to the queue
    Node NewNode = CreateNode(inValue, inArrivalTime);
    if (inQueue->CurrentSize == 0) // Checking if the new node is the first in the queue
        inQueue->Head = NewNode;        
    else
        inQueue->Tail->Next = NewNode;
    inQueue->Tail = NewNode;

    // Incrementing the size of the queue
    inQueue->CurrentSize++;
}

void PopQueueNode(Queue inQueue)
{
    // Checking if the queue has any nodes
    if (inQueue->CurrentSize == 0)
        return;

    // Removing the head and replacing it with the next node
    Node NextNode = inQueue->Head->Next;
    free(inQueue->Head);
    inQueue->Head = NextNode;

    // Decrementing the size of the queue
    inQueue->CurrentSize--;

    // Adjusting the tail
    if (inQueue->CurrentSize < 2)
        inQueue->Tail = NextNode;
}

void RemoveQueueNode(Queue inQueue, int inValue)
{
    // Checking if the queue has any nodes
    if (inQueue->CurrentSize == 0)
        return;

    // Checking if the node is the head
    if (inQueue->Head->Value == inValue){
        PopQueueNode(inQueue);
        return;
    }
        
    // Finding the node and freeing it
    Node Prev = inQueue->Head;
    Node Iter = inQueue->Head->Next;
    while (Iter) {
        if(Iter->Value == inValue){
            Prev->Next = Iter->Next;
            if (inValue == inQueue->Tail->Value)
                inQueue->Tail = Prev;
            
            free(Iter);
            inQueue->CurrentSize--;
            return;
        }

        Prev = Iter;
        Iter = Iter->Next;
    }
}

int FreeRandom(Queue inQueue)
{
    // Choosing a random index and freeing the node
    int RandomIndex = rand() % inQueue->CurrentSize;
    int Value;

    // Checking if the chosen node is the head
    if (RandomIndex == 0){
        Value = inQueue->Head->Value;
        PopQueueNode(inQueue);
        return Value;
    }

    RandomIndex--;
    // Finding the node and freeing it
    Node Prev = inQueue->Head;
    Node Iter = inQueue->Head->Next;
    while (RandomIndex) {
        Prev = Iter;
        Iter = Iter->Next;
        RandomIndex--;
    }

    Value = Iter->Value;
    Prev->Next = Iter->Next;
    if (Iter->Value == inQueue->Tail->Value)
                inQueue->Tail = Prev;
    free(Iter);

    inQueue->CurrentSize--;
    return Value;
}

bool QueueEmpty(Queue inQueue)
{
    return (inQueue->CurrentSize == 0);
}

bool QueueFull(Queue inQueue)
{
    return (inQueue->CurrentSize == inQueue->MaxSize);
}

bool QueuesFull(Queue inQueue1, Queue inQueue2)
{
    return ((inQueue1->CurrentSize + inQueue2->CurrentSize) == inQueue1->MaxSize);
}

Node GetQueueHead(Queue inQueue)
{
    return inQueue->Head;
}

int GetQueueSize(Queue inQueue)
{
    return inQueue->CurrentSize;
}
