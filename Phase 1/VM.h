#ifndef VM_H //Include guard
#define VM_H
typedef struct VM //Define structure of the machine
{
    char M[100][4]; //Main memory
    char IR[4];//Instruction Register
    short IC;//Instruction Counter
    char R[4];//General Purpose Register
    char C;//Toggle Function
    char SI;//Supervisory interrupt
    FILE *input;//Input file (Cards)
    FILE *output;//Output file (Line printer)
} VM;

void INIT(VM *vm); // Resets the VM

void printMemory(VM *vm); //Prints memory for easy debugging.

void FILEprintMemory(FILE *memoryFile, VM *vm); //Dumps memory contents into a file in a structured manner for easy debugging.

VM *BOOT(FILE *input, FILE *output); // NOT IN SPECS - Returns a new instance of the Virtual Machine.

#endif