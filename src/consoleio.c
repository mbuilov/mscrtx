/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* consoleio.c */

/* helpers for reading/writing to windows console, transparently
  converting characters between locale and console encodings */

#define WIN32_LEAN_AND_MEAN
#include <windows.h> /* for GetConsoleMode()/GetConsoleOutputCP() */
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <assert.h>

#include "mscrtx/consoleio.h"
#include "mscrtx/console_setup.h"
#include "libutf16/utf16_char.h"
#include "libutf16/utf8_to_utf16.h"
#include "libutf16/utf16_to_utf8.h"
#include "libutf16/utf16_to_utf8_one.h"
#include "libutf16/utf8_to_utf16_one.h"

/* should be able to handle utf-8 encoding */
#if defined MB_LEN_MAX && MB_LEN_MAX < UTF8_MAX_LEN
# undef MB_LEN_MAX
#endif
#ifndef MB_LEN_MAX
# define MB_LEN_MAX UTF8_MAX_LEN
#endif

/* not defined under MinGW.org */
#ifndef INT_MAX
#define INT_MAX ((unsigned)-1/2)
#endif

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#endif

/* from localerpl.h */
extern int localerpl_is_utf8(void);
unsigned get_locale_code_page(void);

int turn_on_console_fd(int fd)
{
	int fmode;
	DWORD con_mode;
	intptr_t h;

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

	h = _get_osfhandle(fd);

	/* -2 is a special handle value when the file descriptor is not associated with a stream, see
	  https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/get-osfhandle */
	if (-2 == h)
		goto not_console;

	if (!GetConsoleMode((HANDLE)h, &con_mode))
		goto not_console;

	/* Old mode, likely _O_TEXT */
	return fmode;

not_console:
	(void)_setmode(fd, fmode);
	return -1;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(ctype, A_In_z)
#endif
static unsigned parse_locale_cp(const char ctype[])
{
	const char *s = strchr(ctype, '.');

	if (s && '0' <= s[1] && s[1] <= '9') {
		/* try to parse code page - a 16-bit unsigned number */
		unsigned cp = 0;
		for (;;) {
			unsigned x = (unsigned)(*++s - '0');
			if (x > 65535 - cp)
				return 0;  /* Number is too big.  */
			cp += x;
			if ('\0' == s[1])
				return cp;
			if (s[1] < '0' || '9' < s[1])
				return 0;
			if (cp > 65535/10)
				return 0;  /* Number is too big.  */
			cp *= 10;
		}
	}

	return 0;
}

unsigned get_locale_code_page(void)
{
	const char *ctype;

	if (localerpl_is_utf8())
		return CP_UTF8;

	ctype = setlocale(LC_CTYPE, NULL);
	if (!ctype)
		return 0;

	return parse_locale_cp(ctype);
}

/* note: this size is effectively limits maximum length of strings
   which can be entered interactively in the console window */
#ifndef CONSOLEIO_READ_BUF_SIZE
#define CONSOLEIO_READ_BUF_SIZE 65536
#endif

/* read whole buffer at once, then copy from that buffer by small amounts */
struct console_read_buf {
	unsigned offset;      /* from where to copy */
	unsigned filled;      /* number of bytes filled in buf from the beginning */
	unsigned con_cp;      /* code page of bytes in buf */
	unsigned rem_cp;      /* code page of bytes in rem_buf */
	unsigned rem_offset;
	unsigned rem_filled;
	void *buf;
	wchar_t *wbuf;
	char rem_buf[MB_LEN_MAX];
};

#define console_rb_is_initialized(rb)    (!!(rb)->buf)

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(rb, A_Inout)
#endif
static int console_rb_init(struct console_read_buf *const rb)
{
	/* initialize */
	rb->buf = malloc(CONSOLEIO_READ_BUF_SIZE);
	return rb->buf ? 0 : -1;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(count, A_In_range(1, INT_MAX))
A_At(buf, A_Notnull)
A_At(buf, A_Pre_writable_byte_size(count))
A_At(buf, A_Post_readable_byte_size(return))
A_Success(return >= 0)
#endif
static int read_fn(const int do_read, const intptr_t data,
	void *const buf, const unsigned count/*[1..INT_MAX]*/)
{
	int r = -1;

	if (do_read)
		r = _read((int)data, buf, count);
	else {
		const size_t sz = fread(buf, 1, count, (FILE*)data);
		if (sz || feof((FILE*)data))
			r = (int)sz;
	}

	/* File descriptor is in binary mode, remove CRs.  */
	if (r > 0) {
		char *from = (char*)buf, *to = NULL, *cr = NULL;
		const char *const end = from + r;

		do {
			from = (char*)memchr(from, (unsigned char)'\r', (size_t)(end - from));
			if (cr) {
				const size_t len = (size_t)((from ? from : end) - (cr + 1));
				if (len) {
					to += len;
					memmove(to - len, cr + 1, len);
				}
			}
			if (!from)
				break;
			cr = from++;
			if (!to)
				to = cr;
		} while (from != end);

		if (to)
			r = (int)(to - (char*)buf);
	}

	return r;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dst, A_Outptr)
A_At(lim, A_Notnull)
A_At(*dst, A_Out_writes_to_ptr(lim))
A_At(offset, A_Inout)
A_At(from, A_In_reads_bytes(filled - *offset))
#endif
static void copy_from(char **const dst, const char *const lim,
	const void *const from, unsigned *const offset, const unsigned filled)
{
	unsigned to_copy = filled - *offset;
	if (to_copy > (size_t)(lim - *dst))
		to_copy = (unsigned)(lim - *dst);
	assert(to_copy);
	(*offset) += to_copy;
	(*dst) += to_copy;
	memcpy(*dst - to_copy, (const char*)from + *offset - to_copy, to_copy);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(dst, A_Outptr)
A_At(lim, A_Notnull)
A_At(*dst, A_Out_writes_to_ptr(lim))
A_At(offset, A_Inout)
A_At(from, A_In_reads_bytes(filled - *offset))
#endif
static const char *copy_from_until_nl(char **const dst, const char *const lim,
	const void *const from, unsigned *const offset, const unsigned filled)
{
	const char *const f = (const char*)from + *offset;
	const char *nl;
	unsigned to_copy = filled - *offset;
	if (to_copy > (size_t)(lim - *dst))
		to_copy = (unsigned)(lim - *dst);
	assert(to_copy);
	nl = (const char*)memchr(f, (unsigned char)'\n', to_copy);
	if (nl)
		to_copy = (unsigned)(nl - f) + 1;
	(*offset) += to_copy;
	(*dst) += to_copy;
	memcpy(*dst - to_copy, f, to_copy);
	return nl;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dst, A_Outptr)
A_At(lim, A_Notnull)
A_At(*dst, A_Out_writes_to_ptr(lim))
A_At(offset, A_Inout)
A_At(from, A_In_reads(filled - *offset))
#endif
static void copy_from_w(wchar_t **const dst, const wchar_t *const lim,
	const wchar_t *const from, unsigned *const offset, const unsigned filled)
{
	unsigned to_copy = filled - *offset;
	if (to_copy > (size_t)(lim - *dst))
		to_copy = (unsigned)(lim - *dst);
	assert(to_copy);
	(*offset) += to_copy;
	(*dst) += to_copy;
	memcpy(*dst - to_copy, from + *offset - to_copy, to_copy*sizeof(*from));
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(str, A_In_reads(count))
A_At(count, A_In_range(1, INT_MAX))
A_At(wsz, A_Out)
A_Success(return)
#endif
static wchar_t *str_to_wide(const char str[/*len*/],
	unsigned count/*[1..INT_MAX]*/, const unsigned cp, unsigned *const wsz)
{
	const char *s = str;
	wchar_t *wbuf;
	size_t sz;

	for (;;) {
		if (CP_UTF8 == cp) {
			const utf8_char_t *q = (const utf8_char_t*)s;
			sz = utf8_to_utf16_size(&q, count);
		}
		else {
			const int len = MultiByteToWideChar(cp, 0, s, (int)count, NULL, 0);
			sz = (unsigned)len;
		}

		if (sz)
			break;

		s++; /* skip invalid byte */
		if (!--count)
			return NULL;
	}

	wbuf = (wchar_t*)malloc(sizeof(wchar_t)*sz);
	if (wbuf) {
		if (CP_UTF8 == cp) {
			const utf8_char_t *q = (const utf8_char_t*)s;
			utf16_char_t *b = wbuf;
			sz = utf8_to_utf16(&q, &b, sz, count);
		}
		else {
			const int len = MultiByteToWideChar(cp, 0, s, (int)count, wbuf, (int)sz);
			sz = (unsigned)len;
		}
		*wsz = (unsigned)sz;
	}

	return wbuf;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(rb, A_Inout)
A_When(!return, A_Post_satisfies(!!rb->wbuf))
#endif
static int console_rb_to_wide(struct console_read_buf *const rb)
{
	rb->wbuf = str_to_wide(
		(const char*)rb->buf + rb->offset, rb->filled - rb->offset,
		rb->con_cp, &rb->filled);

	if (rb->wbuf)
		rb->offset = 0;

	return rb->wbuf ? 0 : -1;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(repl_char, A_Ret_writes(return))
A_Ret_range(>,0)
#endif
static unsigned get_repl(const unsigned cp, char repl_char[MAX_DEFAULTCHAR])
{
	unsigned repl_len = 0;
	if (cp) {
		CPINFO info;
		if (GetCPInfo(cp, &info)) {
			unsigned q = 0;
			for (; q < MAX_DEFAULTCHAR; q++) {
				if (!info.DefaultChar[q])
					break;
				repl_char[q] = (char)(unsigned char)info.DefaultChar[q];
			}
			repl_len = q;
		}
	}
	if (!repl_len) {
		repl_len = 1;
		repl_char[0] = '?';
	}
	return repl_len;
}

#define DO_READ    1 /* call read(), not fread() */
#define STOP_ON_NL 2 /* stop after reading '\n' */

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(rb, A_Inout)
A_At(count, A_In_range(1, A_Size_t(-2)))
A_At(buf, A_Notnull)
A_At(buf, A_Pre_writable_byte_size(count))
A_At(buf, A_Post_readable_byte_size(return))
A_Success(return != A_Size_t(-1))
#endif
static size_t read_console_internal(struct console_read_buf *const rb,
	const int flags, const intptr_t data,
	void *const buf, const size_t count/*[1..(size_t)-2]*/)
{
	unsigned console_cp = 0;
	char *dst = (char*)buf;
	const char *const lim = dst + count;
	unsigned i, f, n;

	unsigned repl_len = 0;
	char repl_char[MAX_DEFAULTCHAR];

	const unsigned locale_cp = get_locale_code_page();

	if (rb->rem_cp && rb->rem_cp != locale_cp)
		rb->rem_cp = 0; /* reset */

	if (rb->rem_cp) {
		rb->rem_cp = 0; /* reset */
		i = rb->rem_offset;
		f = rb->rem_filled;
		goto read_remaining;
	}

	do {
		if (rb->wbuf) {
			/* read char-by-char, converting wide-characters to multibyte */
			n = 1;

			if (utf16_is_surrogate(rb->wbuf[rb->offset])) {
				f = 0;

				if (!utf16_is_high_surrogate(rb->wbuf[rb->offset]))
					goto skip_invalid;

				if (rb->offset + 1 == rb->filled ||
					!utf16_is_low_surrogate(rb->wbuf[rb->offset + 1]))
				{
					/* no lower surrogate - this should not happen - assume
					  cannot read incomplete multibyte characters from console */
					goto skip_invalid;
				}
				n = 2;
			}

			if (locale_cp == CP_UTF8) {
				utf8_state_t st = {0};
				size_t r = utf16_to_utf8_one((utf8_char_t*)rb->rem_buf, rb->wbuf[rb->offset], &st);
				if (!r) {
					assert(n == 2);
					r = utf16_to_utf8_one((utf8_char_t*)rb->rem_buf, rb->wbuf[rb->offset + 1], &st);
				}
				assert(r);
				if ((size_t)-1 == r)
					r = 0;
				f = (unsigned)r;
			}
			else if (locale_cp) {
				f = (unsigned)WideCharToMultiByte(locale_cp, 0, &rb->wbuf[rb->offset],
					(int)n, rb->rem_buf, MB_LEN_MAX, NULL, NULL);
			}
			else {
				/* convert back to multibyte */
				f = (unsigned)WideCharToMultiByte(rb->con_cp, 0, &rb->wbuf[rb->offset],
					(int)n, rb->rem_buf, MB_LEN_MAX, NULL, NULL);
				/* can't convert multibyte character to ASCII */
				if (f > 1)
					f = 0;
			}

skip_invalid:
			rb->offset += n;
			if (rb->offset == rb->filled) {
				free(rb->wbuf);
				rb->wbuf = NULL;
			}

			if (!f) {
				if (!repl_len)
					repl_len = get_repl(locale_cp, repl_char);
				for (; f < repl_len; f++)
					rb->rem_buf[f] = repl_char[f];
			}

			i = 0;
read_remaining:
			for (;;) {
				const char c = rb->rem_buf[i++];
				*dst++ = c;
				if (i == f) {
					if (f == 1 && '\n' == c && (flags & STOP_ON_NL))
						return (size_t)(dst - (char*)buf);
					break;
				}
				if (dst == lim) {
					assert(locale_cp);
					rb->rem_offset = i;
					rb->rem_filled = (unsigned)f;
					rb->rem_cp = locale_cp; /* !=0 */
					return (size_t)(dst - (char*)buf);
				}
			}
			continue;
		}

		if (rb->offset == rb->filled) {
			int r;
			if (!console_cp) {
				console_cp = GetConsoleCP();
				if (!console_cp)
					return (size_t)-1;
				rb->con_cp = console_cp;
			}

			r = read_fn(flags & DO_READ, data, rb->buf, CONSOLEIO_READ_BUF_SIZE);
			if (r < 0)
				return (size_t)-1;
			if (r == 0)
				break;

			rb->offset = 0;
			rb->filled = (unsigned)r;
		}

		if (rb->con_cp != locale_cp) {
			CPINFO info;
			/* If locale_cp is 0, avoid double mb->wide->mb conversion for the
			  single-byte console codepage.  */
			if (locale_cp || (GetCPInfo(rb->con_cp, &info) && info.MaxCharSize != 1)) {
				if (console_rb_to_wide(rb))
					rb->offset = rb->filled; /* reset: skip what failed to read */
				continue;
			}
		}

		if (!(flags & STOP_ON_NL))
			copy_from(&dst, lim, rb->buf, &rb->offset, rb->filled);
		else if (copy_from_until_nl(&dst, lim, rb->buf, &rb->offset, rb->filled))
			break;
	} while (dst != lim);

	return (size_t)(dst - (char*)buf);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(rb, A_Inout)
A_At(count, A_In_range(1, A_Size_t(-2)))
A_At(buf, A_Notnull)
A_At(buf, A_Pre_writable_size(count))
A_At(buf, A_Post_readable_size(return))
A_Success(return != A_Size_t(-1))
#endif
static size_t read_console_w_internal(struct console_read_buf *const rb,
	const int do_read, const intptr_t data,
	wchar_t buf[], const size_t count/*[1..(size_t)-2]*/)
{
	unsigned console_cp = 0;
	wchar_t *dst = buf;
	const wchar_t *const lim = dst + count;

	rb->rem_cp = 0; /* reset */

	do {
		if (!rb->wbuf) {
			if (rb->offset == rb->filled) {
				int r;
				if (!console_cp) {
					console_cp = GetConsoleCP();
					if (!console_cp)
						return (size_t)-1;
					rb->con_cp = console_cp;
				}

				r = read_fn(do_read, data, rb->buf, CONSOLEIO_READ_BUF_SIZE);
				if (r < 0)
					return (size_t)-1;
				if (r == 0)
					break;

				rb->offset = 0;
				rb->filled = (unsigned)r;
			}

			if (console_rb_to_wide(rb)) {
				rb->offset = rb->filled; /* reset: skip what failed to read */
				continue;
			}
		}

		copy_from_w(&dst, lim, rb->wbuf, &rb->offset, rb->filled);

		if (rb->offset == rb->filled) {
			free(rb->wbuf);
			rb->wbuf = NULL;
		}
	} while (dst != lim);

	return (size_t)(dst - buf);
}

static struct console_read_buf con_rbuf = {
#ifndef __cplusplus
	0
#endif
};

A_Use_decl_annotations
int read_console(int fd, void *buf, unsigned count/*[1..INT_MAX]*/)
{
	size_t r;
	if (!console_rb_is_initialized(&con_rbuf) && console_rb_init(&con_rbuf))
		return -1;
	r = read_console_internal(&con_rbuf, /*flags:*/DO_READ, fd, buf, count);
	return (r == (size_t)-1) ? -1 : (int)r;
}

A_Use_decl_annotations
int read_console_w(int fd, wchar_t buf[], unsigned count/*[1..INT_MAX]*/)
{
	size_t r;
	if (!console_rb_is_initialized(&con_rbuf) && console_rb_init(&con_rbuf))
		return -1;
	r = read_console_w_internal(&con_rbuf, /*do_read:*/1, fd, buf, count);
	return (r == (size_t)-1) ? -1 : (int)r;
}

A_Use_decl_annotations
size_t fread_console_(void *buf, size_t count/*[1..(size_t)-2]*/, FILE *stream, int until_nl)
{
	if (!console_rb_is_initialized(&con_rbuf) && console_rb_init(&con_rbuf))
		return (size_t)-1;
	return read_console_internal(&con_rbuf,
		/*flags:*/until_nl ? STOP_ON_NL : 0, (intptr_t)stream, buf, count);
}

A_Use_decl_annotations
size_t fread_console_w(wchar_t buf[], size_t count/*[1..(size_t)-2]*/, FILE *stream)
{
	if (!console_rb_is_initialized(&con_rbuf) && console_rb_init(&con_rbuf))
		return (size_t)-1;
	return read_console_w_internal(&con_rbuf, /*do_read:*/0, (intptr_t)stream, buf, count);
}

/* two stack-buffers are used for converting wide-characters to console code page:
  1) buffer of wide characters of CONSOLEIO_WRITE_BUF_SIZE size,
  2) buffer of multibyte characters of CONSOLEIO_WRITE_BUF_SIZE*MB_LEN_MAX size.  */
#ifndef CONSOLEIO_WRITE_BUF_SIZE
#define CONSOLEIO_WRITE_BUF_SIZE 128
#endif

/* build a unicode character, then convert it to console code page */
struct console_write_buf {
	unsigned rem_cp;      /* code page of rem buffer */
	unsigned filled;
	wchar_t high_surrogate;
	char rem[MB_LEN_MAX];
};

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(buf, A_In_reads_bytes(count))
A_At(count, A_In_range(>,0))
A_Success(!return)
#endif
static int write_fn(const int do_write, const intptr_t data,
	const void *const buf, const size_t count/*>0*/)
{
	assert(count);

	if (do_write) {
		/* Cannot write more than INT_MAX at once.  */
		const char *b = (const char*)buf;
		const char *const e = b + count;
		for (;;) {
			const size_t to_write = (size_t)(e - b);
			const unsigned n = (to_write <= INT_MAX) ? (unsigned)to_write : INT_MAX;
			assert(n <= count);
			{
				const int r = _write((int)data, b, n);
				if (r != (int)n)
					return -1;
			}
			b += n;
			if (b == e)
				return 0;
		}
	}

	{
		const size_t sz = fwrite(buf, 1, count, (FILE*)data);
		return (sz == count) ? 0 : -1;
	}
}

#define DO_WRITE  1 /* call write(), not fwrite() */
#define APPEND_NL 2 /* additionally write '\n' at end */

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(wbuf, A_In_reads(wbuf_filled))
A_At(wbuf_filled, A_In_range(1, CONSOLEIO_WRITE_BUF_SIZE))
A_Success(!return)
#endif
static int flush_wbuf(const unsigned console_cp,
	const int flags, const intptr_t data,
	const wchar_t wbuf[], const unsigned wbuf_filled/*>0*/)
{
	size_t count;
	char mb_buf[CONSOLEIO_WRITE_BUF_SIZE*MB_LEN_MAX + 1/*'\n'*/];
	if (console_cp == CP_UTF8) {
		const utf16_char_t *w = wbuf;
		utf8_char_t *b = (utf8_char_t*)mb_buf;
		count = utf16_to_utf8(&w, &b, sizeof(mb_buf) - 1/*'\n'*/, wbuf_filled);
	}
	else {
		const int len = WideCharToMultiByte(console_cp, 0, wbuf, (int)wbuf_filled,
			mb_buf, sizeof(mb_buf) - 1/*'\n'*/, NULL, NULL);
		count = (unsigned)len;
	}
	/* note: count should be < sizeof(mb_buf) */
	if (!count || count >= sizeof(mb_buf)) {
		/* ignore encoding error */
		count = get_repl(console_cp, mb_buf);
	}
	if (flags & APPEND_NL)
		mb_buf[count++] = '\n';
	return write_fn(flags & DO_WRITE, data, mb_buf, count);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(console_cp, A_Inout)
A_At(wbuf, A_Pre_writable_size(CONSOLEIO_WRITE_BUF_SIZE))
A_At(wbuf_filled, A_Inout)
A_At(*wbuf_filled,
	A_In_range(<=, CONSOLEIO_WRITE_BUF_SIZE - 2)
	A_Out_range(<=, CONSOLEIO_WRITE_BUF_SIZE - 2))
A_Success(!return)
#endif
static int put_uc(unsigned *const console_cp,
	const int flags, const intptr_t data,
	wchar_t wbuf[], unsigned *const wbuf_filled,
	const utf32_char_t uc)
{
	unsigned filled = *wbuf_filled;
	assert(filled <= CONSOLEIO_WRITE_BUF_SIZE - 2);
	if (uc <= 0xFFFF) {
#ifndef NDEBUG
		assert(!utf16_is_surrogate(uc));
#endif
		wbuf[filled++] = (wchar_t)(uc & 0xFFFF);
	}
	else {
		wbuf[filled++] = (wchar_t)utf32_get_high_surrogate(uc);
		wbuf[filled++] = (wchar_t)utf32_get_low_surrogate(uc);
	}
	if (filled <= CONSOLEIO_WRITE_BUF_SIZE - 2) {
		*wbuf_filled = filled;
		return 0;
	}
	if (!*console_cp) {
		*console_cp = GetConsoleOutputCP();
		if (!*console_cp)
			return -1;
	}
	*wbuf_filled = 0;
	return flush_wbuf(*console_cp, flags, data, wbuf, filled);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(wb, A_Inout)
A_At(count, A_In_range(>,0))
A_At(buf, A_In_reads_bytes(count))
A_Success(!return)
#endif
static int write_console_internal(struct console_write_buf *const wb,
	const int flags, const intptr_t data,
	const void *const buf, const size_t count/*>0*/)
{
	static const mbstate_t zero_st = {
#ifndef __cplusplus
		0
#endif
	};
	const char *s = (const char*)buf;
	const char *const lim = s + count;

	unsigned console_cp = 0;
	unsigned wbuf_filled = 0;
	wchar_t wbuf[CONSOLEIO_WRITE_BUF_SIZE];

	const unsigned locale_cp = get_locale_code_page();

	if (wb->high_surrogate)
		wb->high_surrogate = 0; /* reset */

	if (wb->rem_cp && wb->rem_cp != locale_cp)
		wb->rem_cp = 0; /* reset */

	if (wb->rem_cp) {
		for (;;) {
			/* try to complete unicode character */
			unsigned offset = 0;
			wb->rem[wb->filled++] = *s++;

			for (;;) {
				size_t sz;
				utf32_char_t uc;

				if (wb->rem_cp == CP_UTF8) {
					utf8_state_t st = {0};
					sz = utf8_to_utf32_one(&uc,
						(const utf8_char_t*)&wb->rem[offset], wb->filled - offset, &st);
				}
				else {
					mbstate_t st = zero_st;
					wchar_t wc;
					sz = mbrtowc(&wc, &wb->rem[offset], wb->filled - offset, &st);
					uc = wc;
				}

				if ((size_t)-2 == sz) {
					if (offset) {
						wb->filled -= offset;
						memmove(wb->rem, &wb->rem[offset], wb->filled);
					}
					else if (wb->filled == sizeof(wb->rem))
						break; /* should not happen, reset: skip invalid bytes */
					goto get_more;
				}

				if ((size_t)-1 == sz) {
					/* try to find leading byte: skip invalid bytes */
					if (++offset == wb->filled)
						break;
				}
				else {
					offset += sz ? (unsigned)sz : 1;
					if (put_uc(&console_cp, flags & ~APPEND_NL, data, wbuf, &wbuf_filled, uc)) {
						wb->rem_cp = 0; /* reset */
						return -1;
					}
					if (offset >= wb->filled)
						break;
				}
			} /* for */

			wb->rem_cp = 0; /* reset */
			if (s == lim)
				goto flush_return;
			break;
get_more:
			if (s == lim)
				goto flush_return;
		} /* for */
	}

	if (!console_cp) {
		console_cp = GetConsoleOutputCP();
		if (!console_cp)
			return -1;
	}

	if (locale_cp && console_cp != locale_cp) {
		/* convert to wide characters */

		if (locale_cp == CP_UTF8) {
			for (;;) {
				utf16_char_t *b = &wbuf[wbuf_filled];
				const utf8_char_t *q = (const utf8_char_t*)s;
				const size_t sz = utf8_to_utf16_partial(&q, &b,
					sizeof(wbuf)/sizeof(wbuf[0]) - wbuf_filled, (size_t)(lim - s));
				if (!sz && q == (const utf8_char_t*)s)
					break;
				/* even if s contains invalid utf8 characters (sz == 0), some of valid
				  utf8 characters have been successfully converted - account that */
				s = (const char*)q;
				wbuf_filled = (unsigned)(b - wbuf);
				/* need to flush wbuf if there is no space for 2 wchar_t's */
				if (!sz && wbuf_filled <= sizeof(wbuf)/sizeof(wbuf[0]) - 2)
					break;
				/* if wbuf is too small - flush it;
				   if utf8 string is too short - flush wbuf and return.  */
				if (flush_wbuf(console_cp,
					s != lim ? flags & ~APPEND_NL : flags,
					data, wbuf, wbuf_filled))
				{
					return -1;
				}
				if (s == lim)
					return 0;
				wbuf_filled = 0;
				if (!sz)
					break;
			}
			/* utf8 string contains invalid/incomplete characters,
			  fall back to char-by-char processing */
		}

		do {
			size_t sz;
			utf32_char_t uc;

			if (locale_cp == CP_UTF8) {
				utf8_state_t st = {0};
				sz = utf8_to_utf32_one(&uc,
					(const utf8_char_t*)s, (size_t)(lim - s), &st);
			}
			else {
				mbstate_t st = zero_st;
				wchar_t wc;
				sz = mbrtowc(&wc, s, (size_t)(lim - s), &st);
				uc = wc;
			}

			if ((size_t)-2 == sz) {
				wb->filled = (unsigned)(lim - s);
				s = lim;
				if (wb->filled >= sizeof(wb->rem))
					break; /* should not happen, reset: skip invalid bytes */
				wb->rem_cp = locale_cp;
				memcpy(wb->rem, lim - wb->filled, wb->filled);
				break;
			}

			if ((size_t)-1 == sz)
				s++; /* skip invalid byte */
			else {
				s += sz ? sz : 1;
				if (put_uc(&console_cp, flags & ~APPEND_NL, data, wbuf, &wbuf_filled, uc))
					return -1;
			}
		} while (s < lim);
	}

flush_return:
	if (wbuf_filled)
		return flush_wbuf(console_cp, flags, data, wbuf, wbuf_filled);

	if (s < lim && write_fn(flags & ~APPEND_NL, data, s, (size_t)(lim - s)))
		return -1;

	return (flags & APPEND_NL) ? write_fn(flags & ~APPEND_NL, data, "\n", 1) : 0;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(wb, A_Inout)
A_At(count, A_In_range(>,0))
A_At(buf, A_In_reads(count))
A_Success(!return)
#endif
static int write_console_w_internal(struct console_write_buf *const wb,
	const int flags, const intptr_t data,
	const wchar_t *buf, const size_t count/*>0*/)
{
	const wchar_t *const lim = buf + count;
	const unsigned console_cp = GetConsoleOutputCP();
	if (!console_cp)
		return -1;

	if (wb->rem_cp)
		wb->rem_cp = 0; /* reset */

	do {
		const wchar_t *b;
		wchar_t pair[2];
		unsigned to_write;

		if (wb->high_surrogate) {
			pair[0] = wb->high_surrogate;
			wb->high_surrogate = 0; /* reset */
			pair[1] = *buf++;
			b = pair;
			to_write = 2;
		}
		else {
			to_write = CONSOLEIO_WRITE_BUF_SIZE;
			if (to_write > (size_t)(lim - buf))
				to_write = (unsigned)(lim - buf);

			assert(to_write <= count);

			/* avoid writing partial surrogate pairs */
			if (utf16_is_high_surrogate(buf[to_write - 1])) {
				wb->high_surrogate = buf[--to_write];
				if (!to_write)
					return 0;
			}

			b = buf;
			buf = b + to_write;
		}

		if (flush_wbuf(console_cp, flags, data, b, to_write))
			return -1;
	} while (buf != lim);

	return 0;
}

static struct console_write_buf con_wbuf = {
#ifndef __cplusplus
	0
#endif
};

A_Use_decl_annotations
int write_console(int fd, const void *buf, unsigned count/*[1..INT_MAX]*/)
{
	return write_console_internal(&con_wbuf, /*flags:*/DO_WRITE, fd, buf, count);
}

A_Use_decl_annotations
int write_console_w(int fd, const wchar_t buf[], unsigned count/*[1..INT_MAX]*/)
{
	return write_console_w_internal(&con_wbuf, /*flags:*/DO_WRITE, fd, buf, count);
}

A_Use_decl_annotations
int fwrite_console_(const void *buf, size_t count/*>0*/, FILE *stream, int append_nl)
{
	return write_console_internal(&con_wbuf,
		/*flags:*/append_nl ? APPEND_NL : 0, (intptr_t)stream, buf, count);
}

A_Use_decl_annotations
int fwrite_console_w(const wchar_t buf[], size_t count/*>0*/, FILE *stream)
{
	return write_console_w_internal(&con_wbuf, /*flags:*/0, (intptr_t)stream, buf, count);
}

void console_buffers_reset(void)
{
	if (console_rb_is_initialized(&con_rbuf)) {
		free(con_rbuf.buf);
		if (con_rbuf.wbuf)
			free(con_rbuf.wbuf);
		memset(&con_rbuf, '\0', sizeof(con_rbuf));
	}
	memset(&con_wbuf, '\0', sizeof(con_wbuf));
}
