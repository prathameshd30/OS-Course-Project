#include "VM.h"
#include <time.h>
#include <ctype.h>

#define LINE_SIZE 42

char lineBuffer[LINE_SIZE];
FILE *stateFile;

void LOAD(VM *vm);
void PTR_INIT(VM *vm);
int ALLOCATE(VM *vm);
void START_EXECUTION(VM *vm);
void EXECUTE_USER_PROGRAM(VM *vm);
int ADDRESS_MAP(VM *vm, int VA);
void MOS(VM *vm);
void READ(VM *vm);
void WRITE(VM *vm);
void LR(VM *vm);
void SR(VM *vm);
void CR(VM *vm);
void BT(VM *vm);
void SIMULATION(VM *vm);
void TERMINATE(VM *vm, int EM);

void LOAD(VM *vm)
{
    char *test;
    while ((test = fgets(lineBuffer, sizeof(lineBuffer), vm->input)) != NULL)
    {
        if ((strnlen(lineBuffer, sizeof(lineBuffer)) >= 1))
        {
            lineBuffer[strnlen(lineBuffer, sizeof(lineBuffer)) - 1] = 0;
        }
        // Can handle bigger than expected line error
        if (lineBuffer[0] == '$')
        {
            if (lineBuffer[1] == 'A')
            {
                VM_INIT(vm);
                int strIndex = 4;
                int temp = 0;
                for (; strIndex < 8; ++strIndex)
                {
                    temp = temp * 10;
                    temp += (lineBuffer[strIndex] - '0');
                }
                vm->pcb->JID = temp;
                temp = 0;
                for (; strIndex < 12; ++strIndex)
                {
                    temp = temp * 10;
                    temp += (lineBuffer[strIndex] - '0');
                }
                vm->pcb->TTL = temp;
                temp = 0;
                for (; strIndex < 16; ++strIndex)
                {
                    temp = temp * 10;
                    temp += (lineBuffer[strIndex] - '0');
                }
                vm->pcb->TLL = temp;
                temp = 0;
                vm->cpu->PTBR = (ALLOCATE(vm)) * 10;
                vm->cpu->PTR = vm->cpu->PTBR;
                PTR_INIT(vm);
                printf("PTR = %u\n", vm->cpu->PTR);
            }
            else if (lineBuffer[1] == 'D')
            {
                START_EXECUTION(vm);
            }
            else if (lineBuffer[1] == 'E')
            {
                printf("$END Detected Moving On\n");
                break;
            }
            else
            {
                printf("Unexpected char after $\n");
                return;
            }
        }
        else // Program Card
        {
            int frameNum = ALLOCATE(vm);
            vm->memcon->M[vm->cpu->PTR][0] = '1';
            vm->memcon->M[vm->cpu->PTR][2] = (frameNum / 10) + '0';
            vm->memcon->M[vm->cpu->PTR][3] = (frameNum % 10) + '0';
            ++(vm->cpu->PTR);
            int j = 0;
            int RA = frameNum * 10;
            for (int i = 0; i < strnlen(lineBuffer, sizeof(lineBuffer)); ++i)
            {
                vm->memcon->M[RA][j] = lineBuffer[i];
                ++j;
                if (i > 0 && (i + 1) % 4 == 0)
                {
                    ++RA;
                    j = 0;
                }
            }
        }
        memset(lineBuffer, 0, sizeof(lineBuffer));
    }
}

void PTR_INIT(VM *vm)
{
    for (int i = (vm->cpu->PTR); i < (vm->cpu->PTR) + 10; ++i)
    {
        vm->memcon->M[i][0] = 0;
        vm->memcon->M[i][2] = '*';
        vm->memcon->M[i][3] = '*';
    }
}

int ALLOCATE(VM *vm)
{
    while (1)
    {
        int random = (rand() % 30);
        if (vm->memcon->allocatedFrames[random] != 1)
        {
            vm->memcon->allocatedFrames[random] = 1;
            return random;
        }
    }
}

