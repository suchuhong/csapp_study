#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<headers/cpu.h>
#include<headers/memory.h>
#include<headers/common.h>

/*======================================*/
/*      instruction set architecture    */
/*======================================*/

core_t cores[NUM_CORES];
// active core for current task
uint64_t ACTIVE_CORE;

// data structures
typedef enum INST_OPERATOR
{
    INST_MOV,           // 0
    INST_PUSH,          // 1
    INST_POP,           // 2
    INST_LEAVE,         // 3
    INST_CALL,          // 4
    INST_RET,           // 5
    INST_ADD,           // 6
    INST_SUB,           // 7
    INST_CMP,           // 8
    INST_JNE,           // 9
    INST_JMP,           // 10
} op_t;

// csapp 217
// Figure 3.3
// Operand forms. Operands can denote immediate (constant) values, register
// values, or values from memory. The scaling factor s must be either 1, 2, 4, or 8.
// Type        Form          Operand value              Name
// Immediate   $Imm          Imm                        Immediate
// Register    ra            R[ra]                      Register
// Memory      Imm           M[Imm]                     Absolute
// Memory      (ra)          M[R[ra]]                   Indirect
// Memory      Imm(rb)       M[Imm + R[rb]]             Base + displacement
// Memory      (rb,ri)       M[R[rb] + R[ri]]           Indexed
// Memory      Imm(rb,ri)    M[Imm + R[rb] + R[ri]]     Indexed
// Memory      (,ri,s)       M[R[ri] . s]               Scaled indexed
// Memory      Imm(,ri,s)    M[Imm + R[ri] . s]         Scaled indexed
// Memory      (rb,ri,s)     M[R[rb] + R[ri] . s]       Scaled indexed
// Memory      Imm(rb,ri,s)  M[Imm + R[rb] + R[ri] . s] Scaled indexed
typedef enum OPERAND_TYPE
{
    EMPTY,                  // 0
    IMM,                    // 1
    REG,                    // 2
    MEM_IMM,                // 3
    MEM_REG1,               // 4
    MEM_IMM_REG1,           // 5
    MEM_REG1_REG2,          // 6
    MEM_IMM_REG1_REG2,      // 7
    MEM_REG2_SCAL,          // 8
    MEM_IMM_REG2_SCAL,      // 9
    MEM_REG1_REG2_SCAL,     // 10
    MEM_IMM_REG1_REG2_SCAL  // 11
} od_type_t;

typedef struct OPERAND_STRUCT
{
    od_type_t   type;   // IMM, REG, MEM
    uint64_t    imm;    // immediate number
    uint64_t    scal;   // scale number to register 2,1 2 4 8
    uint64_t    reg1;   // main register
    uint64_t    reg2;   // register 2
} od_t;

// local variables are allocated in stack in run-time
// we don't consider local STATIC variables
// ref: Computer Systems: A Programmer's Perspective 3rd
// Chapter 7 Linking: 7.5 Symbols and Symbol Tables
typedef struct INST_STRUCT
{
    op_t    op;         // enum of operators. e.g. mov, call, etc.
    od_t    src;        // operand src of instruction
    od_t    dst;        // operand dst of instruction
} inst_t;

/*======================================*/
/*      parse assembly instruction      */
/*======================================*/

// functions to map the string assembly code to inst_t instance
// 解析指令
static void parse_instruction(const char *str, inst_t *inst, core_t *cr);
// 寄存器映射
// 获取运行是的寄存器信息
static uint64_t reflect_register(const char *str, core_t *cr);
// 解析操作数
static void parse_operand(const char *str, od_t *od, core_t *cr);
static uint64_t decode_operand(od_t *od);

