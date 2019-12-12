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
        memoryHandler->basePointer = malloc(memory_size);
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
    if (ma_availible(myHandler) >= memSize)
    {
        struct listItem* item = (struct listItem*)malloc(sizeof(item));
        item->allocationSize = memSize;
        item->THID = THID;
        item->IDENT = IDENT;
        item->next = NULL;

        if (myHandler->first != NULL)
        {
            struct listItem* temp = myHandler->first;
            while (temp->next != NULL)
            {
                temp = temp->next;
            }
            temp->next = item;
            myHandler->allocatedItems++;
            
            if (myHandler->allocatedSize % myHandler->allocatedItems == 0)
            {
                myHandler->first->endAddress = ((char *)myHandler->basePointer + (memSize - 1));
                temp = myHandler->first;
                while (temp->next != NULL)
                {
                    
                }
            }
        }
        else
        {
            myHandler->first = item;
            myHandler->allocatedItems++;
            myHandler->first->startingAddress = (char *)myHandler->basePointer;
            myHandler->first->endAddress = (char *)myHandler->basePointer + (memSize - 1);
            return (char*)myHandler->first->startingAddress;
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
        struct listItem* temp = myHandler->first;
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
    return 3;
}

double getVersion(void)
{
    return 34.00;
}