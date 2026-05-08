#include "COMMAND_parsers.h"
#include "COMMAND_exports.h"
#include "Lexer/LEXER_exports.h"
#include "RC/RC_exports.h"
#include "ffi.h"
#include <string.h>

RC_t command__parse_symbol_name(COMMAND__command_t *command, LEXER__lexer_t *lexer)
{
    RC_t rc = RC__UNINITIALIZED;

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(lexer, rc, cleanup);

    if (0 != *command->symbol)
    {
        RC__SET_RC_AND_GOTO(rc, RC__COMMAND__SYMBOL_ALREADY_EXISTS, cleanup);
    }

    if (lexer->lexer.string_len < 0)
    {
        RC__SET_RC_AND_GOTO(rc, RC__COMMAND__SYMBOL_INVALID_LENGTH, cleanup);
    }

    (void)memcpy(command->symbol, lexer->lexer.string, (size_t)lexer->lexer.string_len);

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

RC_t command__parse_int_literal(COMMAND__command_t *command, LEXER__lexer_t *lexer)
{
    RC_t rc = RC__UNINITIALIZED;
    COMMAND__arg_t *current_arg = NULL;

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(lexer, rc, cleanup);

    current_arg = &command->args[command->current_arg_index];

    current_arg->type = &ffi_type_sint;
    (void)memcpy(&current_arg->value, &lexer->lexer.int_number, sizeof(lexer->lexer.int_number));

    command->current_arg_index++;

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

RC_t command__parse_string(COMMAND__command_t *command, LEXER__lexer_t *lexer)
{
    RC_t rc = RC__UNINITIALIZED;
    COMMAND__arg_t *current_arg = NULL;

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(lexer, rc, cleanup);

    current_arg = &command->args[command->current_arg_index];

    current_arg->type = &ffi_type_pointer;
    current_arg->value = lexer->lexer.where_firstchar + 1;         // skip the "
    lexer->lexer.where_firstchar[lexer->lexer.string_len + 1] = 0; // put a null terminator on the closing "

    command->current_arg_index++;

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

RC_t command__parse_info(COMMAND__command_t *command, LEXER__lexer_t *lexer)
{
    RC_t rc = RC__UNINITIALIZED;

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(lexer, rc, cleanup);

    command->type = COMMAND__COMMAND_TYPE__INFORMATION;

    // Invalidating the lexer, so the next token is EOF
    lexer->lexer.parse_point = lexer->lexer.eof;

    rc = RC__SUCCESS;
cleanup:
    return rc;
}
