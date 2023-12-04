#ifndef CPU_H
#define CPU_H
#define WORD_SIZE 4

typedef struct
{
    char R[WORD_SIZE];
    char IR[WORD_SIZE];
    unsigned int IC;
    unsigned int TTC;
    unsigned int LLC;
    unsigned int SI;
    unsigned int PI;
    unsigned int TI;
    unsigned int PTR;
    unsigned int PTBR;
    unsigned int RA;
    unsigned int VA;
    char C;
}CPU;

#endif