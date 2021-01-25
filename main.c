
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

//Author: Mateusz Wozakowski
// SCC211 Assignment

//Struct which creates node containing metadata of the block
typedef struct memoryBlock{
    size_t size; //Size of block
    struct memoryBlock *Next; //Pointer to next block in list
    struct memoryBlock *Previous; //Pointer to previous block in list
    int Flag; // 1 = taken, 0 = free
} memoryBlock_t;

memoryBlock_t *First = NULL; //Head of list


//Splits block into free and used sections and adjusts pointers/metadata
void splitBlock(memoryBlock_t *Block, size_t size)
{
    int x = Block->size - sizeof(memoryBlock_t) - size;
    if(x > 0)
    {
        memoryBlock_t *newBlock = ((void*) Block + size + sizeof(memoryBlock_t)); //Right -> New free block
        newBlock->size = (Block->size)- sizeof(memoryBlock_t) - size;
        newBlock->Flag = 0;
        newBlock->Next = Block->Next;
        newBlock->Previous = Block;
        Block->size = size;
        Block->Flag = 1;
        Block->Next = newBlock;
    }
}

//Used for debugging and demo
void debugPrint(){
    memoryBlock_t *temp = First;
    int i = 0;
    while(temp != NULL){
        printf("%d - %p | previous - %p | next - %p | size - %zu | flag - %d ", i,temp,temp->Previous,temp->Next, temp->size, temp->Flag);
        unsigned char *debug = (unsigned char*)temp;
        for (int j = 0; j < temp->size + sizeof(memoryBlock_t); j++){
            printf("%02x",debug[j]);
        }
        temp = temp->Next;
        i++;
        printf("\n");
    }
}
//Initialise header block
void *init(size_t size)
{
    memoryBlock_t *firstBlock;
    size_t alloc_size;
    if(size > 8192)
    {
        firstBlock = mmap(NULL, size,
                          PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS,
                          0, 0);
        alloc_size = size;
    }
    else{
        firstBlock = sbrk(8192);
        alloc_size = 8192;
    }

    firstBlock->size = alloc_size - sizeof(memoryBlock_t);
    firstBlock->Previous = NULL;
    firstBlock->Next = NULL;
    firstBlock->Flag = 0;
    splitBlock(firstBlock, size);
    return firstBlock;
}
//Malloc implementation with best fit algorithm
void *my_malloc(size_t size)
{
    if (First == NULL) //If uninitialised
    {
        First = init(size);
        return ((void *) ((long) First + sizeof(memoryBlock_t)));
    }
    size_t alloc_size;
    if(size > 8192)
    {
        alloc_size = size + sizeof(memoryBlock_t);
    }
    else{
        alloc_size = 8192 + sizeof(memoryBlock_t);
    }
    memoryBlock_t *currentBlock;
    memoryBlock_t *smallest; //Smallest block for best fit
    int split = 0;
    memoryBlock_t *Latest;
    currentBlock = First;
    smallest = currentBlock;
    while(currentBlock != NULL) { //Iterates through list until null
        if (currentBlock->Flag == 0) {
            if (currentBlock->size + sizeof(memoryBlock_t) == size) //If perfect size
            {
               currentBlock->Flag = 1;
                return ((void *) ((long) currentBlock + sizeof(memoryBlock_t)));
            } else if (currentBlock->size > sizeof(memoryBlock_t)  + size) { //Free block is bigger than block user wants to add
                split = 1;
                if(currentBlock->size < smallest->size || smallest == First) //Changes smallest block for best fit
                {
                    smallest = currentBlock;
                }
            }
        }
        Latest = currentBlock; //Stores latest not-null block
        currentBlock = currentBlock->Next;  //Iterates
    }
    if(split == 1)
    {
            splitBlock(smallest, size);
            return ((void *)((long) smallest + sizeof(memoryBlock_t)));
    }
    //If no free block found on list, ask system for more memory, set all links.
    if(size > 8192)
    {
         currentBlock = (memoryBlock_t*) mmap(NULL, size,
                            PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS,
                            0, 0);
    }
    else{
        currentBlock = sbrk(8192);
    }
    Latest->Previous->Next = currentBlock;
    currentBlock->size = alloc_size + Latest->size;
    currentBlock->Previous = Latest->Previous;
    currentBlock->Next = NULL;
    if(size + sizeof(memoryBlock_t) <= alloc_size) //Split block if smaller than value allocated
    {
        splitBlock(currentBlock, size);
    }
    return ((void *)((long)currentBlock + sizeof(memoryBlock_t)));
}

//Free implementation, coalesces any free blocks next to each other
void my_free(void *ptr)
{
    memoryBlock_t *current = (void*)((long)ptr - sizeof(memoryBlock_t));
    current->Flag = 0;
    void *top = sbrk(0); //Gets break address

    //Validity checks and coalescing free regions:
    if(current >= First && (void*) current <= top)
    {
        if(current->Next != NULL && current->Next->Flag == 0)
        {
            current->size = current->size + current->Next->size + sizeof(memoryBlock_t);
            current->Next = current->Next->Next;
            if(current->Next != NULL && current->Next->Next != NULL)
            {
                current->Next->Next->Previous = current;
            }
        }
        if(current->Previous != NULL && current->Previous->Flag == 0)
        {
            current->Previous->Next = current->Next;
            current->Previous->size = current->Previous->size + current->size + sizeof(memoryBlock_t);
            current->Previous->Next = current->Next;
            if(current->Previous->Previous != NULL)
            {
                current->Previous->Previous->Next = current;
            }

        }

    }
    //Frees mmap'ed block
    if(current->size > 8192)
    {
        int err = munmap(current, current->size);
        if(err == -1){
            printf("Failed to free!\n");
            return;
        }
    }
}

//UI for testing code
_Noreturn void interface()
{
    char mOrF;
    size_t size;
    while(1)
    {
        printf("//");
        scanf("%c", &mOrF);
        if(mOrF == 'm')
        {
            printf("Enter size\n");
            scanf("%zd", &size);
            my_malloc(size);
            debugPrint();
        }
        else if (mOrF == 'f')
        {
            void *ptr;
            printf("Enter address\n");
            scanf("%p", &ptr);
            printf("%p\n", ptr);
            ptr = ptr + sizeof(memoryBlock_t);
            my_free(ptr);
            debugPrint();
        }
    }
}

int main()
{
    interface();
}