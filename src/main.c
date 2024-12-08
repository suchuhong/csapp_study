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


uint64_t decode_od(od_t od) 
{
    if (od.type == IMM) 
    {
        return od.imm;
    } else if (od.type == REG) 
    {
        // a = (uint64_t *)od.reg  强转指针
        // *a 取值
        return *(uint64_t *)od.reg1;
    } else 
    {
        uint64_t addr = MM_LEN + 0xff;

        // ....

        return mm[addr % MM_LEN];
    }
}