#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VM.h"
void INIT(VM *vm) // Resets the VM
{
    memset(vm->M, 0, sizeof(vm->M));
    memset(vm->IR, 0, sizeof(vm->IR));
    memset(vm->R, 0, sizeof(vm->R));
    vm->IC = 0;
    vm->C = 0;
    vm->SI = 0;
}

void printMemory(VM *vm) //Prints memory for easy debugging.
{
    puts("===MEM ORY===");
    for (char i = 0; i < 100; ++i)
    {
        printf("%2u", i);
        putchar('|');
        putchar(' ');
        for (char j = 0; j < 4; ++j)
        {
            if (vm->M[i][j] == 0)
            {
                printf("0 ");
            }
            else if (vm->M[i][j] == '\n')
            {
                printf("\\ ");
            }
            else
            {
                printf("%c ", vm->M[i][j]);
            }
        }
        putchar('|');
        putchar('\n');
    }
    puts("=====END=====");
}


void FILEprintMemory(FILE *memoryFile, VM *vm) //Dumps memory contents into a file in a structured manner for easy debugging.
{
    fputs("===  I R  ===\n", memoryFile);
    putc('|', memoryFile);
    putc(' ', memoryFile);
    for (char i = 0; i < sizeof(vm->IR); i++)
    {
        if (vm->IR[i] == 0)
        {
            fprintf(memoryFile, "0 ");
        }
        else
        {
            fprintf(memoryFile, "%c ", vm->IR[i]);
        }
    }
    putc('|', memoryFile);
    putc('\n', memoryFile);
    fputs("====END IR===\n\n", memoryFile);
    fputs("===  R R  ===\n", memoryFile);
    putc('|', memoryFile);
    putc(' ', memoryFile);
    for (char i = 0; i < sizeof(vm->R); i++)
    {
        if (vm->R[i] == 0)
        {
            fprintf(memoryFile, "0 ");
        }
        else
        {
            fprintf(memoryFile, "%c ", vm->R[i]);
        }
    }
    putc('|', memoryFile);
    putc('\n', memoryFile);
    fputs("====END RR===\n\n", memoryFile);
    fprintf(memoryFile,"IC - %hd\n\n",vm->IC);
    fprintf(memoryFile,"C - %c\n\n",vm->C);
    fputs("===MEM ORY===\n", memoryFile);
    for (char i = 0; i < 100; ++i)
    {
        fprintf(memoryFile, "%2u", i);
        putc('|', memoryFile);
        putc(' ', memoryFile);
        for (char j = 0; j < 4; ++j)
        {
            if (vm->M[i][j] == 0)
            {
                fprintf(memoryFile, "0 ");
            }
            else if (vm->M[i][j] == '\n')
            {
                fprintf(memoryFile, "\\ ");
            }
            else
            {
                fprintf(memoryFile, "%c ", vm->M[i][j]);
            }
        }
        putc('|', memoryFile);
        putc('\n', memoryFile);
    }
    fputs("====ENDMM===\n", memoryFile);
    putc('\n', memoryFile);
    putc('\n', memoryFile);
}

VM *BOOT(FILE *input, FILE *output) // NOT IN SPECS - Returns a new instance of the Virtual Machine.
{
    VM *vm = malloc(sizeof(VM));
    INIT(vm);
    vm->input = input;
    vm->output = output;
    return vm;
}