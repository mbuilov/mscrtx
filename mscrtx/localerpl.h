#ifndef LOCALERPL_H_INCLUDED
#define LOCALERPL_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* localerpl.h */

/* Replace locale-dependent standard C library API.  */

/* No standard headers should be processed after this file - include them now
   in the hope for include guards */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <wchar.h>
#include <locale.h>
#include <direct.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <process.h>
#include <time.h>

/* for ATTRIBUTE_PRINTF */
#include "mscrtx/attributes.h"

/* in UTF-8 locale MB_CUR_MAX == 4 */
#if defined MB_LEN_MAX && MB_LEN_MAX < 4
# undef MB_LEN_MAX
#endif
#ifndef MB_LEN_MAX
# define MB_LEN_MAX 4
#endif

struct tm;
struct __stat64;
typedef unsigned short c32ctype_t;

/* Change behaviour of localerpl wrappers.  */
void localerpl_change(int to_utf8);

/* Returns non-zero if using UTF-8 replacements.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_is_utf8(void);

/* Returns locale code page number (based on the value of LC_CTYPE).
  For example, for UTF-8 locale, code page number is 65001.  */
/* Returns 0 if using "C" locale.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
unsigned get_locale_code_page(void);

/* wcslen(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(str, A_In_z)
#endif
size_t localerpl_c32slen(const unsigned *str);

#ifndef localerpl_do_not_redefine_c32slen
# ifndef LOCALE_RPL_IMPL
#  ifdef c32slen
#   undef c32slen
#  endif
#  define c32slen localerpl_c32slen
# endif
#endif

/* wcschr(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_In_z)
A_Ret_maybenull
#endif
unsigned *localerpl_c32schr(const unsigned *s, unsigned c);

#ifndef localerpl_do_not_redefine_c32schr
# ifndef LOCALE_RPL_IMPL
#  ifdef c32schr
#   undef c32schr
#  endif
#  define c32schr localerpl_c32schr
# endif
#endif

/* wcsrchr(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_In_z)
A_Ret_maybenull
#endif
unsigned *localerpl_c32srchr(const unsigned *s, unsigned c);

#ifndef localerpl_do_not_redefine_c32srchr
# ifndef LOCALE_RPL_IMPL
#  ifdef c32srchr
#   undef c32srchr
#  endif
#  define c32srchr localerpl_c32srchr
# endif
#endif

/* strchrnul(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_In_z)
A_Ret_never_null
A_Ret_z
#endif
unsigned *localerpl_c32schrnul(const unsigned *s, unsigned c);

#ifndef localerpl_do_not_redefine_c32schrnul
# ifndef LOCALE_RPL_IMPL
#  ifdef c32schrnul
#   undef c32schrnul
#  endif
#  define c32schrnul localerpl_c32schrnul
# endif
#endif

/* strcmp(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s1, A_In_z)
A_At(s2, A_In_z)
#endif
int localerpl_c32scmp(const unsigned *s1, const unsigned *s2);

#ifndef localerpl_do_not_redefine_c32scmp
# ifndef LOCALE_RPL_IMPL
#  ifdef c32scmp
#   undef c32scmp
#  endif
#  define c32scmp localerpl_c32scmp
# endif
#endif

/* note: wide-character string formats, e.g. "%ls"/"%lc"/"%ws"/"%S", are not supported */
ATTRIBUTE_PRINTF(format, 1, 2)
int localerpl_printf(const char *format, ...);

#if !defined(_MSC_VER) || !defined(_PREFAST_)
#ifndef localerpl_do_not_redefine_printf
# ifndef LOCALE_RPL_IMPL
#  ifdef printf
#   undef printf
#  endif
#  define printf localerpl_printf
# endif
#endif
#endif

/* note: wide-character string formats, e.g. "%ls"/"%lc"/"%ws"/"%S", are not supported */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_arg(1)
#endif
ATTRIBUTE_PRINTF(format, 2, 3)
int localerpl_fprintf(FILE *stream, const char *format, ...);

#if !defined(_MSC_VER) || !defined(_PREFAST_)
#ifndef localerpl_do_not_redefine_fprintf
# ifndef LOCALE_RPL_IMPL
#  ifdef fprintf
#   undef fprintf
#  endif
#  define fprintf localerpl_fprintf
# endif
#endif
#endif

/* note: wide-character string formats, e.g. "%ls"/"%lc"/"%ws"/"%S", are not supported */
ATTRIBUTE_PRINTF(format, 1, 0)
int localerpl_vprintf(const char *format, va_list ap);

#ifndef localerpl_do_not_redefine_vprintf
# ifndef LOCALE_RPL_IMPL
#  ifdef vprintf
#   undef vprintf
#  endif
#  define vprintf localerpl_vprintf
# endif
#endif

/* note: wide-character string formats, e.g. "%ls"/"%lc"/"%ws"/"%S", are not supported */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_arg(1)
#endif
ATTRIBUTE_PRINTF(format, 2, 0)
int localerpl_vfprintf(FILE *stream, const char *format, va_list ap);

#ifndef localerpl_do_not_redefine_vfprintf
# ifndef LOCALE_RPL_IMPL
#  ifdef vfprintf
#   undef vfprintf
#  endif
#  define vfprintf localerpl_vfprintf
# endif
#endif

/* note: multibyte-character string formats, e.g. "%s"/"%c"/"%hs"/"%S", are not supported */
ATTRIBUTE_WPRINTF(format, 1, 2)
int localerpl_wprintfmb(const wchar_t *format, ...);

