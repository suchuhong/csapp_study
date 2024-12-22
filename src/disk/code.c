#include<stdlib.h>

#include "disk/elf.h"
#include "cpu/register.h"

/**
 * 需要解析的汇编代码
 */
inst_t program[INST_LEN] = 
{
    { // rip
        // op
        PUSH_REG,
        // src
        { REG, 0, 0, (uint64_t *)&reg.rbp, NULL },
        // dst
        { EMPTY, 0, 0, NULL,                 NULL },
        // code
        "push   \%rbp"
    },
    { // rip
        // op
        MOV_REG_REG,
        // src
        { REG, 0, 0, (uint64_t *)&reg.rsp, NULL },
        // dst
        { REG, 0, 0, (uint64_t *)&reg.rbp, NULL },
        // code
        "mov    \%rsp,\%rbp"
    },
    { // rip
        // op
        MOV_REG_MEM,
        // src
        { REG, 0, 0, (uint64_t *)&reg.rdi, NULL },
        // dst
        { MM_IMM_REG, -0x18, 0, (uint64_t *)&reg.rbp, NULL },
        // code
        "mov    \%rdi,-0x18(\%rbp)"
    },
    { // rip
        // op
        MOV_REG_MEM,
        // src
        { REG, 0, 0, (uint64_t *)&reg.rsi, NULL },
        // dst
        { MM_IMM_REG, -0x20, 0, (uint64_t *)&reg.rbp, NULL },
        // code
        "mov    \%rsi,-0x20(\%rbp)"
    },
    { // rip
        // op
        MOV_MEM_REG,
        // src
        { MM_IMM_REG, -0x18, 0, (uint64_t *)&reg.rbp, NULL },
        // dst
        { REG, 0, 0, (uint64_t *)&reg.rdx, NULL },
        // code
        "mov    -0x18(\%rbp),\%rdx"
    },
    { // rip
        // op
        MOV_MEM_REG,
        // src
        { MM_IMM_REG, -0x20, 0, (uint64_t *)&reg.rbp, NULL },
        // dst
        { REG, 0, 0, (uint64_t *)&reg.rax, NULL },
        // code
        "mov    -0x20(\%rbp),\%rax"
    },
    { // rip
        // op
        ADD_REG_REG,
        // src
        { REG, 0, 0, (uint64_t *)&reg.rdx, NULL },
        // dst
        { REG, 0, 0, (uint64_t *)&reg.rax, NULL },
        // code
        "add    \%rdx,\%rax"
    },
    { // rip
        // op
        MOV_REG_MEM,
        // src
        { REG, 0, 0, (uint64_t *)&reg.rax, NULL },
        // dst
        { MM_IMM_REG, -0x8, 0, (uint64_t *)&reg.rbp, NULL },
        // code
        "mov    \%rax,-0x8(\%rbp)"
    },
    { // rip
        // op
        MOV_MEM_REG,
        // src
        { MM_IMM_REG, -0x8, 0, (uint64_t *)&reg.rbp, NULL },
        // dst
        { REG, 0, 0, (uint64_t *)&reg.rax, NULL },
        // code
        "mov    -0x8(\%rbp),\%rax"
    },
    { // rip
        // op
        POP_REG,
        // src
        { REG, 0, 0, (uint64_t *)&reg.rbp, NULL },
        // dst
        { EMPTY, 0, 0, NULL, NULL },
        // code
        "pop    \%rbp"
    },
    { // rip
        // op
        RET,
        // src
        { EMPTY, 0, 0, NULL, NULL },
        // dst
        { EMPTY, 0, 0, NULL, NULL },
        // code
        "retq"
    },
    // mian entry 
    { // rip
        // op
        MOV_REG_REG,
        // src
        { REG, 0, 0, (uint64_t *)&reg.rdx, NULL },
        // dst
        { REG, 0, 0, (uint64_t *)&reg.rsi, NULL },
        // code
        "mov    \%rdx,\%rsi"
    },
    { // rip
        // op
        MOV_REG_REG,
        // src
        { REG, 0, 0, (uint64_t *)&reg.rax, NULL },
        // dst
        { REG, 0, 0, (uint64_t *)&reg.rdi, NULL },
        // code
        "mov    \%rax,\%rdi"
    },
    { // rip
        // op
        CALL,
        // src
        { IMM, (uint64_t)&(program[0]), 0, NULL, NULL },
        // dst
        { EMPTY, 0, 0, NULL, NULL },
        // code
        "callq   <add>"
    },
    { // rip
        // op
        MOV_REG_MEM,
        // src
        { REG, 0, 0, (uint64_t *)&reg.rax, NULL },
        // dst
        { MM_IMM_REG, -0x8, 0, (uint64_t *)&reg.rbp, NULL },
        // code
        "mov    \%rax,-0x8(\%rbp)"
    }
};