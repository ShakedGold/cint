#include <string.h>
#include <ffi.h>

#include "Lexer/LEXER_exports.h"
#include "RUNNER_exports.h"

#define RUNNER__MAX_TOKEN_AMOUNT (256)
#define RUNNER__parse_arg(__command, __context, __arg, __ffi_type, __lexer_field) \
    do                                                                            \
    {                                                                             \
    } while (0)
#define RUNNER__is_token_argument(__token) (CLEX_intlit == (__token) || CLEX_dqstring == (__token))

static RC_t runner__parse_symbol_name(LEXER__lexer_t *context, RUNNER__command_t *command)
{
    RC_t rc = RC__UNINITIALIZED;

    RC__IF_NULL_SET_AND_GOTO(context, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);

    if (0 != *command->symbol)
    {
        RC__SET_RC_AND_GOTO(rc, RC__RUNNER__COMMAND_ALREADY_HAS_A_SYMBOL_NAME, cleanup);
    }

    if (context->lexer.string_len < 0)
    {
        RC__SET_RC_AND_GOTO(rc, RC__RUNNER__INVALID_ID_LENGTH, cleanup);
    }

    (void)memcpy(command->symbol, context->lexer.string, (size_t)context->lexer.string_len);

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

static RC_t runner__add_argument(LEXER__lexer_t *context, RUNNER__command_t *command, LEXER__token_t token)
{
    RC_t rc = RC__UNINITIALIZED;
    RUNNER__arg_t current_arg = {0};

    RC__IF_NULL_SET_AND_GOTO(context, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);

    switch (token)
    {
    case CLEX_id:
        rc = runner__parse_symbol_name(context, command);
        RC__ON_ERROR_GOTO(rc, cleanup);
        break;
    case CLEX_intlit:
        RUNNER__parse_arg(command, context, &current_arg, ffi_type_sint, int_number);
        current_arg.type = &ffi_type_sint;
        (void)memcpy(&current_arg.value, &context->lexer.int_number, sizeof(context->lexer.int_number));

        break;
    case CLEX_dqstring:
        current_arg.type = &ffi_type_pointer;
        current_arg.value = context->lexer.where_firstchar + 1;            // skip the "
        context->lexer.where_firstchar[context->lexer.string_len + 1] = 0; // put a null terminator on the closing "
        break;
    case CLEX_eof:
        break;
    default:
        (void)printf("Got unexpected token! (%ld)\n", token);
        break;
    }

    if (RUNNER__is_token_argument(token) && command->current_arg_index < RUNNER__MAX_ARGS_COUNT)
    {
        (void)memcpy(&command->args[command->current_arg_index], &current_arg, sizeof(current_arg));
        command->current_arg_index++;
    }

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

static RC_t runner__call(RUNNER__command_t *command)
{
    RC_t rc = RC__UNINITIALIZED;
    ffi_cif cif = {0};
    ffi_type *args[RUNNER__MAX_ARGS_COUNT] = {0};
    void *values[RUNNER__MAX_ARGS_COUNT] = {0};
    ffi_status ffi_result = FFI_BAD_ABI;

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);

    for (size_t i = 0; i < command->current_arg_index; i++)
    {
        args[i] = command->args[i].type;
        values[i] = &command->args[i].value;
    }

    ffi_result = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, (unsigned int)command->current_arg_index, &ffi_type_void, args);

    if (FFI_OK != ffi_result)
    {
        RC__SET_RC_AND_GOTO(rc, RC__RUNNER__FUNCTION_SETUP_FAILED, cleanup);
    }

    ffi_call(&cif, (void (*)(void))printf, NULL, values);

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

RC_t RUNNER__run(const char *command_line, size_t command_length)
{
    RC_t rc = RC__UNINITIALIZED;
    LEXER__lexer_t context = {0};
    LEXER__token_t next_token = {0};
    RUNNER__command_t command = {0};

    RC__IF_NULL_SET_AND_GOTO(command_line, rc, cleanup);

    rc = LEXER__new(&context, command_line, command_length);
    RC__ON_ERROR_GOTO(rc, cleanup);

    do
    {
        rc = LEXER__next_token(&context, &next_token);
        if (RC__LEXER__EOF != rc)
        {
            RC__ON_ERROR_GOTO(rc, cleanup);
        }

        rc = runner__add_argument(&context, &command, next_token);
        RC__ON_ERROR_GOTO(rc, cleanup);
    } while (CLEX_eof != next_token);

    rc = runner__call(&command);
    RC__ON_ERROR_GOTO(rc, cleanup);

    rc = RC__SUCCESS;
cleanup:
    return rc;
}