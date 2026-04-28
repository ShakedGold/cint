#include "LEXER_exports.h"
#include "RC/RC_exports.h"

#define STB_C_LEXER_IMPLEMENTATION
WARN_SUPPRESS__PUSH
WARN_SUPPRESS__IGNORE(WARN_SUPPRESS__IGNORE_UNUSED_FUNCTION)
WARN_SUPPRESS__IGNORE(WARN_SUPPRESS__IGNORE_UNUSED_VARIABLE)
WARN_SUPPRESS__IGNORE(WARN_SUPPRESS__IGNORE_CONVERSION)
#include "stb_c_lexer.h"
WARN_SUPPRESS__POP

RC_t LEXER__new(LEXER__lexer_t *context, const char *contents, size_t length)
{
    RC_t rc = RC__UNINITIALIZED;

    RC__IF_NULL_SET_AND_GOTO(context, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(contents, rc, cleanup);

    stb_c_lexer_init(&context->lexer, contents, contents + length, context->string_store, sizeof(context->string_store));

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

RC_t LEXER__next_token(LEXER__lexer_t *context, LEXER__token_t *next_token)
{
    RC_t rc = RC__UNINITIALIZED;
    int get_token_result = -1;

    RC__IF_NULL_SET_AND_GOTO(context, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(next_token, rc, cleanup);

    get_token_result = stb_c_lexer_get_token(&context->lexer);
    if (-1 == get_token_result)
    {
        RC__SET_RC_AND_GOTO(rc, RC__LEXER__GET_NEXT_TOKEN_FAILED, cleanup);
    }

    if (CLEX_eof == context->lexer.token)
    {
        RC__SET_RC_AND_GOTO_NO_PRINT(rc, RC__LEXER__EOF, cleanup);
    }

    rc = RC__SUCCESS;
cleanup:
    // We want to invalidate the token even when we failed to lex it
    if (NULL != next_token)
    {
        *next_token = context->lexer.token;
    }

    return rc;
}
