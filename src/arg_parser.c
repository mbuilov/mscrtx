/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* arg_parser.c */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>

#include "mscrtx/arg_parser.h"
#include "mscrtx/localerpl.h"

/* not defined under MinGW.org */
#ifndef INT_MAX
#define INT_MAX ((unsigned)-1/2)
#endif

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#endif

/* Size of buffer allocated on stack.  */
#define ARG_BUF_SIZE 1024

#ifndef OFFSETOF
# if defined _MSC_VER && !defined __cplusplus
#  define OFFSETOF(type, member) \
     ((size_t)((const char*)&((type*)NULL)->member - (const char*)NULL))
# else
#  define OFFSETOF(type, member) offsetof(type, member)
# endif
#endif

A_Use_decl_annotations
wchar_t *arg_get_module_name(wchar_t buf[], unsigned sz)
{
	wchar_t *b = buf;
	for (;;) {
		if (sz < 2)
			sz = 128;
		else {
			const unsigned len = GetModuleFileNameW(NULL, b, sz);
			if (len == 0) {
				if (b != buf)
					free(b);
				return NULL; /* should not happen */
			}
			if (len < sz)
				return b;
		}
		if (b != buf)
			free(b);
		if (sz < 65536)
			sz *= 2;
		else if (sz <= (DWORD)-1/sizeof(wchar_t) - 65536)
			sz += 65536;
		else {
			errno = E2BIG;
			return NULL;
		}
		b = (wchar_t*)malloc(sz*sizeof(*b));
		if (b == NULL)
			return NULL;
	}
}

static wchar_t *copy_to(wchar_t d[], wchar_t *const e,
	const wchar_t *const b, const wchar_t *const t)
{
	const size_t copy = (size_t)(t - b);
	if (!copy)
		return d;
	if (copy > (size_t)(e - d))
		return e;
	return (wchar_t*)memcpy(d, b, copy*sizeof(*b)) + copy;
}

static size_t parse_module_name(const wchar_t **const line, wchar_t dst[], const size_t sz)
{
	/* assume there are no escaped double-quotes */
	const wchar_t *w = *line;
	const wchar_t *b = w;
	wchar_t *d = dst;
	wchar_t *const e = d + sz;
	size_t skipped = 0;
	for (;;) {
		switch (*w) {
			default:
				w++;
				continue;
			case L'\0':case L' ':case L'\t':case L'"':
				d = copy_to(d, e, b, w);
				if (L'"' != *w)
					break;
				skipped++; /* skip double-quote */
				/* don't stop on spaces and tabs */
				for (b = ++w;;) {
					switch (*w) {
						default:
							w++;
							continue;
						case L'\0':case L'"':
							break;
					}
					break;
				}
				d = copy_to(d, e, b, w);
				if (L'\0' == *w)
					break;
				skipped++; /* skip double-quote */
				b = ++w;
				continue;
		}
		break;
	}
	{
		const size_t name_sz = (size_t)(w - *line) - skipped;
		*line = w;
		return name_sz;
	}
}

static size_t parse_one_arg(const wchar_t **const line, wchar_t dst[], const size_t sz)
{
	/* double-quotes can be escaped - backslashes before double-quotes are halved:
	  "     ->     (quote-mode)
	  \"    ->  "
	  \\"   ->  \  (quote-mode)
	  \\\"  ->  \"
	  \\\\" ->  \\ (quote-mode) */
	/* note: handle a special case of escaping double-quote inside double-quotes:
	  "abc""def"  ->  abc"def   */
	const wchar_t *w = *line;
	const wchar_t *b = w, *t;
	wchar_t *d = dst;
	wchar_t *const e = d + sz;
	size_t skipped = 0;
	for (;;) {
		switch (*w) {
			default:
				w++;
				continue;
			case L'\\': {
				const wchar_t *const bs = w;
				while (L'\\' == *++w);
				if (L'"' != *w)
					continue; /* backslashes are special only before a double-quote */
				t = w - ((size_t)(w - bs) + 1)/2;
				skipped += (size_t)(w - t);
				if (1u & (size_t)(w - bs)) {
					d = copy_to(d, e, b, t);
					b = w++;
					continue; /* double-quote is escaped */
				}
				goto dquote;
			}
			case L'\0':case L' ':case L'\t':case L'"':
				t = w;
dquote:
				d = copy_to(d, e, b, t);
				if (L'"' != *w)
					break;
				skipped++; /* skip double-quote */
				/* don't stop on spaces and tabs */
				for (b = ++w;; w++) {
					for (;;) {
						switch (*w) {
							default:
								w++;
								continue;
							case L'\\': {
								const wchar_t *const bs = w;
								while (L'\\' == *++w);
								if (L'"' != *w)
									continue; /* backslashes are special only before a double-quote */
								t = w - ((size_t)(w - bs) + 1)/2;
								skipped += (size_t)(w - t);
								if (1u & (size_t)(w - bs)) {
									d = copy_to(d, e, b, t);
									b = w++;
									continue; /* double-quote is escaped */
								}
								break;
							}
							case L'\0':case L'"':
								t = w;
								break;
						}
						break;
					}
					d = copy_to(d, e, b, t);
					if (L'\0' == *w)
						goto ret;
					skipped++; /* skip double-quote */
					b = ++w;
					if (L'"' != *w)
						break;
					/* special case: escaped double-quote inside double quotes:
					  "abc""def"  ->  abc"def   */
				}
				continue;
		}
		break;
	}
ret:
	{
		const size_t arg_sz = (size_t)(w - *line) - skipped;
		*line = w;
		return arg_sz;
	}
}

