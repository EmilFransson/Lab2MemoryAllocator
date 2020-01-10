#include "memhandler.h"
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
memHandler *ma_init(int memory_size)
{
    if (memory_size <= 0)
    {
        return NULL;
    }
    else
    {
        memHandler *memoryHandler = malloc(sizeof(*memoryHandler));
        if (memoryHandler)
        {
            memoryHandler->allocatedSize = memory_size;
            memoryHandler->availableMemory = memory_size;
            memoryHandler->allocatedItems = 0;
            memoryHandler->unique = 0;
            memoryHandler->nrOfMemoryBlobsPointedTo = 1;
            memoryHandler->blobArray = (char **)malloc(sizeof(memoryHandler->blobArray) * memoryHandler->nrOfMemoryBlobsPointedTo);
            if (memoryHandler->blobArray)
            {
                memoryHandler->blobArray[0] = (char *)malloc(memory_size);
            }
            else
            {
                return NULL;
            }
            memoryHandler->bytesInAGivenArray = (int *)malloc(sizeof(memoryHandler->bytesInAGivenArray) * memoryHandler->nrOfMemoryBlobsPointedTo);
            if (memoryHandler->bytesInAGivenArray)
            {
                memoryHandler->bytesInAGivenArray[0] = memory_size;
            }
            else
            {
                return NULL;
            }
            memoryHandler->basePointer = memoryHandler->blobArray[0];
            memoryHandler->first = NULL;
            return memoryHandler;
        }
        else
        {
            return NULL;
        }
    }
}

