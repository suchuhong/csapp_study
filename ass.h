#include<stdlib.h>
#include<stdint.h>

typedef struct CPU_STRUCT
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
} cpu_t;

cpu_t cpu;

// 内存大小，每个位置放一个字节的数据
// 粘贴复制
#define MM_LEN 1000 

uint8_t mm[MM_LEN];

typedef enum OP 
{
    MOV,  // 0
    PUSH, // 1
    CALL  // 2
} op_t;

typedef enum OD_TYPE 
{
    IMM, 
    REG,
    MM_IMM,
    MM_REG,
    MM_IMM_REG,
    MM_REG1_REG2,
    MM_IMM_REG1_REG2,
    MM_REG2_S,
    MM_IMM_REG2_S,
    MM_REG1_REG2_S,
    MM_IMM_REG1_REG2_S

} od_type_t;

typedef struct OD 
{
   od_type_t type;

   uint64_t imm;
   uint64_t scal;
   uint64_t reg1; // 地址
   uint64_t reg2; // 地址
} od_t;

typedef struct INSTRUCT_STRUCT 
{
    op_t op; // mov push
    od_t src;
    od_t dst;
} inst_t;

#define INST_LEN 100

inst_t program[INST_LEN];

// 取指令
// 译码
// 执行

uint64_t decode_od(od_t od);