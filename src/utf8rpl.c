/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* utf8rpl.c */

/* Replacements which do utf8 -> wchar conversion of their arguments.  */

#define WIN32_LEAN_AND_MEAN
#include <windows.h> /* for GetConsoleMode */
#include <errno.h>

#define LOCALE_RPL_IMPL
#include "mscrtx/localerpl.h"
#include "mscrtx/locale_helpers.h"
#include "libutf16/utf8_cstd.h"
#include "libutf16/utf16_to_utf8.h"
#include "libutf16/utf8_to_utf16_one.h"
#include "unicode_ctype/unicode_ctype.h"
#include "unicode_ctype/unicode_toupper.h"
#include "mscrtx/utf8env.h"
#include "mscrtx/utf16cvt.h"
#include "mscrtx/console_setup.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#endif

#ifndef _MSC_VER
#define _fread_nolock fread
#define _fwrite_nolock fwrite
#endif

/* stack bufs  */
#define LOCALE_BUF_SIZE       128
#define PATH_BUF_SIZE         260
#define FPUTS_BUF_SIZE        256
#define FPRINTF_BUF_SIZE      512
#define WRITE_BUF_SIZE        512
#define COLL_BUF_SZ           512
#define SPAWN_CMD_BUF_SIZE    260

/* static buffers */
#define UTF8_STRERROR_BUF_SIZE 1024

/* note: this size is effectively limits maximum length of strings
   which can be entered interactively in the console window */
#ifndef READ_CONSOLE_BUF_SIZE
#define READ_CONSOLE_BUF_SIZE  65536
#endif

static char *utf8_rpl_setlocale(int cat, const char *locale)
{
	if (locale != NULL)
		/* Change current locale.  */
		return set_locale_helper(cat, locale);

	/* Query current locale.  */
	locale = setlocale(cat, NULL);
	if (!locale)
		return NULL;

	/* Replace resulting code page.  */
	return locale_helper_add_utf8_cp(cat, locale);
}

int console_set_wide(int fd)
{
	int fmode;
	DWORD con_mode;
	HANDLE h;

	/* -2 is a special descriptor value if stdin, stdout, and stderr aren't associated with a stream, see
	  https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/get-osfhandle */
	if (-2 == fd)
		return -1;

	if (!_isatty(fd))
		return -1;

	fmode = _setmode(fd, _O_BINARY);
	if (_O_BINARY == fmode)
		return -1;

	if (-1 == fmode)
		goto not_console;

	h = (HANDLE)_get_osfhandle(fd);

	/* -2 is a special handle value when the file descriptor is not associated with a stream, see
	  https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/get-osfhandle */
	if ((HANDLE)-2 == h)
		goto not_console;

	if (!GetConsoleMode(h, &con_mode))
		goto not_console;

	/* Console stream is unbuffered, no need to flush it before changing mode.  */
	(void)_setmode(fd, _O_U16TEXT);
	return fmode;

not_console:
	(void)_setmode(fd, fmode);
	return -1;
}

static int utf8_rpl_open(const char *name, int flags, ...)
{
	wchar_t path_buf[PATH_BUF_SIZE], *wpath;
	va_list args;
	int mode, fd;
	va_start(args, flags);
	mode = va_arg(args, int);
	va_end(args);
	wpath = CVT_UTF8_TO_16_Z(name, path_buf);
	if (!wpath)
		return -1;
	fd = _wopen(wpath, flags, mode);
	if (wpath != path_buf)
		free(wpath);
	return fd;
}

static int utf8_file_op(const char *path, int (*op)(const wchar_t *wpath))
{
	wchar_t path_buf[PATH_BUF_SIZE];
	wchar_t *const wpath = CVT_UTF8_TO_16_Z(path, path_buf);
	int ret;
	if (!wpath)
		return -1;
	ret = (*op)(wpath);
	if (wpath != path_buf)
		free(wpath);
	return ret;
}

static int utf8_rpl_mkdir(const char *dirname)
{
	return utf8_file_op(dirname, _wmkdir);
}

static int utf8_rpl_rmdir(const char *dirname)
{
	return utf8_file_op(dirname, _wrmdir);
}

static int utf8_rpl_remove(const char *pathname)
{
	return utf8_file_op(pathname, _wremove);
}

static int utf8_rpl_unlink(const char *pathname)
{
	return utf8_file_op(pathname, _wunlink);
}

static int utf8_rpl_chdir(const char *path)
{
	return utf8_file_op(path, _wchdir);
}

static int utf8_rpl_stat(const char *path, struct _stat64 *buf)
{
	wchar_t path_buf[PATH_BUF_SIZE];
	wchar_t *const wpath = CVT_UTF8_TO_16_Z(path, path_buf);
	int ret;
	if (!wpath)
		return -1;
	ret = _wstat64(wpath, buf);
	if (wpath != path_buf)
		free(wpath);
	return ret;
}