#ifndef localerpl_do_not_redefine_wprintfmb
# ifndef LOCALE_RPL_IMPL
#  ifdef wprintfmb
#   undef wprintfmb
#  endif
#  define wprintfmb localerpl_wprintfmb
# endif
#endif

/* note: multibyte-character string formats, e.g. "%s"/"%c"/"%hs"/"%S", are not supported */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_arg(1)
#endif
ATTRIBUTE_WPRINTF(format, 2, 3)
int localerpl_fwprintfmb(FILE *stream, const wchar_t *format, ...);

#ifndef localerpl_do_not_redefine_fwprintfmb
# ifndef LOCALE_RPL_IMPL
#  ifdef fwprintfmb
#   undef fwprintfmb
#  endif
#  define fwprintfmb localerpl_fwprintfmb
# endif
#endif

/* note: multibyte-character string formats, e.g. "%s"/"%c"/"%hs"/"%S", are not supported */
ATTRIBUTE_WPRINTF(format, 1, 0)
int localerpl_vwprintfmb(const wchar_t *format, va_list ap);

#ifndef localerpl_do_not_redefine_vwprintfmb
# ifndef LOCALE_RPL_IMPL
#  ifdef vwprintfmb
#   undef vwprintfmb
#  endif
#  define vwprintfmb localerpl_vwprintfmb
# endif
#endif

/* note: multibyte-character string formats, e.g. "%s"/"%c"/"%hs"/"%S", are not supported */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_arg(1)
#endif
ATTRIBUTE_WPRINTF(format, 2, 0)
int localerpl_vfwprintfmb(FILE *stream, const wchar_t *format, va_list ap);

#ifndef localerpl_do_not_redefine_vfwprintfmb
# ifndef LOCALE_RPL_IMPL
#  ifdef vfwprintfmb
#   undef vfwprintfmb
#  endif
#  define vfwprintfmb localerpl_vfwprintfmb
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(name, A_In_z)
A_Success(return >= 0)
#endif
int localerpl_open(const char *name, int flags, ...);

#ifndef localerpl_do_not_redefine_open
# ifndef LOCALE_RPL_IMPL
#  ifdef open
#   undef open
#  endif
#  define open localerpl_open
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(path, A_In_z)
A_At(mode, A_In_z)
A_Success(return)
#endif
FILE *localerpl_fopen(const char *path, const char *mode);

#ifndef localerpl_do_not_redefine_fopen
# ifndef LOCALE_RPL_IMPL
#  ifdef fopen
#   undef fopen
#  endif
#  define fopen localerpl_fopen
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(fd, A_In_range(>=,0))
A_When(count, A_At(buf, A_Notnull))
A_When(count, A_At(buf, A_Pre_writable_byte_size(count)))
A_When(count, A_At(buf, A_Post_readable_byte_size(return)))
A_At(count, A_In_range(<=,INT_MAX))
A_Success(return >= 0)
#endif
int localerpl_read(int fd, void *buf, unsigned count);

#ifndef localerpl_do_not_redefine_read
# ifndef LOCALE_RPL_IMPL
#  ifdef read
#   undef read
#  endif
#  define read localerpl_read
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(fd, A_In_range(>=,0))
A_When(count, A_At(buf, A_In_reads_bytes(count)))
A_At(count, A_In_range(<=,INT_MAX))
A_Success(return == (int)count)
#endif
int localerpl_write(int fd, const void *buf, unsigned count);

#ifndef localerpl_do_not_redefine_write
# ifndef LOCALE_RPL_IMPL
#  ifdef write
#   undef write
#  endif
#  define write localerpl_write
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(4)
A_At(stream, A_In)
A_When(size && nmemb, A_At(buf, A_Notnull))
A_When(size && nmemb, A_At(buf, A_Pre_writable_byte_size(size*nmemb)))
A_When(size && nmemb, A_At(buf, A_Post_readable_byte_size(size*return)))
#endif
size_t localerpl_fread(void *buf, size_t size, size_t nmemb, FILE *stream);

#ifndef localerpl_do_not_redefine_fread
# ifndef LOCALE_RPL_IMPL
#  ifdef fread
#   undef fread
#  endif
#  define fread localerpl_fread
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(4)
A_At(stream, A_In)
A_When(size && nmemb, A_At(buf, A_In_reads_bytes(size*nmemb)))
A_Success(return == nmemb)
#endif
size_t localerpl_fwrite(const void *buf, size_t size, size_t nmemb, FILE *stream);

#ifndef localerpl_do_not_redefine_fwrite
# ifndef LOCALE_RPL_IMPL
#  ifdef fwrite
#   undef fwrite
#  endif
#  define fwrite localerpl_fwrite
# endif
#endif

int localerpl_putchar(int c);

#ifndef localerpl_do_not_redefine_putchar
# ifndef LOCALE_RPL_IMPL
#  ifdef putchar
#   undef putchar
#  endif
#  define putchar localerpl_putchar
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(stream, A_In)
#endif
int localerpl_fputc(int c, FILE *stream);

#ifndef localerpl_do_not_redefine_fputc
# ifndef LOCALE_RPL_IMPL
#  ifdef fputc
#   undef fputc
#  endif
#  define fputc localerpl_fputc
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_getchar(void);

