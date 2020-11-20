
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct memoryBlock{
    size_t size;
    struct memoryBlock *Next;
    struct memoryBlock *Previous;
    bool Flag;
} memoryBlock_t;

memoryBlock_t *First = NULL;


void removeFromList(memoryBlock_t *Block) {
    printf("REmove");
    Block->Flag = true;
    if (Block->Previous != NULL) //If block has a previous block
    {
        Block->Previous->Next = Block->Next;
        if (Block->Next != NULL) //If block has next
        {
            Block->Next->Previous = Block->Previous;
        }
    } else if (Block->Previous == NULL) //If block does not have previous
    {
        if (Block->Next != NULL) //If block has previous
        {
            First = Block->Next;
            Block->Next->Previous = Block->Previous;
        } else if (Block->Next == NULL) {
            First = NULL;

        }
    }
}

void addToList(memoryBlock_t *Block)
{
    printf("Add");
    Block->Next = NULL;
    Block->Previous = NULL;
    Block->Flag = false;
    if(First == NULL)
    {
        Block->Next = First;
        First = Block;
    }
    else if (First != NULL)
    {
        memoryBlock_t *currentBlock;
        currentBlock = First;
        while(currentBlock->Next != NULL)
        {
            currentBlock = currentBlock->Next;
        }
        currentBlock->Next = Block;
        Block->Previous = currentBlock;
    }
}

void merge()
{}

memoryBlock_t *splitBlock(memoryBlock_t *Block, size_t size)
{
    printf("Split");
    memoryBlock_t *newBlock = (void*)((void*) Block + size + sizeof(memoryBlock_t));
    newBlock->size = (Block->size)- sizeof(memoryBlock_t) - size;
    Block->size = size;
    return newBlock;

}

void *my_malloc(size_t size)
{
    memoryBlock_t *currentBlock;
    memoryBlock_t *newBlock;
    currentBlock = First;
    while(currentBlock != NULL) {
        printf("I like you");
        if (currentBlock->Flag == false) {
            if (currentBlock->size + sizeof(memoryBlock_t) == size) //If perfect size
            {
                removeFromList(currentBlock);
                return ((void *) ((long) currentBlock + sizeof(memoryBlock_t)));
            } else if (currentBlock->size + sizeof(memoryBlock_t) > size) {
                newBlock = splitBlock(currentBlock, size);
                addToList(newBlock);
                return ((void *) ((long) currentBlock + sizeof(memoryBlock_t)));
            }

        }
        currentBlock = currentBlock->Next;
    }
    printf("Howdy");
    currentBlock = sbrk(8194);
      currentBlock->size = 8194 - sizeof(memoryBlock_t);
        currentBlock->Previous = NULL;
        currentBlock->Next = NULL;
        currentBlock-> Flag = true;
        if(size + sizeof(memoryBlock_t) < 8194)
        {
            newBlock = splitBlock(currentBlock, size);
            addToList(newBlock);
        }
        return ((void *)((long)currentBlock + sizeof(memoryBlock_t)));
}

void my_free(void *ptr)
{}

int main()
{
    my_malloc(3000);
    my_malloc(9000);


    return 1;
}