static struct wide_arg *create_wide_arg(const size_t value_len)
{
	const size_t offs = OFFSETOF(struct wide_arg, value);
	if (value_len < ((size_t)-1 - offs)/sizeof(wchar_t))
		return (struct wide_arg*)malloc(offs + (value_len + 1)*sizeof(wchar_t));
	errno = E2BIG;
	return NULL;
}

A_Use_decl_annotations
void arg_free_wide_args(struct wide_arg *list)
{
	while (list) {
		struct wide_arg *const next = list->next;
		free(list);
		list = next;
	}
}

A_Use_decl_annotations
struct wide_arg *arg_parse_command_line(int *const argc/*out*/)
{
	unsigned n = 0;
	wchar_t pathbuf[MAX_PATH];
	const wchar_t *cmdline = GetCommandLineW();
	wchar_t *modname = NULL;
	const wchar_t *line;
	struct wide_arg *head;
	struct wide_arg **tail = &head;

	/* Should not happen, but at least we will have a program name as the first arg.  */
	if (!cmdline || !*cmdline) {
		modname = arg_get_module_name(pathbuf, sizeof(pathbuf)/sizeof(pathbuf[0]));
		if (!modname || !*modname) {
			errno = EINVAL;
			return NULL;
		}
		cmdline = modname;
	}

	for (line = cmdline; n < INT_MAX; n++) {
		wchar_t argbuf[ARG_BUF_SIZE];
		size_t sz = sizeof(argbuf)/sizeof(argbuf[0]) - 1;
		const wchar_t *const tmp = line;
		struct wide_arg *wa = NULL;
		wchar_t *a;

		if (n) {
			/* Skip spaces between args.  */
			for (;; line++) {
				switch (*line) {
					case L' ':case L'\t':
						continue;
					default:
						break;
				}
				break;
			}
			if (!*line)
				break;
		}

		sz = !n
			? parse_module_name(&line, argbuf, sz)
			: parse_one_arg(&line, argbuf, sz);

		if (sz < sizeof(argbuf)/sizeof(argbuf[0]))
			a = argbuf;
		else {
			wa = create_wide_arg(sz);
			if (!wa)
				goto err;
			a = wa->value;
			line = tmp;
			sz = !n
				? parse_module_name(&line, a, sz)
				: parse_one_arg(&line, a, sz);
		}

		a[sz] = L'\0';

		/* Expand wildcards '*' or '?' in the arg.  */
		if (n && (wcschr(a, L'*') || wcschr(a, L'?'))) {
			WIN32_FIND_DATAW ffd;
			const HANDLE h = FindFirstFileW(a, &ffd);
			if (INVALID_HANDLE_VALUE != h) {
				do {
					/* Skip . and .. directory entries */
					if (L'.' != ffd.cFileName[0] ||
						(L'\0' != ffd.cFileName[1] && (
							L'.' != ffd.cFileName[1] || L'\0' != ffd.cFileName[2])))
					{
						const size_t len = wcslen(ffd.cFileName);
						struct wide_arg *const file = create_wide_arg(len);
						if (!file) {
							FindClose(h);
							if (wa) {
								*tail = wa;
								tail = &wa->next;
							}
							goto err;
						}
						memcpy(file->value, ffd.cFileName, (len + 1)*sizeof(wchar_t));
						*tail = file;
						tail = &file->next;
					}
				} while (FindNextFileW(h, &ffd));
				FindClose(h);
				if (wa)
					free(wa);
				continue;
			}
		}

		if (!wa) {
			wa = create_wide_arg(sz);
			if (!wa)
				goto err;
			memcpy(wa->value, a, (sz + 1)*sizeof(wchar_t));
		}
		*tail = wa;
		tail = &wa->next;
	} /* for */

	if (modname && modname != pathbuf)
		free(modname);
	*tail = NULL;
	*argc = (int)n; /* >0 */
	return head;

err:
	if (modname && modname != pathbuf)
		free(modname);
	*tail = NULL;
	arg_free_wide_args(head);
	return NULL;
}

/* Free arguments array allocated by arg_convert_wide_args().  */
A_Use_decl_annotations
void arg_free_argv(char **const argv)
{
	char **a = argv;
	for (; *a; a++)
		free(*a);
	free(argv);
}

