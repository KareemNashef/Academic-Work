#include "request.h"

// ===== Global Variables =====

int CurrentlyWorking;
Queue WaitingQueue;
Thread** AllThreads;

int IndexMatcher = 0;

pthread_mutex_t MainMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t WorkCondition = PTHREAD_COND_INITIALIZER;
pthread_cond_t BlockCondition = PTHREAD_COND_INITIALIZER;

// ===== ====== ========= =====


void* ThreadFunction(void* meh){

    // Initiating the thread's data
    int Index = IndexMatcher;
    IndexMatcher++;
    AllThreads[Index]->ID = Index;
    AllThreads[Index]->Total = 0;
    AllThreads[Index]->Static = 0;
    AllThreads[Index]->Dynamic = 0;

    while(1) {

        // Waiting for the next requext
        pthread_mutex_lock(&MainMutex);
        while(QueueEmpty(WaitingQueue)){
            pthread_cond_wait(&WorkCondition, &MainMutex);
        }

        // Moving the request to the working queue and unlocking
        int RequestValue = NodeValue(GetQueueHead(WaitingQueue));
        struct timeval RequestArrivalTime = NodeTime(GetQueueHead(WaitingQueue));

        PopQueueNode(WaitingQueue);
        CurrentlyWorking++;
        AllThreads[Index]->Total++;

        pthread_mutex_unlock(&MainMutex);
        
        // Handling the request
        struct timeval RequestHandleTime;
        gettimeofday(&RequestHandleTime, NULL);

        int Ret = requestHandle(RequestValue, RequestArrivalTime, RequestHandleTime, AllThreads[Index]);
        
        
        // Removing the request from the working queue and closing it
        pthread_mutex_lock(&MainMutex);
        Close(RequestValue);        
        CurrentlyWorking--;
        pthread_cond_signal(&BlockCondition);
        pthread_mutex_unlock(&MainMutex);

    }

}

void getargs(int *PortNumber, int *ThreadCount, int *QueueSize, char *SchedAlg, int argc, char *argv[])
{
    if (argc < 5) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }

    *PortNumber = atoi(argv[1]);
    *ThreadCount = atoi(argv[2]);
    *QueueSize = atoi(argv[3]);
    strcpy(SchedAlg, argv[4]);
}

int main(int argc, char *argv[])
{

    // Getting the starting arguments
    int PortNumber, ThreadCount, QueueSize, listenfd, connfd, clientlen;
    struct sockaddr_in clientaddr;
    struct timeval ArrivalTime;
    char SchedAlg[7];

    getargs(&PortNumber, &ThreadCount, &QueueSize, SchedAlg, argc, argv);

    // Creating the requests queues
    CurrentlyWorking = 0;
    WaitingQueue = CreateQueue(QueueSize);

    // Creating the working threads
    pthread_t* WorkingThreads = malloc(ThreadCount * sizeof(*WorkingThreads));
    AllThreads = malloc(ThreadCount * sizeof(*AllThreads));

    for(int i = 0; i < ThreadCount; i++){
        AllThreads[i] = malloc(sizeof(Thread));
        pthread_create(&WorkingThreads[i], NULL, ThreadFunction, NULL);
    }

    // Opening the port
    listenfd = Open_listenfd(PortNumber);

    while (1) {

        // Accepting a new request to be added to the waiting queue
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

        pthread_mutex_lock(&MainMutex);
        
        // Checking if the waiting queue is full
        if (GetQueueSize(WaitingQueue) + CurrentlyWorking == QueueSize) {

            // Dealing with the overload depending on the case
            if (strcmp(SchedAlg, "block") == 0){            // Block : Blocking until there's a place in the queue
                while(GetQueueSize(WaitingQueue) + CurrentlyWorking == QueueSize) {
                    pthread_cond_wait(&BlockCondition, &MainMutex);
                }
            }
            else if (strcmp(SchedAlg, "dt") == 0) {         // Drop-Tail : Closing the current request and moving on to the next one
                Close(connfd);
                pthread_cond_signal(&WorkCondition);
                pthread_mutex_unlock(&MainMutex);
                continue;
            }
            else if (strcmp(SchedAlg, "dh") == 0) {         // Drop-Head : Closing the first request in the queue
                if(QueueEmpty(WaitingQueue)) {
                    Close(connfd);
                    pthread_cond_signal(&WorkCondition);
                    pthread_mutex_unlock(&MainMutex);
                    continue;
                }
                int ToClose = NodeValue(GetQueueHead(WaitingQueue));
                PopQueueNode(WaitingQueue);
                Close(ToClose);
            }
            else if (strcmp(SchedAlg, "random") == 0) {     // Random : Dropping half of the waiting queue randomly
                int WaitingQueueSize = ((GetQueueSize(WaitingQueue) + 1) / 2);
                if(QueueEmpty(WaitingQueue)) {
                    Close(connfd);
                    pthread_cond_signal(&WorkCondition);
                    pthread_mutex_unlock(&MainMutex);
                    continue;
                }
                for (int i = 0; i < WaitingQueueSize; i++) {
                    if(QueueEmpty(WaitingQueue))
                        break;
                    Close(FreeRandom(WaitingQueue));
                }
            }
        }

        // Getting the arrival time and adding the request to the queue
        gettimeofday(&ArrivalTime, NULL);
        PushQueueNode(WaitingQueue, connfd, ArrivalTime);

        pthread_cond_signal(&WorkCondition);
        pthread_mutex_unlock(&MainMutex);
    
    }

}
