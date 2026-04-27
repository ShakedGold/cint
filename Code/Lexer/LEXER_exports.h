#pragma once

#include <stdlib.h>

#include "RC/RC_exports.h"
#include "WarnSuppress/WARN_SUPPRESS_exports.h"

#define STB_C_LEXER_IMPLEMENTATION
WARN_SUPPRESS__PUSH
WARN_SUPPRESS__IGNORE(WARN_SUPPRESS__IGNORE_UNUSED_FUNCTION)
WARN_SUPPRESS__IGNORE(WARN_SUPPRESS__IGNORE_UNUSED_VARIABLE)
WARN_SUPPRESS__IGNORE(WARN_SUPPRESS__IGNORE_CONVERSION)
#include "stb_c_lexer.h"
WARN_SUPPRESS__POP

#define LEXER__STRING_STORE_MAX_LENGTH (1024)

typedef int LEXER__token_t;

typedef struct LEXER__lexer_s
{
    stb_lexer lexer;
    char string_store[LEXER__STRING_STORE_MAX_LENGTH];
} LEXER__lexer_t;

RC_t LEXER__new(LEXER__lexer_t *lexer, const char *contents, size_t length);
RC_t LEXER__next_token(LEXER__lexer_t *context, LEXER__token_t *next_token);