// interpret the operand
static uint64_t decode_operand(od_t *od)
{
    if (od->type == IMM)
    {
        // immediate signed number can be negative: convert to bitmap
        return *(uint64_t *)&od->imm;
    }
    else if (od->type == REG)
    {
        // default register 1
        return od->reg1;
    }
    else if (od->type == EMPTY)
    {
        return 0;
    }
    else
    {
        // access memory: return the physical address
        uint64_t vaddr = 0;

        if (od->type == MEM_IMM)
        {
            vaddr = od->imm;
        }
        else if (od->type == MEM_REG1)
        {
            vaddr = *((uint64_t *)od->reg1);
        }
        else if (od->type == MEM_IMM_REG1)
        {
            vaddr = od->imm + (*((uint64_t *)od->reg1));
        }
        else if (od->type == MEM_REG1_REG2)
        {
            vaddr = (*((uint64_t *)od->reg1)) + (*((uint64_t *)od->reg2));
        }
        else if (od->type == MEM_IMM_REG1_REG2)
        {
            vaddr = od->imm +  (*((uint64_t *)od->reg1)) + (*((uint64_t *)od->reg2));
        }
        else if (od->type == MEM_REG2_SCAL)
        {
            vaddr = (*((uint64_t *)od->reg2)) * od->scal;
        }
        else if (od->type == MEM_IMM_REG2_SCAL)
        {
            vaddr = od->imm + (*((uint64_t *)od->reg2)) * od->scal;
        }
        else if (od->type == MEM_REG1_REG2_SCAL)
        {
            vaddr = (*((uint64_t *)od->reg1)) + (*((uint64_t *)od->reg2)) * od->scal;
        }
        else if (od->type == MEM_IMM_REG1_REG2_SCAL)
        {
            vaddr = od->imm + (*((uint64_t *)od->reg1)) + (*((uint64_t *)od->reg2)) * od->scal;
        }
        return vaddr;
    }
    
    // empty
    return 0;
}

// lookup table
static const char *reg_name_list[72] = {
    "%rax","%eax","%ax","%ah","%al",
    "%rbx","%ebx","%bx","%bh","%bl",
    "%rcx","%ecx","%cx","%ch","%cl",
    "%rdx","%edx","%dx","%dh","%dl",
    "%rsi","%esi","%si","%sih","%sil",
    "%rdi","%edi","%di","%dih","%dil",
    "%rbp","%ebp","%bp","%bph","%bpl",
    "%rsp","%esp","%sp","%sph","%spl",
    "%r8","%r8d","%r8w","%r8b",
    "%r9","%r9d","%r9w","%r9b",
    "%r10","%r10d","%r10w","%r10b",
    "%r11","%r11d","%r11w","%r11b",
    "%r12","%r12d","%r12w","%r12b",
    "%r13","%r13d","%r13w","%r13b",
    "%r14","%r14d","%r14w","%r14b",
    "%r15","%r15d","%r15w","%r15b",
};

