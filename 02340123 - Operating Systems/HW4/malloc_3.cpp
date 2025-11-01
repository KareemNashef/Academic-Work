#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <cstdlib>
#define MaxSize 100000000
#define LargeSize 131072
#define SplitSize 128

enum Case {Case_A, Case_B_1, Case_B_2, Case_C, Case_D, Case_E, Case_F_1, Case_F_2, Case_G_H};

#include <stdio.h>

// ===== Metadata struct & Global list class =====

typedef struct MallocMetadata {
    int cookie;
    size_t size;
    bool is_free;
    MallocMetadata* next;
    MallocMetadata* prev;
} Metadata;

Case CaseChecker(Metadata* currentBlock, size_t inSize)
{
    // Case checker for reallocation
    Metadata* previousBlock = currentBlock->prev;
    Metadata* nextBlock = currentBlock->next;
    
    if(inSize <= currentBlock->size){
        return Case_A;
    }

    if(previousBlock){
        if(previousBlock->is_free && ((previousBlock->size + sizeof(Metadata) + currentBlock->size) >= inSize))
            return Case_B_1;
        else if (previousBlock->is_free && nextBlock == NULL)
            return Case_B_2;
    }
    
    if(nextBlock == NULL){
        return Case_C;
    }
    
    if(nextBlock){
        if(nextBlock->is_free && ((currentBlock->size + sizeof(Metadata) + nextBlock->size) >= inSize))
            return Case_D;
    }

    if(previousBlock && nextBlock){
        if(previousBlock->is_free && nextBlock->is_free && ((previousBlock->size + sizeof(Metadata) + currentBlock->size + sizeof(Metadata) + nextBlock->size) >= inSize))
            return Case_E;
        else if(previousBlock->is_free && nextBlock->is_free && nextBlock->next == NULL)
            return Case_F_1;
    }
    
    if(nextBlock){
        if(nextBlock->is_free && nextBlock->next == NULL)
            return Case_F_2;
    }
    
    return Case_G_H;
}

class SList {
    public:
    Metadata* SListHeadSmall;
    Metadata* SListHeadLarge;
    int mainCookie;
    
    SList() :
        SListHeadSmall(NULL),
        SListHeadLarge(NULL),
        mainCookie(rand())
    {}
    void* InsertSmallBlock(size_t inSize);
    void* InsertLargeBlock(size_t inSize);
    void* ReallocateSmallBlock(void* inBlock, size_t inSize);
    void* ReallocateLargeBlock(void* inBlock, size_t inSize);
    void FreeBlock(void* inBlock);

    bool Tampered();
};

