#ifndef __MEMHANDLER
#define __MEMHANDLER
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define major_version 0
#define minor_version 1

/* Entry for an memory allocation. 
Tracks starting address, size and end address.
Use THID to identify the thread that allocated the memory, and use
IDENT as an identifier with in the thread. I.e. A thread can allocate 
many blocks of memory, they are however not to be treated as one block. 
Each allocation is uniqely identified by the IDENT field. 
Use ma_getIdentifier() to obtain(generate) an uniqe value. The identifer 
is uniqe per handler, not per thread. If multiple handlers are used,
the same identifer can occur on different handlers, its the 
combination memHandler, THID and IDENT that uniqely identifies an memory
allocation. 
*/
struct listItem{
    uint64_t IDENT; 
    pthread_t THID;
    int allocationSize;
    void *startingAddress;
    void *endAddress;
    struct listItem *next;
};

/* handler where lib stores intermediate data
You are free add additional enties, but not remove. 
*/
typedef struct {
    int allocatedSize;
    int availableMemory;
    int nrOfMemoryBlobsPointedTo;
    char** blobArray; 
    int* bytesInAGivenArray;
    void *basePointer;
    struct listItem *first;
    int allocatedItems;  
    uint64_t unique;
} memHandler;

/* Initialized a memHandler object, allocate <memory_size> space too the 
handler to use. If <memory_size> is not available then return 0, otherwise 
and address to the populated memHandler struct. */
memHandler *ma_init(int memory_size);

/* Return a 64-bit uniqe identifier used to identify a 'customer' */
uint64_t ma_getIdentifier(memHandler *myHandler);

/* From the allocated memory, assign <memSize> to thread with id <THID> and 
customer <IDENT>. Return pointer to assigned memory. If not available, 
return 0. */
char *ma_allocate(memHandler *myHandler,int memSize,uint64_t IDENT, pthread_t 
THID);

/* Release the memory allocated to thread id <THID> and customer <IDENT>, 
return 0 on success, -1 on fail. */
int ma_release(memHandler *myHandler,uint64_t IDENT, pthread_t THID);

/* Increase the memory allocated to thread id <THID> and  customer <IDENT>, to 
a total of <memSize>. This may require the original allocation to be copied/mo-
ved to a new location. On success, return a pointer to the memory location, and 
set the memory size associated with <THID> and <IDENT> in the <memSize> variab-
le. If its not possible to increase the allocation, return the original memory 
location and set the <memSize> variable to the original size.  */
char *ma_increase(memHandler *myHandler, int *memSize,uint64_t IDENT, pthread_t 
THID);

/* Return availible memory, return the total amount of unallocated memory 
(bytes) */
int ma_availible(memHandler *myHandler);

/* Increase the availible memory to the memHandler by allocating another 'blob'
 of memory_size bytes. This may be in a totally different location in the 
 system.  */
int ma_addBlob(memHandler *myHandler,int memory_size);

/* Show Allocations, sprintf to OUTPUT a list of allocations;
startAddress -- endAddress : IDENT1 : THID
startAddress -- endAddress : IDENT2 : THID
....
startAddress -- endAddress : IDENTk : THID
*/
int ma_showAllocations(memHandler *myHandler, char *OUTPUT);

/* Free all the allocated memory and empty the handler */
int ma_terminate(memHandler *myHandler);

/* Return version, MAJOR.MINOR, as double */
double getVersion(void);
#endif