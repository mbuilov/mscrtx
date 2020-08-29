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

/* for ATTRIBUTE_PRINTF_PTR */
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

struct localerpl {
	int (*rpl_open)(const char *name, int flags, ...);
	FILE *(*rpl_fopen)(const char *path, const char *mode);

	int (*rpl_read)(int fd, void *buf, unsigned count);
	int (*rpl_write)(int fd, const void *buf, unsigned count);
	size_t (*rpl_fread)(void *buf, size_t size, size_t nmemb, FILE *stream);
	size_t (*rpl_fwrite)(const void *buf, size_t size, size_t nmemb, FILE *stream);

	int (*rpl_putchar)(int c);
	int (*rpl_fputc)(int c, FILE *stream);
	int (*rpl_getchar)(void);
	int (*rpl_fgetc)(FILE *stream);
	int (*rpl_puts)(const char *s);
	int (*rpl_fputs)(const char *s, FILE *stream);

	char *(*rpl_fgets)(char *s, int size, FILE *stream);

	int (*rpl_mkdir)(const char *dirname);
	int (*rpl_rmdir)(const char *dirname);
	int (*rpl_remove)(const char *pathname);
	int (*rpl_unlink)(const char *pathname);
	int (*rpl_rename)(const char *old_name, const char *new_name);
	int (*rpl_chdir)(const char *path);

	int (*rpl_stat)(const char *path, struct __stat64 *buf);
	int (*rpl_chmod)(const char *path, int mode);

	ATTRIBUTE_PRINTF_PTR(format, 1, 2)
	int (*rpl_printf)(const char *format, ...);

	ATTRIBUTE_PRINTF_PTR(format, 2, 3)
	int (*rpl_fprintf)(FILE *stream, const char *format, ...);

	ATTRIBUTE_PRINTF_PTR(format, 1, 0)
	int (*rpl_vprintf)(const char *format, va_list ap);

	ATTRIBUTE_PRINTF_PTR(format, 2, 0)
	int (*rpl_vfprintf)(FILE *stream, const char *format, va_list ap);

	char *(*rpl_strerror)(int error_number);

	ATTRIBUTE_PRINTF_PTR(format, 3, 0)
	size_t (*rpl_strftime)(char *s, size_t mx, const char *fmt, const struct tm *t);

	int (*rpl_mkstemp)(char *templ);

	char **(*rpl_environ)(void);
	char *(*rpl_getenv)(const char *name);
	int (*rpl_setenv)(const char *name, const char *value, int overwrite);
	int (*rpl_unsetenv)(const char *name);
	int (*rpl_clearenv)(void);

	char *(*rpl_setlocale)(int category, const char *locale);

	int (*rpl_mb_cur_max)(void);

	wint_t (*rpl_btowc)(int c);

	int (*rpl_mblen)(const char *s, size_t n);
	size_t (*rpl_mbrlen)(const char *s, size_t n, mbstate_t *ps);

	size_t (*rpl_mbrtoc16)(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps);
	size_t (*rpl_mbrtoc32)(unsigned *pwi, const char *s, size_t n, mbstate_t *ps);
	size_t (*rpl_c16rtomb)(char *s, wchar_t wc, mbstate_t *ps);
	size_t (*rpl_c32rtomb)(char *s, unsigned wi, mbstate_t *ps);

	size_t (*rpl_mbstowcs)(wchar_t *wcstr, const char *mbstr, size_t count);
	size_t (*rpl_wcstombs)(char *mbstr, const wchar_t *wcstr, size_t count);

	int (*rpl_strcoll)(const char *s1, const char *s2);
	int (*rpl_stricmp)(const char *s1, const char *s2);
	int (*rpl_tolower)(int c);
	int (*rpl_toupper)(int c);
	int (*rpl_isascii)(int c);
	int (*rpl_isalnum)(int c);
	int (*rpl_isalpha)(int c);
	int (*rpl_isblank)(int c);
	int (*rpl_iscntrl)(int c);
	int (*rpl_isdigit)(int c);
	int (*rpl_isgraph)(int c);
	int (*rpl_islower)(int c);
	int (*rpl_isprint)(int c);
	int (*rpl_ispunct)(int c);
	int (*rpl_isspace)(int c);
	int (*rpl_isupper)(int c);
	int (*rpl_isxdigit)(int c);

	int (*rpl_c32scoll)(const unsigned *s1, const unsigned *s2);
	int (*rpl_c32sicmp)(const unsigned *s1, const unsigned *s2);
	unsigned (*rpl_c32tolower)(unsigned c);
	unsigned (*rpl_c32toupper)(unsigned c);
	int (*rpl_c32isascii)(unsigned c);
	int (*rpl_c32isalnum)(unsigned c);
	int (*rpl_c32isalpha)(unsigned c);
	int (*rpl_c32isblank)(unsigned c);
	int (*rpl_c32iscntrl)(unsigned c);
	int (*rpl_c32isdigit)(unsigned c);
	int (*rpl_c32isgraph)(unsigned c);
	int (*rpl_c32islower)(unsigned c);
	int (*rpl_c32isprint)(unsigned c);
	int (*rpl_c32ispunct)(unsigned c);
	int (*rpl_c32isspace)(unsigned c);
	int (*rpl_c32isupper)(unsigned c);
	int (*rpl_c32isxdigit)(unsigned c);
	c32ctype_t (*rpl_c32ctype)(const char *name);
	int (*rpl_c32isctype)(unsigned c, c32ctype_t desc);