#ifndef localerpl_do_not_redefine_getchar
# ifndef LOCALE_RPL_IMPL
#  ifdef getchar
#   undef getchar
#  endif
#  define getchar localerpl_getchar
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(stream, A_In)
#endif
int localerpl_fgetc(FILE *stream);

#ifndef localerpl_do_not_redefine_fgetc
# ifndef LOCALE_RPL_IMPL
#  ifdef fgetc
#   undef fgetc
#  endif
#  define fgetc localerpl_fgetc
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(s, A_In_z)
#endif
int localerpl_puts(const char *s);

#ifndef localerpl_do_not_redefine_puts
# ifndef LOCALE_RPL_IMPL
#  ifdef puts
#   undef puts
#  endif
#  define puts localerpl_puts
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(s, A_In_z)
A_At(stream, A_In)
#endif
int localerpl_fputs(const char *s, FILE *stream);

#ifndef localerpl_do_not_redefine_fputs
# ifndef LOCALE_RPL_IMPL
#  ifdef fputs
#   undef fputs
#  endif
#  define fputs localerpl_fputs
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_Notnull)
A_At(s, A_Pre_writable_byte_size(size))
A_At(s, A_Post_z)
A_At(size, A_In_range(>,0))
A_At(stream, A_In)
A_Success(return)
A_Ret_z
A_Post_satisfies(return == s)
#endif
char *localerpl_fgets(char *s, int size, FILE *stream);

#ifndef localerpl_do_not_redefine_fgets
# ifndef LOCALE_RPL_IMPL
#  ifdef fgets
#   undef fgets
#  endif
#  define fgets localerpl_fgets
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(dirname, A_In_z)
A_Success(!return)
#endif
int localerpl_mkdir(const char *dirname);

#ifndef localerpl_do_not_redefine_mkdir
# ifndef LOCALE_RPL_IMPL
#  ifdef mkdir
#   undef mkdir
#  endif
#  define mkdir localerpl_mkdir
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(dirname, A_In_z)
A_Success(!return)
#endif
int localerpl_rmdir(const char *dirname);

#ifndef localerpl_do_not_redefine_rmdir
# ifndef LOCALE_RPL_IMPL
#  ifdef rmdir
#   undef rmdir
#  endif
#  define rmdir localerpl_rmdir
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(pathname, A_In_z)
A_Success(!return)
#endif
int localerpl_remove(const char *pathname);

#ifndef localerpl_do_not_redefine_remove
# ifndef LOCALE_RPL_IMPL
#  ifdef remove
#   undef remove
#  endif
#  define remove localerpl_remove
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(pathname, A_In_z)
A_Success(!return)
#endif
int localerpl_unlink(const char *pathname);

#ifndef localerpl_do_not_redefine_unlink
# ifndef LOCALE_RPL_IMPL
#  ifdef unlink
#   undef undef
#  endif
#  define unlink localerpl_unlink
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(old_name, A_In_z)
A_At(new_name, A_In_z)
A_Success(!return)
#endif
int localerpl_rename(const char *old_name, const char *new_name);

#ifndef localerpl_do_not_redefine_rename
# ifndef LOCALE_RPL_IMPL
#  ifdef rename
#   undef rename
#  endif
#  define rename localerpl_rename
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(path, A_In_z)
A_Success(!return)
#endif
int localerpl_chdir(const char *path);

#ifndef localerpl_do_not_redefine_chdir
# ifndef LOCALE_RPL_IMPL
#  ifdef chdir
#   undef chdir
#  endif
#  define chdir localerpl_chdir
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(path, A_In_z)
A_At(buf, A_Out)
A_Success(!return)
#endif
int localerpl_stat(const char *path, struct __stat64 *buf);

#ifndef localerpl_do_not_redefine_stat
# ifndef LOCALE_RPL_IMPL
#  ifdef stat
#   undef stat
#  endif
#  define stat(name, buf) localerpl_stat(name, buf)
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(path, A_In_z)
A_Success(!return)
#endif
int localerpl_chmod(const char *path, int mode);

#ifndef localerpl_do_not_redefine_chmod
# ifndef LOCALE_RPL_IMPL
#  ifdef chmod
#   undef chmod
#  endif
#  define chmod localerpl_chmod
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Ret_z
#endif
char *localerpl_strerror(int error_number);

#ifndef localerpl_do_not_redefine_strerror
# ifndef LOCALE_RPL_IMPL
#  ifdef strerror
#   undef strerror
#  endif
#  define strerror localerpl_strerror
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_Notnull)
A_At(s, A_Pre_writable_byte_size(mx))
A_At(s, A_Post_readable_byte_size(return + 1))
A_At(s, A_Post_z)
A_At(fmt, A_In_z)
A_At(t, A_In)
A_Success(return)
A_Post_satisfies(return < mx)
#endif
size_t localerpl_strftime(char *s, size_t mx, const char *fmt, const struct tm *t);

#ifndef localerpl_do_not_redefine_strftime
# ifndef LOCALE_RPL_IMPL
#  ifdef strftime
#   undef strftime
#  endif
#  define strftime localerpl_strftime
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(templ, A_Inout_z)
A_Success(return >= 0)
#endif
int localerpl_mkstemp(char *templ);

#ifndef localerpl_do_not_redefine_mkstemp
# ifndef LOCALE_RPL_IMPL
#  ifdef mkstemp
#   undef mkstemp
#  endif
#  define mkstemp localerpl_mkstemp
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
char **localerpl_environ(void);