void START_EXECUTION(VM *vm)
{
    vm->cpu->IC = 0;
    vm->cpu->PTR = vm->cpu->PTBR;
    EXECUTE_USER_PROGRAM(vm);
}

void EXECUTE_USER_PROGRAM(VM *vm)
{
    while (vm->execFlag != 1)
    {
        vm->cpu->RA = ADDRESS_MAP(vm, vm->cpu->IC);
        ++(vm->cpu->IC);
        vm->cpu->IR[0] = vm->memcon->M[vm->cpu->RA][0];
        vm->cpu->IR[1] = vm->memcon->M[vm->cpu->RA][1];
        vm->cpu->IR[2] = vm->memcon->M[vm->cpu->RA][2];
        vm->cpu->IR[3] = vm->memcon->M[vm->cpu->RA][3];
        if(vm->cpu->IR[0]=='H')
        {
            vm->cpu->SI = 3;
        }
        else if (!(isdigit(vm->cpu->IR[2]) && isdigit(vm->cpu->IR[3])))
        {
            vm->cpu->PI = 2;
        }
        else
        {
            vm->cpu->VA = ((vm->cpu->IR[2]) - '0') * 10 + ((vm->cpu->IR[3]) - '0');
            vm->cpu->RA = ADDRESS_MAP(vm, vm->cpu->VA);
            if(vm->cpu->RA == -1)
            {
                ;
            }
            else if (vm->cpu->IR[0] == 'G' && vm->cpu->IR[1] == 'D')
            {
                vm->cpu->SI = 1;
            }
            else if (vm->cpu->IR[0] == 'P' && vm->cpu->IR[1] == 'D')
            {
                vm->cpu->SI = 2;
            }
            else if (vm->cpu->IR[0] == 'H')
            {
                vm->cpu->SI = 3;
            }
            else if (vm->cpu->IR[0] == 'L' && vm->cpu->IR[1] == 'R')
            {
                LR(vm);
            }
            else if (vm->cpu->IR[0] == 'S' && vm->cpu->IR[1] == 'R')
            {
                SR(vm);
            }
            else if (vm->cpu->IR[0] == 'C' && vm->cpu->IR[1] == 'R')
            {
                CR(vm);
            }
            else if (vm->cpu->IR[0] == 'B' && vm->cpu->IR[1] == 'T')
            {
                BT(vm);
            }
            else
            {
                vm->cpu->PI = 1;
            }
        }
        SIMULATION(vm);
        if (vm->cpu->SI!=0 || vm->cpu->PI!=0 || vm->cpu->TI !=0)
        {
            MOS(vm);
        }
    }
}

int ADDRESS_MAP(VM *vm, int VA)
{
    int page = vm->cpu->PTR + VA / 10;
    if (vm->memcon->M[page][2] == '*')
    {
        vm->cpu->PI = 3;
        return -1;
    }
    int realAddress = (vm->memcon->M[page][2] - '0') * 10 + (vm->memcon->M[page][3] - '0');
    // printf("ADDRESS MAP RETURNS = %d\n",(realAddress * 10) + (VA % 10));
    return (realAddress * 10) + (VA % 10);
}