static int utf8_rpl_chmod(const char *path, int mode)
{
	wchar_t path_buf[PATH_BUF_SIZE];
	wchar_t *const wpath = CVT_UTF8_TO_16_Z(path, path_buf);
	int ret;
	if (!wpath)
		return -1;
	ret = _wchmod(wpath, mode);
	if (wpath != path_buf)
		free(wpath);
	return ret;
}

static int utf8_rpl_rename(const char *old_name, const char *new_name)
{
	int ret;
	wchar_t buf[PATH_BUF_SIZE*2], *wp_new = buf;
	size_t wp_old_sz;
	wchar_t *const wp_old = CVT_UTF8_TO_16_Z_SZ(old_name, buf, &wp_old_sz);

	if (!wp_old)
		return -1;

	if (buf == wp_old)
		wp_new = buf + wp_old_sz;
	else
		wp_old_sz = 0;

	wp_new = cvt_utf8_to_16_z(new_name, wp_new, (size_t)(&buf[sizeof(buf)/sizeof(buf[0])] - wp_new));

	if (!wp_new) {
		if (!wp_old_sz)
			free(wp_old);
		return -1;
	}

	ret = _wrename(wp_old, wp_new);

	if (!wp_old_sz)
		free(wp_old);

	if (wp_new != buf + wp_old_sz)
		free(wp_new);

	return ret;
}

static wchar_t conv_fmode(char c)
{
	switch (c) {
		case '+': return L'+';
		case 'a': return L'a';
		case 'r': return L'r';
		case 'w': return L'w';
		case 'b': return L'b';
		case 't': return L't';
		case 'c': return L'c';
		case 'n': return L'n';
		case 'S': return L'S';
		case 'R': return L'R';
		case 'T': return L'T';
		case 'D': return L'D';
		default: return L'\0';
	}
}

static FILE *utf8_rpl_fopen(const char *path, const char *mode)
{
	/* Check open mode.  Recoding via ccs=... is not supported.  */
	wchar_t wmode[sizeof("+arwbtcnSRTD")];
	unsigned i = 0;

	for (;;) {
		wmode[i] = conv_fmode(mode[i]);
		if (wmode[i] == L'\0') {
			if (mode[i] == '\0')
				break;
			errno = EINVAL;
			return NULL;
		}
		if (++i == sizeof(wmode)/sizeof(wmode[0])) {
			errno = EINVAL;
			return NULL;
		}
	}

	{
		FILE *f;
		wchar_t path_buf[PATH_BUF_SIZE];
		wchar_t *const wpath = CVT_UTF8_TO_16_Z(path, path_buf);
		if (!wpath)
			return NULL;
		f = _wfopen(wpath, wmode);
		if (wpath != path_buf)
			free(wpath);
		return f;
	}
}

static int read_fn_read(void *buf, unsigned count/*>0*/, intptr_t data)
{
	const int fd = (int)data;
	return _read(fd, buf, count);
}

static int read_fn_fread(void *buf, unsigned count/*>0*/, intptr_t data)
{
	FILE *stream = (FILE*)data;
	size_t nmemb = count/sizeof(wchar_t);
	const size_t x = fread(buf, sizeof(wchar_t), nmemb, stream);
	if (x != nmemb && ferror(stream))
		return -1;
	return (int)(x*sizeof(wchar_t));
}

static char utf8_buf[UTF8_MAX_LEN];
static unsigned utf8_buf_offset = 0;
static unsigned utf8_buf_filled = 0;

static wchar_t console_buf[READ_CONSOLE_BUF_SIZE];
static unsigned con_buf_offset = 0;
static unsigned con_buf_filled = 0;

static size_t utf8_read_from_console(
	void *buf, size_t count/*>0*/, intptr_t data,
	int (*read_fn)(void *buf, unsigned count/*>0*/, intptr_t data))
{
	utf8_state_t st = {0};

	char *dst = (char*)buf;
	const char *const dst_end = dst + count;
	wchar_t *src = &console_buf[con_buf_offset];
	wchar_t *src_end = src + con_buf_filled;

	unsigned i;
	size_t n;

	if (utf8_buf_filled) {
		i = utf8_buf_offset;
		n = i + utf8_buf_filled;
		utf8_buf_filled = 0;
		goto read_remaining;
	}

	i = 0; /* silence compiler */
	n = 1; /* n is zero if want to read second part of utf16 surrogate pair */

	for (;;) {

		if (src == src_end) {
			const int x = (*read_fn)(console_buf, sizeof(console_buf), data);
			if (!x) {
				con_buf_offset = 0;
				con_buf_filled = 0;
				if (n)
					return (size_t)(dst - (char*)buf);
				return (size_t)-1; /* error: no second part of utf16 surrogate pair */
			}
			/* should read whole wide-chars */
			if (x < 0 || (x % sizeof(wchar_t))) {
				con_buf_offset = 0;
				con_buf_filled = 0;
				return (size_t)-1; /* read failed */
			}
			src = console_buf;
			src_end = src + x/sizeof(wchar_t);
		}

		n = utf8_c16rtomb((utf8_char_t*)utf8_buf, *src++, &st);
		if (!n)
			continue; /* read second part of utf16 surrogate pair */

		if ((size_t)-1 == n) {
			/* invalid utf16 char: skip it and return error */
			con_buf_offset = (unsigned)(src - console_buf);
			con_buf_filled = (unsigned)(src_end - src);
			return (size_t)-1;
		}

		*dst++ = utf8_buf[0];
		if (dst == dst_end) {
			i = 1;
			goto dst_buffer_is_full;
		}

		if (n > 1) {
			i = 1;
read_remaining:
			do {
				*dst++ = utf8_buf[i++];
				if (dst == dst_end)
					goto dst_buffer_is_full;
			} while (i < n);
		}
	} /* for */

dst_buffer_is_full:
	utf8_buf_offset = i;
	utf8_buf_filled = (unsigned)(n - i);
	con_buf_offset = (unsigned)(src - console_buf);
	con_buf_filled = (unsigned)(src_end - src);
	return (size_t)(dst - (char*)buf);
}

