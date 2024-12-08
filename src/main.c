#include<stdio.h>

#include "cpu/register.h"
#include "memory/instruction.h"
#include "disk/elf.h"

int main() 
{
    
    init_handler_table();

    for (size_t i = 0; i < INST_LEN; i++)
    {
    //    instruction_cycle();
    }
    

    return 0;
}
