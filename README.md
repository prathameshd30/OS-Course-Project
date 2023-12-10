
# OS Course Project
This repository follows some coursework which documents the implementation of the Operating Systems Course Project.

The project involves simulating some basics of Operating Systems, particularly the early implementations involving card-based input.

The project is split into 2 phases, Phase 1 and Phase 2.

## Basic Structure
- Input programs, called jobs, are read from the input file, line by line, where each line simulates an input "card" like in some original computers.
- Output file contains the output of the programs as dictated by the simulated OS, simulating a line printer.
- Instruction takes the form of - AA00, where AA can be any instruction from the instruction set (given further) and 00 can be any memory address from 00 to 99.
- Each line in the input file denotes an input card. Each card can have a maximum of 40 characters.

## Types of cards
3 Types of cards are used for structuring job.

1. Control cards
    - $AMJ  (Beginning of Job) - Starts with $AMJ, followed 4 digit JID [1] , followed by 4 digit TTL [2], followed by 4 digit TLL [3].
    - $DTA  (Starts the data segment)
    - $END  (Signifies the end of the job) - Starts with $END, followed by 4 digit Job ID.

2. Instruction Cards - Contain instructions to execute (Maximum 10 instructions in one card)

3. Data Cards - Contain data on which the instructions can operate on.

## Generic Memory Model (Applicable for both Phase 1 and Phase 2)
- Each word consists of 4 bytes of memory.
- The machine/OS is word addressable (cannot address individual bytes).
- Memory is split into blocks of 10 contiguous addresses (Important for Blockwise instructions).

Phase 1 has a basic memory model where job is loaded contiguously, starting from address 00.

Phase 2 uses paging, using a page table and splitting the job and data into non-contiguous locations.

## Instruction Set
> **NOTE - The phrase "logical address" means 2 different things for the 2 different phases.**
>
> For Phase 1 - The logical address is just the 2 digit number supplied with the instruction. Essentially, there is no translation between the logical address and the physical/absolute address.
>
> *For example -* GD10  *- In this instruction, 10 would be the address directly used in the memory.*
>
>For Phase 2 - The logical address is the value obtained after referring to the page table, which gives the address where the actual memory resides. So, there needs to be a translation between logical address and the physical/absolute address.
>
>*For example -* GD10  *- In this instruction, 10 would be the logical address, which is used to refer to the page table, which further provides the address directly used in the memory.*

- GD - Get Data - Blockwise Instruction - Starts loading data from the input file from the starting of the block of the logical address.
- PD - Print Data - Blockwise Instruction - Prints memory to the output file from the starting of the block of the logical address.
- LR - Load Register - Loads memory at the logical address into a general purpose register (R).
- SR - Store/(Send) Register (content) into Memory - Sends the current contents of the register into the memory at the logical address.
- CR - Compare Register and Memory - Compares register content with the memory at the logical address.
- BT - Branch Toggle - Compares memory at logical address with the contents of the register.
- H  - Halt - Halts the execution of the job. **Should not be provided with any address.**

# Phase 1
**IMPORTANT - Phase 1 does not consider - JID, TTL, TLL i.e. it does not raise errors/interrupts at the violation of any of these constraints.**

Phase 1 has 100 words of memory, each of which consists of 4 bytes, thus it has 400 bytes of total memory.

Phase 1 contiguously reads program cards and data cards starting from the memory address 00.

Phase 1 does not have any translation between logical address and physical address.

Structure of the VM sufficient to implement Phase 1 is

    char M[100][4]; //Main memory
    char IR[4];//Instruction Register
    short IC;//Instruction Counter
    char R[4];//General Purpose Register
    char C;//Toggle Function
    char SI;//Supervisory interrupt
    FILE *input;//Input file (Cards)
    FILE *output;//Output file (Line printer)

Functions used in Phase 1
    
    void INIT(VM *vm) // Resets the VM
    void printMemory(VM *vm) //Prints memory for easy debugging.
    void FILEprintMemory(FILE *memoryFile, VM *vm) //Dumps memory contents into a file in a structured manner for easy debugging.
    VM *BOOT(FILE *input, FILE *output) // Returns a new instance of the Virtual Machine.
    void LOAD(VM* vm) //Loads the programs into main memory.
    void MOS_START_EXECUTION(VM * vm)//Just sets IC to 0, to comply with SPECS
    void EXECUTE_USER_PROGRAM(VM * vm)//Decides which action to take based on Instruction Register
    void MOS(VM * vm) //Use the Supervisory interrupt to execute needed interrupt handler.
    void READ(VM * vm) // Analog for GD instruction
    void WRITE(VM * vm)// Analog for PD instruction
    void TERMINATE(VM * vm) // Terminate jobs correctly
Rest of the functions are just the instructions given in the instruction set.

Compilation of Phase 1

    gcc -c VM.c
    gcc -o phase1(.exe) main.c VM.o

Please select .exe or other formats as per your platform.

# Phase 2
Phase 2 has 300 words of memory, each of 4 bytes, resulting in 1200 bytes of memory.

It uses the concept of paging, in which, program and data are stored at random locations, these random locations are recorded in the Page Table, which are used as a reference when executing the job.

Main Structure of VM

    CPU* cpu;
    MEMCON* memcon; //Memory Controller
    PCB* pcb;
    int execFlag; //Used to signify if VM is done executing
    FILE* input;
    FILE* output;


Functions used in Phase 2

    void VM_INIT(VM* vm);//Resets the Virtual Machine
    VM* VM_BOOT(FILE* input, FILE* output);//Returns instance of Virtual Machine
    void printState(FILE* stateFile, VM* vm);//Prints the entire state of the virtual machine in a structured manner in a file
    void LOAD(VM *vm); //Loads program into memory
    void PTR_INIT(VM *vm); //Initialises page table
    int ALLOCATE(VM *vm); // Gives unused random frame number
    void START_EXECUTION(VM *vm); // Starts execution by setting IC to 0
    void EXECUTE_USER_PROGRAM(VM *vm); //Decides which action to take based on instructions in IR
    int ADDRESS_MAP(VM *vm, int VA); //Translates Virtual Address to Real Address
    void MOS(VM *vm); // Supervisory interrupt handler
    void READ(VM *vm);
    void WRITE(VM *vm);
    void LR(VM *vm);
    void SR(VM *vm);
    void CR(VM *vm);
    void BT(VM *vm);
    void SIMULATION(VM *vm); // Simulates time taken by execution of instruction
    void TERMINATE(VM *vm, int EM);


Compilation of Phase 2

    gcc -c VM.c
    gcc -o phase2.(exe) main.c VM.o

Please select .exe or other formats as per your platform.


# Appendix
1. JID - Job ID - A 4 digit number assigned to each Job to make identification easier.
2. TTL - Total Time Limit - Maximum number of instructions that can be executed by the current Job.
3. TLL - Total Line Limit - Maximum number of lines that can be printed by the current Job.
4. VM - Virtual Machine - The machine we will be running the OS on.