#ifndef localerpl_do_not_redefine_environ
# ifndef LOCALE_RPL_IMPL
#  ifdef environ
#   undef environ
#  endif
#  define environ localerpl_environ()
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(name, A_In_z)
A_Ret_z
A_Success(return)
#endif
char *localerpl_getenv(const char *name);

#ifndef localerpl_do_not_redefine_getenv
# ifndef LOCALE_RPL_IMPL
#  ifdef getenv
#   undef getenv
#  endif
#  define getenv localerpl_getenv
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(name, A_In_z)
A_At(value, A_In_z)
#endif
int localerpl_setenv(const char *name, const char *value, int overwrite);

#ifndef localerpl_do_not_redefine_setenv
# ifndef LOCALE_RPL_IMPL
#  ifdef setenv
#   undef setenv
#  endif
#  define setenv localerpl_setenv
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(name, A_In_z)
#endif
int localerpl_unsetenv(const char *name);

#ifndef localerpl_do_not_redefine_unsetenv
# ifndef LOCALE_RPL_IMPL
#  ifdef unsetenv
#   undef unsetenv
#  endif
#  define unsetenv localerpl_unsetenv
# endif
#endif

int localerpl_clearenv(void);

#ifndef localerpl_do_not_redefine_clearenv
# ifndef LOCALE_RPL_IMPL
#  ifdef clearenv
#   undef clearenv
#  endif
#  define clearenv localerpl_clearenv
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_At(locale, A_In_opt_z)
A_Ret_z
A_Success(return)
#endif
char *localerpl_setlocale(int category, const char *locale/*NULL?*/);

#ifndef localerpl_do_not_redefine_setlocale
# ifndef LOCALE_RPL_IMPL
#  ifdef setlocale
#   undef setlocale
#  endif
#  define setlocale localerpl_setlocale
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_mb_cur_max(void);

#ifndef localerpl_do_not_redefine_MB_CUR_MAX
# ifndef LOCALE_RPL_IMPL
#  ifdef MB_CUR_MAX
#   undef MB_CUR_MAX
#  endif
#  define MB_CUR_MAX localerpl_mb_cur_max()
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
wint_t localerpl_btowc(int c);

#ifndef localerpl_do_not_redefine_btowc
# ifndef LOCALE_RPL_IMPL
#  ifdef btowc
#   undef btowc
#  endif
#  define btowc localerpl_btowc
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_In_reads_opt(n))
A_Success(return != -1)
#endif
int localerpl_mblen(const char *s/*NULL?*/, size_t n);

#ifndef localerpl_do_not_redefine_mblen
# ifndef LOCALE_RPL_IMPL
#  ifdef mblen
#   undef mblen
#  endif
#  define mblen localerpl_mblen
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_In_reads_opt(n))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
#endif
size_t localerpl_mbrlen(const char *s/*NULL?*/, size_t n, mbstate_t *ps/*NULL?*/);

#ifndef localerpl_do_not_redefine_mbrlen
# ifndef LOCALE_RPL_IMPL
#  ifdef mbrlen
#   undef mbrlen
#  endif
#  define mbrlen localerpl_mbrlen
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_When(s, A_At(pwc, A_Out_opt))
A_At(s, A_In_reads_opt(n))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
#endif
size_t localerpl_mbrtoc16(wchar_t *pwc/*NULL?*/,
	const char *s/*NULL?*/, size_t n, mbstate_t *ps/*NULL?*/);

#ifndef localerpl_do_not_redefine_mbrtoc16
# ifndef LOCALE_RPL_IMPL
#  ifdef mbrtoc16
#   undef mbrtoc16
#  endif
#  define mbrtoc16 localerpl_mbrtoc16
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_When(s, A_At(pwi, A_Out_opt))
A_At(s, A_In_reads_opt(n))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
#endif
size_t localerpl_mbrtoc32(unsigned *pwi/*NULL?*/,
	const char *s/*NULL?*/, size_t n, mbstate_t *ps/*NULL?*/);

#ifndef localerpl_do_not_redefine_mbrtoc32
# ifndef LOCALE_RPL_IMPL
#  ifdef mbrtoc32
#   undef mbrtoc32
#  endif
#  define mbrtoc32 localerpl_mbrtoc32
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_Maybenull A_Post_readable_size(return))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
/* may return 0 if lower surrogate is required to complete a unicode code point */
#endif
size_t localerpl_c16rtomb(char *s/*NULL?*/, wchar_t wc, mbstate_t *ps/*NULL?*/);

#ifndef localerpl_do_not_redefine_c16rtomb
# ifndef LOCALE_RPL_IMPL
#  ifdef c16rtomb
#   undef c16rtomb
#  endif
#  define c16rtomb localerpl_c16rtomb
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_Maybenull A_Post_readable_size(return))
A_At(ps, A_Inout_opt)
A_Ret_range(>,0)
A_Success(return != A_Size_t(-1))
#endif
size_t localerpl_c32rtomb(char *s/*NULL?*/, unsigned wi, mbstate_t *ps/*NULL?*/);

#ifndef localerpl_do_not_redefine_c32rtomb
# ifndef LOCALE_RPL_IMPL
#  ifdef c32rtomb
#   undef c32rtomb
#  endif
#  define c32rtomb localerpl_c32rtomb
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(wcstr, A_Out_writes_opt(count))
A_At(mbstr, A_In_z)
A_Success(return != A_Size_t(-1))
#endif
size_t localerpl_mbstowcs(wchar_t *wcstr, const char *mbstr, size_t count);