uint64_t ma_getIdentifier(memHandler *myHandler)
{
    pthread_mutex_lock(&mutex);
    if (myHandler != NULL)
    {
        myHandler->unique++;
        uint64_t uniqueIdentifier = myHandler->unique;
        pthread_mutex_unlock(&mutex);
        return uniqueIdentifier;
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

char *ma_allocate(memHandler *myHandler, int memSize, uint64_t IDENT, pthread_t THID)
{
    pthread_mutex_lock(&mutex);

    bool allocatedMemory = false;
    if (ma_availible(myHandler) >= memSize)
    {
        if (myHandler->first != NULL)
        {
            struct listItem *temp = myHandler->first;
            //Controlling whether first item truly has the first valid start-address:
            if (myHandler->first->startingAddress != myHandler->basePointer)
            {
                bool sameBlob = false;
                //'Earlier' start-adress-position possible. Is first valid start-address and first-item start-adress in same blob?
                if ((char *)myHandler->first->startingAddress >= (char *)myHandler->basePointer && (char *)myHandler->first->endAddress <= ((char *)myHandler->basePointer + myHandler->bytesInAGivenArray[0] - 1))
                {
                    //They're in the same blob (first!).
                    sameBlob = true;
                }
                if (sameBlob == true)
                {
                    //Is there, in the same blob, space between first valid startAdress and first-item start-adress?
                    if ((char *)myHandler->first->startingAddress - (char *)myHandler->basePointer - 1 >= memSize)
                    {
                        struct listItem *item = ma_createItem(myHandler, NULL, NULL, memSize, IDENT, THID, (char *)myHandler->basePointer, (char *)myHandler->basePointer + memSize - 1);
                        item->next = myHandler->first;
                        myHandler->first = item;
                        item->previous = myHandler->first;
                        allocatedMemory = true;
                        char *startAddress = (char *)item->startingAddress;
                        pthread_mutex_unlock(&mutex);
                        return startAddress;
                    } //If no place exists we'll have to continue searching the list...
                }
                else
                {
                    //First item does not reside in the first blob.
                    //Is the memory in the first blob enough for the item?
                    if (((char *)myHandler->basePointer + myHandler->bytesInAGivenArray[0] - 1) - (char *)myHandler->basePointer >= memSize)
                    {
                        struct listItem *item = ma_createItem(myHandler, NULL, NULL, memSize, IDENT, THID, (char *)myHandler->basePointer, (char *)myHandler->basePointer + memSize - 1);
                        item->next = myHandler->first;
                        myHandler->first = item;
                        item->previous = myHandler->first;
                        allocatedMemory = true;
                        char *startAddress = (char *)item->startingAddress;
                        pthread_mutex_unlock(&mutex);
                        return startAddress;
                    } //If no place exists we'll have to continue searching the list...
                }
            }
            while (temp->next != NULL && allocatedMemory == false)
            {
                //Does place exist between current item and next?
                if ((char *)temp->next->startingAddress - (char *)temp->endAddress - 1 >= memSize)
                {
                    //Place might exist, we need to assert they're in the same blob:
                    char *verified = ma_assertSameBlob(myHandler, temp, temp->next);
                    //Current item and next item is in same blob if verified != NULL
                    if (verified != NULL)
                    {
                        struct listItem *item = ma_createItem(myHandler, temp, temp->next, memSize, IDENT, THID, (char *)temp->endAddress + 1, (char *)temp->endAddress + memSize);
                        temp->next = item;
                        ma_reLink(myHandler, temp);
                        allocatedMemory = true;
                        char *startAddress = (char *)item->startingAddress;
                        pthread_mutex_unlock(&mutex);
                        return startAddress;
                    }
                    else
                    {
                        //Current item and next item is not in same blob.Does a place exist between current item and blob's end-address?
                        //What blob is the current item in:
                        int currentBlob = ma_returnCurrentBlob(myHandler, temp);

                        if (((char *)myHandler->blobArray[currentBlob] + myHandler->bytesInAGivenArray[currentBlob] - 1) - (char *)temp->endAddress >= memSize)
                        {
                            struct listItem *item = ma_createItem(myHandler, temp, temp->next, memSize, IDENT, THID, (char *)temp->endAddress + 1, (char *)temp->endAddress + memSize);
                            temp->next = item;
                            ma_reLink(myHandler, temp);
                            allocatedMemory = true;
                            char *startAddress = (char *)item->startingAddress;
                            pthread_mutex_unlock(&mutex);
                            return startAddress;
                        } //If no place exists, keep searching in list...
                        else
                        {
                            temp = temp->next;
                        }
                    }
                }
                else
                {
                    temp = temp->next;
                }
            }
            //If no place exists in list except at the very end of it:
            if (allocatedMemory == false)
            {
                //Inserting an item after currently last item. We have to make sure we're not attempting to allocate past the blobs end:
                //Function to look up current blob:
                int currentBlob = ma_returnCurrentBlob(myHandler, temp);
                //Controlling place in current blob:
                if (((char *)myHandler->blobArray[currentBlob] + myHandler->bytesInAGivenArray[currentBlob] - 1) - (char *)temp->endAddress >= memSize)
                {
                    //Place exists:
                    struct listItem *item = ma_createItem(myHandler, temp, NULL, memSize, IDENT, THID, (char *)temp->endAddress + 1, (char *)temp->endAddress + memSize);
                    temp->next = item;
                    ma_reLink(myHandler, temp);
                    allocatedMemory = true;
                    char *startAddress = (char *)item->startingAddress;
                    pthread_mutex_unlock(&mutex);
                    return startAddress;
                }
                else
                {
                    //No place exists, are there other blobs with place?
                    if (myHandler->blobArray[currentBlob + 1])
                    {
                        bool found = false;
                        for (int i = currentBlob + 1; i < myHandler->nrOfMemoryBlobsPointedTo && found == false; i++)
                        {
                            if (((char *)myHandler->blobArray[i] + myHandler->bytesInAGivenArray[i]) - (char *)myHandler->blobArray[i] >= memSize)
                            {
                                //Place exists:
                                struct listItem *item = ma_createItem(myHandler, temp, NULL, memSize, IDENT, THID, (char *)myHandler->blobArray[i], (char *)myHandler->blobArray[i] + memSize - 1);
                                temp->next = item;
                                ma_reLink(myHandler, temp);
                                allocatedMemory = true;
                                char *startAddress = (char *)item->startingAddress;
                                pthread_mutex_unlock(&mutex);
                                return startAddress;
                            }
                        }
                    }
                }
            }
        }
        //No items exists yet in the list - this will be the first one:
        else
        {
            //Is there enough place in the blob?
            bool blobSpaceFound = false;
            for (int i = 0; i < myHandler->nrOfMemoryBlobsPointedTo && blobSpaceFound == false; i++)
            {
                if ((((char *)myHandler->blobArray[i] + myHandler->bytesInAGivenArray[i]) - (char *)myHandler->blobArray[i]) >= memSize)
                {
                    blobSpaceFound = true;
                    myHandler->basePointer = myHandler->blobArray[i];
                }
            }
            //Place found in blob number i.
            if (blobSpaceFound == true)
            {
                struct listItem *item = ma_createItem(myHandler, NULL, NULL, memSize, IDENT, THID, (char *)myHandler->basePointer, (char *)myHandler->basePointer + (memSize - 1));
                myHandler->first = item;
                allocatedMemory = true;
                //Returning basePointer to original position, before returning:
                myHandler->basePointer = myHandler->blobArray[0];
                char *startAddress = (char *)item->startingAddress;
                pthread_mutex_unlock(&mutex);
                return startAddress;
            }
            //If a place is not found return NUL will happen at the end of the function.
        }
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int ma_release(memHandler *myHandler, uint64_t IDENT, pthread_t THID)
{
    pthread_mutex_lock(&mutex);
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
                free(myHandler->first);
                myHandler->first = NULL;
            }
            else if (temp->next != NULL && tracer != NULL)
            {
                //Listitem in between 2 other items.
                tracer->next = temp->next;
                tracer->next->previous = tracer;
                //temp->next->previous = tracer->next; //TBR
                myHandler->availableMemory += temp->allocationSize;
                free(temp);
                temp = NULL;
            }
            else if (temp->next != NULL && tracer == NULL)
            {
                //Removing first, has more items in list
                tracer = temp;
                temp = temp->next;
                temp->previous = NULL;
                myHandler->availableMemory += tracer->allocationSize;
                free(tracer);
                tracer = NULL;
                myHandler->first = temp;
                myHandler->first->previous = NULL;
            }
            else if (temp->next == NULL && tracer != NULL)
            {
                //item is last in list, more items before it
                tracer->next = NULL;
                myHandler->availableMemory += temp->allocationSize;
                free(temp);
                temp = NULL;
            }
            myHandler->allocatedItems--;
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        else
        {
            pthread_mutex_unlock(&mutex);
            return -1;
        }
    }
    else
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }
}

char *ma_increase(memHandler *myHandler, int *memSize, uint64_t IDENT, pthread_t THID)
{
    pthread_mutex_lock(&mutex);
    if (ma_availible(myHandler) >= *memSize && myHandler->allocatedItems > 0)
    {
        struct listItem *temp = myHandler->first;
        struct listItem *pointerToCorrectItem = NULL;
        bool found = false;
        if (temp)
        {
            if (temp->IDENT == IDENT && temp->THID == THID)
            {
                found = true;
                pointerToCorrectItem = temp;
            }
            while (temp->next != NULL && found == false)
            {
                if (temp->IDENT == IDENT && temp->THID == THID)
                {
                    found = true;
                    pointerToCorrectItem = temp;
                }
                temp = temp->next;
            }
        }
        //If item exists we'll search for place:
        if (found == true)
        {
            //Making sure no memory exists inbetween pointerToCOrrectItems and next item/end of blob equivalent or greater to *memSize, because if it do we can simply increase the addresses:
            bool searchforAvailableMemory = true;
            if (pointerToCorrectItem->next != NULL)
            {
                char *verified = ma_assertSameBlob(myHandler, pointerToCorrectItem, pointerToCorrectItem->next);
                if (verified)
                {
                    if ((char *)pointerToCorrectItem->next->startingAddress - (char *)pointerToCorrectItem->endAddress - 1 >= *memSize)
                    {
                        pointerToCorrectItem->endAddress += *memSize;
                        myHandler->availableMemory -= *memSize;
                        searchforAvailableMemory = false;
                    }
                }
            }
            else
            {
                int currentBlob = ma_returnCurrentBlob(myHandler, pointerToCorrectItem);
                if (((char *)myHandler->blobArray[currentBlob] + myHandler->bytesInAGivenArray[currentBlob] - 1) - (char *)pointerToCorrectItem->endAddress >= *memSize)
                {
                    pointerToCorrectItem->endAddress += *memSize;
                    myHandler->availableMemory -= *memSize;
                    searchforAvailableMemory = false;
                }
            }
            if (searchforAvailableMemory == true)
            {
                char *verified = ma_allocate(myHandler, (pointerToCorrectItem->allocationSize + *memSize), pointerToCorrectItem->IDENT, pointerToCorrectItem->THID);
                if (verified != NULL)
                {
                    //We've found a spot but we have now two items with same IDENT and THID; we need to remove the first one, which has since switched start/end-address.
                    if (pointerToCorrectItem->next == NULL)
                    {
                        pointerToCorrectItem->previous->next = NULL;
                    }
                    else if (pointerToCorrectItem->next != NULL && pointerToCorrectItem->previous != NULL)
                    {
                        pointerToCorrectItem->previous->next = pointerToCorrectItem->next;
                        pointerToCorrectItem->next->previous = pointerToCorrectItem->previous;
                    }
                    else
                    {
                        myHandler->first = pointerToCorrectItem->next;
                        myHandler->first->previous = NULL;
                    }
                    myHandler->allocatedItems--;
                    myHandler->availableMemory += pointerToCorrectItem->allocationSize;
                    free(pointerToCorrectItem);
                    pthread_mutex_unlock(&mutex);
                    return verified;
                }
            }
        }
    }
    pthread_mutex_unlock(&mutex);
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
    pthread_mutex_lock(&mutex);
    if (memory_size > 0)
    {
        myHandler->nrOfMemoryBlobsPointedTo++;
        char **blobArrayTemporary = (char **)realloc(myHandler->blobArray, sizeof(myHandler->blobArray) * myHandler->nrOfMemoryBlobsPointedTo);
        if (blobArrayTemporary)
        {
            myHandler->blobArray = blobArrayTemporary;
        }
        else
        {
            fprintf(stderr, "Unable to reallocate memory.");
            pthread_mutex_unlock(&mutex);
            return -1;
        }

        myHandler->blobArray[myHandler->nrOfMemoryBlobsPointedTo - 1] = (char *)malloc(memory_size);
        int *bytesInAGivenArrayTemporary = (int *)realloc(myHandler->bytesInAGivenArray, sizeof(myHandler->bytesInAGivenArray) * myHandler->nrOfMemoryBlobsPointedTo);
        if (bytesInAGivenArrayTemporary)
        {
            myHandler->bytesInAGivenArray = bytesInAGivenArrayTemporary;
            myHandler->bytesInAGivenArray[myHandler->nrOfMemoryBlobsPointedTo - 1] = memory_size;
        }
        else
        {
            fprintf(stderr, "Unable to reallocate memory.");
            pthread_mutex_unlock(&mutex);
            return -1;
        }
        myHandler->allocatedSize += memory_size;
        myHandler->availableMemory += memory_size;
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    pthread_mutex_unlock(&mutex);
    return -1;
}

int ma_showAllocations(memHandler *myHandler, char *OUTPUT)
{
    if (myHandler->first != NULL)
    {
        OUTPUT[0] = '\0';
        struct listItem *temp = myHandler->first;
        char tempArr[200];

        if (temp)
        {
            while (temp->next != NULL)
            {
                sprintf(tempArr, "%p -- %p : %d : %ld\n", temp->startingAddress, temp->endAddress, (unsigned int)temp->IDENT, temp->THID);
                strcat(OUTPUT, tempArr);
                temp = temp->next;
            }
            sprintf(tempArr, "%p -- %p : %d : %ld\n", temp->startingAddress, temp->endAddress, (unsigned int)temp->IDENT, temp->THID);
            strcat(OUTPUT, tempArr);
            printf("%s", OUTPUT);
            return 0;
        }
    }
    return -1;
}

int ma_terminate(memHandler *myHandler)
{
    if (myHandler != NULL)
    {
        if (myHandler->allocatedItems > 0)
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
                    tracer = NULL;
                    myHandler->allocatedItems--;
                }
                free(temp);
                temp = NULL;
                myHandler->allocatedItems--;
            }
        }
        for (int i = 0; i < myHandler->nrOfMemoryBlobsPointedTo; i++)
        {
            free(myHandler->blobArray[i]);
            myHandler->blobArray[i] = NULL;
        }
        free(myHandler->blobArray);
        myHandler->blobArray = NULL;
        free(myHandler->bytesInAGivenArray);
        myHandler->bytesInAGivenArray = NULL;
        free(myHandler);
        myHandler = NULL;
        return 0;
    }
    else
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }
}

