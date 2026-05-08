#include <dlfcn.h>
#include <ffi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "COMMAND_exports.h"
#include "COMMAND_parsers.h"
#include "Lexer/LEXER_exports.h"
#include "RC/RC_exports.h"

typedef void (*command__callback_t)(void);

static RC_t command__parse_token(COMMAND__command_t *command, LEXER__lexer_t *lexer, LEXER__token_t token)
{
    RC_t rc = RC__UNINITIALIZED;

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(lexer, rc, cleanup);

    if (NULL == command__parsers_g[token])
    {
        RC__SET_RC_AND_GOTO(rc, RC__COMMAND__NULL_PARSER, cleanup);
    }

    rc = command__parsers_g[token](command, lexer);
    RC__ON_ERROR_GOTO(rc, cleanup);

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

static RC_t command__find_callback(
    COMMAND__command_t *command,
    void **shared_libraries,
    size_t shared_libraries_length,
    command__callback_t *callback
)
{
    RC_t rc = RC__UNINITIALIZED;
    void *dlsym_result = NULL;

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(shared_libraries, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(callback, rc, cleanup);

    for (size_t i = 0; i < shared_libraries_length; i++)
    {
        if (NULL == shared_libraries[i])
        {
            continue;
        }

        dlsym_result = dlsym(shared_libraries[i], command->symbol);
        if (NULL != dlsym_result)
        {
            break;
        }
    }

    (void)memcpy(callback, &dlsym_result, sizeof(dlsym_result));

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

static RC_t command__print_information(COMMAND__command_t *command, command__callback_t callback)
{
    RC_t rc = RC__UNINITIALIZED;

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(callback, rc, cleanup);

    (void)printf("Symbol(%s): %p\n", command->symbol, *(void **)&callback);

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

static RC_t command__run_command(COMMAND__command_t *command, command__callback_t callback)
{
    RC_t rc = RC__UNINITIALIZED;
    ffi_cif cif = { 0 };
    ffi_status ffi_result = FFI_BAD_ABI;
    // we add one more item because libffi requires a NULL to mark the end of the array
    ffi_type *args[COMMAND__MAX_ARGS_COUNT + 1] = { 0 };
    void *values[COMMAND__MAX_ARGS_COUNT + 1] = { 0 };

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(callback, rc, cleanup);

    for (size_t i = 0; i < command->current_arg_index; i++)
    {
        args[i] = command->args[i].type;
        values[i] = &command->args[i].value;
    }

    ffi_result = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, (unsigned int)command->current_arg_index, &ffi_type_void, args);

    if (FFI_OK != ffi_result)
    {
        RC__SET_RC_AND_GOTO(rc, RC__COMMAND__FUNCTION_SETUP_FAILED, cleanup);
    }

    ffi_call(&cif, callback, NULL, values);

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

static RC_t command__execute(COMMAND__command_t *command, command__callback_t callback)
{
    RC_t rc = RC__UNINITIALIZED;

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);

    if (NULL == callback)
    {
        (void)printf("Symbol(%s): not found\n", command->symbol);
        RC__SET_RC_AND_GOTO(rc, RC__SUCCESS, cleanup);
    }

    switch (command->type)
    {
    case COMMAND__COMMAND_TYPE__INFORMATION:
        rc = command__print_information(command, callback);
        RC__ON_ERROR_GOTO(rc, cleanup);
        break;
    case COMMAND__COMMAND_TYPE__RUN:
        rc = command__run_command(command, callback);
        RC__ON_ERROR_GOTO(rc, cleanup);
        break;
    }

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

RC_t COMMAND__parse(COMMAND__command_t *command, const char *line, size_t line_length)
{
    RC_t rc = RC__UNINITIALIZED;
    LEXER__token_t token = 0;
    LEXER__lexer_t lexer = { 0 };

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(line, rc, cleanup);

    rc = LEXER__new(&lexer, line, line_length);
    RC__ON_ERROR_GOTO(rc, cleanup);

    do
    {
        rc = LEXER__next_token(&lexer, &token);
        if (RC__LEXER__EOF == rc)
        {
            break;
        }

        RC__ON_ERROR_GOTO(rc, cleanup);

        rc = command__parse_token(command, &lexer, token);
        RC__ON_ERROR_GOTO(rc, cleanup);
    } while (CLEX_eof != token || command->current_arg_index < COMMAND__MAX_ARGS_COUNT);

    // For now, we silently ignore all arguments above COMMAND__MAX_ARGS_COUNT

    rc = RC__SUCCESS;
cleanup:
    return rc;
}

RC_t COMMAND__execute(COMMAND__command_t *command, void **shared_libraries, size_t shared_libraries_length)
{
    RC_t rc = RC__UNINITIALIZED;
    void (*callback)(void) = NULL;

    RC__IF_NULL_SET_AND_GOTO(command, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(shared_libraries, rc, cleanup);

    rc = command__find_callback(command, shared_libraries, shared_libraries_length, &callback);
    RC__ON_ERROR_GOTO(rc, cleanup);

    rc = command__execute(command, callback);
    RC__ON_ERROR_GOTO(rc, cleanup);

    rc = RC__SUCCESS;
cleanup:
    return rc;
}
