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

/* Function attributes.  */
#ifndef __GNUC__
# define __attribute__(arg)
#endif

/* Annotate function parameter that cannot be NULL.  */
#ifndef ATTRIBUTE_NONNULL
# define ATTRIBUTE_NONNULL(a) __attribute__ ((__nonnull__ (a)))
#endif

/* Used in ATTRIBUTE_PRINTF.  */
#ifndef ATTRIBUTE_PRINTF_FORMAT
# ifdef __MINGW_PRINTF_FORMAT
#  define ATTRIBUTE_PRINTF_FORMAT __MINGW_PRINTF_FORMAT
# else
#  define ATTRIBUTE_PRINTF_FORMAT __printf__
# endif
#endif

/* Annotate parameters of printf-like function, e.g.:
  ATTRIBUTE_PRINTF(fmt, 2, 3) int my_printf(int param, const char *fmt, ...);
   or
  ATTRIBUTE_PRINTF(fmt, 2, 0) int my_vprintf(int param, const char *fmt, va_list ap); */
#ifndef ATTRIBUTE_PRINTF
# if defined(_MSC_VER) && defined(_PREFAST_)
#  define ATTRIBUTE_PRINTF(fmt, m, n) _At_(fmt, _Printf_format_string_)
# else
#  define ATTRIBUTE_PRINTF(fmt, m, n) \
	__attribute__ ((__format__ (ATTRIBUTE_PRINTF_FORMAT, m, n))) \
	ATTRIBUTE_NONNULL(m)
# endif
#endif

/* Annotate parameters of printf-like function pointer, e.g.:
  ATTRIBUTE_PRINTF_PTR(fmt, 2, 3) int (*my_printf)(int param, const char *fmt, ...);
   or
  ATTRIBUTE_PRINTF_PTR(fmt, 2, 0) int (*my_vprintf)(int param, const char *fmt, va_list ap); */
#ifndef ATTRIBUTE_PRINTF_PTR
# if defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 2))
#  define ATTRIBUTE_PRINTF_PTR(fmt, m, n) ATTRIBUTE_PRINTF(fmt, m, n)
# else
#  define ATTRIBUTE_PRINTF_PTR(fmt, m, n)
# endif
#endif

/* in UTF-8 locale MB_CUR_MAX == 4 */
#if MB_LEN_MAX < 4
#undef MB_LEN_MAX
#define MB_LEN_MAX 4
#endif

struct _stat64;
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

	int (*rpl_stat)(const char *path, struct _stat64 *buf);
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
};

/* Change localerpl global pointer.  */
void change_localerpl(int to_utf8);

/* Returns non-zero if using UTF-8 replacements.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
int localerpl_is_utf8(void);

/* Print to buffer, malloc'ate new buffer if necessary.
   Returns -1 on error, else - number of chars printed, not counting terminating '\0'.
   If successful, buffer is always '\0'-terminated.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(buf, A_Notnull)
A_At(*buf, A_Pre_writable_size(buf_size) A_Post_z A_Post_readable_size(return + 1))
A_At(format, A_Printf_format_string)
A_Success(return != -1)
#endif
#ifndef SAL_DEFS_H_INCLUDED
ATTRIBUTE_PRINTF(format, 4, 0)
#endif
int vsnprintf_helper(char **const buf, size_t buf_size, const char format[], va_list ap);

/* wcslen(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(str, A_In_z)
#endif
size_t c32slen(const unsigned *str);

/* wcschr(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_In_z)
A_Ret_maybenull
#endif
unsigned *c32schr(const unsigned *s, unsigned c);

/* wcsrchr(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_In_z)
A_Ret_maybenull
#endif
unsigned *c32srchr(const unsigned *s, unsigned c);

/* strchrnul(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_In_z)
A_Ret_never_null
A_Ret_z
#endif
unsigned *c32schrnul(const unsigned *s, unsigned c);

#ifndef LOCALE_RPL_IMPL

/* Global pointer.  */
extern const struct localerpl *localerpl;

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

#ifdef open
#undef open
#endif
#define open localerpl->rpl_open

#ifdef fopen
#undef fopen
#endif
#define fopen localerpl_rpl_fopen
static inline FILE *fopen(const char *path, const char *mode)
{
	return localerpl->rpl_fopen(path, mode);
}

