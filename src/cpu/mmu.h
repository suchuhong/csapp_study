
#ifndef MMU_H
#define MMU_H

// memory management unit

#include <stdint.h>

/**
 * 虚拟地址转物理地址
 */
uint64_t va2pa(uint64_t vaddr);

#endif 