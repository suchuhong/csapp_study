#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>

#define NUM_INSTRTYPE 30

typedef enum OP 
{
    MOV_REG_REG,
    MOV_REG_MEM,
    MOV_MEM_REG,
    CALL,  
    PUSH_REG,
    POP_REG,
    RET,
    ADD_REG_REG
} op_t;

typedef enum OD_TYPE 
{
    EMPTY,
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

   int64_t imm; // 立即数  正负都有
   int64_t scal; // 正负都有
   uint64_t *reg1; // 地址
   uint64_t *reg2; // 地址

} od_t;

typedef struct INSTRUCT_STRUCT 
{
    op_t op; // mov push
    od_t src;
    od_t dst;

    char code[100];
} inst_t;

typedef void (*handler_t)(uint64_t, uint64_t);

extern handler_t handler_table[NUM_INSTRTYPE];

void init_handler_table();

// 取指令
// 译码
// 执行

void instruction_cycle();

void mov_reg_reg_handler(uint64_t src, uint64_t dst);

void mov_reg_mem_handler(uint64_t src, uint64_t dst);

void mov_mem_reg_handler(uint64_t src, uint64_t dst);

void call_handler(uint64_t src, uint64_t dst);

void push_reg_handler(uint64_t src, uint64_t dst);

void pop_reg_handler(uint64_t src, uint64_t dst);

void ret_handler(uint64_t src, uint64_t dst);

void add_reg_reg_handler(uint64_t src, uint64_t dst);

#endif