#ifdef read
#undef read
#endif
#define read localerpl_rpl_read
static inline int read(int fd, void *buf, unsigned count)
{
	return localerpl->rpl_read(fd, buf, count);
}

#ifdef write
#undef write
#endif
#define write localerpl_rpl_write
static inline int write(int fd, const void *buf, unsigned count)
{
	return localerpl->rpl_write(fd, buf, count);
}

#ifdef fread
#undef fread
#endif
#define fread localerpl_rpl_fread
static inline size_t fread(void *buf, size_t size, size_t nmemb, FILE *stream)
{
	return localerpl->rpl_fread(buf, size, nmemb, stream);
}

#ifdef fwrite
#undef fwrite
#endif
#define fwrite localerpl_rpl_fwrite
static inline size_t fwrite(const void *buf, size_t size, size_t nmemb, FILE *stream)
{
	return localerpl->rpl_fwrite(buf, size, nmemb, stream);
}

#ifdef putchar
#undef putchar
#endif
#define putchar localerpl_rpl_putchar
static inline int putchar(int c)
{
	return localerpl->rpl_putchar(c);
}

#ifdef fputc
#undef fputc
#endif
#define fputc localerpl_rpl_fputc
static inline int fputc(int c, FILE *stream)
{
	return localerpl->rpl_fputc(c, stream);
}

#ifdef getchar
#undef getchar
#endif
#define getchar localerpl_rpl_getchar
static inline int getchar(void)
{
	return localerpl->rpl_getchar();
}

#ifdef fgetc
#undef fgetc
#endif
#define fgetc localerpl_rpl_fgetc
static inline int fgetc(FILE *stream)
{
	return localerpl->rpl_fgetc(stream);
}

#ifdef puts
#undef puts
#endif
#define puts localerpl_rpl_puts
static inline int puts(const char *s)
{
	return localerpl->rpl_puts(s);
}

#ifdef fputs
#undef fputs
#endif
#define fputs localerpl_rpl_fputs
static inline int fputs(const char *s, FILE *stream)
{
	return localerpl->rpl_fputs(s, stream);
}

#ifdef fgets
#undef fgets
#endif
#define fgets localerpl_rpl_fgets
static inline char *fgets(char *s, int size, FILE *stream)
{
	return localerpl->rpl_fgets(s, size, stream);
}

#ifdef mkdir
#undef mkdir
#endif
#define mkdir localerpl_rpl_mkdir
static inline int mkdir(const char *dirname)
{
	return localerpl->rpl_mkdir(dirname);
}

#ifdef rmdir
#undef rmdir
#endif
#define rmdir localerpl_rpl_rmdir
static inline int rmdir(const char *dirname)
{
	return localerpl->rpl_rmdir(dirname);
}

#ifdef remove
#undef remove
#endif
#define remove localerpl_rpl_remove
static inline int remove(const char *pathname)
{
	return localerpl->rpl_remove(pathname);
}

#ifdef unlink
#undef undef
#endif
#define unlink localerpl_rpl_unlink
static inline int unlink(const char *pathname)
{
	return localerpl->rpl_unlink(pathname);
}

#ifdef rename
#undef rename
#endif
#define rename localerpl_rpl_rename
static inline int rename(const char *old_name, const char *new_name)
{
	return localerpl->rpl_rename(old_name, new_name);
}

#ifdef chdir
#undef chdir
#endif
#define chdir localerpl_rpl_chdir
static inline int chdir(const char *path)
{
	return localerpl->rpl_chdir(path);
}

#ifdef stat
#undef stat
#endif
#define stat(name, buf) localerpl->rpl_stat(name, buf)

#ifdef chmod
#undef chmod
#endif
#define chmod localerpl_rpl_chmod
static inline int chmod(const char *path, int mode)
{
	return localerpl->rpl_chmod(path, mode);
}

#ifdef setlocale
#undef setlocale
#endif
#define setlocale localerpl_rpl_setlocale
static inline char *setlocale(int category, const char *locale)
{
	return localerpl->rpl_setlocale(category, locale);
}

#ifdef MB_CUR_MAX
#undef MB_CUR_MAX
#endif
#define MB_CUR_MAX localerpl->rpl_mb_cur_max()

#ifdef btowc
#undef btowc
#endif
#define btowc localerpl_rpl_btowc
static inline wint_t btowc(int c)
{
	return localerpl->rpl_btowc(c);
}

