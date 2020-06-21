/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* utf8env.c */

#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

#include "libutf16/utf16_to_utf8.h"
#include "unicode_ctype/unicode_toupper.h"
#include "mscrtx/utf16cvt.h"
#include "mscrtx/utf8env.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#endif

#ifndef OFFSETOF
# if defined _MSC_VER && !defined __cplusplus
#  define OFFSETOF(type, member) \
     ((size_t)((const char*)&((type*)NULL)->member - (const char*)NULL))
# else
#  define OFFSETOF(type, member) offsetof(type, member)
# endif
#endif

#define UTF8_ENV_TAB_SIZE     32
#define UTF8_ENV_REALLOC_BY   16
#define ENV_NAME_BUF_SIZE     128

struct utf8_env_entry {
	struct utf8_env_entry *next;
	size_t u8name_len;
	size_t name_len;
	wchar_t name[1];              /* uppercase */
	/* ... */
	/*char u8_name_eq_value[1]*/  /* string in form 'name=value' */
};

static struct utf8_env_entry *utf8_env_tab[UTF8_ENV_TAB_SIZE];

/* NULL-terminated array of pointers to 'name=value' strings */
static char **utf8_env = NULL;

/* number of set pointers in utf8_env array, not counting trailing NULL */
static size_t utf8_env_filled;

/* total number of pointers in utf8_env array, not counting trailing NULL */
static size_t utf8_env_size;

void utf8_env_shadow_reset(void)
{
	if (utf8_env) {
		unsigned i = 0;
		for (; i < sizeof(utf8_env_tab)/sizeof(utf8_env_tab[0]); i++) {
			struct utf8_env_entry *e = utf8_env_tab[i];
			while (e) {
				struct utf8_env_entry *n = e->next;
				free(e);
				e = n;
			}
		}
		for (i = 0; i < sizeof(utf8_env_tab)/sizeof(utf8_env_tab[0]); i++)
			utf8_env_tab[i] = NULL;
		free(utf8_env);
		utf8_env = NULL;
	}
}

static unsigned utf8_env_tab_idx(wchar_t c)
{
	/* compute hash from one letter */
	unsigned hash = (unsigned)c - (L'A' - 1);
	unsigned idx = hash % (sizeof(utf8_env_tab)/sizeof(utf8_env_tab[0]));
	return idx;
}

static void name_to_upper(wchar_t out[], const wchar_t wname[], size_t name_len)
{
	size_t i = 0;
	for (; i < name_len; i++)
		out[i] = (wchar_t)unicode_toupper(wname[i]);
}

static int utf8_env_create_(void)
{
	/* initialize the array */
	wchar_t **v = _wenviron;
	while (*v)
		v++;

	utf8_env_size = (size_t)(v - _wenviron);
	utf8_env = (char**)malloc(sizeof(*utf8_env)*(utf8_env_size + 1));
	if (!utf8_env)
		return -1;

	for (v = _wenviron; *v; v++) {
		struct utf8_env_entry *e;
		const wchar_t *wname = *v;
		const wchar_t *const eq = wcschr(wname, L'=');
		const size_t name_len = eq ? (size_t)(eq - wname) : wcslen(wname);
		size_t name_eq_val_u8_sz, e_sz;
		unsigned idx;

		if (!name_len)
			continue; /* no variable name */

		name_eq_val_u8_sz = utf16_to_utf8_z_size((const utf16_char_t**)&wname);
		if (!name_eq_val_u8_sz)
			break;

		e_sz = OFFSETOF(struct utf8_env_entry, name) + !eq;
		if (sizeof(*wname)*name_len > (size_t)-1 - e_sz ||
			name_eq_val_u8_sz > (size_t)-1 - e_sz - sizeof(*wname)*name_len)
			break;

		e_sz += sizeof(*wname)*name_len + name_eq_val_u8_sz;
		e = (struct utf8_env_entry*)malloc(e_sz);
		if (!e)
			break;

		/* convert name to upper case */
		name_to_upper(e->name, wname, name_len);

		idx = utf8_env_tab_idx(e->name[0]);
		e->next = utf8_env_tab[idx];
		utf8_env_tab[idx] = e;
		e->name_len = name_len;
		{
			char *name_eq_val_u8 = (char*)&e->name[name_len];
			utf16_to_utf8_z_unsafe((const utf16_char_t*)wname, (utf8_char_t*)name_eq_val_u8);
			if (eq)
				e->u8name_len = (size_t)(strchr(name_eq_val_u8, '=') - name_eq_val_u8);
			else {
				e->u8name_len = name_eq_val_u8_sz - 1;
				name_eq_val_u8[name_eq_val_u8_sz - 1] = '=';
				name_eq_val_u8[name_eq_val_u8_sz] = '\0';
			}
			utf8_env[utf8_env_filled++] = name_eq_val_u8;
		}
	}
	utf8_env[utf8_env_filled] = NULL;
	return *v ? -1 : 0;
}