static int utf8_rpl_read_con(int fd, void *buf, unsigned count/*>0*/)
{
	size_t n;

	if (count > INT_MAX) {
		errno = E2BIG;
		return -1;
	}

	n = utf8_read_from_console(buf, count, (intptr_t)fd, read_fn_read);
	if ((size_t)-1 != n)
		return (int)n;

	if (errno == 0)
		errno = EILSEQ;
	return -1;
}

static int utf8_rpl_read(int fd, void *buf, unsigned count)
{
	int fmode;

	if (count == 0)
		return 0;

	if (-1 == (fmode = console_set_wide(fd)))
		return _read(fd, buf, count);

	{
		const int n = utf8_rpl_read_con(fd, buf, count);
		(void)_setmode(fd, fmode);
		return n;
	}
}

static size_t utf8_rpl_fread_con(void *buf, size_t size/*>0*/, size_t nmemb/*>0*/, FILE *stream)
{
	size_t n;

	/* note: (size_t)-1 is an error indicator */
	if (size > ((size_t)-1 - 1)/nmemb) {
		errno = E2BIG;
		return 0;
	}

	n = utf8_read_from_console(buf, size*nmemb, (intptr_t)stream, read_fn_fread);
	if ((size_t)-1 != n)
		return n/size;

	if (!ferror(stream))
		errno = EILSEQ;
	return 0;
}

static size_t utf8_rpl_fread(void *buf, size_t size, size_t nmemb, FILE *stream)
{
	int fmode;

	if (size == 0 || nmemb == 0)
		return 0;

	if (-1 == (fmode = console_set_wide(_fileno(stream))))
		return _fread_nolock(buf, size, nmemb, stream);

	{
		const size_t n = utf8_rpl_fread_con(buf, size, nmemb, stream);
		(void)_setmode(_fileno(stream), fmode);
		return n;
	}
}

static int utf8_rpl_fgetc_con(FILE *stream)
{
	wchar_t *src = &console_buf[con_buf_offset];
	wchar_t *src_end = src + con_buf_filled;
	wchar_t c;

	if (src == src_end) {
		const size_t nmemb = sizeof(console_buf)/sizeof(wchar_t);
		const size_t x = fread(console_buf, sizeof(wchar_t), nmemb, stream);
		if (!x) {
			con_buf_offset = 0;
			con_buf_filled = 0;
			return EOF;
		}
		src = console_buf;
		src_end = src + x;
	}

	c = *src++;

	con_buf_offset = (unsigned)(src - console_buf);
	con_buf_filled = (unsigned)(src_end - src);

	if (utf8_is_one_byte(c))
		return (unsigned char)c;

	errno = EILSEQ;
	return EOF;
}

static int utf8_rpl_fgetc(FILE *stream)
{
	int fmode;

	if (-1 == (fmode = console_set_wide(_fileno(stream))))
		return fgetc(stream);

	{
		const int n = utf8_rpl_fgetc_con(stream);
		(void)_setmode(_fileno(stream), fmode);
		return n;
	}
}

static int utf8_rpl_getchar(void)
{
	return utf8_rpl_fgetc(stdin);
}