#ifdef mblen
#undef mblen
#endif
#define mblen localerpl_rpl_mblen
static inline int mblen(const char *s, size_t n)
{
	return localerpl->rpl_mblen(s, n);
}

#ifdef mbrlen
#undef mbrlen
#endif
#define mbrlen localerpl_rpl_mbrlen
static inline size_t mbrlen(const char *s, size_t n, mbstate_t *ps)
{
	return localerpl->rpl_mbrlen(s, n, ps);
}

#ifdef mbrtoc16
#undef mbrtoc16
#endif
#define mbrtoc16 localerpl_rpl_mbrtoc16
static inline size_t mbrtoc16(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps)
{
	return localerpl->rpl_mbrtoc16(pwc, s, n, ps);
}

#ifdef mbrtoc32
#undef mbrtoc32
#endif
#define mbrtoc32 localerpl_rpl_mbrtoc32
static inline size_t mbrtoc32(unsigned *pwi, const char *s, size_t n, mbstate_t *ps)
{
	return localerpl->rpl_mbrtoc32(pwi, s, n, ps);
}

#ifdef c16rtomb
#undef c16rtomb
#endif
#define c16rtomb localerpl_rpl_c16rtomb
static inline size_t c16rtomb(char *s, wchar_t wc, mbstate_t *ps)
{
	return localerpl->rpl_c16rtomb(s, wc, ps);
}

#ifdef c32rtomb
#undef c32rtomb
#endif
#define c32rtomb localerpl_rpl_c32rtomb
static inline size_t c32rtomb(char *s, unsigned wi, mbstate_t *ps)
{
	return localerpl->rpl_c32rtomb(s, wi, ps);
}

#ifdef mbstowcs
#undef mbstowcs
#endif
#define mbstowcs localerpl_rpl_mbstowcs
static inline size_t mbstowcs(wchar_t *wcstr, const char *mbstr, size_t count)
{
	return localerpl->rpl_mbstowcs(wcstr, mbstr, count);
}

#ifdef wcstombs
#undef wcstombs
#endif
#define wcstombs localerpl_rpl_wcstombs
static inline size_t wcstombs(char *mbstr, const wchar_t *wcstr, size_t count)
{
	return localerpl->rpl_wcstombs(mbstr, wcstr, count);
}

#ifdef strcoll
#undef strcoll
#endif
#define strcoll localerpl_rpl_strcoll
static inline int strcoll(const char *s1, const char *s2)
{
	return localerpl->rpl_strcoll(s1, s2);
}

#ifdef tolower
#undef tolower
#endif
#define tolower localerpl_rpl_tolower
static inline int tolower(int c)
{
	return localerpl->rpl_tolower(c);
}

#ifdef toupper
#undef toupper
#endif
#define toupper localerpl_rpl_toupper
static inline int toupper(int c)
{
	return localerpl->rpl_toupper(c);
}

#ifdef isascii
#undef isascii
#endif
#define isascii localerpl_rpl_isascii
static inline int isascii(int c)
{
	return localerpl->rpl_isascii(c);
}

#ifdef isalnum
#undef isalnum
#endif
#define isalnum localerpl_rpl_isalnum
static inline int isalnum(int c)
{
	return localerpl->rpl_isalnum(c);
}

#ifdef isalpha
#undef isalpha
#endif
#define isalpha localerpl_rpl_isalpha
static inline int isalpha(int c)
{
	return localerpl->rpl_isalpha(c);
}

#ifdef isblank
#undef isblank
#endif
#define isblank localerpl_rpl_isblank
static inline int isblank(int c)
{
	return localerpl->rpl_isblank(c);
}

#ifdef iscntrl
#undef iscntrl
#endif
#define iscntrl localerpl_rpl_iscntrl
static inline int iscntrl(int c)
{
	return localerpl->rpl_iscntrl(c);
}

#ifdef isdigit
#undef isdigit
#endif
#define isdigit localerpl_rpl_isdigit
static inline int isdigit(int c)
{
	return localerpl->rpl_isdigit(c);
}

#ifdef isgraph
#undef isgraph
#endif
#define isgraph localerpl_rpl_isgraph
static inline int isgraph(int c)
{
	return localerpl->rpl_isgraph(c);
}

