
# OS Course Project
This repository follows some coursework which documents the implementation of the Operating Systems Course Project.

The project is split into 2 phases, Phase 1 and Phase 2.

Instruction takes the form of - AA00, where AA can be any instruction from the instruction set and 00 can be any memory address from 00 to 99.

Each line in the input file denotes an input card. Each card can have a maximum of 40 characters.
## Types of cards
3 Types of cards are used for structuring job.

1. Control cards
 - $AMJ  (Beginning of Job) - Starts with $AMJ, followed 4 digit Job ID, followed by 4 digit total time limit, followed by 4 digit total line limit.
 - $DTA  (Starts the data segment)
 - $END  (Signifies the end of the job) - Starts with $END, followed by 4 digit Job ID.

2. Instruction Cards - Contain instructions to execute (Maximum 10 instructions in one card)

3. Data Cards - Contain data to operate on.

## Instruction Set
- GD - Get Data - Blockwise Instruction - Starts loading input from the starting of the block of the given address.
- PD - Print Data - Blockwise Instruction
- LR - Load Register
- SR - Store Register into Memory
- CR - Compare Register and Memory
- BT - Branch Toggle
- H  - Halt

## Generic Memory Model (Applicable for both Phase 1 and Phase 2)
- Each word consists of 4 bytes of memory
- The machine is word addressable (cannot address individual bytes)
- Memory is split into blocks of 10 contiguous addresses for Blockwise instructions.

Phase 1 has a basic memory model where job is loaded contiguously, starting from address 00.

Phase 2 explores the paging concepts, using a page table and splitting the job and data into non-contiguous locations.