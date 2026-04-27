#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "readline/readline.h"
#include "readline/history.h"

#include "PROMPT_exports.h"
#include "RC/RC_exports.h"
#include "Utils/UTILS_exports.h"

#define PROMPT__PROMPT ("@ ")

RC_t PROMPT__get_line(char *line, size_t *line_length)
{
    RC_t rc = RC__UNINITIALIZED;
    UTILS__DEFER_FREE char *out_line = NULL;

    RC__IF_NULL_SET_AND_GOTO(line, rc, cleanup);
    RC__IF_NULL_SET_AND_GOTO(line_length, rc, cleanup);

    out_line = readline(PROMPT__PROMPT);

    if (NULL == out_line)
    {
        *line_length = 0;
        RC__SET_RC_AND_GOTO(rc, RC__SUCCESS, cleanup);
    }

    add_history(out_line);

    (void)strncpy(line, out_line, *line_length);
    *line_length = strnlen(out_line, *line_length);

    rc = RC__SUCCESS;
cleanup:
    return rc;
}