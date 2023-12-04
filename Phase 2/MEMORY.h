#ifndef MEMORY_H
#define MEMORY_H

#define WORD_NUM 300
#define TOTAL_FRAMES 30
#define WORD_SIZE 4

typedef struct
{
    char M[WORD_NUM][WORD_SIZE];
    char allocatedFrames[TOTAL_FRAMES];
}MEMCON;

#endif