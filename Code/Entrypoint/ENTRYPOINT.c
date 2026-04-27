#include <stdbool.h>
#include <string.h>

#include "Prompt/PROMPT_exports.h"

#define MAX_LINE (1024)

int main(void)
{
    RC_t rc = RC__UNINITIALIZED;
    char line[MAX_LINE] = {0};
    size_t line_length = MAX_LINE;

    do
    {
        rc = PROMPT__get_line(line, &line_length);
        RC__ON_ERROR_GOTO(rc, cleanup);
    } while (0 != line_length);

    rc = RC__SUCCESS;
cleanup:
    return (int)rc;
}
