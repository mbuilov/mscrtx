#ifndef LOCALE_HELPERS_H_INCLUDED
#define LOCALE_HELPERS_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* locale_helpers.h */

/* try to set locale using different aliases of the same locale name,
  returns currently active locale name or NULL if failed;
  'cat' must be one of 6 values:
  LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME */
/* Note: not thread-safe, returns pointer to global buffer */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(locale, A_In_z)
A_Ret_z
A_Success(return)
#endif
char *set_locale_helper(const int cat, const char locale[]);

/* same as set_locale_helper(), but return zero on success */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(locale, A_In_z)
A_Success(!return)
#endif
int wset_locale_helper(const int cat, const wchar_t locale[]);

/* replace code page specification of the locale name with ".UTF-8",
  returns  new locale name or NULL on failure;
  'cat' must be one of 6 values:
  LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME */
/* Note: not thread-safe, returns pointer to global buffer */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(locale, A_In_z)
A_Ret_z
A_Success(return)
#endif
char *locale_helper_add_utf8_cp(const int cat, const char locale[]);

struct set_locale_err {
	const wchar_t *cat; /* locale category name */
	const wchar_t *lc;  /* value from environment, if NULL - then failed to set 'def' locale */
};

/* Check environment for LC_* and LANG variables and set locale categories appropriately:
  1) if LC_ALL is set - it's a value for all locale categories,
  2) else if LANG is set - it's a default value for all unset locale categories,
  3) locale category (LC_CTYPE, LC_COLLATE, LC_MONETARY, LC_NUMERIC, LC_CTYPE) value
    is used only if LC_ALL is not set.
  def - if not NULL, then used as LANG, if it's not set.
  Returns zero on success.
  On error, fills err structure (if it's not NULL). */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(def, A_In_opt_z)
A_On_failure(A_At(err, A_Out_opt))
A_Success(!return)
#endif
int set_locale_from_env(const char def[]/*NULL?*/, struct set_locale_err *err/*NULL?,out*/);

#endif /* LOCALE_HELPERS_H_INCLUDED */
