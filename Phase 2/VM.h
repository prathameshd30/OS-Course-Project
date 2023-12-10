#include "CPU.h"
#include "MEMORY.h"
#include "PCB.h"

typedef struct{
    CPU* cpu;
    MEMCON* memcon; //Memory Controller
    PCB* pcb;
    int execFlag; //Used to signify if VM is done executing
    FILE* input;
    FILE* output;
}VM;

void VM_INIT(VM* vm);//Resets the Virtual Machine

VM* VM_BOOT(FILE* input, FILE* output);//Returns instance of Virtual Machine

void printState(FILE* stateFile, VM* vm);//Prints the entire state of the virtual machine in a structured manner in a file