#ifndef localerpl_do_not_redefine_mbstowcs
# ifndef LOCALE_RPL_IMPL
#  ifdef mbstowcs
#   undef mbstowcs
#  endif
#  define mbstowcs localerpl_mbstowcs
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(mbstr, A_Out_writes_opt(count))
A_At(wcstr, A_In_z)
A_Success(return != A_Size_t(-1))
#endif
size_t localerpl_wcstombs(char *mbstr, const wchar_t *wcstr, size_t count);

#ifndef localerpl_do_not_redefine_wcstombs
# ifndef LOCALE_RPL_IMPL
#  ifdef wcstombs
#   undef wcstombs
#  endif
#  define wcstombs localerpl_wcstombs
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s1, A_In_z)
A_At(s2, A_In_z)
#endif
int localerpl_strcoll(const char *s1, const char *s2);

#ifndef localerpl_do_not_redefine_strcoll
# ifndef LOCALE_RPL_IMPL
#  ifdef strcoll
#   undef strcoll
#  endif
#  define strcoll localerpl_strcoll
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s1, A_In_z)
A_At(s2, A_In_z)
#endif
int localerpl_stricmp(const char *s1, const char *s2);

#ifndef localerpl_do_not_redefine_stricmp
# ifndef LOCALE_RPL_IMPL
#  ifdef stricmp
#   undef stricmp
#  endif
#  define stricmp localerpl_stricmp
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_tolower(int c);

#ifndef localerpl_do_not_redefine_tolower
# ifndef LOCALE_RPL_IMPL
#  ifdef tolower
#   undef tolower
#  endif
#  define tolower localerpl_tolower
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_toupper(int c);

#ifndef localerpl_do_not_redefine_toupper
# ifndef LOCALE_RPL_IMPL
#  ifdef toupper
#   undef toupper
#  endif
#  define toupper localerpl_toupper
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_isascii(int c);

#ifndef localerpl_do_not_redefine_isascii
# ifndef LOCALE_RPL_IMPL
#  ifdef isascii
#   undef isascii
#  endif
#  define isascii localerpl_isascii
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_isalnum(int c);

#ifndef localerpl_do_not_redefine_isalnum
# ifndef LOCALE_RPL_IMPL
#  ifdef isalnum
#   undef isalnum
#  endif
#  define isalnum localerpl_isalnum
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_isalpha(int c);

#ifndef localerpl_do_not_redefine_isalpha
# ifndef LOCALE_RPL_IMPL
#  ifdef isalpha
#   undef isalpha
#  endif
#  define isalpha localerpl_isalpha
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_isblank(int c);

#ifndef localerpl_do_not_redefine_isblank
# ifndef LOCALE_RPL_IMPL
#  ifdef isblank
#   undef isblank
#  endif
#  define isblank localerpl_isblank
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_iscntrl(int c);

#ifndef localerpl_do_not_redefine_iscntrl
# ifndef LOCALE_RPL_IMPL
#  ifdef iscntrl
#   undef iscntrl
#  endif
#  define iscntrl localerpl_iscntrl
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_isdigit(int c);

#ifndef localerpl_do_not_redefine_isdigit
# ifndef LOCALE_RPL_IMPL
#  ifdef isdigit
#   undef isdigit
#  endif
#  define isdigit localerpl_isdigit
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_isgraph(int c);

#ifndef localerpl_do_not_redefine_isgraph
# ifndef LOCALE_RPL_IMPL
#  ifdef isgraph
#   undef isgraph
#  endif
#  define isgraph localerpl_isgraph
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_islower(int c);

#ifndef localerpl_do_not_redefine_islower
# ifndef LOCALE_RPL_IMPL
#  ifdef islower
#   undef islower
#  endif
#  define islower localerpl_islower
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_isprint(int c);

#ifndef localerpl_do_not_redefine_isprint
# ifndef LOCALE_RPL_IMPL
#  ifdef isprint
#   undef isprint
#  endif
#  define isprint localerpl_isprint
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_ispunct(int c);

#ifndef localerpl_do_not_redefine_ispunct
# ifndef LOCALE_RPL_IMPL
#  ifdef ispunct
#   undef ispunct
#  endif
#  define ispunct localerpl_ispunct
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_isspace(int c);

#ifndef localerpl_do_not_redefine_isspace
# ifndef LOCALE_RPL_IMPL
#  ifdef isspace
#   undef isspace
#  endif
#  define isspace localerpl_isspace
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_isupper(int c);

#ifndef localerpl_do_not_redefine_isupper
# ifndef LOCALE_RPL_IMPL
#  ifdef isupper
#   undef isupper
#  endif
#  define isupper localerpl_isupper
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(c, A_In_range(0,255))
#endif
int localerpl_isxdigit(int c);

#ifndef localerpl_do_not_redefine_isxdigit
# ifndef LOCALE_RPL_IMPL
#  ifdef isxdigit
#   undef isxdigit
#  endif
#  define isxdigit localerpl_isxdigit
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s1, A_In_z)
A_At(s2, A_In_z)
#endif
int localerpl_c32scoll(const unsigned *s1, const unsigned *s2);

#ifndef localerpl_do_not_redefine_c32scoll
# ifndef LOCALE_RPL_IMPL
#  ifdef c32scoll
#   undef c32scoll
#  endif
#  define c32scoll localerpl_c32scoll
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s1, A_In_z)
A_At(s2, A_In_z)
#endif
int localerpl_c32sicmp(const unsigned *s1, const unsigned *s2);