static char *utf8_rpl_fgets_con(char *s, int size, FILE *stream)
{
	utf8_state_t st = {0};

	char *dst = s;
	const char *const dst_end = dst + (size > 0 ? size - 1 : 0);
	wchar_t *src = &console_buf[con_buf_offset];
	wchar_t *src_end = src + con_buf_filled;

	unsigned i;
	size_t n;

	if (size <= 0)
		return NULL; /* Bad parameters.  Should not happen.  */

	if (size < 2) {
		*dst = '\0';
		return NULL; /* No place for the characters.  */
	}

	if (utf8_buf_filled) {
		/* Read remaining part of utf8 character.  */
		i = utf8_buf_offset;
		n = i + utf8_buf_filled;
		utf8_buf_filled = 0;
		goto read_remaining;
	}

	i = 0; /* silence compiler */
	n = 1; /* n is zero if want to read second part of utf16 surrogate pair */

	for (;;) {

		if (src == src_end) {
			const size_t nmemb = sizeof(console_buf)/sizeof(wchar_t);
			const size_t x = fread(console_buf, sizeof(wchar_t), nmemb, stream);
			if (!x) {
				con_buf_offset = 0;
				con_buf_filled = 0;
				*dst = '\0';
				if (n) {
					if (dst != s)
						return s;
					return NULL; /* No characters have been read.  */
				}
				if (!ferror(stream))
					errno = EILSEQ; /* feof(stream) should be true, set at least errno to indicate an error */
				return NULL; /* error: no second part of utf16 surrogate pair */
			}
			src = console_buf;
			src_end = src + x;
		}

		n = utf8_c16rtomb((utf8_char_t*)utf8_buf, *src++, &st);
		if (!n)
			continue; /* read second part of utf16 surrogate pair */

		if ((size_t)-1 == n) {
			/* invalid utf16 char: skip it and return error */
			con_buf_offset = (unsigned)(src - console_buf);
			con_buf_filled = (unsigned)(src_end - src);
			*dst = '\0';
			errno = EILSEQ; /* ferror(stream) should be false, set at least errno to indicate an error */
			return NULL;
		}

		*dst++ = utf8_buf[0];
		if (dst == dst_end) {
			i = 1;
			goto dst_buffer_is_full;
		}

		if (n > 1) {
			i = 1;
read_remaining:
			do {
				*dst++ = utf8_buf[i++];
				if (dst == dst_end)
					goto dst_buffer_is_full;
			} while (i < n);
		}
		else if ('\n' == utf8_buf[0])
			break;

	} /* for */

dst_buffer_is_full:
	utf8_buf_offset = i;
	utf8_buf_filled = (unsigned)(n - i);
	con_buf_offset = (unsigned)(src - console_buf);
	con_buf_filled = (unsigned)(src_end - src);
	*dst = '\0';
	return s;
}

static char *utf8_rpl_fgets(char *s, int size, FILE *stream)
{
	int fmode;

	if (-1 == (fmode = console_set_wide(_fileno(stream))))
		return fgets(s, size, stream);

	{
		char *const ret = utf8_rpl_fgets_con(s, size, stream);
		(void)_setmode(_fileno(stream), fmode);
		return ret;
	}
}

static size_t utf8_write_to_console(int fd, const wchar_t wbuf[], const size_t sz)
{
	const wchar_t *wb = wbuf;
	const wchar_t *const we = wb + sz;
	while (wb < we) {
		/* Cannot write more than INT_MAX at once.  */
		const size_t to_write = (size_t)(we - wb);
		const unsigned n = to_write <= INT_MAX/sizeof(wchar_t) ?
			(unsigned)to_write : INT_MAX/sizeof(wchar_t);
		const int ret = _write(fd, wb, (unsigned)(n*sizeof(wchar_t)));
		if (ret != (int)(n*sizeof(wchar_t))) {
			if (ret > 0)
				wb += ret/sizeof(wchar_t);
			break;
		}
		wb += n;
	}
	return (size_t) (wb - wbuf);
}

static int utf8_rpl_write_con(int fd, const void *buf, unsigned count/*>0*/)
{
	int ret;
	wchar_t stack_buf[WRITE_BUF_SIZE], *wbuf;
	size_t sz = count;

	wbuf = CVT_UTF8_TO_16((const char*)buf, &sz, stack_buf);
	if (!wbuf)
		return -1;

	ret = utf8_write_to_console(fd, wbuf, sz) == sz ? (int)count : -1;

	if (wbuf != stack_buf)
		free(wbuf);

	return ret;
}

static int utf8_rpl_write(int fd, const void *buf, unsigned count)
{
	int fmode;

	if (count == 0)
		return 0;

	if (-1 == (fmode = console_set_wide(fd)))
		return _write(fd, buf, count);

	{
		const int n = utf8_rpl_write_con(fd, buf, count);
		(void)_setmode(fd, fmode);
		return n;
	}
}

static size_t utf8_rpl_fwrite_con(const void *buf, size_t size/*>0*/, size_t nmemb/*>0*/, FILE *stream)
{
	wchar_t stack_buf[WRITE_BUF_SIZE], *wbuf;
	size_t ret;
	size_t sz;

	if (nmemb > (size_t)-1/size) {
		errno = E2BIG;
		return 0;
	}

	sz = nmemb*size;

	wbuf = CVT_UTF8_TO_16((const char*)buf, &sz, stack_buf);
	if (!wbuf)
		return 0;

	/* Console stream is unbuffered, fwrite() will write by one byte - but console
	   expects wide characters (each one of 2 bytes), so use write(). */
	ret = utf8_write_to_console(_fileno(stream), wbuf, sz) == sz ? nmemb : 0;

	if (wbuf != stack_buf)
		free(wbuf);

	return ret;
}

