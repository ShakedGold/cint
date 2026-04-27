#pragma once

#if defined(__clang__) || defined(__GNUC__)
#define WARN_SUPPRESS__PUSH \
    _Pragma("GCC diagnostic push")
#define WARN_SUPPRESS__POP \
    _Pragma("GCC diagnostic pop")
/* helper to turn string into pragma */
#define WARN_SUPPRESS__IGNORE_STR(x) _Pragma(x)

/* common ignores as string literals for use with WARN_SUPPRESS__IGNORE */
#define WARN_SUPPRESS__IGNORE_UNUSED_FUNCTION "GCC diagnostic ignored \"-Wunused-function\""
#define WARN_SUPPRESS__IGNORE_UNUSED_VARIABLE "GCC diagnostic ignored \"-Wunused-variable\""
#define WARN_SUPPRESS__IGNORE_SIGN_COMPARE "GCC diagnostic ignored \"-Wsign-compare\""
#define WARN_SUPPRESS__IGNORE_CONVERSION "GCC diagnostic ignored \"-Wconversion\""
#define WARN_SUPPRESS__IGNORE_ALL_PROTOTYPE_WARNINGS "GCC diagnostic ignored \"-Wmissing-prototypes\""
/* convenience: ignore one warning token (exact string for _Pragma) */
#define WARN_SUPPRESS__IGNORE(w) _Pragma(w)
#elif defined(_MSC_VER)
/* MSVC */
#define WARN_SUPPRESS__PUSH __pragma(warning(push))
#define WARN_SUPPRESS__POP __pragma(warning(pop))
#define WARN_SUPPRESS__IGNORE(w) __pragma(warning(disable : w))
#else
#define WARN_SUPPRESS__PUSH
#define WARN_SUPPRESS__POP
#define WARN_SUPPRESS__IGNORE(w)
#endif