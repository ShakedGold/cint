#include <dlfcn.h>
#include <ffi.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "Command/COMMAND_exports.h"
#include "RC/RC_exports.h"
#include "RUNNER_exports.h"
#include "Utils/UTILS_exports.h"

static RUNNER__context_t runner__context_g = { 0 };

RC_t RUNNER__run(const char *command_line, size_t command_length)
{
    RC_t rc = RC__UNINITIALIZED;
    COMMAND__command_t command = { 0 };

    RC__IF_NULL_SET_AND_GOTO(command_line, rc, cleanup);

    rc = COMMAND__parse(&command, command_line, command_length);
    RC__ON_ERROR_GOTO(rc, cleanup);

    rc = COMMAND__execute(&command, runner__context_g.shared_libraries, RUNNER__MAX_LIBRARY_COUNT);
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
