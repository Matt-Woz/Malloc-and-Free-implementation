
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct memoryBlock{
    size_t size;
    struct memoryBlock *Next;
    struct memoryBlock *Previous;
    int Flag; // 1 = taken, 0 = free
} memoryBlock_t;

memoryBlock_t *First = NULL;


void removeFromList(memoryBlock_t *Block) {
 //   printf("Remove");
    Block->Flag = 1;
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
//    printf("Add");
    Block->Next = NULL;
    Block->Previous = NULL;
    Block->Flag = 0;
    if(First == NULL)
    {
        Block->Next = First;
        First = Block;
    }
    else if (First != NULL)
    {
        memoryBlock_t *currentBlock;
        currentBlock = First;
        while(currentBlock->Next != NULL) //Infinite loop
        {
            currentBlock = currentBlock->Next;
        }
        currentBlock->Next = Block;
        Block->Previous = currentBlock;
    }
}

void merge()
{}

void splitBlock(memoryBlock_t *Block, size_t size)
{
   // printf("Split");
    memoryBlock_t *newBlock = ((void*) Block + size + sizeof(memoryBlock_t));
    newBlock->size = (Block->size)- sizeof(memoryBlock_t) - size;
    newBlock->Flag = 0;
    newBlock->Previous = Block;
    Block->size = size;
    Block->Flag = 1;
    Block->Next = newBlock;
   // return newBlock;

}
void debugPrint(){
    memoryBlock_t *temp = First;
    int i = 0;
    while(temp != NULL){
        printf("%d - %p | previous - %p | next - %p | size - %d | flag - %d ", i,temp,temp->Previous,temp->Next, temp->size, temp->Flag);
        unsigned char *debug = (unsigned char*)temp;
        for (int j = 0; j < temp->size + sizeof(memoryBlock_t); j++){
            printf("%02x",debug[j]);
        }
        temp = temp->Next;
        i++;
        printf("\n");
    }
}

void *my_malloc(size_t size)
{
    memoryBlock_t *currentBlock;
    memoryBlock_t *newBlock;
    currentBlock = First;
    while(currentBlock != NULL) {
      //  printf("I like you");
        if (currentBlock->Flag == 0) {
            if (currentBlock->size + sizeof(memoryBlock_t) == size) //If perfect size
            {
                removeFromList(currentBlock);
                return ((void *) ((long) currentBlock + sizeof(memoryBlock_t)));
            } else if (currentBlock->size + sizeof(memoryBlock_t) > size) {
                splitBlock(currentBlock, size);
                //addToList(newBlock);
                return ((void *) ((long) currentBlock + sizeof(memoryBlock_t)));
            }

        }
        currentBlock = currentBlock->Next;
    }
//    printf("Howdy");
    currentBlock = sbrk(8194);
    printf("SBRK\n");
      currentBlock->size = 8194 - sizeof(memoryBlock_t);
        currentBlock->Previous = NULL;
        currentBlock->Next = NULL;
        currentBlock-> Flag = 1;
        First = currentBlock;
        if(size + sizeof(memoryBlock_t) < 8194)
        {
            splitBlock(currentBlock, size);
           // newBlock = splitBlock(currentBlock, size);
          //  addToList(newBlock);
        }
        return ((void *)((long)currentBlock + sizeof(memoryBlock_t)));
}

void my_free(void *ptr)
{}

int main()
{
    my_malloc(22);
    my_malloc(10);
    my_malloc(2000);
    my_malloc(3000);
    debugPrint();


    return 1;
}