	size_t (*rpl_mbstoc32s)(unsigned *dst, const char *src, size_t n);
	size_t (*rpl_c32stombs)(char *dst, const unsigned *src, size_t n);
	size_t (*rpl_wcstoc32s)(unsigned *dst, const wchar_t *src, size_t n);
	size_t (*rpl_c32stowcs)(wchar_t *dst, const unsigned *src, size_t n);

	intptr_t (*rpl_spawnvp)(int mode, const char *cmdname, const char *const *argv);
	intptr_t (*rpl_spawnl)(int mode, const char *cmdname, const char *arg0, ...);
	FILE *(*rpl_popen)(const char *command, const char *mode);
};

/* Change localerpl global pointer.  */
void change_localerpl(int to_utf8);

/* Returns non-zero if using UTF-8 replacements.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_is_utf8(void);

/* wcslen(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(str, A_In_z)
#endif
#ifndef localerpl_do_not_redefine_c32slen
#ifdef c32slen
#undef c32slen
#endif
#define c32slen localerpl_rpl_c32slen
#endif
size_t localerpl_rpl_c32slen(const unsigned *str);

/* wcschr(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_In_z)
A_Ret_maybenull
#endif
#ifndef localerpl_do_not_redefine_c32schr
#ifdef c32schr
#undef c32schr
#endif
#define c32schr localerpl_rpl_c32schr
#endif
unsigned *localerpl_rpl_c32schr(const unsigned *s, unsigned c);

/* wcsrchr(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_In_z)
A_Ret_maybenull
#endif
#ifndef localerpl_do_not_redefine_c32srchr
#ifdef c32srchr
#undef c32srchr
#endif
#define c32srchr localerpl_rpl_c32srchr
#endif
unsigned *localerpl_rpl_c32srchr(const unsigned *s, unsigned c);

/* strchrnul(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_In_z)
A_Ret_never_null
A_Ret_z
#endif
#ifndef localerpl_do_not_redefine_c32schrnul
#ifdef c32schrnul
#undef c32schrnul
#endif
#define c32schrnul localerpl_rpl_c32schrnul
#endif
unsigned *localerpl_rpl_c32schrnul(const unsigned *s, unsigned c);

/* strcmp(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s1, A_In_z)
A_At(s2, A_In_z)
#endif
#ifndef localerpl_do_not_redefine_c32scmp
#ifdef c32scmp
#undef c32scmp
#endif
#define c32scmp localerpl_rpl_c32scmp
#endif
int localerpl_rpl_c32scmp(const unsigned *s1, const unsigned *s2);

/* Global pointer.  */
extern const struct localerpl *localerpl;

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

#ifndef localerpl_do_not_redefine_open
#ifdef open
#undef open
#endif
#define open localerpl_rpl_open
#endif
#define localerpl_rpl_open localerpl->rpl_open

#ifndef localerpl_do_not_redefine_fopen
#ifdef fopen
#undef fopen
#endif
#define fopen localerpl_rpl_fopen
#endif
static inline FILE *localerpl_rpl_fopen(const char *path, const char *mode)
{
	return localerpl->rpl_fopen(path, mode);
}

#ifndef localerpl_do_not_redefine_read
#ifdef read
#undef read
#endif
#define read localerpl_rpl_read
#endif
static inline int localerpl_rpl_read(int fd, void *buf, unsigned count)
{
	return localerpl->rpl_read(fd, buf, count);
}

#ifndef localerpl_do_not_redefine_write
#ifdef write
#undef write
#endif
#define write localerpl_rpl_write
#endif
static inline int localerpl_rpl_write(int fd, const void *buf, unsigned count)
{
	return localerpl->rpl_write(fd, buf, count);
}

#ifndef localerpl_do_not_redefine_fread
#ifdef fread
#undef fread
#endif
#define fread localerpl_rpl_fread
#endif
static inline size_t localerpl_rpl_fread(void *buf, size_t size, size_t nmemb, FILE *stream)
{
	return localerpl->rpl_fread(buf, size, nmemb, stream);
}

#ifndef localerpl_do_not_redefine_fwrite
#ifdef fwrite
#undef fwrite
#endif
#define fwrite localerpl_rpl_fwrite
#endif
static inline size_t localerpl_rpl_fwrite(const void *buf, size_t size, size_t nmemb, FILE *stream)
{
	return localerpl->rpl_fwrite(buf, size, nmemb, stream);
}

#ifndef localerpl_do_not_redefine_putchar
#ifdef putchar
#undef putchar
#endif
#define putchar localerpl_rpl_putchar
#endif
static inline int localerpl_rpl_putchar(int c)
{
	return localerpl->rpl_putchar(c);
}

