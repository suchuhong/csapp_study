// Dynamic Random Access Memory
#include<string.h>
#include<assert.h>
#include<headers/cpu.h>
#include<headers/memory.h>
#include<headers/common.h>

uint8_t pm[PHYSICAL_MEMORY_SPACE];

/*  
Be careful with the x86-64 little endian integer encoding
e.g. write 0x00007fd357a02ae0 to cache, the memory lapping should be:
    e0 2a a0 57 d3 7f 00 00
*/

// memory accessing used in instructions
uint64_t read64bits_dram(uint64_t paddr, core_t *cr)
{
    if (DEBUG_ENABLE_SRAM_CACHE == 1)
    {
        // try to load uint64_t from SRAM cache
        // little-endian
    }
    else
    {
        // read from DRAM directly
        // little-endian
        uint64_t val = 0x0;

        val += (((uint64_t)pm[paddr + 0 ]) << (8 * 0));
        val += (((uint64_t)pm[paddr + 1 ]) << (8 * 1));
        val += (((uint64_t)pm[paddr + 2 ]) << (8 * 2));
        val += (((uint64_t)pm[paddr + 3 ]) << (8 * 3));
        val += (((uint64_t)pm[paddr + 4 ]) << (8 * 4));
        val += (((uint64_t)pm[paddr + 5 ]) << (8 * 5));
        val += (((uint64_t)pm[paddr + 6 ]) << (8 * 6));
        val += (((uint64_t)pm[paddr + 7 ]) << (8 * 7));

        return val;
    }
}

void write64bits_dram(uint64_t paddr, uint64_t data, core_t *cr)
{
    if (DEBUG_ENABLE_SRAM_CACHE == 1)
    {
        // try to write uint64_t to SRAM cache
        // little-endian
    }
    else
    {
        // write to DRAM directly
        // little-endian
        pm[paddr + 0] = (data >> (8 * 0)) & 0xff;
        pm[paddr + 1] = (data >> (8 * 1)) & 0xff;
        pm[paddr + 2] = (data >> (8 * 2)) & 0xff;
        pm[paddr + 3] = (data >> (8 * 3)) & 0xff;
        pm[paddr + 4] = (data >> (8 * 4)) & 0xff;
        pm[paddr + 5] = (data >> (8 * 5)) & 0xff;
        pm[paddr + 6] = (data >> (8 * 6)) & 0xff;
        pm[paddr + 7] = (data >> (8 * 7)) & 0xff;
    }
}

void readinst_dram(uint64_t paddr, char *buf, core_t *cr)
{
    for (int i = 0; i < MAX_INSTRUCTION_CHAR; ++ i)
    {
        buf[i] = (char)pm[paddr + i];
    }
}
void writeinst_dram(uint64_t paddr, const char *str, core_t *cr)
{
    int len = strlen(str);
    assert(len < MAX_INSTRUCTION_CHAR);
    for (int i = 0; i < MAX_INSTRUCTION_CHAR; ++ i)
    {
        if (i < len)
        {
            pm[paddr + i] = (uint8_t)str[i];
        }
        else
        {
            pm[paddr + i] = 0;
        }
    }
}