// %rax -> cr.reg.rax
// %rdx -> cr.reg.rdx
// %rip -> cr.reg.rip
//  .....
static uint64_t reflect_register(const char *str, core_t *cr)
{
    // lookup table
    reg_t *reg = &(cr->reg);
    uint64_t reg_addr[72] = {
        (uint64_t)&(reg->rax),(uint64_t)&(reg->eax),(uint64_t)&(reg->ax),(uint64_t)&(reg->ah),(uint64_t)&(reg->al),
        (uint64_t)&(reg->rbx),(uint64_t)&(reg->ebx),(uint64_t)&(reg->bx),(uint64_t)&(reg->bh),(uint64_t)&(reg->bl),
        (uint64_t)&(reg->rcx),(uint64_t)&(reg->ecx),(uint64_t)&(reg->cx),(uint64_t)&(reg->ch),(uint64_t)&(reg->cl),
        (uint64_t)&(reg->rdx),(uint64_t)&(reg->edx),(uint64_t)&(reg->dx),(uint64_t)&(reg->dh),(uint64_t)&(reg->dl),
        (uint64_t)&(reg->rsi),(uint64_t)&(reg->esi),(uint64_t)&(reg->si),(uint64_t)&(reg->sih),(uint64_t)&(reg->sil),
        (uint64_t)&(reg->rdi),(uint64_t)&(reg->edi),(uint64_t)&(reg->di),(uint64_t)&(reg->dih),(uint64_t)&(reg->dil),
        (uint64_t)&(reg->rbp),(uint64_t)&(reg->ebp),(uint64_t)&(reg->bp),(uint64_t)&(reg->bph),(uint64_t)&(reg->bpl),
        (uint64_t)&(reg->rsp),(uint64_t)&(reg->esp),(uint64_t)&(reg->sp),(uint64_t)&(reg->sph),(uint64_t)&(reg->spl),
        (uint64_t)&(reg->r8),(uint64_t)&(reg->r8d),(uint64_t)&(reg->r8w),(uint64_t)&(reg->r8b),
        (uint64_t)&(reg->r9),(uint64_t)&(reg->r9d),(uint64_t)&(reg->r9w),(uint64_t)&(reg->r9b),
        (uint64_t)&(reg->r10),(uint64_t)&(reg->r10d),(uint64_t)&(reg->r10w),(uint64_t)&(reg->r10b),
        (uint64_t)&(reg->r11),(uint64_t)&(reg->r11d),(uint64_t)&(reg->r11w),(uint64_t)&(reg->r11b),
        (uint64_t)&(reg->r12),(uint64_t)&(reg->r12d),(uint64_t)&(reg->r12w),(uint64_t)&(reg->r12b),
        (uint64_t)&(reg->r13),(uint64_t)&(reg->r13d),(uint64_t)&(reg->r13w),(uint64_t)&(reg->r13b),
        (uint64_t)&(reg->r14),(uint64_t)&(reg->r14d),(uint64_t)&(reg->r14w),(uint64_t)&(reg->r14b),
        (uint64_t)&(reg->r15),(uint64_t)&(reg->r15d),(uint64_t)&(reg->r15w),(uint64_t)&(reg->r15b),
    };
    for (int i = 0; i < 72; ++ i)
    {
        if (strcmp(str, reg_name_list[i]) == 0)
        {
            // now we know that i is the index inside reg_name_list
            return reg_addr[i];
        }
    }
    printf("parse register %s error\n", str);
    exit(0);
}

static void parse_instruction(const char *str, inst_t *inst, core_t *cr)
{
    char op_str[64] = {'\0'};
    int op_len = 0;
    char src_str[64] = {'\0'};
    int src_len = 0;
    char dst_str[64] = {'\0'};
    int dst_len = 0;

    char c;
    int count_parentheses = 0;// ()
    int state = 0;

    // op od1,od2
    // op (),()
    for (int i = 0; i < strlen(str); i++)
    {
        c = str[i];
        if (c == '(' || c == ')') 
        {
            count_parentheses ++;
        }

        if (state == 0 && c != ' ')
        {
            state = 1;
        }
        else if (state == 1 && c == ' ')
        {
            state = 2;
            continue;
        }
        else if (state == 2 && c != ' ')
        {
            state = 3;
        }
        else if (state == 3 && c == ',' && (count_parentheses == 0 || count_parentheses == 2))
        {
            state = 4;
            continue;
        }
        else if (state == 4 && c != ' ' && c != ',')
        {
            state = 5;
        }
        else if (state == 5 && c == ' ')
        {
            state = 6;
            continue;
        }

        if (state == 1)
        {
            op_str[op_len] = c;
            op_len ++;
            continue;
        }
        else if (state == 3)
        {
            src_str[src_len] = c;
            src_len ++;
            continue;
        }
        else if (state == 5)
        {
            dst_str[dst_len] = c;
            dst_len ++;
            continue;
        }
        
    }

    // op_str, src_str, dst_str
    
    // typedef enum INST_OPERATOR
    // {
    //     INST_MOV,           // 0
    //     INST_PUSH,          // 1
    //     INST_POP,           // 2
    //     INST_LEAVE,         // 3
    //     INST_CALL,          // 4
    //     INST_RET,           // 5
    //     INST_ADD,           // 6
    //     INST_SUB,           // 7
    //     INST_CMP,           // 8
    //     INST_JNE,           // 9
    //     INST_JMP,           // 10
    // } op_t;
    parse_operand(src_str, &(inst->src), cr);
    parse_operand(dst_str, &(inst->dst), cr);

    if (strcmp(op_str, "mov") == 0 || strcmp(op_str, "movq") == 0)
    {
        inst->op = INST_MOV;
    }
    else if (strcmp(op_str, "push") == 0)
    {
        inst->op = INST_PUSH;
    }
    else if (strcmp(op_str, "pop") == 0)
    {
        inst->op = INST_POP;
    }
    else if (strcmp(op_str, "leaveq") == 0)
    {
        inst->op = INST_LEAVE;
    }
    else if (strcmp(op_str, "callq") == 0)
    {
        inst->op = INST_CALL;
    }
    else if (strcmp(op_str, "retq") == 0)
    {
        inst->op = INST_RET;
    }
    else if (strcmp(op_str, "add") == 0)
    {
        inst->op = INST_ADD;
    }
    else if (strcmp(op_str, "sub") == 0)
    {
        inst->op = INST_SUB;
    }
    else if (strcmp(op_str, "cmpq") == 0)
    {
        inst->op = INST_CMP;
    }
    else if (strcmp(op_str, "jne") == 0)
    {
        inst->op = INST_JNE;
    }
    else if (strcmp(op_str, "jmp") == 0)
    {
        inst->op = INST_JMP;
    }

    debug_printf(DEBUG_PARSEINST, "[%s (%d)] [%s (%d)] [%s (%d)]\n", 
    op_str, inst->op, src_str, inst->src.type, dst_str, inst->dst.type);
    
}

