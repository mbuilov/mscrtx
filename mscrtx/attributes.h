#ifndef ATTRIBUTES_H_INCLUDED
#define ATTRIBUTES_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* attributes.h */

/* Function attributes.  */
#ifndef __attribute__
# if !defined(__GNUC__) && !defined(__clang__)
#  define __attribute__(arg)
# endif
#endif

/* Annotate function parameter that cannot be NULL.  */
#ifndef ATTRIBUTE_NONNULL
# define ATTRIBUTE_NONNULL(a) __attribute__ ((__nonnull__ (a)))
#endif

/* Must use MinGW.org/mingw-w64 variants of *printf functions,
   which support c99 "%zu"/"%llu" format specifiers.  */
#if defined(__USE_MINGW_ANSI_STDIO) && !__USE_MINGW_ANSI_STDIO
# error Must use MinGW.org/mingw-w64 variants of printf functions (define _POSIX_C_SOURCE)
#endif

/* Annotate parameters of printf-like function, e.g.:
  ATTRIBUTE_PRINTF(fmt, 2, 3) int my_printf(int param, const char *fmt, ...);
   or
  ATTRIBUTE_PRINTF(fmt, 2, 0) int my_vprintf(int param, const char *fmt, va_list ap); */
#ifndef ATTRIBUTE_PRINTF
# if defined(_PREFAST_)
/* MSVC */
#  define ATTRIBUTE_PRINTF(fmt, m, n) _At_(fmt, _Printf_format_string_ _Notnull_)
# elif defined(__Wformat_mingw_printf)
/* MinGW.org */
#  define ATTRIBUTE_PRINTF(fmt, m, n) __Wformat_mingw_printf(m, n) ATTRIBUTE_NONNULL(m)
# elif defined(__MINGW_PRINTF_FORMAT) && !defined(__clang__)
/* mingw-w64 */
#  define ATTRIBUTE_PRINTF(fmt, m, n) \
	__attribute__ ((__format__ (__MINGW_PRINTF_FORMAT, m, n))) ATTRIBUTE_NONNULL(m)
# else
/* generic gcc-compatible comiler (e.g., clang) */
#  define ATTRIBUTE_PRINTF(fmt, m, n) \
	__attribute__ ((__format__ (__printf__, m, n))) ATTRIBUTE_NONNULL(m)
# endif
#endif

/* Annotate parameters of printf-like function pointer, e.g.:
  ATTRIBUTE_PRINTF_PTR(fmt, 2, 3) int (*my_printf)(int param, const char *fmt, ...);
   or
  ATTRIBUTE_PRINTF_PTR(fmt, 2, 0) int (*my_vprintf)(int param, const char *fmt, va_list ap); */
#ifndef ATTRIBUTE_PRINTF_PTR
# if defined(__GNUC__) && __GNUC__ > 3 - (__GNUC_MINOR__ >= 2)
#  define ATTRIBUTE_PRINTF_PTR(fmt, m, n) ATTRIBUTE_PRINTF(fmt, m, n)
# else
#  define ATTRIBUTE_PRINTF_PTR(fmt, m, n)
# endif
#endif

/* Annotate parameters of wprintf-like function, e.g.:
  ATTRIBUTE_WPRINTF(fmt, 2, 3) int my_wprintf(int param, const wchar_t *fmt, ...);
   or
  ATTRIBUTE_WPRINTF(fmt, 2, 0) int my_vwprintf(int param, const wchar_t *fmt, va_list ap); */
#ifndef ATTRIBUTE_WPRINTF
# if defined(_PREFAST_)
/* MSVC */
#  define ATTRIBUTE_WPRINTF(fmt, m, n) _At_(fmt, _Printf_format_string_ _Notnull_)
# elif defined(__Wformat_mingw_printf)
/* MinGW.org */
#  define ATTRIBUTE_WPRINTF(fmt, m, n) /*__Wformat_mingw_printf(m, n)*/ ATTRIBUTE_NONNULL(m)
# elif defined(__MINGW_PRINTF_FORMAT) && !defined(__clang__)
/* mingw-w64 */
#  define ATTRIBUTE_WPRINTF(fmt, m, n) \
	/*__attribute__ ((__format__ (__MINGW_PRINTF_FORMAT, m, n)))*/ ATTRIBUTE_NONNULL(m)
# else
/* generic gcc-compatible comiler (e.g., clang) */
#  define ATTRIBUTE_WPRINTF(fmt, m, n) \
	/*__attribute__ ((__format__ (__printf__, m, n)))*/ ATTRIBUTE_NONNULL(m)
# endif
#endif

#endif /* ATTRIBUTES_H_INCLUDED */
