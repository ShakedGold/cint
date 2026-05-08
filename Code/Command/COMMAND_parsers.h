#pragma once

#include "Command/COMMAND_exports.h"
#include "Lexer/LEXER_exports.h"
#include "RC/RC_exports.h"

typedef RC_t (*command__parser_function_t)(COMMAND__command_t *command, LEXER__lexer_t *lexer);

RC_t command__parse_symbol_name(COMMAND__command_t *command, LEXER__lexer_t *lexer);
RC_t command__parse_int_literal(COMMAND__command_t *command, LEXER__lexer_t *lexer);
RC_t command__parse_string(COMMAND__command_t *command, LEXER__lexer_t *lexer);
RC_t command__parse_info(COMMAND__command_t *command, LEXER__lexer_t *lexer);

__attribute__((unused)) static command__parser_function_t command__parsers_g[] = {
    ['?'] = command__parse_info,

    [CLEX_id] = command__parse_symbol_name,
    [CLEX_intlit] = command__parse_int_literal,
    [CLEX_dqstring] = command__parse_string,

    [CLEX_first_unused_token] = NULL, // Protecting from out-of-memory bugs by setting the array size to
                                      // the maximum token possible
};