static void utf8_env_create(void)
{
	if (utf8_env_create_())
		utf8_env_fatal();
}

A_Use_decl_annotations
char **utf8_environ(void)
{
	if (!utf8_env)
		utf8_env_create();
#if defined _MSC_VER
	__assume(utf8_env);
#endif
	return utf8_env;
}

int utf8_clearenv(void)
{
	utf8_env_shadow_reset();
	_wenviron = NULL;
	return 0;
}

static struct utf8_env_entry **utf8_env_lookup(const char name[])
{
	unsigned idx;
	struct utf8_env_entry **pe;
	size_t name_sz = 0, name_len;
	wchar_t name_buf[ENV_NAME_BUF_SIZE];
	wchar_t *wname = *name ? CVT_UTF8_TO_16_Z_SZ(name, name_buf, &name_sz) : NULL;

	if (!wname)
		return NULL;

	name_len = name_sz - 1; /* >0 */

	/* convert name to upper case */
	name_to_upper(wname, wname, name_len);

	idx = utf8_env_tab_idx(wname[0]);

	/* lookup */
	pe = &utf8_env_tab[idx];
	for (; *pe; pe = &(*pe)->next) {
		if ((*pe)->name_len == name_len &&
			!memcmp((*pe)->name, wname, name_len*sizeof(*wname)))
			break;
	}

	if (wname != name_buf)
		free(wname);
	return *pe ? pe : NULL;
}

static char *utf8_getenv_(const char name[])
{
	struct utf8_env_entry **pe = utf8_env_lookup(name);
	if (pe) {
		struct utf8_env_entry *e = *pe;
		char *u8_value = (char*)&e->name[e->name_len] + e->u8name_len + 1/*'='*/;
		return u8_value;
	}
	return NULL;
}

A_Use_decl_annotations
char *utf8_getenv(const char name[])
{
	(void)utf8_environ();
	return utf8_getenv_(name);
}

