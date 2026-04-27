#pragma once

#include <stdio.h>

#define RC__GOTO(__rc, __label)                                                                   \
    do                                                                                            \
    {                                                                                             \
        if (RC__SUCCESS != (__rc))                                                                \
        {                                                                                         \
            (void)printf("RC %s (%s:%d)[%s]", RC__strings[(__rc)], __FILE__, __LINE__, __func__); \
        }                                                                                         \
        goto __label;                                                                             \
    } while (0)

#define RC__SET_RC_AND_GOTO(__rc, __rc_value, __label) \
    do                                                 \
    {                                                  \
        __rc = __rc_value;                             \
        RC__GOTO(__rc_value, __label);                 \
    } while (0)

#define RC__IF_NULL_SET_AND_GOTO(__expression, __rc, __label)               \
    do                                                                      \
    {                                                                       \
        if (NULL == (__expression))                                         \
        {                                                                   \
            RC__SET_RC_AND_GOTO(__rc, RC__NULL_POINTER_EXCEPTION, __label); \
        }                                                                   \
    } while (0)

#define RC__ON_ERROR_GOTO(__rc, __label) \
    do                                   \
    {                                    \
        if (RC__SUCCESS != (__rc))       \
        {                                \
            RC__GOTO(__rc, __label);     \
        }                                \
    } while (0)

typedef enum RC_s
{
    RC__UNINITIALIZED = -1,
    RC__SUCCESS,
    RC__NULL_POINTER_EXCEPTION,

#define RC(__module, __rc) RC__##__module##__##__rc,
#include "RC.hx"
#undef RC

    RC__MAX,
} RC_t;

static char *RC__strings[] = {
    [RC__SUCCESS] = "RC__SUCCESS",
    [RC__NULL_POINTER_EXCEPTION] = "RC__NULL_POINTER_EXCEPTION",
#define RC(__module, __rc) [RC__##__module##__##__rc] = "RC__" #__module "__" #__rc,
#include "RC.hx"
#undef RC
    [RC__MAX] = "RC__MAX"};