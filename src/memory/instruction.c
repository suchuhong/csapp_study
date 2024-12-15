#include "memory/instruction.h"
#include "memory/dram.h"
#include "cpu/mmu.h"
#include "cpu/register.h"

/**
 * virtual addr to physcial addr
 */
static uint64_t decode_od(od_t od) 
{
    if (od.type == IMM) 
    {
        // Type Form Operand value Name
        // Immediate $Imm Imm Immediate
        return *((uint64_t *)&od.imm);
    } 
    else if (od.type == REG) 
    {
        // Register ra R[ra] Register
        return (uint64_t)od.reg1;
    } 
    else 
    {
        // mm
        uint64_t vaddr = 0;

        if (od.type == MM_IMM) 
        {
            // Memory Imm M[Imm] Absolute
            vaddr = od.imm;
        }
        else if (od.type == MM_REG) 
        {
            // Memory (ra) M[R[ra]] Indirect
            vaddr = *(od.reg1);
        }
        else if (od.type == MM_IMM_REG) 
        {
            // Memory Imm(rb) M[Imm + R[rb]] Base + displacement
            vaddr = od.imm + *(od.reg1);
        }
        else if (od.type == MM_REG1_REG2) 
        {
            // Memory (rb,ri) M[R[rb] + R[ri]] Indexed
            vaddr = *(od.reg1) + *(od.reg2);
        }
        else if (od.type == MM_IMM_REG1_REG2) 
        {
            // Memory Imm(rb,ri) M[Imm + R[rb] + R[ri]] Indexed
            vaddr = od.imm + *(od.reg1) + *(od.reg2);
        }
        else if (od.type == MM_REG2_S) 
        {
            // Memory (,ri,s) M[R[ri]. s] Scaled indexed
            vaddr = (*(od.reg2)) * od.scal;
        }
        else if (od.type == MM_IMM_REG2_S) 
        {
            // Memory Imm(,ri,s) M[Imm + R[ri]. s] Scaled indexed
            vaddr = od.imm + (*(od.reg2)) * od.scal;
        }
        else if (od.type == MM_REG1_REG2_S) 
        {
            // Memory (rb,ri,s) M[R[rb] + R[ri]. s] Scaled indexed
            vaddr = *(od.reg1) + (*(od.reg2)) * od.scal;
        }
        else if (od.type == MM_IMM_REG1_REG2_S) 
        {
            // Memory Imm(rb,ri,s) M[Imm + R[rb] + R[ri]. s] Scaled indexed
            vaddr = od.imm + *(od.reg1) + (*(od.reg2)) * od.scal;
        }

        return vaddr;
    }
}

handler_t handler_table[NUM_INSTRTYPE];

void init_handler_table()
{
    handler_table[MOV_REG_REG] = mov_reg_reg_handler;
    handler_table[MOV_REG_MEM] = mov_reg_mem_handler;
    handler_table[MOV_MEM_REG] = mov_mem_reg_handler;
    handler_table[CALL] = call_handler;
    handler_table[PUSH_REG] = push_reg_handler;
    handler_table[POP_REG] = pop_reg_handler;
    handler_table[RET] = ret_handler;
    handler_table[ADD_REG_REG] = add_reg_reg_handler;
};

void mov_reg_reg_handler(uint64_t src, uint64_t dst)
{
    // src : reg
    // dst : reg
    *(uint64_t *)dst = *(uint64_t *)src;
    // PC/rip 指向 下一条指令
    reg.rip =  reg.rip + sizeof(inst_t);
}

void mov_reg_mem_handler(uint64_t src, uint64_t dst)
{
    // src : reg
    // dst : mem vaddr
    write64bit_dram(
        va2pa(dst),
        *(uint64_t *)src
    );
    
    // PC/rip 指向 下一条指令
    reg.rip =  reg.rip + sizeof(inst_t);
}

void mov_mem_reg_handler(uint64_t src, uint64_t dst)
{
    // src : mem vaddr
    // dst : reg
    // 读内存
    uint64_t val = read64bits_dram(va2pa(src));
    // printf("val == %16lx\n", val);
    // 写 reg
    *(uint64_t *)dst = val;
    // PC/rip 指向 下一条指令
    reg.rip =  reg.rip + sizeof(inst_t);
}

/**
 * 1、rsp 往下移动
 * 2、rsp 写入call调用返回后的下一条指令地址
 */
void call_handler(uint64_t src, uint64_t dst)
{
    // src : imm address of called function
    // 移动 rsp 指针
    reg.rsp = reg.rsp - 8;
    // write return address to rsp memory
    // 写rsp值
    write64bit_dram(
        va2pa(reg.rsp),
        reg.rip + sizeof(inst_t)
    );
    // PC 放到 call xx 的开始位置
    reg.rip = src;
}

void push_reg_handler(uint64_t src, uint64_t dst)
{
    // src : reg
    // dst : empty
    // rsp 向下移动
    reg.rsp = reg.rsp - 8;
    // 写入值
    write64bit_dram(
        va2pa(reg.rsp),
        *(uint64_t *)src
    );
    // PC/rip 指向 下一条指令
    reg.rip =  reg.rip + sizeof(inst_t);
}

void pop_reg_handler(uint64_t src, uint64_t dst)
{

}

void ret_handler(uint64_t src, uint64_t dst)
{

}

void add_reg_reg_handler(uint64_t src, uint64_t dst) 
{
    /*
    rax pmm[0x1234] = 0x12340000
    rbx pmm[0x1235] = 0xabcd

    src: 0x1234
    dst: 0x1235

    *(uint64_t *)src = 0x12340000
    *(uint64_t *)dst = 0xabcd

    pmm[0x1235] = 0x12340000 + 0xabcd = 0x1234abcd
    
     */

    *(uint64_t *)dst = *(uint64_t *)src + *(uint64_t *)dst;
    // PC/rip 指向 下一条指令
    reg.rip =  reg.rip + sizeof(inst_t);
}

// while (1) {
//   从PC指示的存储器位置取出指令;
//   执行指令;
//   更新PC;
// }
void instruction_cycle() 
{
    // 当前 PC/rip 指向
    inst_t *instr = (inst_t *)reg.rip;
   
    // imm: imm
    // reg: value
    // mm: paddr
    uint64_t src = decode_od(instr -> src);
    uint64_t dst = decode_od(instr -> dst);

    // eg:
    // add rax rbx
    // op = ADD_REG_REG = 3
    // handler_table[ADD_REG_REG] = handler_table[3]

    handler_t handler = handler_table[instr->op];

    // add_reg_reg_handler(src = &rax, dst = &rbx)

    handler(src, dst);

    printf("  %s\n", instr->code);

}