void MOS(VM *vm)
{
    if (vm->cpu->TI == 0 && vm->cpu->SI == 1)
    {
        READ(vm);
    }
    else if (vm->cpu->TI == 0 && vm->cpu->SI == 2)
    {
        WRITE(vm);
    }
    else if (vm->cpu->TI == 0 && vm->cpu->SI == 3)
    {
        TERMINATE(vm, 0);
    }
    else if (vm->cpu->TI == 2 && vm->cpu->SI == 0 && vm->cpu->PI == 0)
    {
        (--vm->cpu->TTC);
        TERMINATE(vm, 3);
    }
    else if (vm->cpu->TI == 2 && vm->cpu->SI == 1)
    {
        TERMINATE(vm, 3);
    }
    else if (vm->cpu->TI == 2 && vm->cpu->SI == 2)
    {
        WRITE(vm);
        TERMINATE(vm, 3);
    }
    else if (vm->cpu->TI == 2 && vm->cpu->SI == 3)
    {
        TERMINATE(vm, 0);
    }
    else if (vm->cpu->TI == 0 && vm->cpu->PI == 1)
    {
        
        TERMINATE(vm, 4);
    }
    else if (vm->cpu->TI == 0 && vm->cpu->PI == 2)
    {
        
        TERMINATE(vm, 5);
    }
    else if (vm->cpu->TI == 0 && vm->cpu->PI == 3) // Valid Page Fault
    {
        if (vm->cpu->IR[0] == 'G' || vm->cpu->IR[0] == 'S')
        {
            int frameNum = ALLOCATE(vm);
            int num = (vm->cpu->IR[2]-'0')*10 + (vm->cpu->IR[3]-'0');
            vm->memcon->M[(vm->cpu->PTR) + num/10][0] = '1';
            vm->memcon->M[(vm->cpu->PTR) + num/10][2] = (frameNum / 10) + '0';
            vm->memcon->M[(vm->cpu->PTR) + num/10][3] = (frameNum % 10) + '0';
            --(vm->cpu->IC);
            --(vm->cpu->TTC);
        }
        else
        {
            TERMINATE(vm, 6);    
        }
    }
    else if (vm->cpu->TI == 2 && vm->cpu->PI == 1)
    {
        
        TERMINATE(vm, 7);
    }
    else if (vm->cpu->TI == 2 && vm->cpu->PI == 2)
    {
        
        TERMINATE(vm, 8);
    }
    else if (vm->cpu->TI == 2 && vm->cpu->PI == 3)
    {
        TERMINATE(vm, 3);
    }
    vm->cpu->SI = 0;
    vm->cpu->PI = 0;
    vm->cpu->TI = 0;
}

void READ(VM *vm)
{
    fgets(lineBuffer, sizeof(lineBuffer), vm->input);
    if (lineBuffer[0] == '$')
    {
        TERMINATE(vm, 1);
    }
    if (strnlen(lineBuffer, sizeof(lineBuffer)) > 41)
    {
        puts("Larger than allowed DATA Card");
        return;
    }
    if (strnlen(lineBuffer, sizeof(lineBuffer)) > 1) // Null Termination
    {
        lineBuffer[strnlen(lineBuffer, sizeof(lineBuffer)) - 1] = 0;
    }
    int j = 0;
    int tempAddr = vm->cpu->RA;
    for (int i = 0; i < strnlen(lineBuffer, sizeof(lineBuffer)) && lineBuffer[i] != 0; ++i)
    {
        vm->memcon->M[tempAddr][j] = lineBuffer[i];
        ++j;
        if ((j > 0 && j + 1) % 4 == 0)
        {
            ++(tempAddr);
            j = 0;
        }
    }
}

void WRITE(VM *vm)
{
    if (vm->cpu->LLC== vm->pcb->TLL)
    {
        TERMINATE(vm, 2);
        return;
    }
    ++(vm->cpu->LLC);
    printf("Writing RA %u\n",vm->cpu->RA);
    for (int i = vm->cpu->RA; i < vm->cpu->RA + 10; ++i)
    {
        for (int k = 0; k < 4; ++k)
        {
            if (!(vm->memcon->M[i][k] == 0))
            {
                fputc(vm->memcon->M[i][k], vm->output);
            }
        }
    }
    fputc('\n', vm->output);
}

void LR(VM *vm)
{
    vm->cpu->R[0] = vm->memcon->M[vm->cpu->RA][0];
    vm->cpu->R[1] = vm->memcon->M[vm->cpu->RA][1];
    vm->cpu->R[2] = vm->memcon->M[vm->cpu->RA][2];
    vm->cpu->R[3] = vm->memcon->M[vm->cpu->RA][3];
}