#ifdef islower
#undef islower
#endif
#define islower localerpl_rpl_islower
static inline int islower(int c)
{
	return localerpl->rpl_islower(c);
}

#ifdef isprint
#undef isprint
#endif
#define isprint localerpl_rpl_isprint
static inline int isprint(int c)
{
	return localerpl->rpl_isprint(c);
}

#ifdef ispunct
#undef ispunct
#endif
#define ispunct localerpl_rpl_ispunct
static inline int ispunct(int c)
{
	return localerpl->rpl_ispunct(c);
}

#ifdef isspace
#undef isspace
#endif
#define isspace localerpl_rpl_isspace
static inline int isspace(int c)
{
	return localerpl->rpl_isspace(c);
}

#ifdef isupper
#undef isupper
#endif
#define isupper localerpl_rpl_isupper
static inline int isupper(int c)
{
	return localerpl->rpl_isupper(c);
}

#ifdef isxdigit
#undef isxdigit
#endif
#define isxdigit localerpl_rpl_isxdigit
static inline int isxdigit(int c)
{
	return localerpl->rpl_isxdigit(c);
}

/* banned functions */

#ifdef wcscoll
#undef wcscoll
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

#ifdef c32scoll
#undef c32scoll
#endif
#define c32scoll localerpl_rpl_c32scoll
static inline int c32scoll(const unsigned *s1, const unsigned *s2)
{
	return localerpl->rpl_c32scoll(s1, s2);
}

#ifdef c32tolower
#undef c32tolower
#endif
#define c32tolower localerpl_rpl_c32tolower
static inline unsigned c32tolower(unsigned c)
{
	return localerpl->rpl_c32tolower(c);
}

#ifdef c32toupper
#undef c32toupper
#endif
#define c32toupper localerpl_rpl_c32toupper
static inline unsigned c32toupper(unsigned c)
{
	return localerpl->rpl_c32toupper(c);
}

#ifdef c32isascii
#undef c32isascii
#endif
#define c32isascii localerpl_rpl_c32isascii
static inline int c32isascii(unsigned c)
{
	return localerpl->rpl_c32isascii(c);
}

#ifdef c32isalnum
#undef c32isalnum
#endif
#define c32isalnum localerpl_rpl_c32isalnum
static inline int c32isalnum(unsigned c)
{
	return localerpl->rpl_c32isalnum(c);
}

#ifdef c32isalpha
#undef c32isalpha
#endif
#define c32isalpha localerpl_rpl_c32isalpha
static inline int c32isalpha(unsigned c)
{
	return localerpl->rpl_c32isalpha(c);
}

#ifdef c32isblank
#undef c32isblank
#endif
#define c32isblank localerpl_rpl_c32isblank
static inline int c32isblank(unsigned c)
{
	return localerpl->rpl_c32isblank(c);
}

#ifdef c32iscntrl
#undef c32iscntrl
#endif
#define c32iscntrl localerpl_rpl_c32iscntrl
static inline int c32iscntrl(unsigned c)
{
	return localerpl->rpl_c32iscntrl(c);
}

#ifdef c32isdigit
#undef c32isdigit
#endif
#define c32isdigit localerpl_rpl_c32isdigit
static inline int c32isdigit(unsigned c)
{
	return localerpl->rpl_c32isdigit(c);
}

#ifdef c32isgraph
#undef c32isgraph
#endif
#define c32isgraph localerpl_rpl_c32isgraph
static inline int c32isgraph(unsigned c)
{
	return localerpl->rpl_c32isgraph(c);
}

#ifdef c32islower
#undef c32islower
#endif
#define c32islower localerpl_rpl_c32islower
static inline int c32islower(unsigned c)
{
	return localerpl->rpl_c32islower(c);
}

#ifdef c32isprint
#undef c32isprint
#endif
#define c32isprint localerpl_rpl_c32isprint
static inline int c32isprint(unsigned c)
{
	return localerpl->rpl_c32isprint(c);
}

#ifdef c32ispunct
#undef c32ispunct
#endif
#define c32ispunct localerpl_rpl_c32ispunct
static inline int c32ispunct(unsigned c)
{
	return localerpl->rpl_c32ispunct(c);
}