#ifndef localerpl_do_not_redefine_c32sicmp
# ifndef LOCALE_RPL_IMPL
#  ifdef c32sicmp
#   undef c32sicmp
#  endif
#  define c32sicmp localerpl_c32sicmp
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
unsigned localerpl_c32tolower(unsigned c);

#ifndef localerpl_do_not_redefine_c32tolower
# ifndef LOCALE_RPL_IMPL
#  ifdef c32tolower
#   undef c32tolower
#  endif
#  define c32tolower localerpl_c32tolower
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
unsigned localerpl_c32toupper(unsigned c);

#ifndef localerpl_do_not_redefine_c32toupper
# ifndef LOCALE_RPL_IMPL
#  ifdef c32toupper
#   undef c32toupper
#  endif
#  define c32toupper localerpl_c32toupper
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_c32isascii(unsigned c);

#ifndef localerpl_do_not_redefine_c32isascii
# ifndef LOCALE_RPL_IMPL
#  ifdef c32isascii
#   undef c32isascii
#  endif
#  define c32isascii localerpl_c32isascii
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_c32isalnum(unsigned c);

#ifndef localerpl_do_not_redefine_c32isalnum
# ifndef LOCALE_RPL_IMPL
#  ifdef c32isalnum
#   undef c32isalnum
#  endif
#  define c32isalnum localerpl_c32isalnum
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_c32isalpha(unsigned c);

#ifndef localerpl_do_not_redefine_c32isalpha
# ifndef LOCALE_RPL_IMPL
#  ifdef c32isalpha
#   undef c32isalpha
#  endif
#  define c32isalpha localerpl_c32isalpha
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_c32isblank(unsigned c);

#ifndef localerpl_do_not_redefine_c32isblank
# ifndef LOCALE_RPL_IMPL
#  ifdef c32isblank
#   undef c32isblank
#  endif
#  define c32isblank localerpl_c32isblank
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_c32iscntrl(unsigned c);

#ifndef localerpl_do_not_redefine_c32iscntrl
# ifndef LOCALE_RPL_IMPL
#  ifdef c32iscntrl
#   undef c32iscntrl
#  endif
#  define c32iscntrl localerpl_c32iscntrl
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_c32isdigit(unsigned c);

#ifndef localerpl_do_not_redefine_c32isdigit
# ifndef LOCALE_RPL_IMPL
#  ifdef c32isdigit
#   undef c32isdigit
#  endif
#  define c32isdigit localerpl_c32isdigit
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_c32isgraph(unsigned c);

#ifndef localerpl_do_not_redefine_c32isgraph
# ifndef LOCALE_RPL_IMPL
#  ifdef c32isgraph
#   undef c32isgraph
#  endif
#  define c32isgraph localerpl_c32isgraph
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_c32islower(unsigned c);

#ifndef localerpl_do_not_redefine_c32islower
# ifndef LOCALE_RPL_IMPL
#  ifdef c32islower
#   undef c32islower
#  endif
#  define c32islower localerpl_c32islower
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_c32isprint(unsigned c);

#ifndef localerpl_do_not_redefine_c32isprint
# ifndef LOCALE_RPL_IMPL
#  ifdef c32isprint
#   undef c32isprint
#  endif
#  define c32isprint localerpl_c32isprint
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_c32ispunct(unsigned c);

#ifndef localerpl_do_not_redefine_c32ispunct
# ifndef LOCALE_RPL_IMPL
#  ifdef c32ispunct
#   undef c32ispunct
#  endif
#  define c32ispunct localerpl_c32ispunct
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_c32isspace(unsigned c);

#ifndef localerpl_do_not_redefine_c32isspace
# ifndef LOCALE_RPL_IMPL
#  ifdef c32isspace
#   undef c32isspace
#  endif
#  define c32isspace localerpl_c32isspace
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_c32isupper(unsigned c);

#ifndef localerpl_do_not_redefine_c32isupper
# ifndef LOCALE_RPL_IMPL
#  ifdef c32isupper
#   undef c32isupper
#  endif
#  define c32isupper localerpl_c32isupper
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_c32isxdigit(unsigned c);

#ifndef localerpl_do_not_redefine_c32isxdigit
# ifndef LOCALE_RPL_IMPL
#  ifdef c32isxdigit
#   undef c32isxdigit
#  endif
#  define c32isxdigit localerpl_c32isxdigit
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(name, A_In_z)
#endif
c32ctype_t localerpl_c32ctype(const char *name);

#ifndef localerpl_do_not_redefine_c32ctype
# ifndef LOCALE_RPL_IMPL
#  ifdef c32ctype
#   undef c32ctype
#  endif
#  define c32ctype localerpl_c32ctype
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_c32isctype(unsigned c, c32ctype_t desc);

#ifndef localerpl_do_not_redefine_c32isctype
# ifndef LOCALE_RPL_IMPL
#  ifdef c32isctype
#   undef c32isctype
#  endif
#  define c32isctype localerpl_c32isctype
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(dst, A_Out_writes_opt(n))
A_At(src, A_In_z)
A_When(dst, A_At(n, A_In_range(<=,INT_MAX)))
A_Success(return != A_Size_t(-1))
#endif
size_t localerpl_mbstoc32s(unsigned *dst, const char *src, size_t n);