void SR(VM *vm)
{
    vm->memcon->M[vm->cpu->RA][0] = vm->cpu->R[0];
    vm->memcon->M[vm->cpu->RA][1] = vm->cpu->R[1];
    vm->memcon->M[vm->cpu->RA][2] = vm->cpu->R[2];
    vm->memcon->M[vm->cpu->RA][3] = vm->cpu->R[3];
}

void CR(VM *vm)
{
    if (vm->memcon->M[vm->cpu->RA][0] == vm->cpu->R[0] && vm->memcon->M[vm->cpu->RA][1] == vm->cpu->R[1] && vm->memcon->M[vm->cpu->RA][2] == vm->cpu->R[2] && vm->memcon->M[vm->cpu->RA][3] == vm->cpu->R[3])
    {
        vm->cpu->C = 'T';
        return;
    }
    vm->cpu->C = 'F';
    return;
}

void BT(VM *vm)
{
    int branchToIndex = (vm->cpu->IR[2] - '0') * 10 + (vm->cpu->IR[3] - '0');
    if (vm->cpu->C == 'T')
    {
        vm->cpu->IC = branchToIndex;
    }
}

void SIMULATION(VM *vm)
{
    ++(vm->cpu->TTC);
    if (vm->cpu->TTC > vm->pcb->TTL)
    {
        vm->cpu->TI = 2;
    }
}

void TERMINATE(VM *vm, int EM)
{
    vm->cpu->SI = 0;
    if(vm->execFlag==1)
    {
        return;
    }
    fprintf(vm->output, "JOB ID\t:\t%d\n", vm->pcb->JID);
    if (EM == 0)
    {
        fputs("NO ERROR\n", vm->output);
    }
    else if (EM == 1)
    {
        --(vm->cpu->TTC);
        fputs("OUT OF DATA\n", vm->output);
    }
    else if (EM == 2)
    {
        fputs("LINE LIMIT EXCEEDED\n", vm->output);
    }
    else if (EM == 3)
    {
        fputs("TIME LIMIT EXCEEDED\n", vm->output);
    }
    else if (EM == 4)
    {
        --(vm->cpu->TTC);
        fputs("OPCODE ERROR\n", vm->output);
    }
    else if (EM == 5)
    {
        --(vm->cpu->TTC);
        fputs("OPERAND ERROR\n", vm->output);
    }
    else if (EM == 6)
    {
        --(vm->cpu->TTC);
        fputs("INVALID PAGE FAULT\n", vm->output);
    }
    else if (EM == 7)
    {
        --(vm->cpu->TTC);
        fputs("TIME LIMIT EXCEEDED AND OPCODE ERROR\n", vm->output);
    }
    else if (EM == 8)
    {
        --(vm->cpu->TTC);
        fputs("TIME LIMIT EXCEEDED AND OPERAND ERROR\n", vm->output);
    }
    fprintf(vm->output, "IC\t\t:\t%d\n", vm->cpu->IC);
    fprintf(vm->output, "IR\t\t:\t");
    for (int i = 0; i < WORD_SIZE; ++i)
    {
        fprintf(vm->output, "%c", vm->cpu->IR[i]);
    }
    fputc('\n', vm->output);
    fprintf(vm->output, "TTC\t\t:\t%d\n", vm->cpu->TTC);
    fprintf(vm->output, "LLC\t\t:\t%d\n", vm->cpu->LLC);
    fputc('\n', vm->output);
    fputc('\n', vm->output);
    vm->execFlag = 1;
    printState(stateFile, vm);
}

int main(void)
{
    srand(time(0));
    FILE *inputFile = fopen("input.txt", "r");
    FILE *outputFile = fopen("output.txt", "w");
    stateFile = fopen("state.txt", "w");
    VM *vm = VM_BOOT(inputFile, outputFile);
    char tempC;
    {
        while ((tempC = getc(inputFile)) != EOF)
        {
            ungetc(tempC, inputFile);
            LOAD(vm);
        }
    }
    fclose(inputFile);
    fclose(outputFile);
    fclose(stateFile);
}