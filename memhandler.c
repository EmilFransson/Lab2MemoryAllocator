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
        memoryHandler->allocatedItems = 0;
        memoryHandler->nrOfMemoryBlobsPointedTo = 1;
        memoryHandler->blobArray = (char **)malloc(sizeof(memoryHandler->blobArray) * memoryHandler->nrOfMemoryBlobsPointedTo);
        memoryHandler->blobArray[0] = (char *)malloc(memory_size);
        memoryHandler->bytesInAGivenArray = (int *)malloc(sizeof(memoryHandler->bytesInAGivenArray) * memoryHandler->nrOfMemoryBlobsPointedTo);
        memoryHandler->bytesInAGivenArray[0] = memory_size;
        memoryHandler->basePointer = memoryHandler->blobArray;
        memoryHandler->first = malloc(sizeof(*memoryHandler->first));
        return memoryHandler;
    }
}

uint64_t ma_getIdentifier(memHandler *myHandler)
{
    return 3;
}

char *ma_allocate(memHandler *myHandler, int memSize, uint64_t IDENT, pthread_t THID)
{
    bool allocatedMemory = false;

    struct listItem *item = (struct listItem *)malloc(sizeof(item));
    item->allocationSize = memSize;
    item->THID = THID;
    item->IDENT = IDENT;
    item->next = NULL;

    if (ma_availible(myHandler) >= memSize)
    {
        int i = 0;

        if (myHandler->first != NULL)
        {
            struct listItem *temp = myHandler->first;
            while (temp->next != NULL && allocatedMemory == false)
            {
                if (*((char *)(temp->next->startingAddress - temp->endAddress - 1)) >= memSize && (int)myHandler->blobArray[i][myHandler->bytesInAGivenArray[i] - 1] >= *((char*)(item->next->startingAddress)))
                {
                    item->startingAddress = (char *)temp->endAddress + 1;
                    item->endAddress = (char *)temp->endAddress + memSize;
                    //Linked list, so we need to link correctly in between!
                    item->next = temp->next;
                    temp->next = item;
                    myHandler->allocatedItems++;
                    allocatedMemory = true;
                    return (char *)item->startingAddress;
                }
                else
                {
                    temp = temp->next;
                }
            }
            if (allocatedMemory == false)
            {
            }
        }
        else
        {
            //If first is released then should a new item be first or should first be availible...
            myHandler->first = item;
            myHandler->allocatedItems++;
            myHandler->first->startingAddress = (char *)myHandler->basePointer;
            myHandler->first->endAddress = (char *)myHandler->basePointer + (memSize - 1);
            allocatedMemory = true;
            return (char *)myHandler->first->startingAddress;
        }
    }

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
    if (myHandler->first != NULL)
    {
        int size = myHandler->allocatedSize;
        struct listItem *temp = myHandler->first;
        while (temp->next != NULL)
        {
            size = size - temp->allocationSize;
            temp = temp->next;
        }
        return size;
    }
    else
    {
        return myHandler->allocatedSize;
    }
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
    return 89;
}

double getVersion(void)
{
    return 34.00;
}