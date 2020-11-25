
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct memoryBlock{
    size_t size;
    struct memoryBlock *Next;
    struct memoryBlock *Previous;
    int Flag; // 1 = taken, 0 = free
} memoryBlock_t;

memoryBlock_t *First = NULL;


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
    firstBlock = sbrk(8192);
    firstBlock->size = 8192 - sizeof(memoryBlock_t);
    firstBlock->Previous = NULL;
    firstBlock->Next = NULL;
    firstBlock->Flag = 0;
    splitBlock(firstBlock, size);
    return firstBlock;
}

void *my_malloc(size_t size)
{
    memoryBlock_t *Lists[8];

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
               currentBlock->Flag = 1;
                return ((void *) ((long) currentBlock + sizeof(memoryBlock_t)));
            } else if (currentBlock->size + sizeof(memoryBlock_t) > size) {
                splitBlock(currentBlock, size);
                return ((void *) ((long) currentBlock + sizeof(memoryBlock_t)));
            }
        }
        Latest = currentBlock;
        currentBlock = currentBlock->Next;
    }
    currentBlock = sbrk(8192);
    Latest->Previous->Next = currentBlock;
    currentBlock->size = 8192 + Latest->size;
    currentBlock->Previous = Latest->Previous;
    currentBlock->Next = NULL;
    if(size + sizeof(memoryBlock_t) < 8192)
    {
        splitBlock(currentBlock, size);
    }
    return ((void *)((long)currentBlock + sizeof(memoryBlock_t)));
}

void my_free(void *ptr)
{
    memoryBlock_t *current = (void*)((long)ptr - sizeof(memoryBlock_t));
    current->Flag = 0;
    void *top = sbrk(0);
    if(current >= First && (void*) current <= top)
    {
        if(current->Next->Flag == 0)
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
}

int main()
{
    char *ptr;
    int *A = my_malloc(40);
    int *B = my_malloc(60);
    int*C = my_malloc(100);
    my_free(B);
    ptr = my_malloc(92);
    debugPrint();


    return 1;
}