static size_t utf8_rpl_fwrite(const void *buf, size_t size, size_t nmemb, FILE *stream)
{
	int fmode;

	if (size == 0 || nmemb == 0) {
		errno = 0;
		return 0;
	}

	if (-1 == (fmode = console_set_wide(_fileno(stream))))
		return _fwrite_nolock(buf, size, nmemb, stream);

	{
		const size_t n = utf8_rpl_fwrite_con(buf, size, nmemb, stream);
		(void)_setmode(_fileno(stream), fmode);
		return n;
	}
}

static int utf8_fputc_to_console(int c, FILE *stream)
{
	const utf8_char_t r = (utf8_char_t)c;
	if (!utf8_is_one_byte(r)) {
		errno = EILSEQ;
		return EOF;
	}
	if (WEOF == fputwc(utf8_one_byte_to_utf16(r), stream))
		return EOF;
	return c;
}

static int utf8_rpl_fputc(int c, FILE *stream)
{
	int fmode;

	if (-1 == (fmode = console_set_wide(_fileno(stream))))
		return fputc(c, stream);

	{
		const int n = utf8_fputc_to_console(c, stream);
		(void)_setmode(_fileno(stream), fmode);
		return n;
	}
}

static int utf8_rpl_putchar(int c)
{
	return utf8_rpl_fputc(c, stdout);
}

static int utf8_fputs_to_console(int fd, const char *s)
{
	if (*s) {
		size_t ret;
		size_t sz;
		wchar_t buf[FPUTS_BUF_SIZE];
		wchar_t *const ws = CVT_UTF8_TO_16_Z_SZ(s, buf, &sz);
		if (!ws)
			return EOF;
		ret = utf8_write_to_console(fd, ws, sz - 1);
		if (ws != buf)
			free(ws);
		if (ret != sz - 1)
			return EOF;
	}
	return 0;
}

static int utf8_rpl_fputs(const char *s, FILE *stream)
{
	int fmode;

	if (-1 == (fmode = console_set_wide(_fileno(stream))))
		return fputs(s, stream);

	{
		const int n = utf8_fputs_to_console(_fileno(stream), s);
		(void)_setmode(_fileno(stream), fmode);
		return n;
	}
}

static int utf8_puts_to_console(int fd, const char *s)
{
	size_t ret;
	wchar_t buf[FPUTS_BUF_SIZE];
	size_t sz;
	wchar_t *const ws = CVT_UTF8_TO_16_Z_SZ(s, buf, &sz);
	if (!ws)
		return EOF;
	ws[sz - 1] = L'\n'; /* overwrite L'\0' */
	ret = utf8_write_to_console(fd, ws, sz);
	if (ws != buf)
		free(ws);
	return ret == sz ? 0 : EOF;
}

static int utf8_rpl_puts(const char *s)
{
	int fmode;

	if (-1 == (fmode = console_set_wide(_fileno(stdout))))
		return puts(s);

	{
		const int n = utf8_puts_to_console(_fileno(stdout), s);
		(void)_setmode(_fileno(stdout), fmode);
		return n;
	}
}

static int utf8_rpl_mb_cur_max(void)
{
	return UTF8_MAX_LEN;
}

static wint_t utf8_rpl_btowc(int c)
{
	if (c != EOF) {
		const utf8_char_t r = (utf8_char_t)c;
		if (utf8_is_one_byte(r))
			return utf8_one_byte_to_utf16(r);
	}
	return WEOF;
}

static size_t utf8_rpl_c32rtomb(char *s, unsigned wi, mbstate_t *ps)
{
	/* will use mbstate_t object as utf8_state_t */
	(void)sizeof(int[1-2*(sizeof(mbstate_t) < sizeof(utf8_state_t))]);
	return utf8_c32rtomb((utf8_char_t*)s, wi, (utf8_state_t*)ps);
}

static int utf8_rpl_strcoll(const char *s1, const char *s2)
{
	int ret = -2;
	wchar_t buf[COLL_BUF_SZ];
	size_t ws1_sz;
	wchar_t *const ws1 = CVT_UTF8_TO_16_Z_SZ(s1, buf, &ws1_sz);
	if (ws1) {
		const size_t offs = (buf == ws1) ? ws1_sz : 0;
		wchar_t *const ws2 = cvt_utf8_to_16_z(s2, buf + offs, sizeof(buf)/sizeof(buf[0]) - offs);
		if (ws2) {
			ret = wcscoll(ws1, ws2);
			if (buf + offs != ws2)
				free(ws2);
		}
		if (!offs)
			free(ws1);
	}
	return ret;
}

