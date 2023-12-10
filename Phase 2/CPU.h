#ifndef CPU_H
#define CPU_H
#define WORD_SIZE 4

typedef struct
{
    char R[WORD_SIZE]; //General Purpose register
    char IR[WORD_SIZE]; //Instruction Register
    unsigned int IC; //Instruction Counter
    unsigned int TTC; //Total Time Counter Can also be referred to as TLC Time Limit Counter
    unsigned int LLC; //Line Limit Counter
    unsigned int SI; //Supervisory Interrupt
    unsigned int PI; // Program Interrupt
    unsigned int TI; // Timer Interrupt
    unsigned int PTR; // Page Table Register
    unsigned int PTBR; // Page Table Base Register
    unsigned int RA; // Real Address (Address variables use to make function calls tidier)
    unsigned int VA; // Virtual Address
    char C;
}CPU;

#endif