void* SList::InsertSmallBlock(size_t inSize) 
{
    // Checking for tampered cookies
    if(Tampered())
        exit(0xdeadbeef);

    // Trying to find a block with a size that is the tightest fit to the size of the new data otherwise we allocate a new block
    Metadata* iter = SListHeadSmall;
    Metadata* current = NULL;
    size_t freeSpace = MaxSize;

    // Going over all of the blocks and getting the one that fits the best
    while (iter) {
        if (iter->is_free && (iter->size - inSize) >= 0 && (iter->size - inSize) < freeSpace){
            freeSpace = iter->size - inSize;
            current = iter;
        }
        iter = iter->next;
    }

    // If we found a block that satisfies the given restrictions we return it, Otherwise we proceed
    if (current != NULL) {
        // Checking if the free space is more than 128 bytes & splitting the block if so
        if (freeSpace >= (SplitSize + sizeof(Metadata))) {
            void* temp = ((char*)current + sizeof(Metadata) + inSize); // Segfault for some reason without this step
            Metadata* addedBlock = (Metadata*)temp;
            addedBlock->cookie = mainCookie;
            addedBlock->size = freeSpace - sizeof(Metadata);
            addedBlock->is_free = true;
            addedBlock->next = current->next;
            addedBlock->prev = current;
            current->next = addedBlock;
            current->size = current->size - freeSpace;
            if(current->next)
                current->next->prev = current;
        }

        current->is_free = false;
        return ((char*)current + sizeof(Metadata));
    }

    // Checking if a "wilderness" block exists
    iter = SListHeadSmall;
    if(iter){
        while (iter->next) {
        iter = iter->next;
        }
    
        if (iter->is_free) {
            // Calculating and allocating the needed extra memory
            sbrk(inSize - iter->size);
            iter->size = inSize;
            iter->is_free = false;
            return ((char*)iter + sizeof(Metadata));
        }
    }

    // Calculating the total memory needed and allocating it using sbrk
    size_t totalSize = inSize + sizeof(Metadata);
    void* newBlock = sbrk(totalSize);
    if (newBlock == (void*)-1)
        return NULL;

    // Adjusting the metadata the start of the newly allocated block
    Metadata* newMetadata = (Metadata*)newBlock;
    newMetadata->cookie = mainCookie;
    newMetadata->size = inSize;
    newMetadata->is_free = false;

    // Adding the metadata to the list
    if (SListHeadSmall == NULL) {
        newMetadata->next = NULL;
        newMetadata->prev = NULL;
        SListHeadSmall = newMetadata;
    }
    else {
        iter = SListHeadSmall;
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

void* SList::InsertLargeBlock(size_t inSize) 
{
    // Checking for tampered cookies
    if(Tampered())
        exit(0xdeadbeef);

    // Calculating the total memory needed and allocating it using mmap
    size_t totalSize = inSize + sizeof(Metadata);
    void* newBlock = mmap(NULL, totalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (newBlock == NULL)
        return NULL;

    // Adjusting the metadata the start of the newly allocated block
    Metadata* newMetadata = (Metadata*)newBlock;
    newMetadata->cookie = mainCookie;
    newMetadata->size = inSize;
    newMetadata->is_free = false;

    // Adding the metadata to the list
    if (SListHeadLarge == NULL) {
        newMetadata->next = NULL;
        newMetadata->prev = NULL;
        SListHeadLarge = newMetadata;
    }
    else {
        Metadata* iter = SListHeadLarge;
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

void* SList::ReallocateSmallBlock(void *inBlock, size_t inSize)
{
    // Checking for tampered cookies
    if(Tampered())
        exit(0xdeadbeef);

    // Getting the metadata for the block
    Metadata* toReallocate = (Metadata*)((char*)inBlock - sizeof(Metadata));
    Metadata* reallocatedBlock;

    // Checking the case and dealing with it
    switch (CaseChecker(toReallocate, inSize))
    {
        // A - Reusing the block without merging
        case Case_A :
            reallocatedBlock = toReallocate;
            break;
        
        // B.1 - Merging with the previous block
        case Case_B_1 :
            reallocatedBlock = toReallocate->prev;
            reallocatedBlock->size = reallocatedBlock->size + sizeof(Metadata) + toReallocate->size;
            reallocatedBlock->next = toReallocate->next;
            if (reallocatedBlock->next)
                reallocatedBlock->next->prev = reallocatedBlock;
            break;

        // B.2 - Merging with the previous block & expanding the current block
        case Case_B_2 :
            sbrk(inSize - (toReallocate->prev->size + sizeof(Metadata) + toReallocate->size));
            reallocatedBlock = toReallocate->prev;
            reallocatedBlock->size = inSize;
            reallocatedBlock->next = NULL;
            break;

        // C - Expanding the current block
        case Case_C :
            sbrk(inSize - (toReallocate->size));
            reallocatedBlock = toReallocate;
            reallocatedBlock->size = inSize;
            break;
            
        // D - Merging with the next block
        case Case_D :
            reallocatedBlock = toReallocate;
            reallocatedBlock->size = reallocatedBlock->size + sizeof(Metadata) + toReallocate->next->size;
            reallocatedBlock->next = toReallocate->next->next;
            if (reallocatedBlock->next)
                reallocatedBlock->next->prev = reallocatedBlock;
            break;

        // E - Merging with both the previous and next block   
        case Case_E :
            reallocatedBlock = toReallocate->prev;
            reallocatedBlock->size = toReallocate->prev->size + sizeof(Metadata) + toReallocate->size + sizeof(Metadata) + toReallocate->next->size;
            reallocatedBlock->next = toReallocate->next->next;
            if (reallocatedBlock->next)
                reallocatedBlock->next->prev = reallocatedBlock;
            break;

        // F.1 - Merging with both the previous and next block & expanding the next block
        case Case_F_1 :
            sbrk(inSize - (toReallocate->prev->size + sizeof(Metadata) + toReallocate->size + sizeof(Metadata) + toReallocate->next->size));
            reallocatedBlock = toReallocate->prev;
            reallocatedBlock->size = inSize;
            reallocatedBlock->next = NULL;
            break;

        // F.2 - Merging with the next block & expanding it
        case Case_F_2 :
            sbrk(inSize - (toReallocate->size + sizeof(Metadata) + toReallocate->next->size));
            reallocatedBlock = toReallocate;
            reallocatedBlock->size = inSize;
            reallocatedBlock->next = NULL;
            break;
            
        case Case_G_H :
            // G - Looking for a different block that can hold the new size
            Metadata* iter = SListHeadSmall;
            while(iter) {
                if (iter->is_free && inSize <= iter->size)
                    break;
                iter = iter->next;
            }
            if (iter) {
                reallocatedBlock = iter;
            }

            // H - Allocating a new block
            else {
                reallocatedBlock = (Metadata*)sbrk(inSize + sizeof(Metadata));
                reallocatedBlock->cookie = mainCookie;
                reallocatedBlock->size = inSize;
                
                iter = SListHeadSmall;
                while(iter->next) {
                    iter = iter->next;
                }

                reallocatedBlock->prev = iter;
                reallocatedBlock->next = NULL;
                iter->next = reallocatedBlock;
            }

            // Both these cases didn't use the original block so we set it to free
            toReallocate->is_free = true;
            break;
    }

    // Adjusting the free flag of the reallocated block
    reallocatedBlock->is_free = false;

    // Reallocating the data
    void* toReturn = memcpy(((char*)reallocatedBlock + sizeof(Metadata)), inBlock, inSize);
    
    // Splitting the block if needed
    size_t freeMemory = reallocatedBlock->size - inSize;
    if (freeMemory >= SplitSize + sizeof(Metadata)) {
        void* temp = ((char*)reallocatedBlock + sizeof(Metadata) + inSize); // Segfault for some reason without this step
        Metadata* addedBlock = (Metadata*)temp;
        addedBlock->cookie = mainCookie;
        addedBlock->size = freeMemory - sizeof(Metadata);
        addedBlock->is_free = true;
        addedBlock->next = reallocatedBlock->next;
        addedBlock->prev = reallocatedBlock;
        reallocatedBlock->next = addedBlock;
        reallocatedBlock->size = reallocatedBlock->size - (addedBlock->size + sizeof(Metadata));
        if(addedBlock->next) {
            addedBlock->next->prev = addedBlock;

            // Merging with the next block if needed
            if(addedBlock->next->is_free) {
                addedBlock->size += addedBlock->next->size + sizeof(Metadata);
                addedBlock->next = addedBlock->next->next;
                if(addedBlock->next) {
                    addedBlock->next->prev = addedBlock;
                }
            }
        }
    }
    
    return toReturn;
}

void* SList::ReallocateLargeBlock(void *inBlock, size_t inSize)
{
    // Checking for tampered cookies
    if(Tampered())
        exit(0xdeadbeef);

    // Getting the metadata for the block
    Metadata* toReallocate = (Metadata*)((char*)inBlock - sizeof(Metadata));
    
    // Attempting to reuse the block without merging
    if (inSize == toReallocate->size) {
        return inBlock;
    }

    // Allocating a new block and copying the data there
    void* newBlock = memcpy(InsertLargeBlock(inSize), inBlock, inSize);

    // Freeing the old block
    FreeBlock(inBlock);

    // Returning the new block
    return newBlock;
}

void SList::FreeBlock(void* inBlock)
{
    // Checking for tampered cookies
    if(Tampered())
        exit(0xdeadbeef);
        
    // Getting the metadata of the given block
    Metadata* toFree = (Metadata*)((char*)inBlock - sizeof(Metadata));

    // Checking if the block is alocated using mmap
    if (toFree->size >= LargeSize) {

        // Adjusting the block's neighbors
        if(toFree->prev)
            toFree->prev->next = toFree->next;
        else
            SListHeadLarge = toFree->next;
        if(toFree->next)
            toFree->next->prev = toFree->prev;
            
        munmap(inBlock, sizeof(Metadata) + toFree->size);
        return;
    }

    // Checking if the neighboring blocks exist and their is_free status
    if (toFree->prev && toFree->next) {                         // Both exist
        if (toFree->prev->is_free && toFree->next->is_free) {   // Both free
            // Merging with both blocks
            toFree->prev->size += 2 * sizeof(Metadata) + toFree->size + toFree->next->size;
            toFree->prev->next = toFree->next->next;
            if (toFree->prev->next)
                toFree->prev->next->prev = toFree->prev;
            return;
        }
    }
    if (toFree->prev) {                         // Previous exists
        if (toFree->prev->is_free) {            // Previous free
            // Merging with the previous block
            toFree->prev->size += sizeof(Metadata) + toFree->size;
            toFree->prev->next = toFree->next;
            if (toFree->prev->next)
                toFree->prev->next->prev = toFree->prev;
            return;
        }
    }
    if (toFree->next) {                         // Next exists
        if (toFree->next->is_free) {            // Next free
            // Merging with the next block
            toFree->size += sizeof(Metadata) + toFree->next->size;
            toFree->next = toFree->next->next;
            if (toFree->next)
                toFree->next->prev = toFree;
            toFree->is_free = true;
            return;
        }
    }

    // No merging & setting the is_free flag to true
    toFree->is_free = true;
}

bool SList::Tampered()
{
    // Going over every block in both lists and checking their cookie value
    Metadata* iter = SListHeadSmall;
    while(iter) {
        if(iter->cookie != mainCookie)
            return true;
        iter = iter->next;
    }
    iter = SListHeadLarge;
    while(iter) {
        if(iter->cookie != mainCookie)
            return true;
        iter = iter->next;
    }

    return false;
}

SList GlobalList = SList();

// ===== Allocation functions =====

void* smalloc(size_t size) 
{
    // Checking the input
    if (size == 0 || size > MaxSize)
        return NULL;

    // Adding the block to the global list and returning the outcome
    return (size >= LargeSize ? GlobalList.InsertLargeBlock(size) : GlobalList.InsertSmallBlock(size));
}

void* scalloc(size_t num, size_t size) 
{
    // Checking the input
    if (size == 0 || num == 0 || (size * num) > MaxSize)
        return NULL;

    // Adding the block to the global list and checking the outcome
    void* returnValue = smalloc(size * num);
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
    if(oldp == NULL)
        return smalloc(size);
    else
        return (size >= LargeSize ? GlobalList.ReallocateLargeBlock(oldp, size) : GlobalList.ReallocateSmallBlock(oldp, size));
}

// ===== Stats methods =====

size_t _num_free_blocks()
{
    // Going over the whole list and returning the wanted data
    size_t total = 0;
    Metadata* iter = GlobalList.SListHeadSmall;
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
    Metadata* iter = GlobalList.SListHeadSmall;
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
    Metadata* iter = GlobalList.SListHeadSmall;
    while(iter) {
        total++;
        iter = iter->next;
    }
    iter = GlobalList.SListHeadLarge;
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
    Metadata* iter = GlobalList.SListHeadSmall;
    while(iter) {
        total += iter->size;
        iter = iter->next;
    }
    iter = GlobalList.SListHeadLarge;
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