#ifndef localerpl_do_not_redefine_fputc
#ifdef fputc
#undef fputc
#endif
#define fputc localerpl_rpl_fputc
#endif
static inline int localerpl_rpl_fputc(int c, FILE *stream)
{
	return localerpl->rpl_fputc(c, stream);
}

#ifndef localerpl_do_not_redefine_getchar
#ifdef getchar
#undef getchar
#endif
#define getchar localerpl_rpl_getchar
#endif
static inline int localerpl_rpl_getchar(void)
{
	return localerpl->rpl_getchar();
}

#ifndef localerpl_do_not_redefine_fgetc
#ifdef fgetc
#undef fgetc
#endif
#define fgetc localerpl_rpl_fgetc
#endif
static inline int localerpl_rpl_fgetc(FILE *stream)
{
	return localerpl->rpl_fgetc(stream);
}

#ifndef localerpl_do_not_redefine_puts
#ifdef puts
#undef puts
#endif
#define puts localerpl_rpl_puts
#endif
static inline int localerpl_rpl_puts(const char *s)
{
	return localerpl->rpl_puts(s);
}

#ifndef localerpl_do_not_redefine_fputs
#ifdef fputs
#undef fputs
#endif
#define fputs localerpl_rpl_fputs
#endif
static inline int localerpl_rpl_fputs(const char *s, FILE *stream)
{
	return localerpl->rpl_fputs(s, stream);
}

#ifndef localerpl_do_not_redefine_fgets
#ifdef fgets
#undef fgets
#endif
#define fgets localerpl_rpl_fgets
#endif
static inline char *localerpl_rpl_fgets(char *s, int size, FILE *stream)
{
	return localerpl->rpl_fgets(s, size, stream);
}

#ifndef localerpl_do_not_redefine_mkdir
#ifdef mkdir
#undef mkdir
#endif
#define mkdir localerpl_rpl_mkdir
#endif
static inline int localerpl_rpl_mkdir(const char *dirname)
{
	return localerpl->rpl_mkdir(dirname);
}

#ifndef localerpl_do_not_redefine_rmdir
#ifdef rmdir
#undef rmdir
#endif
#define rmdir localerpl_rpl_rmdir
#endif
static inline int localerpl_rpl_rmdir(const char *dirname)
{
	return localerpl->rpl_rmdir(dirname);
}

#ifndef localerpl_do_not_redefine_remove
#ifdef remove
#undef remove
#endif
#define remove localerpl_rpl_remove
#endif
static inline int localerpl_rpl_remove(const char *pathname)
{
	return localerpl->rpl_remove(pathname);
}

#ifndef localerpl_do_not_redefine_unlink
#ifdef unlink
#undef undef
#endif
#define unlink localerpl_rpl_unlink
#endif
static inline int localerpl_rpl_unlink(const char *pathname)
{
	return localerpl->rpl_unlink(pathname);
}

#ifndef localerpl_do_not_redefine_rename
#ifdef rename
#undef rename
#endif
#define rename localerpl_rpl_rename
#endif
static inline int localerpl_rpl_rename(const char *old_name, const char *new_name)
{
	return localerpl->rpl_rename(old_name, new_name);
}

#ifndef localerpl_do_not_redefine_chdir
#ifdef chdir
#undef chdir
#endif
#define chdir localerpl_rpl_chdir
#endif
static inline int localerpl_rpl_chdir(const char *path)
{
	return localerpl->rpl_chdir(path);
}

#ifndef localerpl_do_not_redefine_stat
#ifdef stat
#undef stat
#endif
#define stat(name, buf) localerpl_rpl_stat(name, buf)
#endif
#define localerpl_rpl_stat(name, buf) localerpl->rpl_stat(name, buf)

#ifndef localerpl_do_not_redefine_chmod
#ifdef chmod
#undef chmod
#endif
#define chmod localerpl_rpl_chmod
#endif
static inline int localerpl_rpl_chmod(const char *path, int mode)
{
	return localerpl->rpl_chmod(path, mode);
}

#ifndef localerpl_do_not_redefine_setlocale
#ifdef setlocale
#undef setlocale
#endif
#define setlocale localerpl_rpl_setlocale
#endif
static inline char *localerpl_rpl_setlocale(int category, const char *locale)
{
	return localerpl->rpl_setlocale(category, locale);
}

#ifndef localerpl_do_not_redefine_MB_CUR_MAX
#ifdef MB_CUR_MAX
#undef MB_CUR_MAX
#endif
#define MB_CUR_MAX localerpl_MB_CUR_MAX
#endif
#define localerpl_MB_CUR_MAX localerpl->rpl_mb_cur_max()

#ifndef localerpl_do_not_redefine_btowc
#ifdef btowc
#undef btowc
#endif
#define btowc localerpl_rpl_btowc
#endif
static inline wint_t localerpl_rpl_btowc(int c)
{
	return localerpl->rpl_btowc(c);
}

