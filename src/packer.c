#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/definitions.h"
#include "../include/snx.h"
#include "../include/flags.h"

char *input = NULL;
char output_name[MAX_FILENAME_LENGTH] = "out.snx";

void CleanAndExit(FILE *input_fd, FILE *output_fd) {
    if (input_fd)   fclose(input_fd);
    if (output_fd)  fclose(output_fd);
    if (input)      free(input);
}

int main(int argc, char **argv) {
    if (ParseFlags(argc, argv, &input) != 0) return EXIT_FAILURE;
    if (!input) {
        LogInfo("(-) No input file provided!\n");
        return EXIT_FAILURE;
    }

    if (LINKER_FLAGS.output_name) {
        snprintf(output_name, MAX_FILENAME_LENGTH, "%s.snx", LINKER_FLAGS.output_name);
    }

    LogDebug(" Input file: %s\n", input);
    LogDebug(" Output file: %s\n", output_name);

    FILE *input_fd  = fopen(input, "r");
    FILE *output_fd = NULL;

    if (!input_fd) {
        LogInfo("(-) Failed to open input file: %s\n", input);
        CleanAndExit(input_fd, output_fd);
        return EXIT_FAILURE;
    }

    output_fd = fopen(output_name, "wb");
    if (!output_fd) {
        LogInfo("(-) Failed to open output file: %s\n", output_name);
        CleanAndExit(input_fd, output_fd);
        return EXIT_FAILURE;
    }

    char line[MAX_LINE_LENGTH] = {0};
    SNXHeader header = {0};

    LogInfo("(*) Reading header line from input file...\n");
    if (!fgets(line, MAX_LINE_LENGTH, input_fd)) {
        LogInfo("(-) Empty file\n");
        CleanAndExit(input_fd, output_fd);
        return EXIT_FAILURE;
    }

    char *dcf_str = strchr(line, OBJ_HEADER_DELIM);
    if (!dcf_str) {
        LogInfo("(-) Malformed .sno header (missing '|')\n");
        CleanAndExit(input_fd, output_fd);
        return EXIT_FAILURE;
    }

    *dcf_str = '\0';
    dcf_str++;

    int icf = atoi(line);
    int dcf = atoi(dcf_str);

    if (icf < 0 || dcf < 0) {
        LogInfo("(-) Illegal ICF or DCF values: %d, %d\n", icf, dcf);
        CleanAndExit(input_fd, output_fd);
        return EXIT_FAILURE;
    }

    header.magic        = SNX_MAGIC;
    header.text_size    = (uint32_t) icf;
    header.data_size    = (uint32_t) dcf;
    header.total_size   = header.text_size + header.data_size;
    header.entry_offset = 0;

    LogVerbose("Parsed text size: %u bytes (%u words)\n", header.text_size, header.text_size / 4);
    LogVerbose("Parsed data size: %u bytes (%u words)\n", header.data_size, header.data_size / 4);
    LogVerbose("Total section size: %u bytes\n", header.total_size);

    LogDebug("Writing temporary SNX header...\n");
    fwrite(&header, sizeof(SNXHeader), 1, output_fd);

    uint32_t max_words  = header.total_size / 4;
    uint32_t word_count = 0;

    uint32_t word = 0, addr = 0;

    LogInfo("(*) Writing memory image...\n");

    while (fgets(line, MAX_LINE_LENGTH, input_fd)) {
        if (sscanf(line, "%u : 0x%x", &addr, &word) == 2) {
            fwrite(&word, sizeof(uint32_t), 1, output_fd);
            LogDebug("Wrote word @0x%06X = 0x%08X\n", addr, word);
            word_count++;
            if (word_count >= max_words) {
                LogVerbose("Reached maximum words defined by header.\n");
                break;
            }
        } else {
            LogInfo("(-) Malformed program body at line: %s", line);
            CleanAndExit(input_fd, output_fd);
            return EXIT_FAILURE;
        }
    }

    LogInfo("(*) Searching for entry label...\n");

    bool entry_found = false;
    char label[MAX_LABEL_NAME] = {0};
    uint32_t entry_offset = 0;

    while (fgets(line, MAX_LINE_LENGTH, input_fd)) {
        if (sscanf(line, "%[^:]: %u", label, &entry_offset) == 2) {
            char *trim = label + strlen(label) - 1;
            while (trim >= label && (*trim == '\n' || *trim == ' ' || *trim == '\r'))
                *trim-- = '\0';

            LogDebug("Found label '%s' at offset 0x%06X\n", label, entry_offset);

            if ((LINKER_FLAGS.entry_label && strcmp(label, LINKER_FLAGS.entry_label) == 0) ||
                (!LINKER_FLAGS.entry_label && strcmp(label, "START") == 0)) {
                header.entry_offset = entry_offset;
                entry_found = true;
                break;
            }
        }
    }

    if (!entry_found) {
        if (LINKER_FLAGS.entry_label)
            LogInfo("(-) Failed to find specified entry label: %s\n", LINKER_FLAGS.entry_label);
        else
            LogInfo("(-) Failed to find default entry label: START\n");

        CleanAndExit(input_fd, output_fd);
        return EXIT_FAILURE;
    }

    LogInfo("(*) Found entry point at address: 0x%06X\n", header.entry_offset);

    rewind(output_fd);
    LogDebug("Rewriting final SNX header...\n");
    fwrite(&header, sizeof(SNXHeader), 1, output_fd);

    LogInfo("(+) Packed %s successfully!\n", output_name);
    CleanAndExit(input_fd, output_fd);
    return EXIT_SUCCESS;
}