#ifndef localerpl_do_not_redefine_mbstoc32s
# ifndef LOCALE_RPL_IMPL
#  ifdef mbstoc32s
#   undef mbstoc32s
#  endif
#  define mbstoc32s localerpl_mbstoc32s
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(dst, A_Out_writes_opt(n))
A_At(src, A_In_z)
A_Success(return != A_Size_t(-1))
#endif
size_t localerpl_c32stombs(char *dst, const unsigned *src, size_t n);

#ifndef localerpl_do_not_redefine_c32stombs
# ifndef LOCALE_RPL_IMPL
#  ifdef c32stombs
#   undef c32stombs
#  endif
#  define c32stombs localerpl_c32stombs
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(dst, A_Out_writes_opt(n))
A_At(src, A_In_z)
A_Success(return != A_Size_t(-1))
#endif
size_t localerpl_wcstoc32s(unsigned *dst, const wchar_t *src, size_t n);

#ifndef localerpl_do_not_redefine_wcstoc32s
# ifndef LOCALE_RPL_IMPL
#  ifdef wcstoc32s
#   undef wcstoc32s
#  endif
#  define wcstoc32s localerpl_wcstoc32s
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(dst, A_Out_writes_opt(n))
A_At(src, A_In_z)
A_Success(return != A_Size_t(-1))
#endif
size_t localerpl_c32stowcs(wchar_t *dst, const unsigned *src, size_t n);

#ifndef localerpl_do_not_redefine_c32stowcs
# ifndef LOCALE_RPL_IMPL
#  ifdef c32stowcs
#   undef c32stowcs
#  endif
#  define c32stowcs localerpl_c32stowcs
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(cmdname, A_In_z)
A_At(argv, A_Notnull)
#endif
intptr_t localerpl_spawnvp(int mode, const char *cmdname, const char *const *argv);

#ifndef localerpl_do_not_redefine_spawnvp
# ifndef LOCALE_RPL_IMPL
#  ifdef spawnvp
#   undef spawnvp
#  endif
#  define spawnvp localerpl_spawnvp
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(cmdname, A_In_z)
#endif
intptr_t localerpl_spawnl_utf8(int mode, const char *cmdname, ...);

#define localerpl_spawnl (localerpl_is_utf8() ? localerpl_spawnl_utf8 : _spawnl)

#ifndef localerpl_do_not_redefine_spawnl
# ifndef LOCALE_RPL_IMPL
#  ifdef spawnl
#   undef spawnl
#  endif
#  define spawnl localerpl_spawnl
# endif
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(command, A_In_z)
A_At(mode, A_In_z)
A_Success(return)
#endif
FILE *localerpl_popen(const char *command, const char *mode);

#ifndef localerpl_do_not_redefine_popen
# ifndef LOCALE_RPL_IMPL
#  ifdef popen
#   undef popen
#  endif
#  define popen localerpl_popen
# endif
#endif

#ifndef LOCALE_RPL_IMPL

/* utf8/utf16/utf32 conversion routines */
#ifdef __cplusplus
extern "C" {
#endif
/* from libutf16 library */
extern size_t utf8_mbrtoc16(unsigned short *pwc, const unsigned char *s, size_t n, unsigned *ps);
extern size_t utf8_mbrtoc32(unsigned int *pwi, const unsigned char *s, size_t n, unsigned *ps);
extern size_t utf8_c16rtomb(unsigned char *s, unsigned short wc, unsigned *ps);
extern size_t utf8_c32rtomb(unsigned char *s, unsigned int wi);
/* from unicode_ctype library */
extern int unicode_ctype(const char name[]);
extern int unicode_isctype(unsigned w, int t);
#ifdef __cplusplus
}
#endif

/* banned functions */

#ifdef wcscoll
#undef wcscoll
#endif
#ifdef wcsicmp
#undef wcsicmp
#endif
#ifdef towlower
#undef towlower
#endif
#ifdef towupper
#undef towupper
#endif
#ifdef iswascii
#undef iswascii
#endif
#ifdef iswalnum
#undef iswalnum
#endif
#ifdef iswalpha
#undef iswalpha
#endif
#ifdef iswblank
#undef iswblank
#endif
#ifdef iswcntrl
#undef iswcntrl
#endif
#ifdef iswdigit
#undef iswdigit
#endif
#ifdef iswgraph
#undef iswgraph
#endif
#ifdef iswlower
#undef iswlower
#endif
#ifdef iswprint
#undef iswprint
#endif
#ifdef iswpunct
#undef iswpunct
#endif
#ifdef iswspace
#undef iswspace
#endif
#ifdef iswupper
#undef iswupper
#endif
#ifdef iswxdigit
#undef iswxdigit
#endif
#ifdef wctype
#undef wctype
#endif
#ifdef iswctype
#undef iswctype
#endif
#ifdef mbtowc
#undef mbtowc
#endif
#ifdef mbrtowc
#undef mbrtowc
#endif
#ifdef wctomb
#undef wctomb
#endif
#ifdef wcrtomb
#undef wcrtomb
#endif

#ifndef LOCALERPL_NEED_CALLBACKS

