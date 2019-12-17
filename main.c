#include "memhandler.h"
#include <time.h>

int spawnThread(memHandler *mh, int blocks, int blockSize, int threadID);
//void grabStartTime();
//void grabEndTime();

pthread_t *pthreads;

int main(int argc, char **argv)
{
    memHandler *memoryHandler;
    //srand((unsigned int)time(NULL));
    //int blocks=atoi(argv[1]);
    //int blockSize=atoi(argv[2]);
    //int threads=atoi(argv[3]);

    int blocks = 10;
    int blockSize = 10000;
    int threads = 1;

    //Skapar 'threads antal' trådar, thread blir då en array av threads.
    pthreads = malloc(sizeof(pthread_t) * threads);

    int totalMemorySize = blocks * blockSize;
    memoryHandler = ma_init(totalMemorySize);
    if (memoryHandler == NULL)
    {
        fprintf(stderr, "Error allocation memory, exiting program...");
        exit(-1);
    }

    for (int i = 0; i < threads; i++)
    {
        if (spawnThread(memoryHandler, blocks, blockSize, i) == 0)
        {
            fprintf(stderr, "Unable to launch thread %d.", i);
        }
    }
    return (0);
}

int spawnThread(memHandler *mh, int blocks, int blockSize, int threadID)
{
    //pthread_create(&thread[threadID], NULL, );
    return (-1);
}