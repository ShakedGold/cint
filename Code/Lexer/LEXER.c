#include "LEXER_exports.h"
#include "RC/RC_exports.h"

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
    LEXER__token_t token = CLEX_eof;

    RC__IF_NULL_SET_AND_GOTO(context, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(next_token, rc, cleanup);

    token = stb_c_lexer_get_token(&context->lexer);

    if (CLEX_eof == token)
    {
        RC__SET_RC_AND_GOTO(rc, RC__LEXER__EOF, cleanup);
    }

    rc = RC__SUCCESS;
cleanup:
    // We want to invalidate the token even when we failed to lex it
    if (NULL != next_token)
    {
        *next_token = token;
    }

    return rc;
}