#ifndef localerpl_do_not_redefine_mblen
#ifdef mblen
#undef mblen
#endif
#define mblen localerpl_rpl_mblen
#endif
static inline int localerpl_rpl_mblen(const char *s, size_t n)
{
	return localerpl->rpl_mblen(s, n);
}

#ifndef localerpl_do_not_redefine_mbrlen
#ifdef mbrlen
#undef mbrlen
#endif
#define mbrlen localerpl_rpl_mbrlen
#endif
static inline size_t localerpl_rpl_mbrlen(const char *s, size_t n, mbstate_t *ps)
{
	return localerpl->rpl_mbrlen(s, n, ps);
}

#ifndef localerpl_do_not_redefine_mbrtoc16
#ifdef mbrtoc16
#undef mbrtoc16
#endif
#define mbrtoc16 localerpl_rpl_mbrtoc16
#endif
static inline size_t localerpl_rpl_mbrtoc16(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps)
{
	return localerpl->rpl_mbrtoc16(pwc, s, n, ps);
}

#ifndef localerpl_do_not_redefine_mbrtoc32
#ifdef mbrtoc32
#undef mbrtoc32
#endif
#define mbrtoc32 localerpl_rpl_mbrtoc32
#endif
static inline size_t localerpl_rpl_mbrtoc32(unsigned *pwi, const char *s, size_t n, mbstate_t *ps)
{
	return localerpl->rpl_mbrtoc32(pwi, s, n, ps);
}

#ifndef localerpl_do_not_redefine_c16rtomb
#ifdef c16rtomb
#undef c16rtomb
#endif
#define c16rtomb localerpl_rpl_c16rtomb
#endif
static inline size_t localerpl_rpl_c16rtomb(char *s, wchar_t wc, mbstate_t *ps)
{
	return localerpl->rpl_c16rtomb(s, wc, ps);
}

#ifndef localerpl_do_not_redefine_c32rtomb
#ifdef c32rtomb
#undef c32rtomb
#endif
#define c32rtomb localerpl_rpl_c32rtomb
#endif
static inline size_t localerpl_rpl_c32rtomb(char *s, unsigned wi, mbstate_t *ps)
{
	return localerpl->rpl_c32rtomb(s, wi, ps);
}

#ifndef localerpl_do_not_redefine_mbstowcs
#ifdef mbstowcs
#undef mbstowcs
#endif
#define mbstowcs localerpl_rpl_mbstowcs
#endif
static inline size_t localerpl_rpl_mbstowcs(wchar_t *wcstr, const char *mbstr, size_t count)
{
	return localerpl->rpl_mbstowcs(wcstr, mbstr, count);
}

#ifndef localerpl_do_not_redefine_wcstombs
#ifdef wcstombs
#undef wcstombs
#endif
#define wcstombs localerpl_rpl_wcstombs
#endif
static inline size_t localerpl_rpl_wcstombs(char *mbstr, const wchar_t *wcstr, size_t count)
{
	return localerpl->rpl_wcstombs(mbstr, wcstr, count);
}

#ifndef localerpl_do_not_redefine_strcoll
#ifdef strcoll
#undef strcoll
#endif
#define strcoll localerpl_rpl_strcoll
#endif
static inline int localerpl_rpl_strcoll(const char *s1, const char *s2)
{
	return localerpl->rpl_strcoll(s1, s2);
}

#ifndef localerpl_do_not_redefine_stricmp
#ifdef stricmp
#undef stricmp
#endif
#define stricmp localerpl_rpl_stricmp
#endif
static inline int localerpl_rpl_stricmp(const char *s1, const char *s2)
{
	return localerpl->rpl_stricmp(s1, s2);
}

#ifndef localerpl_do_not_redefine_tolower
#ifdef tolower
#undef tolower
#endif
#define tolower localerpl_rpl_tolower
#endif
static inline int localerpl_rpl_tolower(int c)
{
	return localerpl->rpl_tolower(c);
}

#ifndef localerpl_do_not_redefine_toupper
#ifdef toupper
#undef toupper
#endif
#define toupper localerpl_rpl_toupper
#endif
static inline int localerpl_rpl_toupper(int c)
{
	return localerpl->rpl_toupper(c);
}

#ifndef localerpl_do_not_redefine_isascii
#ifdef isascii
#undef isascii
#endif
#define isascii localerpl_rpl_isascii
#endif
static inline int localerpl_rpl_isascii(int c)
{
	return localerpl->rpl_isascii(c);
}

#ifndef localerpl_do_not_redefine_isalnum
#ifdef isalnum
#undef isalnum
#endif
#define isalnum localerpl_rpl_isalnum
#endif
static inline int localerpl_rpl_isalnum(int c)
{
	return localerpl->rpl_isalnum(c);
}

#ifndef localerpl_do_not_redefine_isalpha
#ifdef isalpha
#undef isalpha
#endif
#define isalpha localerpl_rpl_isalpha
#endif
static inline int localerpl_rpl_isalpha(int c)
{
	return localerpl->rpl_isalpha(c);
}

