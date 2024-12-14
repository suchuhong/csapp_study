
#ifndef DRAM_H
#define DRAM_H

#include <stdint.h>

// 内存大小，每个位置放一个字节的数据
// 粘贴复制
#define MM_LEN 1000 

// physical memory
extern uint8_t mm[MM_LEN];

// 使用一下函数可以不用直接修改内容
// 增加 缓存的使用
uint64_t read64bits_dram(uint64_t paddr);
void write64bit_dram(uint64_t paddr, uint64_t data);

#endif