char *ma_assertSameBlob(memHandler *myHandler, struct listItem *item1, struct listItem *item2)
{
    bool asserted = false;
    for (int i = 0; i < myHandler->nrOfMemoryBlobsPointedTo && asserted == false; i++)
    {
        if ((char *)item1->endAddress >= myHandler->blobArray[i] && (char *)item1->endAddress <= (myHandler->blobArray[i] + myHandler->bytesInAGivenArray[i] - 1) && (char *)item2->startingAddress >= myHandler->blobArray[i] && (char *)item2->startingAddress <= (myHandler->blobArray[i] + myHandler->bytesInAGivenArray[i] - 1))
        {
            asserted = true;
        }
    }
    if (asserted == true)
    {
        return (char *)item1->endAddress + 1;
    }
    else
    {
        return NULL;
    }
}

int ma_returnCurrentBlob(memHandler *myHandler, struct listItem *item)
{
    if (myHandler && item)
    {
        for (int i = 0; i < myHandler->nrOfMemoryBlobsPointedTo; i++)
        {
            if ((char *)item->startingAddress >= myHandler->blobArray[i] && (char *)item->endAddress <= (myHandler->blobArray[i] + myHandler->bytesInAGivenArray[i] - 1))
            {
                return i;
            }
        }
    }
    return -1;
}