#ifndef localerpl_do_not_redefine_isblank
#ifdef isblank
#undef isblank
#endif
#define isblank localerpl_rpl_isblank
#endif
static inline int localerpl_rpl_isblank(int c)
{
	return localerpl->rpl_isblank(c);
}

#ifndef localerpl_do_not_redefine_iscntrl
#ifdef iscntrl
#undef iscntrl
#endif
#define iscntrl localerpl_rpl_iscntrl
#endif
static inline int localerpl_rpl_iscntrl(int c)
{
	return localerpl->rpl_iscntrl(c);
}

#ifndef localerpl_do_not_redefine_isdigit
#ifdef isdigit
#undef isdigit
#endif
#define isdigit localerpl_rpl_isdigit
#endif
static inline int localerpl_rpl_isdigit(int c)
{
	return localerpl->rpl_isdigit(c);
}

#ifndef localerpl_do_not_redefine_isgraph
#ifdef isgraph
#undef isgraph
#endif
#define isgraph localerpl_rpl_isgraph
#endif
static inline int localerpl_rpl_isgraph(int c)
{
	return localerpl->rpl_isgraph(c);
}

#ifndef localerpl_do_not_redefine_islower
#ifdef islower
#undef islower
#endif
#define islower localerpl_rpl_islower
#endif
static inline int localerpl_rpl_islower(int c)
{
	return localerpl->rpl_islower(c);
}

#ifndef localerpl_do_not_redefine_isprint
#ifdef isprint
#undef isprint
#endif
#define isprint localerpl_rpl_isprint
#endif
static inline int localerpl_rpl_isprint(int c)
{
	return localerpl->rpl_isprint(c);
}

#ifndef localerpl_do_not_redefine_ispunct
#ifdef ispunct
#undef ispunct
#endif
#define ispunct localerpl_rpl_ispunct
#endif
static inline int localerpl_rpl_ispunct(int c)
{
	return localerpl->rpl_ispunct(c);
}

#ifndef localerpl_do_not_redefine_isspace
#ifdef isspace
#undef isspace
#endif
#define isspace localerpl_rpl_isspace
#endif
static inline int localerpl_rpl_isspace(int c)
{
	return localerpl->rpl_isspace(c);
}

#ifndef localerpl_do_not_redefine_isupper
#ifdef isupper
#undef isupper
#endif
#define isupper localerpl_rpl_isupper
#endif
static inline int localerpl_rpl_isupper(int c)
{
	return localerpl->rpl_isupper(c);
}

#ifndef localerpl_do_not_redefine_isxdigit
#ifdef isxdigit
#undef isxdigit
#endif
#define isxdigit localerpl_rpl_isxdigit
#endif
static inline int localerpl_rpl_isxdigit(int c)
{
	return localerpl->rpl_isxdigit(c);
}

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

#define wcscoll localerpl_rpl_wcscoll
static inline int wcscoll(const wchar_t *s1, const wchar_t *s2)
{
	(void)s1, (void)s2;
	assert(0); /* use c32scoll instead */
	return -1;
}

#define wcsicmp localerpl_rpl_wcsicmp
static inline int wcsicmp(const wchar_t *s1, const wchar_t *s2)
{
	(void)s1, (void)s2;
	assert(0); /* use c32sicmp instead */
	return -1;
}

#define towlower localerpl_rpl_towlower
static inline wint_t towlower(wint_t c)
{
	assert(0); /* use c32tolower instead */
	return c;
}

#define towupper localerpl_rpl_towupper
static inline wint_t towupper(wint_t c)
{
	assert(0); /* use c32toupper instead */
	return c;
}

#define iswascii localerpl_rpl_iswascii
static inline int iswascii(wint_t c)
{
	(void)c;
	assert(0); /* use c32isascii instead */
	return 0;
}

#define iswalnum localerpl_rpl_iswalnum
static inline int iswalnum(wint_t c)
{
	(void)c;
	assert(0); /* use c32isalnum instead */
	return 0;
}

#define iswalpha localerpl_rpl_iswalpha
static inline int iswalpha(wint_t c)
{
	(void)c;
	assert(0); /* use c32isalpha instead */
	return 0;
}

#define iswblank localerpl_rpl_iswblank
static inline int iswblank(wint_t c)
{
	(void)c;
	assert(0); /* use c32isblank instead */
	return 0;
}

#define iswcntrl localerpl_rpl_iswcntrl
static inline int iswcntrl(wint_t c)
{
	(void)c;
	assert(0); /* use c32iscntrl instead */
	return 0;
}

#define iswdigit localerpl_rpl_iswdigit
static inline int iswdigit(wint_t c)
{
	(void)c;
	assert(0); /* use c32isdigit instead */
	return 0;
}

#define iswgraph localerpl_rpl_iswgraph
static inline int iswgraph(wint_t c)
{
	(void)c;
	assert(0); /* use c32isgraph instead */
	return 0;
}

#define iswlower localerpl_rpl_iswlower
static inline int iswlower(wint_t c)
{
	(void)c;
	assert(0); /* use c32islower instead */
	return 0;
}