/* Free arguments array allocated by arg_convert_mb_args().  */
A_Use_decl_annotations
void arg_free_wargv(wchar_t **const wargv)
{
	void *const arr = wargv;
	arg_free_argv((char**)arr);
}

#ifndef PRAGMA_WARNING_PUSH
#ifdef _MSC_VER
# define PRAGMA_WARNING_PUSH __pragma(warning(push))
#elif defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
# define PRAGMA_WARNING_PUSH _Pragma ("GCC diagnostic push")
#elif defined __clang__
# define PRAGMA_WARNING_PUSH _Pragma ("clang diagnostic push")
#else
# define PRAGMA_WARNING_PUSH
#endif
#endif

#ifndef PRAGMA_WARNING_POP
#ifdef _MSC_VER
# define PRAGMA_WARNING_POP __pragma(warning(pop))
#elif defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
# define PRAGMA_WARNING_POP _Pragma ("GCC diagnostic pop")
#elif defined __clang__
# define PRAGMA_WARNING_POP _Pragma ("clang diagnostic pop")
#else
# define PRAGMA_WARNING_POP
#endif
#endif

#ifndef PRAGMA_WARNING_DISABLE_COND_IS_CONST
#ifdef __clang__
# define PRAGMA_WARNING_DISABLE_COND_IS_CONST \
	_Pragma ("clang diagnostic ignored \"-Wtautological-constant-out-of-range-compare\"") \
	_Pragma ("clang diagnostic ignored \"-Wtautological-type-limit-compare\"")
#elif defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
# define PRAGMA_WARNING_DISABLE_COND_IS_CONST \
	_Pragma ("GCC diagnostic ignored \"-Wtype-limits\"")
#elif defined _MSC_VER
# define PRAGMA_WARNING_DISABLE_COND_IS_CONST __pragma(warning(disable:4127))
#else
# define PRAGMA_WARNING_DISABLE_COND_IS_CONST
#endif
#endif

A_Use_decl_annotations
char **arg_convert_wide_args(const unsigned argc, const struct wide_arg *list,
	struct arg_convert_err *const err/*NULL?,out*/)
{
	/* Note: argv array must be NULL-terminated.  */
PRAGMA_WARNING_PUSH
PRAGMA_WARNING_DISABLE_COND_IS_CONST
	if (argc < (size_t)-1/sizeof(char*)) {
PRAGMA_WARNING_POP
		char **const argv = (char**)malloc(sizeof(*argv)*(argc + 1));
		if (argv) {
			char **p = argv;
			for (; list; list = list->next, p++) {
				const size_t need = wcstombs(NULL, list->value, 0);
				if ((size_t)-1 != need) {
					char *const a = (char*)malloc(need + 1);
					if (a) {
						*p = a;
						(void)!wcstombs(a, list->value, need + 1);
						continue;
					}
					if (err) {
						err->number = (unsigned)-1;
						err->arg = NULL;
					}
				}
				else if (err) {
					err->number = (unsigned)(p - argv);
					err->arg = list->value;
				}
				*p = NULL;
				arg_free_argv(argv);
				return NULL;
			}
			*p = NULL;
			return argv;
		}
	}
	else
		errno = E2BIG;
	if (err) {
		err->number = (unsigned)-1;
		err->arg = NULL;
	}
	return NULL;
}

static size_t get_argc(char *const argv[])
{
	char *const *a = argv;
	for (; *a; a++);
	return (size_t)(a - argv);
}

A_Use_decl_annotations
wchar_t **arg_convert_mb_args(char *const argv[]/*!=NULL*/,
	struct arg_convert_mb_err *const err/*NULL?,out*/)
{
	/* Note: argv array must be NULL-terminated.  */
	const size_t argc = get_argc(argv);
PRAGMA_WARNING_PUSH
PRAGMA_WARNING_DISABLE_COND_IS_CONST
	if (argc <= (unsigned)-1 && (unsigned)argc < (size_t)-1/sizeof(wchar_t*)) {
PRAGMA_WARNING_POP
		wchar_t **const wargv = (wchar_t**)malloc(sizeof(*wargv)*(argc + 1));
		if (wargv) {
			wchar_t **p = wargv;
			char *const *a = argv;
			for (; *a; a++, p++) {
				const size_t need = mbstowcs(NULL, *a, 0);
				if ((size_t)-1 != need) {
					wchar_t *const w = (wchar_t*)malloc(sizeof(wchar_t)*(need + 1));
					if (w) {
						*p = w;
						(void)!mbstowcs(w, *a, need + 1);
						continue;
					}
					if (err) {
						err->number = (unsigned)-1;
						err->arg = NULL;
					}
				}
				else if (err) {
					err->number = (unsigned)(a - argv);
					err->arg = *a;
				}
				*p = NULL;
				arg_free_wargv(wargv);
				return NULL;
			}
			*p = NULL;
			return wargv;
		}
	}
	else
		errno = E2BIG;
	if (err) {
		err->number = (unsigned)-1;
		err->arg = NULL;
	}
	return NULL;
}
