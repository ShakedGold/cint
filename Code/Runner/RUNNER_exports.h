#pragma once
#include <ffi.h>
#include <stdbool.h>

#include "RC/RC_exports.h"

#define RUNNER__MAX_SYMBOL_LENGTH (256)
#define RUNNER__MAX_ARGS_COUNT (256)
#define RUNNER__MAX_LIBRARY_COUNT (256)

typedef enum RUNNER__command_type_e
{
    RUNNER__COMMAND_TYPE__RUN = 0,
    RUNNER__COMMAND_TYPE__INFORMATION
} RUNNER__command_type_t;

typedef struct RUNNER__arg_s
{
    ffi_type *type;
    void *value;
} RUNNER__arg_t;

typedef struct RUNNER__command_s
{
    RUNNER__command_type_t type;

    char symbol[RUNNER__MAX_SYMBOL_LENGTH];
    RUNNER__arg_t args[RUNNER__MAX_ARGS_COUNT];
    size_t current_arg_index;
} RUNNER__command_t;

typedef struct RUNNER__context_s
{
    void *shared_libraries[RUNNER__MAX_LIBRARY_COUNT];
    bool is_self_loaded;
} RUNNER__context_t;

RC_t RUNNER__run(const char *command_line, size_t command_length);
RC_t RUNNER__init(const char *libraries[RUNNER__MAX_LIBRARY_COUNT]);
void RUNNER__fini();