#define iswprint localerpl_rpl_iswprint
static inline int iswprint(wint_t c)
{
	(void)c;
	assert(0); /* use c32isprint instead */
	return 0;
}

#define iswpunct localerpl_rpl_iswpunct
static inline int iswpunct(wint_t c)
{
	(void)c;
	assert(0); /* use c32ispunct instead */
	return 0;
}

#define iswspace localerpl_rpl_iswspace
static inline int iswspace(wint_t c)
{
	(void)c;
	assert(0); /* use c32isspace instead */
	return 0;
}

#define iswupper localerpl_rpl_iswupper
static inline int iswupper(wint_t c)
{
	(void)c;
	assert(0); /* use c32isupper instead */
	return 0;
}

#define iswxdigit localerpl_rpl_iswxdigit
static inline int iswxdigit(wint_t c)
{
	(void)c;
	assert(0); /* use c32isxdigit instead */
	return 0;
}

#define wctype localerpl_rpl_wctype
static inline wctype_t wctype(const char *name)
{
	(void)name;
	assert(0); /* use c32ctype instead */
	return 0;
}

#define iswctype localerpl_rpl_iswctype
static inline int iswctype(wint_t c, wctype_t desc)
{
	(void)c, (void)desc;
	assert(0); /* use c32isctype instead */
	return 0;
}

#define mbtowc localerpl_rpl_mbtowc
static inline int mbtowc(wchar_t *pwc, const char *s, size_t n)
{
	(void)pwc, (void)s, (void)n;
	assert(0); /* use mbrtoc16 or mbrtoc32 instead */
	return 0;
}

#define mbrtowc localerpl_rpl_mbrtowc
static inline size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps)
{
	(void)pwc, (void)s, (void)n, (void)ps;
	assert(0); /* use mbrtoc16 or mbrtoc32 instead */
	return 0;
}

#define wctomb localerpl_rpl_wctomb
static inline int wctomb(char *s, wchar_t wc)
{
	(void)s, (void)wc;
	assert(0); /* use c16rtomb or c32rtomb intead */
	return 0;
}

#define wcrtomb localerpl_rpl_wcrtomb
static inline size_t wcrtomb(char *s, wchar_t wc, mbstate_t *ps)
{
	(void)s, (void)wc, (void)ps;
	assert(0); /* use c16rtomb or c32rtomb intead */
	return 0;
}

#endif /* LOCALERPL_NEED_CALLBACKS */

#ifndef localerpl_do_not_redefine_c32scoll
#ifdef c32scoll
#undef c32scoll
#endif
#define c32scoll localerpl_rpl_c32scoll
#endif
static inline int localerpl_rpl_c32scoll(const unsigned *s1, const unsigned *s2)
{
	return localerpl->rpl_c32scoll(s1, s2);
}

#ifndef localerpl_do_not_redefine_c32sicmp
#ifdef c32sicmp
#undef c32sicmp
#endif
#define c32sicmp localerpl_rpl_c32sicmp
#endif
static inline int localerpl_rpl_c32sicmp(const unsigned *s1, const unsigned *s2)
{
	return localerpl->rpl_c32sicmp(s1, s2);
}

#ifndef localerpl_do_not_redefine_c32tolower
#ifdef c32tolower
#undef c32tolower
#endif
#define c32tolower localerpl_rpl_c32tolower
#endif
static inline unsigned localerpl_rpl_c32tolower(unsigned c)
{
	return localerpl->rpl_c32tolower(c);
}

#ifndef localerpl_do_not_redefine_c32toupper
#ifdef c32toupper
#undef c32toupper
#endif
#define c32toupper localerpl_rpl_c32toupper
#endif
static inline unsigned localerpl_rpl_c32toupper(unsigned c)
{
	return localerpl->rpl_c32toupper(c);
}

#ifndef localerpl_do_not_redefine_c32isascii
#ifdef c32isascii
#undef c32isascii
#endif
#define c32isascii localerpl_rpl_c32isascii
#endif
static inline int localerpl_rpl_c32isascii(unsigned c)
{
	return localerpl->rpl_c32isascii(c);
}

#ifndef localerpl_do_not_redefine_c32isalnum
#ifdef c32isalnum
#undef c32isalnum
#endif
#define c32isalnum localerpl_rpl_c32isalnum
#endif
static inline int localerpl_rpl_c32isalnum(unsigned c)
{
	return localerpl->rpl_c32isalnum(c);
}

#ifndef localerpl_do_not_redefine_c32isalpha
#ifdef c32isalpha
#undef c32isalpha
#endif
#define c32isalpha localerpl_rpl_c32isalpha
#endif
static inline int localerpl_rpl_c32isalpha(unsigned c)
{
	return localerpl->rpl_c32isalpha(c);
}

#ifndef localerpl_do_not_redefine_c32isblank
#ifdef c32isblank
#undef c32isblank
#endif
#define c32isblank localerpl_rpl_c32isblank
#endif
static inline int localerpl_rpl_c32isblank(unsigned c)
{
	return localerpl->rpl_c32isblank(c);
}

