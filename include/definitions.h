#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// Global includes

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "logger.h"
#include "flags.h"

// Constants

#define SNX_MAGIC       0x534E5801 
#define VERSION                  1
#define MAX_LINE_LENGTH        128
#define MAX_FILENAME_LENGTH    128
#define MAX_LABEL_NAME          31

#define STATUS_ERROR            -1

#define OBJECT_EX           ".sno"
#define ENTRIES_EX          ".sne"

#define OBJ_HEADER_DELIM       '|'

#endif