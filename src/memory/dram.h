
#ifndef DRAM_H
#define DRAM_H

#include <stdint.h>

// 内存大小，每个位置放一个字节的数据
// 粘贴复制
#define MM_LEN 1000 

// physical memory
extern uint8_t mm[MM_LEN];

#endif