// c 语言字符串以  '\0' 结尾
// csapp 217
// Figure 3.3
// Operand forms. Operands can denote immediate (constant) values, register
// values, or values from memory. The scaling factor s must be either 1, 2, 4, or 8.
// Type        Form          Operand value              Name
// Immediate   $Imm          Imm                        Immediate
// Register    ra            R[ra]                      Register
// Memory      Imm           M[Imm]                     Absolute
// Memory      (ra)          M[R[ra]]                   Indirect
// Memory      Imm(rb)       M[Imm + R[rb]]             Base + displacement
// Memory      (rb,ri)       M[R[rb] + R[ri]]           Indexed
// Memory      Imm(rb,ri)    M[Imm + R[rb] + R[ri]]     Indexed
// Memory      (,ri,s)       M[R[ri] . s]               Scaled indexed
// Memory      Imm(,ri,s)    M[Imm + R[ri] . s]         Scaled indexed
// Memory      (rb,ri,s)     M[R[rb] + R[ri] . s]       Scaled indexed
// Memory      Imm(rb,ri,s)  M[Imm + R[rb] + R[ri] . s] Scaled indexed
// 类型 Form 
static void parse_operand(const char *str, od_t *od, core_t *cr)
{
    // str: assembly code string , mov %rsp,%rbp
    // od： pointer to the address to store the parsed operand
    // cr:  active core processor
    od->type = EMPTY;
    od->imm = 0;
    od->scal = 0;
    od->reg1 = 0;
    od->reg2 = 0;

    int str_len = strlen(str);
    if (str_len == 0)
    {
        // empty operand string
        return;
    }

    if (str[0] == '$')
    {
        // Immediate   $Imm          Imm                        Immediate
        // immediate number
        od->type = IMM;
        // 字符串解析为 uint64
        od->imm = string2uint_range(str, 1, -1);
        return;
    } 
    else if (str[0] == '%')
    {
        // register 
        // Register    ra            R[ra]                      Register
        od->type = REG;
        od->reg1 = reflect_register(str, cr);
        return;
    }
    else
    {
        // memory assess
        // Memory      Imm           M[Imm]                     Absolute
        // Memory      (ra)          M[R[ra]]                   Indirect
        // Memory      Imm(rb)       M[Imm + R[rb]]             Base + displacement
        // Memory      (rb,ri)       M[R[rb] + R[ri]]           Indexed
        // Memory      Imm(rb,ri)    M[Imm + R[rb] + R[ri]]     Indexed
        // Memory      (,ri,s)       M[R[ri] . s]               Scaled indexed
        // Memory      Imm(,ri,s)    M[Imm + R[ri] . s]         Scaled indexed
        // Memory      (rb,ri,s)     M[R[rb] + R[ri] . s]       Scaled indexed
        // Memory      Imm(rb,ri,s)  M[Imm + R[rb] + R[ri] . s] Scaled indexed
        char imm[64] = {'\0'};
        int imm_len = 0;
        char reg1[64] = {'\0'};
        int reg1_len = 0;
        char reg2[64] = {'\0'};
        int reg2_len = 0;
        char scal[64] = {'\0'};
        int scal_len = 0;

        int count_parentheses = 0; // ()
        int count_comma = 0; // ,
        
        for (int i = 0; i < str_len; i++)
        {
            char c =str[i];

            if(c == '(' || c == ')')
            {
                count_parentheses ++;
                continue;
            } 
            else if (c == ',')
            {
                count_comma ++;
                continue;
            }
            else 
            {
                // pasre imm(reg1,reg2,scal)
                if (count_parentheses == 0)
                {
                    // xxxx
                    imm[imm_len] = c;
                    imm_len++;
                    continue;
                }
                else if (count_parentheses == 1)
                {
                    if (count_comma == 0) 
                    {
                        // ???(xxxx
                        // (xxxx
                        reg1[reg1_len] = c;
                        reg1_len++;
                        continue;
                    }
                    else if (count_comma == 1) 
                    {
                        // ???(???,xxxx
                        // (???,xxx
                        // ???(,xxxx
                        // (,xxx
                        reg2[reg2_len] = c;
                        reg2_len++;
                        continue;
                    }
                    else if (count_comma == 2) 
                    {
                        // ???(???,???,xxxx
                        // (???,???,xxxx
                        // ???(,???,xxxx
                        // (,???,xxxx
                        scal[scal_len] = c;
                        scal_len++;
                        continue;
                    }
                }
                // else if (ca == 2)
                // {
                //     // ???(???,???,???)
                //     // (???,???,???)
                // }
            }
        }

        // imm, reg1, reg2, scal

        if (imm_len > 0)
        {
            od->imm = string2uint(imm);

            if(count_parentheses == 0)
            {
                // Memory      Imm           M[Imm]                     Absolute
                od->type = MEM_IMM;
                return;
            }
        }

        if (scal_len > 0)
        {
            od->scal = string2uint(scal);

            if (od->scal != 1 && od->scal != 2 && od->scal != 4 && od->scal != 8)
            {
                printf("%s is not a legal sacler\n", scal);
                exit(0);
            }

        }

        if (reg1_len > 0)
        {
            od->reg1 = reflect_register(reg1, cr);
        }

        if (reg2_len > 0)
        {
            od->reg2 = reflect_register(reg2, cr);
        }
        
        // set operand type
        if (count_comma == 0)
        {
            if (imm_len > 0)
            {
                // xx()
                // Memory      Imm(rb)       M[Imm + R[rb]]             Base + displacement
                od->type = MEM_IMM_REG1;
                return;
            }
            else 
            {
                // ()
                // Memory      (ra)          M[R[ra]]                   Indirect
                od->type = MEM_REG1;
                return;
            }
        }
        else if (count_comma == 1)
        {
            if (imm_len > 0)
            {
                // xx(xx,xxx)
                // Memory      Imm(rb,ri)    M[Imm + R[rb] + R[ri]]     Indexed
                od->type = MEM_IMM_REG1_REG2;
                return;
            }
            else 
            {
                // (xxx,xxx)
                // Memory      (rb,ri)       M[R[rb] + R[ri]]           Indexed
                od->type = MEM_REG1_REG2;
                return;
            }
        }
        else if (count_comma == 2)
        {
            if (reg1_len > 0)
            {
                if (imm_len > 0)
                {
                    // xx(xx,xxx,xxx)
                    // Memory      Imm(rb,ri,s)  M[Imm + R[rb] + R[ri] . s] Scaled indexed
                    od->type = MEM_IMM_REG1_REG2_SCAL;
                    return;
                }
                else 
                {
                    // (xx,xxx,xxx)
                    // Memory      (rb,ri,s)     M[R[rb] + R[ri] . s]       Scaled indexed
                    od->type = MEM_REG1_REG2_SCAL;
                    return;
                }
            }
            else 
            {
                if (imm_len > 0)
                {
                    // xx(,xxx,xxx)
                    // Memory      Imm(,ri,s)    M[Imm + R[ri] . s]         Scaled indexed
                    od->type = MEM_IMM_REG2_SCAL;
                    return;
                }
                else 
                {
                    // (,xxx,xxx)
                    // Memory      (,ri,s)       M[R[ri] . s]               Scaled indexed
                    od->type = MEM_REG2_SCAL;
                    return;
                }
            }
        }
    }
}