#ifdef c32isspace
#undef c32isspace
#endif
#define c32isspace localerpl_rpl_c32isspace
static inline int c32isspace(unsigned c)
{
	return localerpl->rpl_c32isspace(c);
}

#ifdef c32isupper
#undef c32isupper
#endif
#define c32isupper localerpl_rpl_c32isupper
static inline int c32isupper(unsigned c)
{
	return localerpl->rpl_c32isupper(c);
}

#ifdef c32isxdigit
#undef c32isxdigit
#endif
#define c32isxdigit localerpl_rpl_c32isxdigit
static inline int c32isxdigit(unsigned c)
{
	return localerpl->rpl_c32isxdigit(c);
}

#ifdef c32ctype
#undef c32ctype
#endif
#define c32ctype localerpl_rpl_c32ctype
static inline c32ctype_t c32ctype(const char *name)
{
	return localerpl->rpl_c32ctype(name);
}

#ifdef c32isctype
#undef c32isctype
#endif
#define c32isctype localerpl_rpl_c32isctype
static inline int c32isctype(unsigned c, c32ctype_t desc)
{
	return localerpl->rpl_c32isctype(c, desc);
}

#ifdef mbstoc32s
#undef mbstoc32s
#endif
#define mbstoc32s localerpl_rpl_mbstoc32s
static inline size_t mbstoc32s(unsigned *dst, const char *src, size_t n)
{
	return localerpl->rpl_mbstoc32s(dst, src, n);
}

#ifdef c32stombs
#undef c32stombs
#endif
#define c32stombs localerpl_rpl_c32stombs
static inline size_t c32stombs(char *dst, const unsigned *src, size_t n)
{
	return localerpl->rpl_c32stombs(dst, src, n);
}

#ifdef wcstoc32s
#undef wcstoc32s
#endif
#define wcstoc32s localerpl_rpl_wcstoc32s
static inline size_t wcstoc32s(unsigned *dst, const wchar_t *src, size_t n)
{
	return localerpl->rpl_wcstoc32s(dst, src, n);
}

#ifdef c32stowcs
#undef c32stowcs
#endif
#define c32stowcs localerpl_rpl_c32stowcs
static inline size_t c32stowcs(wchar_t *dst, const unsigned *src, size_t n)
{
	return localerpl->rpl_c32stowcs(dst, src, n);
}

#ifdef printf
#undef printf
#endif
#ifdef fprintf
#undef fprintf
#endif

#if !defined(_MSC_VER) || !defined(_PREFAST_)
#define printf  localerpl->rpl_printf
#define fprintf localerpl->rpl_fprintf
#endif

#ifdef vprintf
#undef vprintf
#endif
#define vprintf localerpl->rpl_vprintf

#ifdef vfprintf
#undef vfprintf
#endif
#define vfprintf localerpl->rpl_vfprintf

#ifdef strerror
#undef strerror
#endif
#define strerror localerpl_rpl_strerror
static inline char *strerror(int error_number)
{
	return localerpl->rpl_strerror(error_number);
}

#ifdef mkstemp
#undef mkstemp
#endif
#define mkstemp localerpl_rpl_mkstemp
static inline int mkstemp(char *templ)
{
	return localerpl->rpl_mkstemp(templ);
}

#ifdef environ
#undef environ
#endif
#define environ localerpl->rpl_environ()

#ifdef getenv
#undef getenv
#endif
#define getenv localerpl_rpl_getenv
static inline char *getenv(const char *name)
{
	return localerpl->rpl_getenv(name);
}

#ifdef setenv
#undef setenv
#endif
#define setenv localerpl_rpl_setenv
static inline int setenv(const char *name, const char *value, int overwrite)
{
	return localerpl->rpl_setenv(name, value, overwrite);
}

#ifdef unsetenv
#undef unsetenv
#endif
#define unsetenv localerpl_rpl_unsetenv
static inline int unsetenv(const char *name)
{
	return localerpl->rpl_unsetenv(name);
}

#ifdef clearenv
#undef clearenv
#endif
#define clearenv localerpl_rpl_clearenv
static inline int clearenv(void)
{
	return localerpl->rpl_clearenv();
}

#ifdef spawnvp
#undef spawnvp
#endif
#define spawnvp localerpl->rpl_spawnvp

#endif /* LOCALE_RPL_IMPL */

#endif /* LOCALERPL_H_INCLUDED */