static int utf8_rpl_c32scoll(const unsigned *s1, const unsigned *s2)
{
	int ret = -2;
	wchar_t buf[COLL_BUF_SZ];
	size_t ws1_sz;
	wchar_t *const ws1 = CVT_UTF32_TO_16_Z_SZ(s1, buf, &ws1_sz);
	if (ws1) {
		const size_t offs = (buf == ws1) ? ws1_sz : 0;
		wchar_t *const ws2 = cvt_utf32_to_16_z(s2, buf + offs, sizeof(buf)/sizeof(buf[0]) - offs);
		if (ws2) {
			ret = wcscoll(ws1, ws2);
			if (buf + offs != ws2)
				free(ws2);
		}
		if (!offs)
			free(ws1);
	}
	return ret;
}

static int utf8_rpl_stricmp(const char *s1, const char *s2)
{
	for (;;) {
		utf32_char_t w1, w2;
		s1 = (const char*)utf8_to_utf32_one_z(&w1, (const utf8_char_t*)s1);
		s2 = (const char*)utf8_to_utf32_one_z(&w2, (const utf8_char_t*)s2);
		if (!s1 || !s2)
			return (int)((unsigned)-1/2); /* INT_MAX */
		w1 = unicode_tolower(w1);
		w2 = unicode_tolower(w2);
		if (w1 < w2)
			return -1;
		if (w1 != w2)
			return 1;
		if (!w1)
			return 0;
	}
}

static int utf8_rpl_c32sicmp(const unsigned *s1, const unsigned *s2)
{
	for (;; s1++, s2++) {
		const unsigned w1 = unicode_tolower(*s1);
		const unsigned w2 = unicode_tolower(*s2);
		if (w1 < w2)
			return -1;
		if (w1 != w2)
			return 1;
		if (!w1)
			return 0;
	}
}

static int utf8_rpl_tolower(int c)
{
	return utf8_is_one_byte((unsigned)c) ? (int)unicode_tolower((unsigned)c) : c;
}

static int utf8_rpl_toupper(int c)
{
	return utf8_is_one_byte((unsigned)c) ? (int)unicode_toupper((unsigned)c) : c;
}

static int utf8_rpl_isascii(int c)
{
	return utf8_is_one_byte((unsigned)c);
}

static int utf8_rpl_isalnum(int c)
{
	return utf8_is_one_byte((unsigned)c) ? unicode_isalnum((unsigned)c) : 0;
}

static int utf8_rpl_isalpha(int c)
{
	return utf8_is_one_byte((unsigned)c) ? unicode_isalpha((unsigned)c) : 0;
}

static int utf8_rpl_isblank(int c)
{
	return utf8_is_one_byte((unsigned)c) ? unicode_isblank((unsigned)c) : 0;
}

static int utf8_rpl_iscntrl(int c)
{
	return utf8_is_one_byte((unsigned)c) ? unicode_iscntrl((unsigned)c) : 0;
}

static int utf8_rpl_isdigit(int c)
{
	return utf8_is_one_byte((unsigned)c) ? unicode_isdigit((unsigned)c) : 0;
}

static int utf8_rpl_isgraph(int c)
{
	return utf8_is_one_byte((unsigned)c) ? unicode_isgraph((unsigned)c) : 0;
}

static int utf8_rpl_islower(int c)
{
	return utf8_is_one_byte((unsigned)c) ? unicode_islower((unsigned)c) : 0;
}

static int utf8_rpl_isprint(int c)
{
	return utf8_is_one_byte((unsigned)c) ? unicode_isprint((unsigned)c) : 0;
}

static int utf8_rpl_ispunct(int c)
{
	return utf8_is_one_byte((unsigned)c) ? unicode_ispunct((unsigned)c) : 0;
}

static int utf8_rpl_isspace(int c)
{
	return utf8_is_one_byte((unsigned)c) ? unicode_isspace((unsigned)c) : 0;
}

static int utf8_rpl_isupper(int c)
{
	return utf8_is_one_byte((unsigned)c) ? unicode_isupper((unsigned)c) : 0;
}

static int utf8_rpl_isxdigit(int c)
{
	return utf8_is_one_byte((unsigned)c) ? unicode_isxdigit((unsigned)c) : 0;
}

static int unicode_isascii_(unsigned c)
{
	return utf8_is_one_byte(c);
}

static c32ctype_t unicode_ctype_(const char *name)
{
	return (c32ctype_t)unicode_ctype(name);
}

static int unicode_isctype_(unsigned c, c32ctype_t desc)
{
	return unicode_isctype(c, (int)desc);
}

A_Use_decl_annotations
#ifndef SAL_DEFS_H_INCLUDED
ATTRIBUTE_PRINTF(format, 4, 0)
#endif
int vsnprintf_helper(char **buf, size_t buf_size, const char format[], va_list ap)
{
	char *b = *buf; /* buffer that cannot be reallocated, likely a stack-buffer */
	for (;;) {
		if (buf_size < 2)
			buf_size = 128;
		else {
			int n = _vsnprintf(b, buf_size - 1, format, ap);
			if (-1 != n) {
				b[n] = '\0';
				*buf = b;
				return n;
			}
		}
		if (b != *buf)
			free(b);
		if (buf_size < 65536)
			buf_size *= 2;
		else if (buf_size <= INT_MAX - 65536)
			buf_size += 65536;
		else {
			errno = E2BIG;
			return -1;
		}
		b = (char*)malloc(buf_size);
		if (!b)
			return -1;
		assert(b != *buf);
	}
}

