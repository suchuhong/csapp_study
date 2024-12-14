// #include "dram.h"
// 等同下面
#include <memory/dram.h>

// 使用cache
#define SRAM_CACHE_SETTING 0

// 每一个地址只能放一个 byte
uint8_t mm[MM_LEN];  // 定义数组

uint64_t read64bits_dram(uint64_t paddr)
{
    if (SRAM_CACHE_SETTING == 1) {
        return 0x0;
    }

    uint64_t data = 0x0;
    
    data += (((uint64_t)mm[paddr + 0]) << (8 * 0));
    data += (((uint64_t)mm[paddr + 1]) << (8 * 1));
    data += (((uint64_t)mm[paddr + 2]) << (8 * 2));
    data += (((uint64_t)mm[paddr + 3]) << (8 * 3));
    data += (((uint64_t)mm[paddr + 4]) << (8 * 4));
    data += (((uint64_t)mm[paddr + 5]) << (8 * 5));
    data += (((uint64_t)mm[paddr + 6]) << (8 * 6));
    data += (((uint64_t)mm[paddr + 7]) << (8 * 7));

    return data;

}

void write64bit_dram(uint64_t paddr, uint64_t data)
{
    if (SRAM_CACHE_SETTING == 1) {
        return;
    }

    mm[paddr + 0] = (data >> (8 * 0)) & 0xff;
    mm[paddr + 1] = (data >> (8 * 1)) & 0xff;
    mm[paddr + 2] = (data >> (8 * 2)) & 0xff;
    mm[paddr + 3] = (data >> (8 * 3)) & 0xff;
    mm[paddr + 4] = (data >> (8 * 4)) & 0xff;
    mm[paddr + 5] = (data >> (8 * 5)) & 0xff;
    mm[paddr + 6] = (data >> (8 * 6)) & 0xff;
    mm[paddr + 7] = (data >> (8 * 7)) & 0xff;
    
}
