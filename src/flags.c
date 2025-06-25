#include "../include/flags.h"

Flags LINKER_FLAGS = {0};

void PrintHelp() {
    printf("Usage: ./SNLINK [options] input.as input2.as ...\n");
    printf("Options:\n");
    printf("  -v, --verbose        Enable verbose logging\n");
    printf("  -d, --debug          Enable debug-level logging\n");
    printf("  -q, --quiet          Suppress all logging\n");
    printf("  -e, --entry          Output entries table\n");
    printf("  -o, --output <file>  Specify output file\n");
    printf("  -l  --legacy-24      Use Legacy linking for a 24-bit architecture\n");
    printf("      --version        Show linker version\n");
    printf("      --help           Show this help message\n");
}

void PrintVersion(void) {
    printf(
        "SNLINK: Super-Neat Linker\n"
        "- Developed and maintained by Dror Sheffer\n"
        "- Built for Super-Neat Object Code (See github for details)\n"
        "- VERSION: (v1.0.0)\n"
    );
}

int ParseFlags(int argc, char **argv, char **input_file) {
    int input_count = 0;
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        if (strcmp(arg, "-v") == 0 || strcmp(arg, "--verbose") == 0) {
            SetLogLevel(LOG_VERBOSE);
        } else if (strcmp(arg, "-d") == 0 || strcmp(arg, "--debug") == 0) {
            SetLogLevel(LOG_DEBUG);
        } else if (strcmp(arg, "-q") == 0 || strcmp(arg, "--quiet") == 0) {
            SetLogLevel(LOG_QUIET);
        } else if (strcmp(arg, "-e") == 0 || strcmp(arg, "--entry") == 0) {
            LINKER_FLAGS.entry_label = argv[++i];
        } else if (strcmp(arg, "-l") == 0 || strcmp(arg, "--legacy-24") == 0) {
            LINKER_FLAGS.legacy_24_bit = true;
        } else if (strcmp(arg, "--help") == 0) {
            PrintHelp();
            exit(0);
        } else if (strcmp(arg, "--version") == 0) {
            PrintVersion();
            exit(0);
        } else if ((strcmp(arg, "-o") == 0 || strcmp(arg, "--output") == 0) && (i + 1 < argc)) {
            LINKER_FLAGS.output_name = argv[++i];
        } else if (arg[0] == '-') {
            printf("(-) Unknown option: %s\n", arg);
            PrintHelp();
            free(*input_file);
            return STATUS_ERROR;
        } else {
            // Treat as input file
            if (input_count == 0) {
                *input_file = strdup(arg);
            } else {
                printf("(-) Only one input file is supported.\n");
                PrintHelp();
                free(*input_file);
                return STATUS_ERROR;
            }
        }
    }
    
    return 0;
}