ATTRIBUTE_PRINTF(format, 2, 0)
static int utf8_vfprintf_to_console(int fd, const char *format, va_list ap)
{
	char stack_buf[FPRINTF_BUF_SIZE], *buf = stack_buf;
	const int n = vsnprintf_helper(&buf, sizeof(stack_buf), format, ap);
	if (n == 0) {
		if (buf != stack_buf)
			free(buf);
		return 0;
	}
	if (-1 != n) {
		wchar_t wstack_buf[WRITE_BUF_SIZE];
		size_t sz = (size_t)n;
		wchar_t *const wbuf = CVT_UTF8_TO_16(buf, &sz, wstack_buf);
		if (buf != stack_buf)
			free(buf);
		if (wbuf) {
			size_t ret = utf8_write_to_console(fd, wbuf, sz);
			if (wbuf != wstack_buf)
				free(wbuf);
			if (ret == sz)
				return n;
		}
	}
	return -1;
}

ATTRIBUTE_PRINTF(format, 2, 0)
static int utf8_rpl_vfprintf(FILE *stream, const char *format, va_list ap)
{
	int fmode;

	if (-1 == (fmode = console_set_wide(_fileno(stream))))
		return vfprintf(stream, format, ap);

	{
		const int n = utf8_vfprintf_to_console(_fileno(stream), format, ap);
		(void)_setmode(_fileno(stream), fmode);
		return n;
	}
}

ATTRIBUTE_PRINTF(format, 1, 0)
static int utf8_rpl_vprintf(const char *format, va_list ap)
{
	return utf8_rpl_vfprintf(stdout, format, ap);
}

ATTRIBUTE_PRINTF(format, 1, 2)
static int utf8_rpl_printf(const char *format, ...)
{
	int ret;
	va_list ap;
	va_start(ap, format);
	ret = utf8_rpl_vprintf(format, ap);
	va_end(ap);
	return ret;
}

ATTRIBUTE_PRINTF(format, 2, 3)
static int utf8_rpl_fprintf(FILE *stream, const char *format, ...)
{
	int ret;
	va_list ap;
	va_start(ap, format);
	ret = utf8_rpl_vfprintf(stream, format, ap);
	va_end(ap);
	return ret;
}

static char *utf8_rpl_strerror(int error_number)
{
	static char strerror_buf[UTF8_STRERROR_BUF_SIZE];
	wchar_t *err = _wcserror(error_number);
	if (err != NULL) {
		const utf16_char_t *w = err;
		utf8_char_t *b = (utf8_char_t*) strerror_buf;
		size_t n = utf16_to_utf8_z(&w, &b, UTF8_STRERROR_BUF_SIZE - 1);
		if (n == 0) {
#define ERR_MSG "failed to convert error message to utf8\n"
			(void) sizeof(int[1-2*(sizeof(strerror_buf) < sizeof(ERR_MSG))]);
			memcpy(strerror_buf, ERR_MSG, sizeof(ERR_MSG));
#undef ERR_MSG
		}
		else if (n >= UTF8_STRERROR_BUF_SIZE)
			*b = '\0';
		return strerror_buf;
	}
	return NULL;
}

static int utf8_rpl_mkstemp(char *templ)
{
	int fd;
	wchar_t path_buf[PATH_BUF_SIZE], *wbuf, *wtempl;
	const size_t len = strlen(templ);
	size_t sz = len + 1;
	if (len == 0) {
		errno = EINVAL;
		return -1;
	}

	wbuf = CVT_UTF8_TO_16(templ, &sz, path_buf);
	if (!wbuf)
		return -1;

	wtempl = _wmktemp(wbuf);
	if (!wtempl) {
		if (wbuf != path_buf)
			free(wbuf);
		return -1;
	}

	/* update template */
	{
		const utf16_char_t *w = wtempl;
		utf8_char_t *b = (utf8_char_t*)templ;
		const size_t x = utf16_to_utf8_z(&w, &b, len + 1);
		if (x < 2 || x > len + 1) {
			if (wbuf != path_buf)
				free(wbuf);
			errno = EINVAL;
			return -1;
		}
	}

	fd = _wopen(wtempl,
			O_RDWR | O_CREAT | O_EXCL,
			S_IREAD | S_IWRITE);

	if (wbuf != path_buf)
		free(wbuf);
	return fd;
}

