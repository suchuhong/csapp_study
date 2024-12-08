#include<stdlib.h>

#include "disk/elf.h"
#include "cpu/register.h"

inst_t program[INST_LEN] = {

    { // rip
        // op
        MOV,
        // src
        { REG, 0, 0, (uint64_t *)&reg.rax, NULL },
        // dst
        { REG, 0, 0, (uint64_t *)&reg.rbx, NULL },
        // code
        "mov \%rax \%rbx"
    }

};