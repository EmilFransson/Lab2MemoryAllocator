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
        memoryHandler->availableMemory = memory_size;
        memoryHandler->allocatedItems = 0;
        memoryHandler->unique = 0;
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
    myHandler->unique++;
    return myHandler->unique;
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
                if (((char *)temp->next->startingAddress - (char *)temp->endAddress - 1) >= memSize && (int)myHandler->blobArray[i][myHandler->bytesInAGivenArray[i] - 1] >= *(int *)(item->next->startingAddress))
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
    if (myHandler->allocatedItems > 0)
    {

        bool found = false;
        struct listItem *temp = myHandler->first;
        struct listItem *tracer = NULL;
        if (temp->IDENT == IDENT && temp->THID == THID)
        {
            found = true;
        }
        while (temp->next != NULL && found == false)
        {
            tracer = temp;
            temp = temp->next;
            if (temp->IDENT == IDENT && temp->THID == THID)
            {
                found = true;
            }
        }
        if (found == true)
        {
            if (temp->next == NULL && tracer == NULL)
            {
                //Only one listItem. (first)
                myHandler->availableMemory += temp->allocationSize;
                free(temp);
            }
            else if (temp->next != NULL && tracer != NULL)
            {
                //Listitem in between 2 other items.
                tracer = temp->next;
                temp->next = NULL;
                myHandler->availableMemory += temp->allocationSize;
                free(temp);
            }
            else if (temp->next != NULL && tracer == NULL)
            {
                //Removing first, has more items in list
                tracer = temp;
                temp = temp->next;
                tracer->next = NULL;
                myHandler->availableMemory += tracer->allocationSize;
                free(tracer);
                myHandler->first = temp;
            }
            myHandler->allocatedItems--;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

char *ma_increase(memHandler *myHandler, int *memSize, uint64_t IDENT, pthread_t THID)
{
    return NULL;
}

int ma_availible(memHandler *myHandler)
{
    if (myHandler != NULL)
    {
        return myHandler->availableMemory;
    }
    return -1;
}

int ma_addBlob(memHandler *myHandler, int memory_size)
{
    return 3;
}

int ma_showAllocations(memHandler *myHandler, char *OUTPUT)
{
    if (myHandler->first != NULL)
    {
        OUTPUT = (char *)malloc((((sizeof(void *) * 2) + sizeof(myHandler->first->IDENT) + sizeof(myHandler->first->THID) + 12) * myHandler->allocatedItems) + 1);
        char* intermediate = (char *)malloc((((sizeof(void *) * 2) + sizeof(myHandler->first->IDENT) + sizeof(myHandler->first->THID) + 12) * myHandler->allocatedItems) + 1);
        struct listItem *temp = myHandler->first;
        while (temp->next != NULL || temp == myHandler->first)
        {
            char* tempArr = (char*)malloc(sizeof(void*) + 4);
            sprintf(tempArr, "%08x -- ", *(char*)temp->startingAddress);
            strcat(intermediate, tempArr);
            char* tempArr2 = (char*)malloc(sizeof(void*) + 3);
            sprintf(tempArr2, "%08x : ", *(char*)temp->endAddress);
            strcat(intermediate, tempArr2);
            char* tempArr3 = (char*)malloc(sizeof(temp->IDENT + 3));
            sprintf(tempArr3, "%ld : ", temp->IDENT);
            strcat(intermediate, tempArr3);
            char* tempArr4 = (char*)malloc(sizeof(temp->THID) + 1);
            sprintf(tempArr4, "%ld\n", temp->THID);
            strcat(intermediate, tempArr4);
            temp = temp->next;
        }
        strcat(intermediate, "\0");
        printf("%s", intermediate);

        //sprintf(OUTPUT, "%08x -- %08x : %d : %d\n", *(char *)temp->startingAddress, *(char *)temp->endAddress, temp->IDENT, temp->THID);
        free(intermediate);
        free(OUTPUT);
    }
    return 5;
}

int ma_terminate(memHandler *myHandler)
{
    if (myHandler != NULL)
    {
        if (myHandler->first != NULL)
        {
            struct listItem *temp = myHandler->first;
            struct listItem *tracer = NULL;
            while (temp->next != NULL)
            {
                tracer = temp;
                temp = temp->next;
                tracer->next = NULL;
                free(tracer);
            }
            free(temp);
        }
        for (int i = 0; i < myHandler->nrOfMemoryBlobsPointedTo; i++)
        {
            free(myHandler->blobArray[i]);
        }
        free(myHandler->blobArray);
        free(myHandler->bytesInAGivenArray);
        free(myHandler);
        return 0;
    }
    else
    {
        return -1;
    }
}

double getVersion(void)
{
    return 34.00;
}