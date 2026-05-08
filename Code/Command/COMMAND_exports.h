#pragma once

#include <ffi.h>
#include <stddef.h>
#include <stdlib.h>

#include "RC/RC_exports.h"

#define COMMAND__MAX_SYMBOL_LENGTH (256)
#define COMMAND__MAX_ARGS_COUNT (256)

typedef enum COMMAND__command_type_e
{
    COMMAND__COMMAND_TYPE__RUN = 0,
    COMMAND__COMMAND_TYPE__INFORMATION
} COMMAND__command_type_t;

typedef struct COMMAND__arg_s
{
    ffi_type *type;
    void *value;
} COMMAND__arg_t;

typedef struct COMMAND__command_s
{
    COMMAND__command_type_t type;

    char symbol[COMMAND__MAX_SYMBOL_LENGTH];
    COMMAND__arg_t args[COMMAND__MAX_ARGS_COUNT];
    size_t current_arg_index;
} COMMAND__command_t;

RC_t COMMAND__parse(COMMAND__command_t *command, const char *line, size_t line_length);
RC_t COMMAND__execute(COMMAND__command_t *command, void **shared_libraries, size_t shared_libraries_length);
