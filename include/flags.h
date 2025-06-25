#ifndef FLAGS_H
#define FLAGS_H

#include "definitions.h"
#include <stdlib.h>
#include <string.h>

typedef struct flags_s {
    const char *output_name;
    const char *entry_label;
    bool legacy_24_bit;
    bool append_to_output;
} Flags;

extern Flags LINKER_FLAGS;

int ParseFlags(int argc, char **argv, char **input_file);
void PrintHelp();
void PrintVersion();

#endif