static int utf8_setenv_(const char name[], const char value[], int overwrite)
{
	unsigned idx;
	struct utf8_env_entry **pe, *e;
	size_t sz = 0, u8sz = 0, name_len, u8name_len, val_sz, u8val_sz;
	wchar_t name_buf[ENV_NAME_BUF_SIZE];
	char *u8_name_eq_value;
	wchar_t *wstr = *name ? CVT_UTF8_TO_16_Z_RESERVE(name, name_buf, &sz, &u8sz) : NULL;
	size_t e_sz;

	if (!wstr) {
		if (!*name)
			errno = EINVAL;
		return -1;
	}

	name_len = sz - 1; /* >0 */

	/* convert name to upper case */
	name_to_upper(wstr, wstr, name_len);

	idx = utf8_env_tab_idx(wstr[0]);

	/* lookup */
	pe = &utf8_env_tab[idx];
	for (; *pe; pe = &(*pe)->next) {
		if ((*pe)->name_len == name_len &&
			!memcmp((*pe)->name, wstr, name_len*sizeof(*wstr)))
		{
			if (!overwrite) {
				if (wstr != name_buf)
					free(wstr);
				return 0;
			}
			break;
		}
	}

	/* reserve a place in 'environ' array */
	if (!*pe && utf8_env_filled == utf8_env_size) {
		if (utf8_env_size > (size_t)-1/sizeof(*utf8_env) - 1 - UTF8_ENV_REALLOC_BY) {
			errno = E2BIG;
			goto err_wstr;
		}
		{
			char **new_env = (char**)realloc(utf8_env,
				sizeof(*utf8_env)*(utf8_env_size + UTF8_ENV_REALLOC_BY + 1));
			if (!new_env)
				goto err_wstr;
			utf8_env = new_env;
		}
		utf8_env_size += UTF8_ENV_REALLOC_BY;
	}

	/* create new entry */
	e_sz = OFFSETOF(struct utf8_env_entry, name) + 1/*'='*/;
	u8name_len = u8sz - 1; /* >0 */
	u8val_sz = strlen(value) + 1;
	if (sizeof(*wstr)*name_len > (size_t)-1 - e_sz ||
		u8name_len > (size_t)-1 - e_sz - sizeof(*wstr)*name_len ||
		u8val_sz > (size_t)-1 - e_sz - sizeof(*wstr)*name_len - u8name_len)
	{
		errno = E2BIG;
		goto err_wstr;
	}

	e_sz += sizeof(*wstr)*name_len + u8name_len + u8val_sz;
	e = (struct utf8_env_entry*)malloc(e_sz);
	if (!e)
		goto err_wstr;

	/* fill new entry */
	e->u8name_len = u8name_len;
	e->name_len = name_len;
	memcpy(e->name, wstr, sizeof(*wstr)*name_len);
	u8_name_eq_value = (char*)&e->name[name_len];
	utf16_to_utf8_unsafe((const utf16_char_t*)wstr, (utf8_char_t*)u8_name_eq_value, name_len);
	u8_name_eq_value[u8name_len] = '=';
	memcpy(u8_name_eq_value + u8name_len + 1, value, u8val_sz);

	/* construct 'name=value' string for _wputenv() */
	if (wstr != name_buf)
		free(wstr);

	/* reserve a space for 'name=' at head of dynamically allocated memory */
	val_sz = name_len + 1/*L'='*/;
	wstr = CVT_UTF8_TO_16_Z_RESERVE(value, name_buf, &val_sz, &u8sz);

	if (!wstr)
		goto err_e;

	if (val_sz > (size_t)-1/sizeof(wchar_t) - 1 - name_len) {
		errno = E2BIG;
		goto err_e_wstr;
	}

	if (name_len + val_sz + 1 <= sizeof(name_buf)/sizeof(name_buf[0]))
		memmove(wstr + name_len + 1, wstr, val_sz*sizeof(wstr[0]));
	else if (wstr == name_buf) {
		wchar_t *buf = (wchar_t*)malloc((name_len + val_sz + 1)*sizeof(*buf));
		if (!buf)
			goto err_e;
		memcpy(buf + name_len + 1, wstr, val_sz);
		wstr = buf;
	}
	/* fill reserved space */
	memcpy(wstr, e->name, name_len*sizeof(e->name[0]));
	wstr[name_len] = L'=';

	if (_wputenv(wstr))
		goto err_e_wstr;

	if (wstr != name_buf)
		free(wstr);

	if (*pe) {
		/* find and replace old entry */
		char **old = utf8_env;
		char *const old_name_eq_value = (char*)&(*pe)->name[(*pe)->name_len];
		for (;; old++) {
			if (*old == old_name_eq_value)
				break;
			if (!*old)
				goto err_e; /* not expected */
		}
		*old = u8_name_eq_value;
		e->next = (*pe)->next;
		free(*pe);
	}
	else {
		utf8_env[utf8_env_filled++] = u8_name_eq_value;
		e->next = NULL;
	}

	*pe = e;
	return 0;

err_e_wstr:
	free(e);
err_wstr:
	if (wstr != name_buf)
		free(wstr);
	return -1;
err_e:
	free(e);
	return -1;
}

A_Use_decl_annotations
int utf8_setenv(const char name[], const char value[], int overwrite)
{
	(void)utf8_environ();
	return utf8_setenv_(name, value, overwrite);
}

static int utf8_unsetenv_(const char name[])
{
	struct utf8_env_entry **pe = utf8_env_lookup(name);
	if (pe) {
		struct utf8_env_entry *e = *pe;
		char *old_name_eq_value = (char*)&e->name[e->name_len];

		/* find and remove old entry */
		char **old = utf8_env;
		for (;; old++) {
			if (*old == old_name_eq_value)
				break;
			if (!*old)
				return -1; /* not expected */
		}

		{
			char **end = old + 1;
			while (*end)
				end++;
			memmove(old, old + 1, (size_t)(end - old)*sizeof(*old));
		}

		*pe = e->next;
		free(e);
		utf8_env_filled--;
	}
	return 0;
}

A_Use_decl_annotations
int utf8_unsetenv(const char name[])
{
	(void)utf8_environ();
	return utf8_unsetenv_(name);
}