void ma_reLink(memHandler *myHandler, struct listItem *item)
{
    bool initiateRelink = false;
    if (myHandler->allocatedItems > 1)
    {
        initiateRelink = true;
    }
    if (initiateRelink == true && myHandler->allocatedItems > 2 && (char *)item->startingAddress > (char *)item->next->startingAddress)
    {
        item->previous->next = item->next;
        item->next->previous = item->previous->next;
        item->next->next = item;
        item->previous = item->next->next;
    }
    else if (initiateRelink == true && myHandler->allocatedItems == 2 && (char *)item->startingAddress > (char *)item->next->startingAddress)
    {
        myHandler->first = item->next;
        myHandler->first->previous = NULL;
        myHandler->first->next = item;
        item->previous = myHandler->first;
        item->next = NULL;
    }
}

struct listItem *ma_createItem(memHandler *myHandler, struct listItem *previous, struct listItem *next, int allocationSize, uint64_t IDENT, pthread_t THID, char *startingAddress, char *endAddress)
{
    struct listItem *item = (struct listItem *)malloc(sizeof(struct listItem));
    if (item)
    {
        item->previous = previous;
        item->next = next;
        item->allocationSize = allocationSize;
        item->IDENT = IDENT;
        item->THID = THID;
        item->startingAddress = startingAddress;
        item->endAddress = endAddress;
        myHandler->allocatedItems++;
        myHandler->availableMemory -= allocationSize;
        return item;
    }
    return NULL;
}

void ma_clear(char *addressPointer, int memorySize)
{
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < memorySize; i++)
    {
        if (addressPointer)
        {
            *addressPointer = 0;
        }
        addressPointer++;
    }
    pthread_mutex_unlock(&mutex);

}

double getVersion(void)
{
    return (double)(major_version + (minor_version * 0.1));
}