/*======================================*/
/*      instruction handlers            */
/*======================================*/

// insturction (sub)set
// In this simulator, the instructions have been decoded and fetched
// so there will be no page fault during fetching
// otherwise the instructions must handle the page fault (swap in from disk) first
// and then re-fetch the instruction and do decoding
// and finally re-run the instruction

static void mov_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void push_handler            (od_t *src_od, od_t *dst_od, core_t *cr);
static void pop_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void leave_handler           (od_t *src_od, od_t *dst_od, core_t *cr);
static void call_handler            (od_t *src_od, od_t *dst_od, core_t *cr);
static void ret_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void add_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void sub_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void cmp_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void jne_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void jmp_handler             (od_t *src_od, od_t *dst_od, core_t *cr);

// handler table storing the handlers to different instruction types
typedef void (*handler_t)(od_t *, od_t *, core_t *);
// look-up table of pointers to function
static handler_t handler_table[NUM_INSTRTYPE] = {
    &mov_handler,               // 0
    &push_handler,              // 1
    &pop_handler,               // 2
    &leave_handler,             // 3
    &call_handler,              // 4
    &ret_handler,               // 5
    &add_handler,               // 6
    &sub_handler,               // 7
    &cmp_handler,               // 8
    &jne_handler,               // 9
    &jmp_handler,               // 10
};

