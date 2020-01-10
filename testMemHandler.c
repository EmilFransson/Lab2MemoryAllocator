#include "memhandler.h"
#include <time.h>

struct myStruct
{
    memHandler *myHandler;
    int blocks;
    int blockSize;
    int threadID;
    bool differentBlockSizes;
    int *uniqueBlocks;
};
struct timespec timeStart;
struct timespec timeEnd;

void *threadRunner(void *arg);
void grabStartTime();
void grabEndTime();
void displayTimeElapsed(char *string);
memHandler *runTest(int blocks, int blockSize, int threads);
void callocComparison(int blocks, int blockSize, char *testSpecifics);

pthread_t pthreads[3];
int main(int argc, char **argv)
{
    memHandler *myHandler = NULL;

    if (argc < 2)
    {
        int nrOfThreads = 1;

        /*What follows are numerous tests that, at first, might seem to make main quite big. This is true,
        however for practical purposes it makes it easy to follow along and to actually read and understand what
        tests what and when it does. That's why it is left this way; it does the job and provides a good overview.*/

        //10 blocks, 10000 blocksize, 1 thread:
        grabStartTime();
        myHandler = runTest(10, 10000, 1);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("1 thread: 10 blocks, 10000 blocksize");
        callocComparison(10, 100000, "Calloc: 10 blocks, 10000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //100 blocks, 10000 blocksize, 1 thread:
        grabStartTime();
        myHandler = runTest(100, 10000, 1);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("1 thread: 100 blocks, 10000 blocksize");
        callocComparison(100, 10000, "Calloc: 100 blocks, 10000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //1000 blocks, 10000 blocksize, 1 thread:
        grabStartTime();
        myHandler = runTest(1000, 10000, 1);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("1 thread: 1000 blocks, 10000 blocksize");
        callocComparison(1000, 10000, "Calloc: 1000 blocks, 10000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //10000 blocks, 10000 blocksize, 1 thread:
        grabStartTime();
        myHandler = runTest(10000, 10000, 1);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("1 thread: 10000 blocks, 10000 blocksize");
        callocComparison(10000, 10000, "Calloc: 10000 blocks, 10000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //10 blocks, 100000 blocksize, 1 thread:
        grabStartTime();
        myHandler = runTest(10, 100000, 1);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("1 thread: 10 blocks, 100000 blocksize");
        callocComparison(10, 100000, "Calloc: 10 blocks, 100000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //100 blocks, 100000 blocksize, 1 thread:
        grabStartTime();
        myHandler = runTest(100, 100000, 1);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("1 thread: 100 blocks, 100000 blocksize");
        callocComparison(100, 100000, "Calloc: 100 blocks, 100000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //1000 blocks, 100000 blocksize, 1 thread:
        grabStartTime();
        myHandler = runTest(1000, 100000, 1);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("1 thread: 1000 blocks, 100000 blocksize");
        callocComparison(1000, 100000, "Calloc: 1000 blocks, 100000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //10000 blocks, 100000 blocksize, 1 thread:
        myHandler = runTest(10000, 100000, 1);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("1 thread: 10000 blocks, 100000 blocksize");
        callocComparison(10000, 100000, "Calloc: 10000 blocks, 100000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        nrOfThreads = 2;

        //10 blocks, 10000 blocksize, 2 threads:
        grabStartTime();
        myHandler = runTest(10, 10000, 2);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("2 threads: 10 blocks, 10000 blocksize");
        callocComparison(10, 10000, "Calloc: 10 blocks, 10000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //100 blocks, 10000 blocksize, 2 threads:
        myHandler = runTest(100, 10000, 2);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("2 threads: 100 blocks, 10000 blocksize");
        callocComparison(100, 10000, "Calloc: 100 blocks, 10000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //1000 blocks, 10000 blocksize, 2 threads:
        grabStartTime();
        myHandler = runTest(1000, 10000, 2);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("2 threads: 1000 blocks, 10000 blocksize");
        callocComparison(1000, 10000, "Calloc: 1000 blocks, 10000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //10000 blocks, 10000 blocksize, 2 threads:
        grabStartTime();
        myHandler = runTest(10000, 10000, 2);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("2 threads: 10000 blocks, 10000 blocksize");
        callocComparison(10000, 10000, "Calloc: 10000 blocks, 10000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //10 blocks, 100000 blocksize, 2 threads:
        grabStartTime();
        myHandler = runTest(10, 100000, 2);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("2 threads: 10 blocks, 100000 blocksize");
        callocComparison(10, 100000, "Calloc: 10 blocks, 100000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //100 blocks, 100000 blocksize, 2 threads:
        grabStartTime();
        myHandler = runTest(100, 100000, 2);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("2 threads: 100 blocks, 100000 blocksize");
        callocComparison(100, 100000, "Calloc: 100 blocks, 100000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //1000 blocks, 100000 blocksize, 2 threads:
        grabStartTime();
        myHandler = runTest(1000, 100000, 2);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("2 threads: 1000 blocks, 100000 blocksize");
        callocComparison(1000, 100000, "Calloc: 1000 blocks, 100000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //10000 blocks, 100000 blocksize, 2 threads:
        grabStartTime();
        myHandler = runTest(10000, 100000, 2);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("2 threads: 10000 blocks, 100000 blocksize");
        callocComparison(10000, 100000, "Calloc: 10000 blocks, 100000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        nrOfThreads = 3;

        //10 blocks, 10000 blocksize, 3 threads:
        grabStartTime();
        myHandler = runTest(10, 10000, 3);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("3 threads: 10 blocks, 10000 blocksize");
        callocComparison(10, 10000, "Calloc: 10 blocks, 10000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //100 blocks, 10000 blocksize, 3 threads:
        grabStartTime();
        runTest(100, 10000, 3);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("3 threads: 100 blocks, 10000 blocksize");
        callocComparison(100, 10000, "Calloc: 100 blocks, 10000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //1000 blocks, 10000 blocksize, 3 threads:
        grabStartTime();
        myHandler = runTest(1000, 10000, 3);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("3 threads: 1000 blocks, 10000 blocksize");
        callocComparison(1000, 10000, "Calloc: 1000 blocks, 10000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //10000 blocks, 10000 blocksize, 3 threads:
        grabStartTime();
        myHandler = runTest(10000, 10000, 3);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("3 threads: 10000 blocks, 10000 blocksize");
        callocComparison(10000, 10000, "Calloc: 10000 blocks, 10000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //10 blocks, 100000 blocksize, 3 threads:
        grabStartTime();
        myHandler = runTest(10, 100000, 3);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("3 threads: 10 blocks, 100000 blocksize");
        callocComparison(10, 10000, "Calloc: 10 blocks, 100000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //100 blocks, 100000 blocksize, 3 threads:
        grabStartTime();
        myHandler = runTest(100, 100000, 3);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("3 threads: 100 blocks, 100000 blocksize");
        callocComparison(100, 100000, "Calloc: 100 blocks, 100000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //1000 blocks, 100000 blocksize, 3 threads:
        grabStartTime();
        myHandler = runTest(1000, 100000, 3);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("3 threads: 1000 blocks, 100000 blocksize");
        callocComparison(1000, 100000, "Calloc: 1000 blocks, 100000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");

        //10000 blocks, 10000 blocksize, 3 threads:
        grabStartTime();
        myHandler = runTest(10000, 100000, 3);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        displayTimeElapsed("3 threads: 10000 blocks, 100000 blocksize");
        callocComparison(10000, 100000, "Calloc: 10000 blocks, 100000 blocksize");
        ma_terminate(myHandler);
        printf("\n\n");
    }
    else
    {
        int blocks = atoi(argv[1]);
        int blockSize = atoi(argv[2]);
        int nrOfThreads = atoi(argv[3]);

        if (nrOfThreads > 3)
        {
            fprintf(stderr, "Test built for maximum 3 threads (although the library can handle more).\nExiting program...");
            exit(-1);
        }

        grabStartTime();
        myHandler = runTest(blocks, blockSize, nrOfThreads);
        for (int i = 0; i < nrOfThreads; i++)
        {
            pthread_join(pthreads[i], NULL);
        }
        grabEndTime();
        printf("\n%d thread(s): %d blocks, %d blocksize", nrOfThreads, blocks, blockSize);
        displayTimeElapsed("");
        printf("Calloc: %d blocks, %d blocksize", blocks, blockSize);
        callocComparison(blocks, blockSize, "");
        ma_terminate(myHandler);
        printf("\n\n");
    }

    return (0);
}

void *threadRunner(void *arg)
{
    struct myStruct structOne = *(struct myStruct *)arg;
    char *startAddresses[structOne.blocks];
    int memorySize[structOne.blocks];
    uint64_t identifiers[structOne.blocks];
    if (structOne.differentBlockSizes == false)
    {
        for (int i = 0; i < structOne.blocks; i++)
        {
            identifiers[i] = ma_getIdentifier(structOne.myHandler);
            startAddresses[i] = ma_allocate(structOne.myHandler, structOne.blockSize, identifiers[i], structOne.threadID);
            memorySize[i] = structOne.blockSize;
        }
    }
    else
    {
        //Handy theoretical option if blocksizes actually varies, which they don't in these tests:
        //The library can handle it though.
        for (int i = 0; i < structOne.blocks; i++)
        {
            identifiers[i] = ma_getIdentifier(structOne.myHandler);
            startAddresses[i] = ma_allocate(structOne.myHandler, structOne.blockSize, identifiers[i], structOne.threadID);
            memorySize[i] = structOne.uniqueBlocks[i];
        }
    }

    for (int j = 0; j < structOne.blocks; j++)
    {
        ma_clear(startAddresses[j], memorySize[j]);
    }
    for (int k = 0; k < structOne.blocks; k++)
    {
        ma_release(structOne.myHandler, identifiers[k], structOne.threadID);
    }
    struct myStruct *temp = (struct myStruct *)arg;
    free(temp);
    temp = NULL;
    pthread_exit(NULL);
}

void grabStartTime()
{
    timeStart.tv_sec = 0;
    timeStart.tv_nsec = 0;
    clock_gettime(1, &timeStart);
}

void grabEndTime()
{
    timeEnd.tv_sec = 0;
    timeEnd.tv_sec = 0;
    clock_gettime(1, &timeEnd);
}

void displayTimeElapsed(char *string)
{
    double timeSec = (double)timeEnd.tv_sec - (double)timeStart.tv_sec;
    double timeNanSec = (1.0e-9 * timeEnd.tv_nsec) - (1.0e-9 * timeStart.tv_nsec);
    double finalTime = timeSec + timeNanSec;
    printf("%s: %.8f seconds.\n", string, finalTime);
}

memHandler *runTest(int blocks, int blockSize, int nrOfThreads)
{
    int totalMemorySize = blocks * blockSize;

    memHandler *myHandler = ma_init(totalMemorySize);
    if (myHandler == NULL)
    {
        fprintf(stderr, "Error allocation memory for memoryhandler, exiting program...");
        exit(-1);
    }
    //1 thread:
    if (nrOfThreads == 1)
    {
        struct myStruct *structOne = (struct myStruct *)malloc(sizeof(struct myStruct));
        structOne->myHandler = myHandler;
        structOne->blocks = blocks;
        structOne->blockSize = blockSize;
        structOne->threadID = 0;

        if (pthread_create(&pthreads[0], NULL, threadRunner, structOne) != 0)
        {
            fprintf(stderr, "Unable to create thread 0, terminating program...");
            exit(-1);
        }
    }
    //2 threads:
    else if (nrOfThreads == 2)
    {
        int correctNrOfBlocks = (blocks / 2);
        for (int i = 0; i < nrOfThreads; i++)
        {
            struct myStruct *structOne = (struct myStruct *)malloc(sizeof(struct myStruct));
            structOne->myHandler = myHandler;
            structOne->blocks = correctNrOfBlocks;
            structOne->blockSize = blockSize;
            structOne->threadID = i;

            if (pthread_create(&pthreads[i], NULL, threadRunner, structOne) != 0)
            {
                fprintf(stderr, "Unable to create thread %d, terminating program...", i);
                exit(-1);
            }
            //Adjusting second thread's nr of blocks, taking into account that 'blocks' might be uneven.
            correctNrOfBlocks = blocks - correctNrOfBlocks;
        }
    }
    //3 threads:
    else if (nrOfThreads == 3)
    {
        if (blocks % 3 != 0)
        {
            int blocksForGivenThreads[3];
            //Odd nrOfBlocks:
            if (blocks % 2 != 0)
            {
                blocksForGivenThreads[0] = (blocks / 3) + 1;
                if (blocksForGivenThreads[0] % 2 == 0)
                {
                    blocksForGivenThreads[1] = blocksForGivenThreads[0];
                }
                else
                {
                    blocksForGivenThreads[1] = blocksForGivenThreads[0] - 1;
                }
                blocksForGivenThreads[2] = blocks - blocksForGivenThreads[0] - blocksForGivenThreads[1];
            }
            //Even nrOfBlocks
            else
            {
                blocksForGivenThreads[0] = (blocks / 3) + 1;
                blocksForGivenThreads[1] = blocksForGivenThreads[0] - 1;
                blocksForGivenThreads[2] = blocks - blocksForGivenThreads[0] - blocksForGivenThreads[1];
            }
            for (int i = 0; i < nrOfThreads; i++)
            {
                struct myStruct *structOne = (struct myStruct *)malloc(sizeof(struct myStruct));
                structOne->myHandler = myHandler;
                structOne->blocks = blocksForGivenThreads[i];
                structOne->blockSize = blockSize;
                structOne->threadID = i;

                if (pthread_create(&pthreads[i], NULL, threadRunner, structOne) != 0)
                {
                    fprintf(stderr, "Unable to create thread %d, terminating program...", i);
                    exit(-1);
                }
            }
        }
        else
        {
            for (int i = 0; i < nrOfThreads; i++)
            {
                struct myStruct *structOne = (struct myStruct *)malloc(sizeof(struct myStruct));
                structOne->myHandler = myHandler;
                structOne->blocks = (blocks / 3);
                structOne->blockSize = blockSize;
                structOne->threadID = i;

                if (pthread_create(&pthreads[i], NULL, threadRunner, structOne) != 0)
                {
                    fprintf(stderr, "Unable to create thread %d, terminating program...", i);
                    exit(-1);
                }
            }
        }
    }
    return myHandler;
}

void callocComparison(int blocks, int blockSize, char *testSpecifics)
{
    grabStartTime();
    char *testWithCalloc = (char *)calloc(blocks * blockSize, sizeof(char));
    free(testWithCalloc);
    grabEndTime();
    displayTimeElapsed(testSpecifics);
}
