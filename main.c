
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

void splitBlock(memoryBlock_t *Block, size_t size)
{
    memoryBlock_t *newBlock = ((void*) Block + size + sizeof(memoryBlock_t)); //Right -> New free
    newBlock->size = (Block->size)- sizeof(memoryBlock_t) - size;
    newBlock->Flag = 0;
    newBlock->Previous = Block;
    Block->size = size;
    Block->Flag = 1;
    Block->Next = newBlock;

}
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

void *init(size_t size)
{
    memoryBlock_t *firstBlock;
    firstBlock = sbrk(8194);
    firstBlock->size = 8194 - sizeof(memoryBlock_t);
    firstBlock->Previous = NULL;
    firstBlock->Next = NULL;
    firstBlock->Flag = 0;
    splitBlock(firstBlock, size);
    return firstBlock;
}

void *my_malloc(size_t size)
{
    if (First == NULL)
    {
        First = init(size);
        return ((void *) ((long) First + sizeof(memoryBlock_t)));
    }
    memoryBlock_t *currentBlock;
    memoryBlock_t *Latest;
    currentBlock = First;
    while(currentBlock != NULL) {
        if (currentBlock->Flag == 0) {
            if (currentBlock->size + sizeof(memoryBlock_t) == size) //If perfect size
            {
                removeFromList(currentBlock);
                return ((void *) ((long) currentBlock + sizeof(memoryBlock_t)));
            } else if (currentBlock->size + sizeof(memoryBlock_t) > size) {
                splitBlock(currentBlock, size);
                return ((void *) ((long) currentBlock + sizeof(memoryBlock_t)));
            }
        }
        Latest = currentBlock;
        currentBlock = currentBlock->Next;
    }
    currentBlock = sbrk(8194);
    Latest->Previous->Next = currentBlock;
    currentBlock->size = 8194 + Latest->size;
    currentBlock->Previous = Latest->Previous;
    currentBlock->Next = NULL;
    if(size + sizeof(memoryBlock_t) < 8194)
    {
        splitBlock(currentBlock, size);
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
    my_malloc(5000);
    my_malloc(8192);
    debugPrint();


    return 1;
}