// reset the condition flags
// inline to reduce cost
// CF  (unsigned) t < (unsigned) a          Unsigned overflow
// ZF  (t == 0)                             Zero 
// SF  (t < 0)                              Negative
// OF  (a < 0 == b < 0) && (t < 0 != a < 0) Signed overflow
static inline void reset_cflags(core_t *cr)
{
    cr->flags.__cpu_flag_value = 0x0;
}

// update the rip pointer to the next instruction sequentially
static inline void next_rip(core_t *cr)
{
    // we are handling the fixed-length of assembly string here
    // but their size can be variable as true X86 instructions
    // that's because the operands' sizes follow the specific encoding rule
    // the risc-v is a fixed length ISA
    cr->rip = cr->rip + sizeof(char) * MAX_INSTRUCTION_CHAR;
}

// instruction handlers

static void mov_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    uint64_t dst = decode_operand(dst_od);

    if (src_od->type == REG && dst_od->type == REG)
    {
        // src: register
        // dst: register
        *(uint64_t *)dst = *(uint64_t *)src;
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
    else if (src_od->type == REG && dst_od->type >= MEM_IMM)
    {
        // src: register
        // dst: virtual address
        write64bits_dram(
            va2pa(dst, cr), 
            *(uint64_t *)src,
            cr
            );
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
    else if (src_od->type >= MEM_IMM && dst_od->type == REG)
    {
        // src: virtual address
        // dst: register
        *(uint64_t *)dst = read64bits_dram(
            va2pa(src, cr), 
            cr);
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
    else if (src_od->type == IMM && dst_od->type == REG)
    {
        // src: immediate number (uint64_t bit map)
        // dst: register
        *(uint64_t *)dst = src;
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
}

static void push_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    // uint64_t dst = decode_operand(dst_od);

    if (src_od->type == REG)
    {
        // src: register
        // dst: empty
        (cr->reg).rsp = (cr->reg).rsp - 8;
        write64bits_dram(
            va2pa((cr->reg).rsp, cr), 
            *(uint64_t *)src, 
            cr
            );
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
}

static void pop_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    // uint64_t dst = decode_operand(dst_od);
    
    if (src_od->type == REG)
    {
        // src: register
        // dst: empty
        uint64_t old_val = read64bits_dram(
            va2pa((cr->reg).rsp, cr),
            cr
            );
        (cr->reg).rsp = (cr->reg).rsp + 8;
        *(uint64_t *)src = old_val;
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
}

static void leave_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
}

static void call_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    // uint64_t dst = decode_operand(dst_od);

    // src: immediate number: virtual address of target function starting
    // dst: empty
    // push the return value
    (cr->reg).rsp = (cr->reg).rsp - 8;
    write64bits_dram(
        va2pa((cr->reg).rsp, cr),
        cr->rip + sizeof(char) * MAX_INSTRUCTION_CHAR,
        cr
        );
    // jump to target function address
    cr->rip = src;
    reset_cflags(cr);
}

static void ret_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    // uint64_t src = decode_operand(src_od);
    // uint64_t dst = decode_operand(dst_od);

    // src: empty
    // dst: empty
    // pop rsp
    uint64_t ret_addr = read64bits_dram(
        va2pa((cr->reg).rsp, cr),
        cr
        );
    (cr->reg).rsp = (cr->reg).rsp + 8;
    // jump to return address
    cr->rip = ret_addr;
    reset_cflags(cr);
}