#ifndef localerpl_do_not_redefine_c32iscntrl
#ifdef c32iscntrl
#undef c32iscntrl
#endif
#define c32iscntrl localerpl_rpl_c32iscntrl
#endif
static inline int localerpl_rpl_c32iscntrl(unsigned c)
{
	return localerpl->rpl_c32iscntrl(c);
}

#ifndef localerpl_do_not_redefine_c32isdigit
#ifdef c32isdigit
#undef c32isdigit
#endif
#define c32isdigit localerpl_rpl_c32isdigit
#endif
static inline int localerpl_rpl_c32isdigit(unsigned c)
{
	return localerpl->rpl_c32isdigit(c);
}

#ifndef localerpl_do_not_redefine_c32isgraph
#ifdef c32isgraph
#undef c32isgraph
#endif
#define c32isgraph localerpl_rpl_c32isgraph
#endif
static inline int localerpl_rpl_c32isgraph(unsigned c)
{
	return localerpl->rpl_c32isgraph(c);
}

#ifndef localerpl_do_not_redefine_c32islower
#ifdef c32islower
#undef c32islower
#endif
#define c32islower localerpl_rpl_c32islower
#endif
static inline int localerpl_rpl_c32islower(unsigned c)
{
	return localerpl->rpl_c32islower(c);
}

#ifndef localerpl_do_not_redefine_c32isprint
#ifdef c32isprint
#undef c32isprint
#endif
#define c32isprint localerpl_rpl_c32isprint
#endif
static inline int localerpl_rpl_c32isprint(unsigned c)
{
	return localerpl->rpl_c32isprint(c);
}

#ifndef localerpl_do_not_redefine_c32ispunct
#ifdef c32ispunct
#undef c32ispunct
#endif
#define c32ispunct localerpl_rpl_c32ispunct
#endif
static inline int localerpl_rpl_c32ispunct(unsigned c)
{
	return localerpl->rpl_c32ispunct(c);
}

#ifndef localerpl_do_not_redefine_c32isspace
#ifdef c32isspace
#undef c32isspace
#endif
#define c32isspace localerpl_rpl_c32isspace
#endif
static inline int localerpl_rpl_c32isspace(unsigned c)
{
	return localerpl->rpl_c32isspace(c);
}

#ifndef localerpl_do_not_redefine_c32isupper
#ifdef c32isupper
#undef c32isupper
#endif
#define c32isupper localerpl_rpl_c32isupper
#endif
static inline int localerpl_rpl_c32isupper(unsigned c)
{
	return localerpl->rpl_c32isupper(c);
}

#ifndef localerpl_do_not_redefine_c32isxdigit
#ifdef c32isxdigit
#undef c32isxdigit
#endif
#define c32isxdigit localerpl_rpl_c32isxdigit
#endif
static inline int localerpl_rpl_c32isxdigit(unsigned c)
{
	return localerpl->rpl_c32isxdigit(c);
}

#ifndef localerpl_do_not_redefine_c32ctype
#ifdef c32ctype
#undef c32ctype
#endif
#define c32ctype localerpl_rpl_c32ctype
#endif
static inline c32ctype_t localerpl_rpl_c32ctype(const char *name)
{
	return localerpl->rpl_c32ctype(name);
}

#ifndef localerpl_do_not_redefine_c32isctype
#ifdef c32isctype
#undef c32isctype
#endif
#define c32isctype localerpl_rpl_c32isctype
#endif
static inline int localerpl_rpl_c32isctype(unsigned c, c32ctype_t desc)
{
	return localerpl->rpl_c32isctype(c, desc);
}

#ifndef localerpl_do_not_redefine_mbstoc32s
#ifdef mbstoc32s
#undef mbstoc32s
#endif
#define mbstoc32s localerpl_rpl_mbstoc32s
#endif
static inline size_t localerpl_rpl_mbstoc32s(unsigned *dst, const char *src, size_t n)
{
	return localerpl->rpl_mbstoc32s(dst, src, n);
}

#ifndef localerpl_do_not_redefine_c32stombs
#ifdef c32stombs
#undef c32stombs
#endif
#define c32stombs localerpl_rpl_c32stombs
#endif
static inline size_t localerpl_rpl_c32stombs(char *dst, const unsigned *src, size_t n)
{
	return localerpl->rpl_c32stombs(dst, src, n);
}

#ifndef localerpl_do_not_redefine_wcstoc32s
#ifdef wcstoc32s
#undef wcstoc32s
#endif
#define wcstoc32s localerpl_rpl_wcstoc32s
#endif
static inline size_t localerpl_rpl_wcstoc32s(unsigned *dst, const wchar_t *src, size_t n)
{
	return localerpl->rpl_wcstoc32s(dst, src, n);
}

#ifndef localerpl_do_not_redefine_c32stowcs
#ifdef c32stowcs
#undef c32stowcs
#endif
#define c32stowcs localerpl_rpl_c32stowcs
#endif
static inline size_t localerpl_rpl_c32stowcs(wchar_t *dst, const unsigned *src, size_t n)
{
	return localerpl->rpl_c32stowcs(dst, src, n);
}

