#ifndef REGISRTER_H
#define REGISRTER_H

#include<stdlib.h>
#include<stdint.h>

typedef struct REG_STRUCT
{
    union 
    {
        struct
        {
            uint8_t al;
            uint8_t ah;
        };
        uint16_t ax;
        uint32_t eax;
        uint64_t rax; // Return value
    };
    union 
    {
        struct
        {
            uint8_t bl;
            uint8_t bh;
        };
        uint16_t bx;
        uint32_t ebx;
        uint64_t rbx; // Callee saved
    };
    union 
    {
        struct
        {
            uint8_t cl;
            uint8_t ch;
        };
        uint16_t cx;
        uint32_t ecx;
        uint64_t rcx; // 4th argument
    };
    union 
    {
        struct
        {
            uint8_t dl;
            uint8_t dh;
        };
        uint16_t dx;
        uint32_t edx;
        uint64_t rdx; // 3rd argument
    };
    union 
    {
        struct
        {
            uint8_t sil;
            uint8_t sih;
        };
        uint16_t si;
        uint32_t esi;
        uint64_t rsi; // 2nd argument
    };
    union 
    {
        struct
        {
            uint8_t dil;
            uint8_t dih;
        };
        uint16_t di;
        uint32_t edi;
        uint64_t rdi; // 1st argument
    };
    union 
    {
        struct
        {
            uint8_t bpl;
            uint8_t bph;
        };
        uint16_t bp;
        uint32_t ebp;
        uint64_t rbp; // Callee saved
    };
    union 
    {
        struct
        {
            uint8_t spl;
            uint8_t sph;
        };
        uint16_t sp;
        uint32_t esp;
        uint64_t rsp; // Stack pointer
    };
    
    
    uint64_t rip; // PC
} reg_t;

extern reg_t reg;

#endif