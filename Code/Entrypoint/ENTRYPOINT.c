#include <stdio.h>
#define _GNU_SOURCE
#include <bits/types/siginfo_t.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "Prompt/PROMPT_exports.h"
#include "RC/RC_exports.h"
#include "Runner/RUNNER_exports.h"
#include "Utils/UTILS_exports.h"

#define MAX_LINE (1024)
#define SEGFAULT_MESSAGE ("SEGFAULT!\n")
#define WRITE_PIPE_INDEX (1)
#define READ_PIPE_INDEX (0)
#define SIGINFO_SIZE (128)

static jmp_buf entrypoint__context_g = { 0 };

static void sigsegv_handler(int signal_number, siginfo_t *info, void *context)
{
    UTILS__unused(signal_number);
    UTILS__unused(info);
    UTILS__unused(context);

    (void)write(STDIN_FILENO, SEGFAULT_MESSAGE, sizeof(SEGFAULT_MESSAGE));

    longjmp(entrypoint__context_g, 1);
}

int main(void)
{
    RC_t rc = RC__UNINITIALIZED;
    char line[MAX_LINE] = { 0 };
    size_t line_length = MAX_LINE;
    const char *libraries[RUNNER__MAX_LIBRARY_COUNT] = { 0 };
    struct sigaction action = { 0 };
    int sigaction_result = UTILS__LIBC_ERROR;

    action.sa_sigaction = sigsegv_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigaction_result = sigaction(SIGSEGV, &action, NULL);
    if (UTILS__LIBC_ERROR == sigaction_result)
    {
        RC__SET_RC_AND_GOTO(rc, RC__ENTRYPOINT__FAILED_TO_REGISTER_SIGTERM_HANDLER, cleanup);
    }

    (void)RUNNER__init(libraries);

    setjmp(entrypoint__context_g);

    do
    {
        line_length = MAX_LINE;
        rc = PROMPT__get_line(line, &line_length);
        if (RC__PROMPT__EOF == rc)
        {
            break;
        }
        RC__ON_ERROR_GOTO(rc, cleanup);

        rc = RUNNER__run(line, line_length);
        RC__ON_ERROR_GOTO(rc, cleanup);
    } while (0 != line_length);

    rc = RC__SUCCESS;
cleanup:
    RUNNER__fini();
    return (int)rc;
}
