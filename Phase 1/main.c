#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VM.h"

#define LINESIZE 42 // Used to detect lines which are greater than 40 characters.
char lineBuffer[LINESIZE]; //Used to get each line, when reading line by line.

void LOAD(VM* vm) //Loads the programs into main memory.
{
    INIT(vm);
    int currWordIndex = 0;
    char* test;
    while((test = fgets(lineBuffer,sizeof(lineBuffer),vm->input))!=NULL)
    {
        if(!strnlen(lineBuffer,sizeof(lineBuffer))<1)
        {
            lineBuffer[strnlen(lineBuffer,sizeof(lineBuffer))-1]=0; // Null the string correctly, get rid of newline at end.
        }
        if(strnlen(lineBuffer,sizeof(lineBuffer))>41)
        {
            printf("Error - Line bigger than admissible\n");
            return;
        }
        if(lineBuffer[0]=='$') //Control Card
        {
            if(lineBuffer[1]=='A')
            {
                INIT(vm);
            }
            else if(lineBuffer[1]=='D')
            {
                return;
            }
            else if(lineBuffer[1]=='E')
            {
                return;
            }
        }
        else //Program Card NOT DATA CARDS
        {
            char j = 0;
            for (char i = 0; i < strnlen(lineBuffer,sizeof(lineBuffer)); ++i)
            {
                vm->M[currWordIndex][j] = lineBuffer[i];
                ++j;
                if(i>0 && (i+1)%4==0)
                {
                    ++currWordIndex;
                    j = 0;
                }
            }
        }
        memset(lineBuffer,0,sizeof(lineBuffer));
        
    }
    return;
}

void READ(VM * vm) // Analog for GD instruction
{
    fgets(lineBuffer,sizeof(lineBuffer),vm->input);
    
    if(strnlen(lineBuffer,sizeof(lineBuffer))>41)
    {
        puts("Larger than allowed DATA Card");
        return;
    }
    if(strnlen(lineBuffer,sizeof(lineBuffer))>1)
    {
        lineBuffer[strnlen(lineBuffer,sizeof(lineBuffer))-1] = 0;
    }   
    int currBlockWordIndex = (vm->IR[2] - 48)*10;
    int j = 0;
    for (int i = 0; i < strnlen(lineBuffer,sizeof(lineBuffer)) && lineBuffer[i]!=0 ; ++i)
    {
        vm->M[currBlockWordIndex][j] = lineBuffer[i];
        ++j;
        if((j>0&&j+1)%4==0)
        {
            ++currBlockWordIndex;
            j = 0;
        }
    }
}

void WRITE(VM * vm)// Analog for PD instruction
{
    int currBlockWordIndex = (vm->IR[2] - '0')*10;
    for(int i = currBlockWordIndex; i<currBlockWordIndex+10 ;++i)
    {
        for (char k = 0; k < 4; ++k)
        {
            if(!(vm->M[i][k]==0))
            {
                fputc(vm->M[i][k],vm->output);
            }
        }
    }
    fputc('\n',vm->output);
}


void TERMINATE(VM * vm) // Terminate jobs correctly/
{
    fputc('\n',vm->output);
    fputc('\n',vm->output);
    return;
}

void MOS(VM * vm) //Use the Supervisory interrupt to execute needed interrupt handler.
{
    if(vm->SI == 1)
    {
        READ(vm);
    }
    else if(vm->SI == 2)
    {
        WRITE(vm);
    }
    else if(vm->SI == 3)
    {
        TERMINATE(vm);
    }
}

void LR(VM * vm)
{
    int wordIndex = (vm->IR[2] - '0')*10 + (vm->IR[3] - '0'); // Just used to convert character numericals into actual numbers.
    vm->R[0] = vm->M[wordIndex][0];
    vm->R[1] = vm->M[wordIndex][1];
    vm->R[2] = vm->M[wordIndex][2];
    vm->R[3] = vm->M[wordIndex][3];
}

void SR(VM * vm)
{
    int wordIndex = (vm->IR[2] - '0')*10 + (vm->IR[3] - '0'); // Just used to convert character numericals into actual numbers.
    vm->M[wordIndex][0] = vm->R[0];
    vm->M[wordIndex][1] = vm->R[1];
    vm->M[wordIndex][2] = vm->R[2];
    vm->M[wordIndex][3] = vm->R[3];
}

void CR(VM * vm)
{
    int wordIndex = (vm->IR[2] - '0')*10 + (vm->IR[3] - '0');
    if (  vm->M[wordIndex][0]==vm->R[0]
        &&vm->M[wordIndex][1]==vm->R[1]
        &&vm->M[wordIndex][2]==vm->R[2]
        &&vm->M[wordIndex][3]==vm->R[3]
        )
    {
        vm->C = 'T';
        return;
    }
    vm->C = 'F';
    return;
}

void BT(VM * vm)
{
    int branchToIndex = (vm->IR[2] - '0')*10 + (vm->IR[3] - '0');
    if(vm->C=='T')
    {
        vm->IC = branchToIndex;
    }
}


void EXECUTE_USER_PROGRAM(VM * vm)//Decides which action to take based on Instruction Register
{
    for (; vm->IC < 100 && vm->IR[0]!='H';)
    {
        vm->IR[0] = vm->M[vm->IC][0];
        vm->IR[1] = vm->M[vm->IC][1];
        vm->IR[2] = vm->M[vm->IC][2];
        vm->IR[3] = vm->M[vm->IC][3];
        ++(vm->IC);
        if(vm->IR[0]=='G')
        {
            vm->SI = 1;
            MOS(vm);
        }
        else if(vm->IR[0]=='P')
        {
            vm->SI = 2;
            MOS(vm);
        }
        else if(vm->IR[0]=='H')
        {
            vm->SI = 3;
            MOS(vm);
        }
        else if(vm->IR[0]=='L')
        {
            LR(vm);
        }
        else if(vm->IR[0]=='S')
        {
            SR(vm);
        }
        else if(vm->IR[0]=='C')
        {
            CR(vm);
        }
        else if(vm->IR[0]=='B')
        {
            BT(vm);
        }
    }
}

void MOS_START_EXECUTION(VM * vm)//Just sets IC to 0, to comply with SPECS
{
    vm->IC = 0;
    EXECUTE_USER_PROGRAM(vm);
}

int main(void)
{
    FILE* input = fopen("input.txt","r");
    FILE* output = fopen("output.txt","w");
    FILE* memoryFileDump = fopen("memoryDump.txt","w");
    VM* vm = BOOT(input,output);
    INIT(vm);
    char tempC;
    while((tempC = getc(input))!=EOF)
    {
        ungetc(tempC,input);
        LOAD(vm);
        MOS_START_EXECUTION(vm);
        FILEprintMemory(memoryFileDump,vm);
    }
    fclose(input);
    fclose(output);
    fclose(memoryFileDump);
}