static intptr_t utf8_rpl_spawnvp(int mode, const char *cmdname, const char *const *argv)
{
	wchar_t cmd_buf[SPAWN_CMD_BUF_SIZE];
	wchar_t *const wcmd = CVT_UTF8_TO_16_Z(cmdname, cmd_buf);
	size_t n;
	const char *const *a;
	wchar_t **wargv, **pwa;
	intptr_t ret = -1;

	if (!wcmd)
		return -1;

	/* Count arguments.  */
	for (a = argv; *a;)
		a++;
	n = (size_t)(a - argv);

	wargv = (wchar_t**)malloc((n + 1)*sizeof(*wargv));
	if (wargv) {

		/* Convert each argument.  */
		for (a = argv; *a; a++) {
			wchar_t *const wa = cvt_utf8_to_16_z(*a, NULL, 0);
			if (!wa)
				break;
			wargv[a - argv] = wa;
		}
		wargv[a - argv] = NULL;

		if (!*a)
			ret = _wspawnvp(mode, wcmd, (const wchar_t *const *)wargv);

		for (pwa = wargv; *pwa; pwa++)
			free(*pwa);
		free(wargv);
	}

	if (wcmd != cmd_buf)
		free(wcmd);
	return ret;
}

/* will use mbstate_t object as utf8_state_t */
typedef int check_size_of_mbstate_t[1-2*(sizeof(mbstate_t) < sizeof(utf8_state_t))];

/* and wchar_t is of 16 bits */
typedef int check_size_of_wchar_t[1-2*(65535 != (wchar_t)-1)];

extern const struct localerpl utf8_funcs;
const struct localerpl utf8_funcs = {
	utf8_rpl_open,
	utf8_rpl_fopen,
	utf8_rpl_read,
	utf8_rpl_write,
	utf8_rpl_fread,
	utf8_rpl_fwrite,
	utf8_rpl_putchar,
	utf8_rpl_fputc,
	utf8_rpl_getchar,
	utf8_rpl_fgetc,
	utf8_rpl_puts,
	utf8_rpl_fputs,
	utf8_rpl_fgets,
	utf8_rpl_mkdir,
	utf8_rpl_rmdir,
	utf8_rpl_remove,
	utf8_rpl_unlink,
	utf8_rpl_rename,
	utf8_rpl_chdir,
	utf8_rpl_stat,
	utf8_rpl_chmod,
	utf8_rpl_printf,
	utf8_rpl_fprintf,
	utf8_rpl_vprintf,
	utf8_rpl_vfprintf,
	utf8_rpl_strerror,
	utf8_rpl_mkstemp,
	utf8_environ,
	utf8_getenv,
	utf8_setenv,
	utf8_unsetenv,
	utf8_clearenv,
	utf8_rpl_setlocale,
	utf8_rpl_mb_cur_max,
	utf8_rpl_btowc,
	/* libutf16 */
	(int (*)(const char *, size_t))                                   utf8_mblen,
	(size_t (*)(const char *, size_t, mbstate_t *))                   utf8_mbrlen,
	(size_t (*)(wchar_t *, const char *, size_t, mbstate_t *))        utf8_mbrtoc16,
	(size_t (*)(unsigned int *, const char *, size_t, mbstate_t *))   utf8_mbrtoc32,
	(size_t (*)(char *, wchar_t, mbstate_t *))                        utf8_c16rtomb,
	utf8_rpl_c32rtomb,
	(size_t (*)(wchar_t *, const char *, size_t))                     utf8_mbstoc16s,
	(size_t (*)(char *, const wchar_t *, size_t))                     utf8_c16stombs,
	utf8_rpl_strcoll,
	utf8_rpl_stricmp,
	utf8_rpl_tolower,
	utf8_rpl_toupper,
	utf8_rpl_isascii,
	utf8_rpl_isalnum,
	utf8_rpl_isalpha,
	utf8_rpl_isblank,
	utf8_rpl_iscntrl,
	utf8_rpl_isdigit,
	utf8_rpl_isgraph,
	utf8_rpl_islower,
	utf8_rpl_isprint,
	utf8_rpl_ispunct,
	utf8_rpl_isspace,
	utf8_rpl_isupper,
	utf8_rpl_isxdigit,
	utf8_rpl_c32scoll,
	utf8_rpl_c32sicmp,
	/* libunicode_ctype */
	unicode_tolower,
	unicode_toupper,
	unicode_isascii_,
	unicode_isalnum,
	unicode_isalpha,
	unicode_isblank,
	unicode_iscntrl,
	unicode_isdigit,
	unicode_isgraph,
	unicode_islower,
	unicode_isprint,
	unicode_ispunct,
	unicode_isspace,
	unicode_isupper,
	unicode_isxdigit,
	unicode_ctype_,
	unicode_isctype_,
	/* libutf16 */
	(size_t (*)(unsigned *, const char *, size_t))    utf8_mbstoc32s,
	(size_t (*)(char *, const unsigned *, size_t))    utf8_c32stombs,
	(size_t (*)(unsigned *, const wchar_t *, size_t)) utf8_c16stoc32s,
	(size_t (*)(wchar_t *, const unsigned *, size_t)) utf8_c32stoc16s,
	utf8_rpl_spawnvp,
};