static void add_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    uint64_t dst = decode_operand(dst_od);

    if (src_od->type == REG && dst_od->type == REG)
    {
        // src: register (value: int64_t bit map)
        // dst: register (value: int64_t bit map)
        uint64_t val = *(uint64_t *)dst + *(uint64_t *)src;

        int val_sign = ((val >> 63) & 0x1);
        int src_sign = ((*(uint64_t *)src >> 63) & 0x1);
        int dst_sign = ((*(uint64_t *)dst >> 63) & 0x1);
        // set condition flags
        // 无符号溢出
        cr->flags.CF = (val < *(uint64_t *)src); // unsigned
        cr->flags.ZF = (val == 0);
        cr->flags.SF = val_sign;
        // 有符号溢出 
        // https://www.bilibili.com/video/BV1Qf4y1e73j?t=4655.7
        // 规则 src dst 异号 一定不溢出
        // src dst val OF
        // 0   0   0   0
        // 0   0   1   1
        // 0   1   0   0
        // 0   1   1   0
        // 1   0   0   0
        // 1   1   0   1
        // 1   0   1   0
        // 1   1   1   0
        cr->flags.OF = (src_sign == 0 && dst_sign == 0 && val_sign == 1) || (src_sign == 1 && dst_sign == 1 && val_sign == 0);


        // update registers
        *(uint64_t *)dst = val;
        // signed and unsigned value follow the same addition. e.g.
        // 5 = 0000000000000101, 3 = 0000000000000011, -3 = 1111111111111101, 5 + (-3) = 0000000000000010
        next_rip(cr);
        return;
    }
}

static void sub_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{

    uint64_t src = decode_operand(src_od);
    uint64_t dst = decode_operand(dst_od);

    if (src_od->type == IMM && dst_od->type == REG)
    {
        // src: IMM (value: int64_t bit map)
        // dst: register (value: int64_t bit map)
        // dst = dst - src
        uint64_t neg_src = ~src + 1;
        uint64_t val = *(uint64_t *)dst + (neg_src);

        int val_sign = ((val >> 63) & 0x1);
        // set condition flags
        cr->flags.CF = 0; // unsigned
        cr->flags.ZF = (val == 0);
        cr->flags.SF = val_sign;
        cr->flags.OF = 0; // singed

        // update registers
        *(uint64_t *)dst = val;
        // signed and unsigned value follow the same addition. e.g.
        // 5 = 0000000000000101, 3 = 0000000000000011, -3 = 1111111111111101, 5 + (-3) = 0000000000000010
        next_rip(cr);
        return;
    }
}

static void cmp_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
}

static void jne_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
}

static void jmp_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
}

// instruction cycle is implemented in CPU
// the only exposed interface outside CPU
void instruction_cycle(core_t *cr)
{
    // FETCH: get the instruction string by program counter

    // const char *inst_str = (const char *)cr->rip;
    // 读取rip物理内存的地址
    char inst_str[MAX_INSTRUCTION_CHAR + 10];
    readinst_dram(va2pa(cr->rip, cr), inst_str, cr);

    debug_printf(DEBUG_INSTRUCTIONCYCLE, "cr->rip %lx    %s\n", cr->rip, inst_str);

    // DECODE: decode the run-time instruction operands
    inst_t inst;
    parse_instruction(inst_str, &inst, cr);
    
    // EXECUTE: get the function pointer or handler by the operator
    handler_t handler = handler_table[inst.op];
    // update CPU and memory according the instruction
    handler(&(inst.src), &(inst.dst), cr);
}

