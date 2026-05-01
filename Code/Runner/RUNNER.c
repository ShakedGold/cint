#include <dlfcn.h>
#include <string.h>
#include <stdint.h>
#include <ffi.h>

#include "Lexer/LEXER_exports.h"
#include "Utils/UTILS_exports.h"
#include "RUNNER_exports.h"

#define RUNNER__MAX_TOKEN_AMOUNT (256)
#define RUNNER__is_token_argument(__token) (CLEX_intlit == (__token) || CLEX_dqstring == (__token))

static RUNNER__context_t runner__context_g = {0};

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
    // Special case to print information on the symbol
    case '?':
        command->type = RUNNER__COMMAND_TYPE__INFORMATION;
        break;
    default:
        (void)printf("Got unexpected token! (%ld) %c\n", token, (char)token);
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

static RC_t runner__run_command(RUNNER__command_t *command, void *callback)
{
    RC_t rc = RC__UNINITIALIZED;
    ffi_cif cif = {0};
    ffi_status ffi_result = FFI_BAD_ABI;
    // we add one more item because libffi requires a NULL to mark the end of the array
    ffi_type *args[RUNNER__MAX_ARGS_COUNT + 1] = {0};
    void *values[RUNNER__MAX_ARGS_COUNT + 1] = {0};
    void (*function)(void) = NULL;

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);

    if (NULL == callback)
    {
        RC__SET_RC_AND_GOTO_NO_PRINT(rc, RC__RUNNER__FAILED_TO_FIND_FUNCTION_SYMBOL, cleanup);
    }

    (void)memcpy(&function, &callback, sizeof(function));

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

    ffi_call(&cif, function, NULL, values);

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

static RC_t runner__print_information(RUNNER__command_t *command, void *callback)
{
    RC_t rc = RC__UNINITIALIZED;

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);

    if (NULL == callback)
    {
        (void)printf("Symbol(%s): not found\n", command->symbol);
    }
    else
    {
        (void)printf("Symbol(%s): %p\n", command->symbol, callback);
    }

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

static RC_t runner__call(RUNNER__command_t *command)
{
    RC_t rc = RC__UNINITIALIZED;
    void *dlsym_result = NULL;

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);

    for (size_t i = 0; i < UTILS__arr_len(runner__context_g.shared_libraries); i++)
    {
        if (NULL == runner__context_g.shared_libraries[i])
        {
            continue;
        }

        dlsym_result = dlsym(runner__context_g.shared_libraries[i], command->symbol);
        if (NULL != dlsym_result)
        {
            break;
        }
    }

    switch (command->type)
    {
    case RUNNER__COMMAND_TYPE__RUN:
        rc = runner__run_command(command, dlsym_result);
        RC__ON_ERROR_GOTO_NO_PRINT(rc, cleanup);
        break;
    case RUNNER__COMMAND_TYPE__INFORMATION:
        rc = runner__print_information(command, dlsym_result);
        RC__ON_ERROR_GOTO(rc, cleanup);
        break;
    }

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

    if (RC__RUNNER__FAILED_TO_FIND_FUNCTION_SYMBOL == rc)
    {
        (void)printf("Symbol(%s): not found\n", command.symbol);
        RC__SET_RC_AND_GOTO(rc, RC__SUCCESS, cleanup);
    }

    RC__ON_ERROR_GOTO(rc, cleanup);

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

RC_t RUNNER__init(const char *libraries[RUNNER__MAX_LIBRARY_COUNT])
{
    RC_t rc = RC__UNINITIALIZED;
    void *current_so = NULL;

    for (size_t i = 0; i < UTILS__arr_len(runner__context_g.shared_libraries); i++)
    {
        if (NULL == libraries[i] && runner__context_g.is_self_loaded)
        {
            continue;
        }

        current_so = dlopen(libraries[i], RTLD_NOW);
        if (NULL == current_so)
        {
            RC__SET_RC_AND_GOTO(rc, RC__RUNNER__FAILED_TO_OPEN_SHARED_OBJECT, cleanup);
        }

        if (NULL == libraries[i])
        {
            runner__context_g.is_self_loaded = true;
        }

        runner__context_g.shared_libraries[i] = current_so;
    }

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

void RUNNER__fini()
{
    int dlclose_result = UTILS__LIBC_ERROR;

    for (size_t i = 0; i < UTILS__arr_len(runner__context_g.shared_libraries); i++)
    {
        if (NULL == runner__context_g.shared_libraries[i])
        {
            continue;
        }

        dlclose_result = dlclose(runner__context_g.shared_libraries[i]);

        // We want to best-effort dlclose all shared objects
        if (UTILS__LIBC_ERROR == dlclose_result)
        {
            continue;
        }
    }
}