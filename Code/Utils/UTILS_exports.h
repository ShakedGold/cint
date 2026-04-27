#pragma once

#define UTILS__DEFER(__func) __attribute__((cleanup(__func)))

#define UTILS__DEFER_FREE __attribute__((cleanup(UTILS__cleanup_free)))

static void UTILS__cleanup_free(void *p)
{
    void **ptr = NULL;

    if (NULL == p)
    {
        RC__GOTO(RC__NULL_POINTER_EXCEPTION, cleanup);
    }

    ptr = (void **)p;
    if (NULL != *ptr)
    {
        free(*ptr);
        *ptr = NULL;
    }

cleanup:
    return;
}