void print_register(core_t *cr)
{
    if ((DEBUG_VERBOSE_SET & DEBUG_REGISTERS) == 0x0)
    {
        return;
    }

    reg_t reg = cr->reg;
    
    printf("rax = %16lx\trbx = %16lx\trcx = %16lx\trdx = %16lx\n",
        reg.rax, reg.rbx, reg.rcx, reg.rdx);
    printf("rsi = %16lx\trdi = %16lx\trbp = %16lx\trsp = %16lx\n",
        reg.rsi, reg.rdi, reg.rbp, reg.rsp);
    printf("rip = %16lx\n", cr->rip);
    printf("CF = %u\tZF = %u\tSF = %u\tOF = %u\n",
        cr->flags.CF, cr->flags.ZF, cr->flags.SF, cr->flags.OF);
}

void print_stack(core_t *cr)
{
    if ((DEBUG_VERBOSE_SET & DEBUG_PRINTSTACK) == 0x0)
    {
        return;
    }

    int n = 10;    
    uint64_t *high = (uint64_t*)&pm[va2pa((cr->reg).rsp, cr)];
    high = &high[n];
    uint64_t va = (cr->reg).rsp + n * 8;
    
    for (int i = 0; i < 2 * n; ++ i)
    {
        uint64_t *ptr = (uint64_t *)(high - i);
        printf("0x%16lx : %16lx", va, (uint64_t)*ptr);

        if (i == n)
        {
            printf(" <== rsp");
        }
        printf("\n");
        va -= 8;
    }
}

void TestParseOperand()
{
    ACTIVE_CORE =0x0;
    core_t *ac = (core_t *) &cores[ACTIVE_CORE];

    const char *strs[11] =
    {
        "$0x1234",
        "%rax",
        "0xabcd",
        "(%rsp)",
        "0xabcd(%rsp)",
        "(%rsp,%rbx)",
        "0xabcd(%rsp,%rbx)",
        "(,%rbx,8)",
        "0xabcd(,%rbx,8)",
        "(%rsp,%rbx,8)",
        "0xabcd(%rsp,%rbx,8)",
    };

    printf("rax %p\n", &(ac->reg.rax));
    printf("rsp %p\n", &(ac->reg.rsp));
    printf("rbx %p\n", &(ac->reg.rbx));
    
    for (int i = 0; i < 11; ++ i)
    {
        od_t od;
        parse_operand(strs[i], &od, ac);
        printf("\n%s\n", strs[i]);
        printf("od enum type: %d\n", od.type);
        printf("od imm: %lx\n", od.imm);
        printf("od reg1: %lx\n", od.reg1);
        printf("od reg2: %lx\n", od.reg2);
        printf("od scal: %lx\n", od.scal);
    }
}

void TestParsingInstruction()
{
    ACTIVE_CORE = 0x0;    
    core_t *ac = (core_t *)&cores[ACTIVE_CORE];
    char assembly[15][MAX_INSTRUCTION_CHAR] = {
        "push   %rbp",              // 0
        "mov    %rsp,%rbp",         // 1
        "mov    %rdi,-0x18(%rbp)",  // 2
        "mov    %rsi,-0x20(%rbp)",  // 3
        "mov    -0x18(%rbp),%rdx",  // 4
        "mov    -0x20(%rbp),%rax",  // 5
        "add    %rdx,%rax",         // 6
        "mov    %rax,-0x8(%rbp)",   // 7
        "mov    -0x8(%rbp),%rax",   // 8
        "pop    %rbp",              // 9
        "retq",                     // 10
        "mov    %rdx,%rsi",         // 11
        "mov    %rax,%rdi",         // 12
        "callq  0",                 // 13
        "mov    %rax,-0x8(%rbp)",   // 14
    };
    
    inst_t inst;
    for (int i = 0; i < 15; ++ i)
    {
        parse_instruction(assembly[i], &inst, ac);
    }
}
