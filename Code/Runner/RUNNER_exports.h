#pragma once
#include <stdbool.h>

#include "RC/RC_exports.h"

#define RUNNER__MAX_LIBRARY_COUNT (256)

typedef struct RUNNER__context_s
{
    void *shared_libraries[RUNNER__MAX_LIBRARY_COUNT];
    bool is_self_loaded;
} RUNNER__context_t;

RC_t RUNNER__run(const char *command_line, size_t command_length);
RC_t RUNNER__init(const char *libraries[RUNNER__MAX_LIBRARY_COUNT]);
void RUNNER__fini();
