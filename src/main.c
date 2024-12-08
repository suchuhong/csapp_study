#include<stdio.h>

#include "cpu/register.h"
#include "memory/instruction.h"

int main() 
{
    
    reg.rax = 0x1234abcd5678ffaa;

    printf("eax: %08x\n", reg.eax);
    printf("ax: %08x\n", reg.ax);
    printf("al: %08x\n", reg.al);
    printf("ah: %08x\n", reg.ah);

    return 0;
}
