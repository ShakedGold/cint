#pragma once
#include <ffi.h>

#include "RC/RC_exports.h"

#define RUNNER__MAX_SYMBOL_LENGTH (256)
#define RUNNER__MAX_ARGS_COUNT (256)

typedef struct RUNNER__arg_s
{
    ffi_type *type;
    void *value;
} RUNNER__arg_t;

typedef struct RUNNER__command_s
{
    char symbol[RUNNER__MAX_SYMBOL_LENGTH];

    RUNNER__arg_t args[RUNNER__MAX_ARGS_COUNT];
    size_t current_arg_index;
} RUNNER__command_t;

RC_t RUNNER__run(const char *command_line, size_t command_length);