#include "CPU.h"
#include "MEMORY.h"
#include "PCB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct{
    CPU* cpu;
    MEMCON* memcon;
    PCB* pcb;
    int execFlag;
    FILE* input;
    FILE* output;
}VM;

void VM_INIT(VM* vm)
{
    memset(vm->cpu->IR,0,sizeof(vm->cpu->IR));
    memset(vm->cpu->R,0,sizeof(vm->cpu->R));
    vm->execFlag = 0;
    vm->cpu->IC = 0;
    vm->cpu->TTC = 0;
    vm->cpu->LLC = 0;
    vm->cpu->SI = 0;
    vm->cpu->PI = 0;
    vm->cpu->TI = 0;
    vm->cpu->PTR = 0;
    vm->cpu->PTBR = 0;
    vm->cpu->C = 0;
    vm->cpu->RA = 0;
    vm->cpu->VA = 0;
    memset(vm->memcon->M,0,sizeof(vm->memcon->M));
    memset(vm->memcon->allocatedFrames,0,sizeof(vm->memcon->allocatedFrames));
    vm->pcb->JID = 0;
    vm->pcb->TLL = 0;
    vm->pcb->TTL = 0;
    vm->execFlag = 0;
}

VM* VM_BOOT(FILE* input, FILE* output)
{
    VM* vm =(VM*) malloc(sizeof(VM));
    vm->cpu = malloc(sizeof(CPU));
    vm->memcon = malloc(sizeof(MEMCON));
    vm->pcb = malloc(sizeof(PCB));
    VM_INIT(vm);
    vm->input = input;
    vm->output = output;
    return vm;
}

void printState(FILE* stateFile, VM* vm)
{
    fputs("===C P U===\n", stateFile);
    fprintf_s(stateFile,"IC - %hd\n\n",vm->cpu->IC);
    fprintf_s(stateFile,"C - %c\n\n",((vm->cpu->C) == 0)? '0':(vm->cpu->C));
    fprintf_s(stateFile,"TTC - %hd\n\n",vm->cpu->TTC);
    fprintf_s(stateFile,"LLC - %hd\n\n",vm->cpu->LLC);
    fprintf_s(stateFile,"SI - %hd\n\n",vm->cpu->SI);
    fprintf_s(stateFile,"PI - %hd\n\n",vm->cpu->PI);
    fprintf_s(stateFile,"TI - %hd\n\n",vm->cpu->TI);
    fprintf_s(stateFile,"PTR - %hd\n\n",vm->cpu->PTR);
    fputs("===ENDCPU===\n", stateFile);
    putc('\n', stateFile);
    fputs("===P C B===\n", stateFile);
    fprintf_s(stateFile,"JID - %hd\n",vm->pcb->JID);
    fprintf_s(stateFile,"TTL - %hd\n",vm->pcb->TTL);
    fprintf_s(stateFile,"TLL - %hd\n",vm->pcb->TLL);
    fputs("===ENDPCB===\n\n", stateFile);
    fputs("===  I R  ===\n", stateFile);
    putc('|', stateFile);
    putc(' ', stateFile);
    for (int i = 0; i < sizeof(vm->cpu->IR); i++)
    {
        if (vm->cpu->IR[i] == 0)
        {
            fprintf_s(stateFile, "0 ");
        }
        else
        {
            fprintf_s(stateFile, "%c ", vm->cpu->IR[i]);
        }
    }
    putc('|', stateFile);
    putc('\n', stateFile);
    fputs("====END IR===\n\n", stateFile);
    fputs("===  R R  ===\n", stateFile);
    putc('|', stateFile);
    putc(' ', stateFile);
    for (int i = 0; i < sizeof(vm->cpu->R); i++)
    {
        if (vm->cpu->R[i] == 0)
        {
            fprintf_s(stateFile, "0 ");
        }
        else
        {
            fprintf_s(stateFile, "%c ", vm->cpu->R[i]);
        }
    }
    putc('|', stateFile);
    putc('\n', stateFile);
    fputs("====END RR===\n\n", stateFile);
    fputs("====MEMORY====\n", stateFile);
    for (int i = 0; i < 300; ++i)
    {
        fprintf_s(stateFile, "%3u", i);
        putc('|', stateFile);
        putc(' ', stateFile);
        for (int j = 0; j < 4; ++j)
        {
            if (vm->memcon->M[i][j] == 0)
            {
                fprintf_s(stateFile, "0 ");
            }
            else if (vm->memcon->M[i][j] == '\n')
            {
                fprintf_s(stateFile, "\\ ");
            }
            else
            {
                fprintf_s(stateFile, "%c ", vm->memcon->M[i][j]);
            }
        }
        putc('|', stateFile);
        putc('\n', stateFile);
    }
    fputs("===ENDMEMORY==\n\n", stateFile);
    putc('\n', stateFile);
    putc('\n', stateFile);
}