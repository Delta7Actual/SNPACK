#ifndef SNX_H
#define SNX_H

#include "definitions.h"

typedef struct s_snx_header {
    uint32_t magic;         // SNX_MAGIC
    uint32_t text_size;     // Size of text section
    uint32_t data_size;     // Size of data section
    uint32_t total_size;    // Size of entire file in bytes
    uint32_t entry_offset;  // Offset from beginning to entry point
} SNXHeader;

#endif