#define wcscoll use_c32scoll_instead
#define wcsicmp use_c32sicmp_instead
#define towlower use_c32tolower_instead
#define towupper use_c32toupper_instead
#define iswascii use_c32isascii_instead
#define iswalnum use_c32isalnum_instead
#define iswalpha use_c32isalpha_instead
#define iswblank use_c32isblank_instead
#define iswcntrl use_c32iscntrl_instead
#define iswdigit use_c32isdigit_instead
#define iswgraph use_c32isgraph_instead
#define iswlower use_c32islower_instead
#define iswprint use_c32isprint_instead
#define iswpunct use_c32ispunct_instead
#define iswspace use_c32isspace_instead
#define iswupper use_c32isupper_instead
#define iswxdigit use_c32isxdigit_instead
#define wctype use_c32ctype_instead
#define iswctype use_c32isctype_instead
#define mbtowc use_mbrtoc16_or_mbrtoc32_instead
#define mbrtowc use_mbrtoc16_or_mbrtoc32_instead
#define wctomb use_c16rtomb_or_c32rtomb_intead
#define wcrtomb use_c16rtomb_or_c32rtomb_intead

#else /* LOCALERPL_NEED_CALLBACKS */

#define wcscoll localerpl_wcscoll
static inline int wcscoll(const wchar_t *s1, const wchar_t *s2)
{
	(void)s1, (void)s2;
	assert(0); /* use c32scoll instead */
	return -1;
}

#define wcsicmp localerpl_wcsicmp
static inline int wcsicmp(const wchar_t *s1, const wchar_t *s2)
{
	(void)s1, (void)s2;
	assert(0); /* use c32sicmp instead */
	return -1;
}

#define towlower localerpl_towlower
static inline wint_t towlower(wint_t c)
{
	assert(0); /* use c32tolower instead */
	return c;
}

#define towupper localerpl_towupper
static inline wint_t towupper(wint_t c)
{
	assert(0); /* use c32toupper instead */
	return c;
}

#define iswascii localerpl_iswascii
static inline int iswascii(wint_t c)
{
	(void)c;
	assert(0); /* use c32isascii instead */
	return 0;
}

#define iswalnum localerpl_iswalnum
static inline int iswalnum(wint_t c)
{
	(void)c;
	assert(0); /* use c32isalnum instead */
	return 0;
}

#define iswalpha localerpl_iswalpha
static inline int iswalpha(wint_t c)
{
	(void)c;
	assert(0); /* use c32isalpha instead */
	return 0;
}

#define iswblank localerpl_iswblank
static inline int iswblank(wint_t c)
{
	(void)c;
	assert(0); /* use c32isblank instead */
	return 0;
}

#define iswcntrl localerpl_iswcntrl
static inline int iswcntrl(wint_t c)
{
	(void)c;
	assert(0); /* use c32iscntrl instead */
	return 0;
}

#define iswdigit localerpl_iswdigit
static inline int iswdigit(wint_t c)
{
	(void)c;
	assert(0); /* use c32isdigit instead */
	return 0;
}

#define iswgraph localerpl_iswgraph
static inline int iswgraph(wint_t c)
{
	(void)c;
	assert(0); /* use c32isgraph instead */
	return 0;
}

#define iswlower localerpl_iswlower
static inline int iswlower(wint_t c)
{
	(void)c;
	assert(0); /* use c32islower instead */
	return 0;
}

#define iswprint localerpl_iswprint
static inline int iswprint(wint_t c)
{
	(void)c;
	assert(0); /* use c32isprint instead */
	return 0;
}

#define iswpunct localerpl_iswpunct
static inline int iswpunct(wint_t c)
{
	(void)c;
	assert(0); /* use c32ispunct instead */
	return 0;
}

#define iswspace localerpl_iswspace
static inline int iswspace(wint_t c)
{
	(void)c;
	assert(0); /* use c32isspace instead */
	return 0;
}

#define iswupper localerpl_iswupper
static inline int iswupper(wint_t c)
{
	(void)c;
	assert(0); /* use c32isupper instead */
	return 0;
}

#define iswxdigit localerpl_iswxdigit
static inline int iswxdigit(wint_t c)
{
	(void)c;
	assert(0); /* use c32isxdigit instead */
	return 0;
}

#define wctype localerpl_wctype
static inline wctype_t wctype(const char *name)
{
	(void)name;
	assert(0); /* use c32ctype instead */
	return 0;
}

#define iswctype localerpl_iswctype
static inline int iswctype(wint_t c, wctype_t desc)
{
	(void)c, (void)desc;
	assert(0); /* use c32isctype instead */
	return 0;
}

#define mbtowc localerpl_mbtowc
static inline int mbtowc(wchar_t *pwc, const char *s, size_t n)
{
	(void)pwc, (void)s, (void)n;
	assert(0); /* use mbrtoc16 or mbrtoc32 instead */
	return 0;
}

#define mbrtowc localerpl_mbrtowc
static inline size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps)
{
	(void)pwc, (void)s, (void)n, (void)ps;
	assert(0); /* use mbrtoc16 or mbrtoc32 instead */
	return 0;
}

#define wctomb localerpl_wctomb
static inline int wctomb(char *s, wchar_t wc)
{
	(void)s, (void)wc;
	assert(0); /* use c16rtomb or c32rtomb intead */
	return 0;
}

#define wcrtomb localerpl_wcrtomb
static inline size_t wcrtomb(char *s, wchar_t wc, mbstate_t *ps)
{
	(void)s, (void)wc, (void)ps;
	assert(0); /* use c16rtomb or c32rtomb intead */
	return 0;
}

#endif /* LOCALERPL_NEED_CALLBACKS */

/* end of banned functions */

#endif /* LOCALE_RPL_IMPL */

#endif /* LOCALERPL_H_INCLUDED */
