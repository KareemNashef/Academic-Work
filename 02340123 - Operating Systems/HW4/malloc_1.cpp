#include <unistd.h>
#define MaxSize 100000000

void* smalloc(size_t size) {
    // Checking the input
    if (size == 0 || size > MaxSize)
        return NULL;

    // Allocating the memory using sbrk & checking the return value
    void* returnValue = sbrk(size);
    return ((returnValue == (void*)-1) ? NULL : returnValue);
}