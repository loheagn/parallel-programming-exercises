#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

int threadCount;
int methodNo;
double gap;
int* taskStartList;
int* taskEndList;

int currentRank;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t globalSem;

double sum;

double compute(int myRank) {
    int start = taskStartList[myRank], end = taskEndList[myRank];
    double mySum = 0;
    for (int i = start; i <= end; i++) {
        double x = i * gap + gap / 2.0;
        double y = x * x + x;
        mySum += y * gap;
    }
    return mySum;
}

void* busyWait(void* rank) {
    int myRank = (int)rank;
    double mySum = compute(myRank);
    while (currentRank != myRank)
        ;
    sum += mySum;
    currentRank++;
}

void* mutexFunc(void* rank) {
    int myRank = (int)rank;
    double mySum = compute(myRank);
    pthread_mutex_lock(&mutex);
    sum += mySum;
    pthread_mutex_unlock(&mutex);
}

void* sem(void* rank) {
    int myRank = (int)rank;
    double mySum = compute(myRank);
    sem_wait(&globalSem);
    sum += mySum;
    sem_post(&globalSem);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("no enough args\n");
        exit(1);
    }
    threadCount = atoi(argv[1]);
    methodNo = atoi(argv[2]);
    pthread_t* threadHandles;
    threadHandles = (pthread_t*)malloc(threadCount * sizeof(pthread_t));

    void* worker;
    switch (methodNo) {
        case 1:
            worker = busyWait;
            currentRank = 0;
            printf("Using method busy-wait\n");
            break;

        case 2:
            worker = mutexFunc;
            pthread_mutex_init(&mutex, NULL);
            printf("Using method mutex\n");
            break;

        case 3:
            worker = sem;
            sem_init(&globalSem, 0, 1);
            printf("Using method sem\n");
            break;

        default:
            printf("unsupported method\n");
            exit(1);
    }
    printf("Input the value of a,b,n:\n");
    int n, per, tail;
    double a, b;
    scanf("%lf %lf %d", &a, &b, &n);
    gap = (b - a) / n;
    tail = n % threadCount;
    per = n / threadCount;

    taskStartList = malloc(threadCount * sizeof(int));
    taskEndList = malloc(threadCount * sizeof(int));
    {
        int current = 0;
        for (int i = 0; i < threadCount; i++) {
            taskStartList[i] = current;
            current += per;
            if (i < tail) current++;
            taskEndList[i] = current - 1;
        }
    }

    sum = 0.0;
    for (int i = 0; i < threadCount; i++) {
        pthread_create(&threadHandles[i], (pthread_attr_t*)NULL, worker,
                       (void*)i);
    }

    for (int i = 0; i < threadCount; i++) {
        pthread_join(threadHandles[i], NULL);
    }

    printf("Estimate of the integral:\n%lf", sum);

    return 0;
}