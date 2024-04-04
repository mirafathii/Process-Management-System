#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
    int id;
    int time;
} Process;

Process* initalize_process(int id)
{
    Process *process = (Process*)malloc(sizeof(Process));
    process->id = id;
    process->time = rand() % 100 + 1;
    return process;
}

void freeProcess(Process *process)
{
    free(process);
}

void printProcess(Process *process)
{
    printf("Process ID: %d, Burst Time: %d\n", process->id, process->time);
}

typedef struct
{
    Process **items;
    int head;
    int tail;
    int capacity;
    int counter;
} Queue;

Queue* initQ(int capacity)
{
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (queue == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    queue->items = (Process**)malloc(capacity * sizeof(Process*));
    if (queue->items == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    queue->head = 0;
    queue->tail = 0;
    queue->capacity = capacity;
    queue->counter = 0;

    return queue;
}

int isQFull(Queue* queue)
{
    return queue->counter == queue->capacity;
}

int isQEmpty(Queue* queue)
{
    return queue->counter == 0;
}

void enqueue(Queue* queue, Process* process)
{
    if (isQFull(queue))
    {
        fprintf(stderr, "Queue is full\n");
        return; // or handle overflow in some way
    }

    queue->items[queue->tail] = process;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->counter++;
}

Process* dequeue(Queue* queue)
{
    if (isQEmpty(queue))
    {
        fprintf(stderr, "Queue is empty\n");
        return NULL; // or handle underflow in some way
    }

    Process* dequeuedProcess = queue->items[queue->head];
    queue->head = (queue->head + 1) % queue->capacity;
    queue->counter--;

    return dequeuedProcess;
}


void printQueue(Queue* q)
{
    printf("Queue Contents:\n");

    if (isQEmpty(q))
    {
        printf("Queue is empty\n");
        return;
    }

    int index = q->head;
    for (int i = 0; i < q->counter; ++i)
    {
        printProcess(q->items[index]);
        index = (index + 1) % q->capacity;
    }
}




Queue *firstQueue, *secondQueue, *thirdQueue;


int numOfCreatedProcess=0;


// ((((this function work as long term schedular))))
void* createAndEnter(void* args)
{
    for(int i=0; i<100; i++)
    {
        if(firstQueue->counter < firstQueue->capacity-1)
        {
            Process* p=initalize_process(i+1);
            enqueue(firstQueue,p);
            //printProcess(p);
            numOfCreatedProcess= numOfCreatedProcess+1;
        }
        else
        {
            usleep(10000);
            i=i-1;
        }
    }
}
int numberOfProcessRun=0;
// ((((this function work as CPU schedular))))
void* run(void* args)
{
    while(numOfCreatedProcess < 100 || !isQEmpty(firstQueue) || !isQEmpty(secondQueue) || !isQEmpty(thirdQueue))
    {
        // printf("\nNum Created Process %d\n", numOfCreatedProcess);
        int r=rand() % 100 + 1;
        if (r>=1&&r<=50&&firstQueue->counter!=0&&!isQFull(secondQueue))
        {
            Process *p=dequeue(firstQueue);
            p->time-=8;
            if (p->time==0||p->time<0)
            {
                numberOfProcessRun=numberOfProcessRun+1;
                printf("process with id = %d finished\n",p->id);
            }

            else
            {
                enqueue(secondQueue,p);
            }
        }
        else if(r>50&&r<=80&&secondQueue->counter!=0)
        {
            Process *p=dequeue(secondQueue);
            p->time-=16;
            if (p->time==0||p->time<0)
            {

                numberOfProcessRun=numberOfProcessRun+1;
                printf("process with id = %d finished\n",p->id);
            }

            else
            {
                int z=(rand() % 2) +1;
                if(z==1&&!isQFull(firstQueue))
                {
                    enqueue(firstQueue,p);

                }
                else if(!isQFull(thirdQueue))
                {
                    enqueue(thirdQueue,p);
                }
                else
                {

                    printf("error");
                    exit(0);
                }
            }

        }
        else if (thirdQueue->counter!=0)
        {
            Process *p=dequeue(thirdQueue);
            p->time=0;
            numberOfProcessRun=numberOfProcessRun+1;
            printf("process with id = %d finished\n",p->id);

        }
        usleep(10000);
    }
    return NULL;
}
void free_queue(Queue *queue)
{
    for (int i = 0; i < queue->counter; ++i)
    {
        freeProcess(queue->items[i]);
    }

    free(queue->items);
    free(queue);
}
int main()
{
    firstQueue = initQ(10);
    secondQueue = initQ(20);
    thirdQueue = initQ(30);

    pthread_t createAndEnterProcess;
    pthread_t runProcess;

    pthread_create(&createAndEnterProcess,NULL,createAndEnter,NULL);
    pthread_create(&runProcess,NULL,run,NULL);

    pthread_join(createAndEnterProcess,NULL);
    pthread_join(runProcess,NULL);

    printf("numberOfProcessRun=%d\n",numberOfProcessRun);

    free_queue(firstQueue);
    free_queue(secondQueue);
    free_queue(thirdQueue);

    return(0);
}
