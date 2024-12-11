#include<stdio.h>

#include "cpu/register.h"
#include "memory/instruction.h"
#include "memory/dram.h"
#include "disk/elf.h"

int main() 
{
    
    init_handler_table();

// (gdb) disassemble
// Dump of assembler code for function main:
//    0x000055555555514e <+0>:     endbr64
//    0x0000555555555152 <+4>:     push   %rbp
//    0x0000555555555153 <+5>:     mov    %rsp,%rbp
//    0x0000555555555156 <+8>:     sub    $0x20,%rsp
//    0x000055555555515a <+12>:    movq   $0x12340000,-0x18(%rbp)
//    0x0000555555555162 <+20>:    movq   $0xabcd,-0x10(%rbp)
//    0x000055555555516a <+28>:    mov    -0x10(%rbp),%rdx
//    0x000055555555516e <+32>:    mov    -0x18(%rbp),%rax
// => 0x0000555555555172 <+36>:    mov    %rdx,%rsi
//    0x0000555555555175 <+39>:    mov    %rax,%rdi
//    0x0000555555555178 <+42>:    call   0x555555555129 <add>
//    0x000055555555517d <+47>:    mov    %rax,-0x8(%rbp)
//    0x0000555555555181 <+51>:    mov    $0x0,%eax
//    0x0000555555555186 <+56>:    leave
//    0x0000555555555187 <+57>:    ret
// End of assembler dump.
// (gdb) info r
// rax            0x12340000          305397760
// rbx            0x0                 0
// rcx            0x555555557df8      93824992247288
// rdx            0xabcd              43981
// rsi            0x7fffffffdbc8      140737488346056
// rdi            0x1                 1
// rbp            0x7fffffffdab0      0x7fffffffdab0
// rsp            0x7fffffffda90      0x7fffffffda90
// r8             0x7ffff7fa5f10      140737353768720
// r9             0x7ffff7fc9040      140737353912384
// r10            0x7ffff7fc3908      140737353890056
// r11            0x7ffff7fde660      140737353999968
// r12            0x7fffffffdbc8      140737488346056
// r13            0x55555555514e      93824992235854
// r14            0x555555557df8      93824992247288
// r15            0x7ffff7ffd040      140737354125376
// rip            0x555555555172      0x555555555172 <main+36>
// eflags         0x206               [ PF IF ]
// cs             0x33                51
// ss             0x2b                43
// ds             0x0                 0
// es             0x0                 0
// fs             0x0                 0
// gs             0x0                 0

    // init 
    reg.rax = 0x12340000;
    reg.rbx = 0x0;
    reg.rcx = 0x555555557df8;
    reg.rdx = 0xabcd;
    reg.rsi = 0x7fffffffdbc8;
    reg.rdi = 0x1;
    // 栈顶指针
    reg.rbp = 0x7fffffffdab0;
    // 栈底指针
    reg.rsp = 0x7fffffffda90;

    // 入口
    reg.rip = (uint64_t)&program[11];

    // mm[va2pa(reg.rsp)] = 0x7fffffffda90;

// (gdb) x/10 0x7fffffffda90
// 0x7fffffffda90: 0xffffde49      0x00007fff      0x12340000      0x00000000
// 0x7fffffffdaa0: 0x0000abcd      0x00000000      0x55555040      0x00005555
// 0x7fffffffdab0: 0x00000001      0x00000000

    mm[va2pa(0x7fffffffdab0)] = 0x1;                  //rbp
    mm[va2pa(0x7fffffffdaa8)] = 0x555555555040;
    mm[va2pa(0x7fffffffdaa0)] = 0xabcd;
    mm[va2pa(0x7fffffffda98)] = 0x12340000;
    mm[va2pa(0x7fffffffda90)] = 0x7fffffffde49;       //rsp


    for (size_t i = 0; i < INST_LEN; i++)
    {
    //    instruction_cycle();
    }
    

// (gdb) disassemble
// Dump of assembler code for function main:
//    0x000055555555514e <+0>:     endbr64
//    0x0000555555555152 <+4>:     push   %rbp
//    0x0000555555555153 <+5>:     mov    %rsp,%rbp
//    0x0000555555555156 <+8>:     sub    $0x20,%rsp
//    0x000055555555515a <+12>:    movq   $0x12340000,-0x18(%rbp)
//    0x0000555555555162 <+20>:    movq   $0xabcd,-0x10(%rbp)
//    0x000055555555516a <+28>:    mov    -0x10(%rbp),%rdx
//    0x000055555555516e <+32>:    mov    -0x18(%rbp),%rax
//    0x0000555555555172 <+36>:    mov    %rdx,%rsi
//    0x0000555555555175 <+39>:    mov    %rax,%rdi
//    0x0000555555555178 <+42>:    call   0x555555555129 <add>
//    0x000055555555517d <+47>:    mov    %rax,-0x8(%rbp)
// => 0x0000555555555181 <+51>:    mov    $0x0,%eax
//    0x0000555555555186 <+56>:    leave
//    0x0000555555555187 <+57>:    ret
// End of assembler dump.
// (gdb) info r
// rax            0x1234abcd          305441741
// rbx            0x0                 0
// rcx            0x555555557df8      93824992247288
// rdx            0x12340000          305397760
// rsi            0xabcd              43981
// rdi            0x12340000          305397760
// rbp            0x7fffffffdab0      0x7fffffffdab0
// rsp            0x7fffffffda90      0x7fffffffda90
// r8             0x7ffff7fa5f10      140737353768720
// r9             0x7ffff7fc9040      140737353912384
// r10            0x7ffff7fc3908      140737353890056
// r11            0x7ffff7fde660      140737353999968
// r12            0x7fffffffdbc8      140737488346056
// r13            0x55555555514e      93824992235854
// r14            0x555555557df8      93824992247288
// r15            0x7ffff7ffd040      140737354125376
// rip            0x555555555181      0x555555555181 <main+51>
// eflags         0x202               [ IF ]
// cs             0x33                51
// ss             0x2b                43
// ds             0x0                 0
// es             0x0                 0
// fs             0x0                 0
// gs             0x0                 0

    // verify

    int match = 1;

    match = match && (reg.rax == 0x1234abcd);
    match = match && (reg.rbx == 0x0);
    match = match && (reg.rcx == 0x555555557df8);
    match = match && (reg.rdx == 0x12340000);
    match = match && (reg.rsi == 0xabcd);
    match = match && (reg.rdi == 0x12340000);
    match = match && (reg.rbp == 0x7fffffffdab0);
    match = match && (reg.rsp == 0x7fffffffda90);

    if (match == 1) 
    {
        printf("register match\n");
    }
    else
    {
        printf("register not match\n");
    }
// (gdb) x/10 0x7fffffffda90
// 0x7fffffffda90: 0xffffde49      0x00007fff      0x12340000      0x00000000
// 0x7fffffffdaa0: 0x0000abcd      0x00000000      0x1234abcd      0x00000000
// 0x7fffffffdab0: 0x00000001      0x00000000

    match = match && (mm[va2pa(0x7fffffffdab0)] == 0x1);                  //rbp
    match = match && (mm[va2pa(0x7fffffffdaa8)] == 0x1234abcd);
    match = match && (mm[va2pa(0x7fffffffdaa0)] == 0xabcd);
    match = match && (mm[va2pa(0x7fffffffda98)] == 0x12340000);
    match = match && (mm[va2pa(0x7fffffffda90)] == 0x7fffffffde49);       //rsp

    if (match == 1) 
    {
        printf("memory match\n");
    }
    else
    {
        printf("memory not match\n");
    }

    return 0;
}