#if !defined(_MSC_VER) || !defined(_PREFAST_)
#ifndef localerpl_do_not_redefine_printf
#ifdef printf
#undef printf
#endif
#define printf localerpl_rpl_printf
#endif
#endif
#define localerpl_rpl_printf localerpl->rpl_printf

#if !defined(_MSC_VER) || !defined(_PREFAST_)
#ifndef localerpl_do_not_redefine_fprintf
#ifdef fprintf
#undef fprintf
#endif
#define fprintf localerpl_rpl_fprintf
#endif
#endif
#define localerpl_rpl_fprintf localerpl->rpl_fprintf

#ifndef localerpl_do_not_redefine_vprintf
#ifdef vprintf
#undef vprintf
#endif
#define vprintf localerpl_rpl_vprintf
#endif
ATTRIBUTE_PRINTF(format, 1, 0)
static inline int localerpl_rpl_vprintf(const char *format, va_list ap)
{
	return localerpl->rpl_vprintf(format, ap);
}

#ifndef localerpl_do_not_redefine_vfprintf
#ifdef vfprintf
#undef vfprintf
#endif
#define vfprintf localerpl_rpl_vfprintf
#endif
ATTRIBUTE_PRINTF(format, 2, 0)
static inline int localerpl_rpl_vfprintf(FILE *stream, const char *format, va_list ap)
{
	return localerpl->rpl_vfprintf(stream, format, ap);
}

#ifndef localerpl_do_not_redefine_strerror
#ifdef strerror
#undef strerror
#endif
#define strerror localerpl_rpl_strerror
#endif
static inline char *localerpl_rpl_strerror(int error_number)
{
	return localerpl->rpl_strerror(error_number);
}

#ifndef localerpl_do_not_redefine_strftime
#ifdef strftime
#undef strftime
#endif
#define strftime localerpl_rpl_strftime
#endif
ATTRIBUTE_PRINTF(fmt, 3, 0)
static inline size_t localerpl_rpl_strftime(char *s, size_t mx, const char *fmt, const struct tm *t)
{
	return localerpl->rpl_strftime(s, mx, fmt, t);
}

#ifndef localerpl_do_not_redefine_mkstemp
#ifdef mkstemp
#undef mkstemp
#endif
#define mkstemp localerpl_rpl_mkstemp
#endif
static inline int localerpl_rpl_mkstemp(char *templ)
{
	return localerpl->rpl_mkstemp(templ);
}

#ifndef localerpl_do_not_redefine_environ
#ifdef environ
#undef environ
#endif
#define environ localerpl_rpl_environ()
#endif
static inline char **localerpl_rpl_environ(void)
{
	return localerpl->rpl_environ();
}

#ifndef localerpl_do_not_redefine_getenv
#ifdef getenv
#undef getenv
#endif
#define getenv localerpl_rpl_getenv
#endif
static inline char *localerpl_rpl_getenv(const char *name)
{
	return localerpl->rpl_getenv(name);
}

#ifndef localerpl_do_not_redefine_setenv
#ifdef setenv
#undef setenv
#endif
#define setenv localerpl_rpl_setenv
#endif
static inline int localerpl_rpl_setenv(const char *name, const char *value, int overwrite)
{
	return localerpl->rpl_setenv(name, value, overwrite);
}

#ifndef localerpl_do_not_redefine_unsetenv
#ifdef unsetenv
#undef unsetenv
#endif
#define unsetenv localerpl_rpl_unsetenv
#endif
static inline int localerpl_rpl_unsetenv(const char *name)
{
	return localerpl->rpl_unsetenv(name);
}

#ifndef localerpl_do_not_redefine_clearenv
#ifdef clearenv
#undef clearenv
#endif
#define clearenv localerpl_rpl_clearenv
#endif
static inline int localerpl_rpl_clearenv(void)
{
	return localerpl->rpl_clearenv();
}

#ifndef localerpl_do_not_redefine_spawnvp
#ifdef spawnvp
#undef spawnvp
#endif
#define spawnvp localerpl_rpl_spawnvp
#endif
static inline intptr_t localerpl_rpl_spawnvp(int mode, const char *cmdname, const char *const *argv)
{
	return localerpl->rpl_spawnvp(mode, cmdname, argv);
}

#ifndef localerpl_do_not_redefine_spawnl
#ifdef spawnl
#undef spawnl
#endif
#define spawnl localerpl_rpl_spawnl
#endif
#define localerpl_rpl_spawnl localerpl->rpl_spawnl

#ifndef localerpl_do_not_redefine_popen
#ifdef popen
#undef popen
#endif
#define popen localerpl_rpl_popen
#endif
static inline FILE *localerpl_rpl_popen(const char *command, const char *mode)
{
	return localerpl->rpl_popen(command, mode);
}

#endif /* LOCALE_RPL_IMPL */

#endif /* LOCALERPL_H_INCLUDED */
