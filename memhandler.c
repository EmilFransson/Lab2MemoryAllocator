#include "memhandler.h"

memHandler *ma_init(int memory_size)
{
    if (memory_size <= 0)
    {
        return NULL;
    }
    else
    {
        memHandler *memoryHandler = malloc(sizeof(*memoryHandler));
        memoryHandler->allocatedSize = memory_size;
        memoryHandler->allocatedItems = 1;
        memoryHandler->basePointer = malloc(memory_size);
        memoryHandler->first = malloc(sizeof(*memoryHandler->first));
        memoryHandler->first->startingAddress = memoryHandler->basePointer;
        memoryHandler->first->endAddress = memoryHandler->first->startingAddress + (memory_size - 1);
        memoryHandler->first->next = NULL;

        return memoryHandler;
    }
}

uint64_t ma_getIdentifier(memHandler *myHandler)
{
    return 3;
}

char *ma_allocate(memHandler *myHandler, int memSize, uint64_t IDENT, pthread_t THID)
{
    return NULL;
}

int ma_release(memHandler *myHandler, uint64_t IDENT, pthread_t THID)
{
    return 3;
}

char *ma_increase(memHandler *myHandler, int *memSize, uint64_t IDENT, pthread_t THID)
{
    return NULL;
}

int ma_availible(memHandler *myHandler)
{
    return 100;
}

int ma_addBlob(memHandler *myHandler, int memory_size)
{
    return 3;
}

int ma_showAllocations(memHandler *myHandler, char *OUTPUT)
{
    return 3;
}

int ma_terminate(memHandler *myHandler)
{
    return 3;
}

double getVersion(void)
{
    return 34.00;
}