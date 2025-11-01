#include <unistd.h>
#include <string.h>
#define MaxSize 100000000

// ===== Metadata struct & Global list class =====

typedef struct MallocMetadata {
    size_t size;
    bool is_free;
    MallocMetadata* next;
    MallocMetadata* prev;
} Metadata;

class SList {
    public:
    Metadata* SListHead;
    
    SList() :
        SListHead(NULL)
    {}
    void* InsertBlock(size_t inSize);
    void* ReallocateBlock(void* inBlock, size_t inSize);
    void FreeBlock(void* inBlock);
};

void *SList::InsertBlock(size_t inSize)
{

    // Trying to find a block that is smaller or equal to the size of the new data otherwise we allocate a new block
    Metadata* iter = SListHead;
    while (iter) {
        if((inSize <= iter->size) && iter->is_free) {
            iter->is_free = false;
            return ((char*)iter + sizeof(Metadata));
        }
        iter = iter->next;
    }

    // Calculating the total memory needed and allocating it using sbrk
    size_t totalSize = inSize + sizeof(Metadata);
    void* newBlock = sbrk(totalSize);
    if (newBlock == (void*)-1)
        return NULL;

    // Adjusting the metadata the start of the newly allocated block
    Metadata* newMetadata = (Metadata*)newBlock;
    newMetadata->size = inSize;
    newMetadata->is_free = false;

    
    // Adding the metadata to the list
    if (SListHead == NULL) {
        newMetadata->next = NULL;
        newMetadata->prev = NULL;
        SListHead = newMetadata;
    }
    else {
        iter = SListHead;
        while (iter->next) {
            iter = iter->next;
        }
        newMetadata->next = NULL;
        newMetadata->prev = iter;
        iter->next = newMetadata;
    }

    // Returning the allocated block after adjusting for the metadata size
    return ((char*)newBlock + sizeof(Metadata));
}

void *SList::ReallocateBlock(void *inBlock, size_t inSize)
{
    // Getting the metadata for the block and checking if the current size is enough
    Metadata* toReallocate = (Metadata*)((char*)inBlock - sizeof(Metadata));
    if (inSize <= toReallocate->size)
        return inBlock;

    // Setting the metadata free flag to true since we're moving to a new block
    toReallocate->is_free = true;

    // Acquiring a new block of the new size
    void* newBlock = InsertBlock(inSize);
    
    // Reallocating the data
    return memcpy(newBlock, inBlock, inSize);
}

void SList::FreeBlock(void* inBlock)
{
    // Getting the metadata and setting the is_free flag to true
    Metadata* toFree = (Metadata*)((char*)inBlock - sizeof(Metadata));
    toFree->is_free = true;
}

SList GlobalList = SList();

// ===== Allocation functions =====

void* smalloc(size_t size) 
{
    // Checking the input
    if (size == 0 || size > MaxSize)
        return NULL;

    // Adding the block to the global list and returning the outcome
    return GlobalList.InsertBlock(size);
}

void* scalloc(size_t num, size_t size) 
{
    // Checking the input
    if (size == 0 || num == 0 || (size * num) > MaxSize)
        return NULL;

    // Adding the block to the global list and checking the outcome
    void* returnValue = GlobalList.InsertBlock(size * num);
    if (returnValue == NULL)
        return NULL;

    // Zeroing the newly allocated block
    return memset(returnValue, 0, size * num);
}

void sfree(void* p)
{
    // Checking the input
    if (p == NULL)
        return;

    // Freeing the block
    GlobalList.FreeBlock(p);
}

void* srealloc(void* oldp, size_t size) 
{
    // Checking the input
    if (size == 0 || size > MaxSize)
        return NULL;

    // Reallocating the data
    return (oldp == NULL ? smalloc(size) : GlobalList.ReallocateBlock(oldp, size));
}

// ===== Stats methods =====

size_t _num_free_blocks()
{
    // Going over the whole list and returning the wanted data
    size_t total = 0;
    Metadata* iter = GlobalList.SListHead;
    while(iter) {
        if(iter->is_free)
            total++;
        iter = iter->next;
    }
    return total;
}

size_t _num_free_bytes()
{
    // Going over the whole list and returning the wanted data
    size_t total = 0;
    Metadata* iter = GlobalList.SListHead;
    while(iter) {
        if(iter->is_free)
            total += iter->size;
        iter = iter->next;
    }
    return total;
}

size_t _num_allocated_blocks()
{
    // Going over the whole list and returning the wanted data
    size_t total = 0;
    Metadata* iter = GlobalList.SListHead;
    while(iter) {
        total++;
        iter = iter->next;
    }
    return total;
}

size_t _num_allocated_bytes()
{
    // Going over the whole list and returning the wanted data
    size_t total = 0;
    Metadata* iter = GlobalList.SListHead;
    while(iter) {
        total += iter->size;
        iter = iter->next;
    }
    return total;
}

size_t _num_meta_data_bytes()
{
    return (_num_allocated_blocks() * sizeof(Metadata));
}

size_t _size_meta_data()
{
    return sizeof(Metadata);
}
